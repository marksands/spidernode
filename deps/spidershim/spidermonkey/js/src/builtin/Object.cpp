/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "builtin/Object.h"

#include "mozilla/ArrayUtils.h"

#include "jscntxt.h"
#include "jsstr.h"

#include "builtin/Eval.h"
#include "frontend/BytecodeCompiler.h"
#include "jit/InlinableNatives.h"
#include "js/UniquePtr.h"
#include "vm/AsyncFunction.h"
#include "vm/RegExpObject.h"
#include "vm/StringBuffer.h"

#include "jsobjinlines.h"

#include "vm/NativeObject-inl.h"
#include "vm/Shape-inl.h"

#ifdef FUZZING
#include "builtin/TestingFunctions.h"
#endif

using namespace js;

using js::frontend::IsIdentifier;
using mozilla::ArrayLength;

bool
js::obj_construct(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedObject obj(cx, nullptr);
    if (args.isConstructing() && (&args.newTarget().toObject() != &args.callee())) {
        RootedObject newTarget(cx, &args.newTarget().toObject());
        obj = CreateThis(cx, &PlainObject::class_, newTarget);
        if (!obj)
            return false;
    } else if (args.length() > 0 && !args[0].isNullOrUndefined()) {
        obj = ToObject(cx, args[0]);
        if (!obj)
            return false;
    } else {
        /* Make an object whether this was called with 'new' or not. */
        if (!NewObjectScriptedCall(cx, &obj))
            return false;
    }

    args.rval().setObject(*obj);
    return true;
}

/* ES5 15.2.4.7. */
bool
js::obj_propertyIsEnumerable(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    HandleValue idValue = args.get(0);

    // As an optimization, provide a fast path when rooting is not necessary and
    // we can safely retrieve the attributes from the object's shape.

    /* Steps 1-2. */
    jsid id;
    if (args.thisv().isObject() && ValueToId<NoGC>(cx, idValue, &id)) {
        JSObject* obj = &args.thisv().toObject();

        /* Step 3. */
        PropertyResult prop;
        if (obj->isNative() &&
            NativeLookupOwnProperty<NoGC>(cx, &obj->as<NativeObject>(), id, &prop))
        {
            /* Step 4. */
            if (!prop) {
                args.rval().setBoolean(false);
                return true;
            }

            /* Step 5. */
            unsigned attrs = GetPropertyAttributes(obj, prop);
            args.rval().setBoolean((attrs & JSPROP_ENUMERATE) != 0);
            return true;
        }
    }

    /* Step 1. */
    RootedId idRoot(cx);
    if (!ToPropertyKey(cx, idValue, &idRoot))
        return false;

    /* Step 2. */
    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    /* Step 3. */
    Rooted<PropertyDescriptor> desc(cx);
    if (!GetOwnPropertyDescriptor(cx, obj, idRoot, &desc))
        return false;

    /* Steps 4-5. */
    args.rval().setBoolean(desc.object() && desc.enumerable());
    return true;
}

#if JS_HAS_TOSOURCE
static bool
obj_toSource(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    if (!CheckRecursionLimit(cx))
        return false;

    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    JSString* str = ObjectToSource(cx, obj);
    if (!str)
        return false;

    args.rval().setString(str);
    return true;
}

template <typename CharT>
static bool
Consume(const CharT*& s, const CharT* e, const char *chars)
{
    size_t len = strlen(chars);
    if (s + len >= e)
        return false;
    if (!EqualChars(s, chars, len))
        return false;
    s += len;
    return true;
}

template <typename CharT>
static void
ConsumeSpaces(const CharT*& s, const CharT* e)
{
    while (*s == ' ' && s < e)
        s++;
}

/*
 * Given a function source string, return the offset and length of the part
 * between '(function $name' and ')'.
 */
template <typename CharT>
static bool
ArgsAndBodySubstring(mozilla::Range<const CharT> chars, size_t* outOffset, size_t* outLen)
{
    const CharT* const start = chars.begin().get();
    const CharT* s = start;
    const CharT* e = chars.end().get();

    if (s == e)
        return false;

    // Remove enclosing parentheses.
    if (*s == '(' && *(e - 1) == ')') {
        s++;
        e--;
    }

    // Support the following cases, with spaces between tokens:
    //
    //   -+---------+-+------------+-+-----+-+- [ - <any> - ] - ( -+-
    //    |         | |            | |     | |                     |
    //    +- async -+ +- function -+ +- * -+ +- <any> - ( ---------+
    //                |            |
    //                +- get ------+
    //                |            |
    //                +- set ------+
    //
    // This accepts some invalid syntax, but we don't care, since it's only
    // used by the non-standard toSource, and we're doing a best-effort attempt
    // here.

    (void) Consume(s, e, "async");
    ConsumeSpaces(s, e);
    (void) (Consume(s, e, "function") || Consume(s, e, "get") || Consume(s, e, "set"));
    ConsumeSpaces(s, e);
    (void) Consume(s, e, "*");
    ConsumeSpaces(s, e);

    // Jump over the function's name.
    if (Consume(s, e, "[")) {
        s = js_strchr_limit(s, ']', e);
        if (!s)
            return false;
        s++;
        ConsumeSpaces(s, e);
        if (*s != '(')
            return false;
    } else {
        s = js_strchr_limit(s, '(', e);
        if (!s)
            return false;
    }

    *outOffset = s - start;
    *outLen = e - s;
    MOZ_ASSERT(*outOffset + *outLen <= chars.length());
    return true;
}

enum class PropertyKind {
    Getter,
    Setter,
    Method,
    Normal
};

JSString*
js::ObjectToSource(JSContext* cx, HandleObject obj)
{
    /* If outermost, we need parentheses to be an expression, not a block. */
    bool outermost = cx->cycleDetectorVector().empty();

    AutoCycleDetector detector(cx, obj);
    if (!detector.init())
        return nullptr;
    if (detector.foundCycle())
        return NewStringCopyZ<CanGC>(cx, "{}");

    StringBuffer buf(cx);
    if (outermost && !buf.append('('))
        return nullptr;
    if (!buf.append('{'))
        return nullptr;

    AutoIdVector idv(cx);
    if (!GetPropertyKeys(cx, obj, JSITER_OWNONLY | JSITER_SYMBOLS, &idv))
        return nullptr;

    bool comma = false;

    auto AddProperty = [cx, &comma, &buf](HandleId id, HandleValue val, PropertyKind kind) -> bool {
        /* Convert id to a string. */
        RootedString idstr(cx);
        if (JSID_IS_SYMBOL(id)) {
            RootedValue v(cx, SymbolValue(JSID_TO_SYMBOL(id)));
            idstr = ValueToSource(cx, v);
            if (!idstr)
                return false;
        } else {
            RootedValue idv(cx, IdToValue(id));
            idstr = ToString<CanGC>(cx, idv);
            if (!idstr)
                return false;

            /*
             * If id is a string that's not an identifier, or if it's a
             * negative integer, then it must be quoted.
             */
            if (JSID_IS_ATOM(id)
                ? !IsIdentifier(JSID_TO_ATOM(id))
                : JSID_TO_INT(id) < 0)
            {
                idstr = QuoteString(cx, idstr, char16_t('\''));
                if (!idstr)
                    return false;
            }
        }

        RootedString valsource(cx, ValueToSource(cx, val));
        if (!valsource)
            return false;

        RootedLinearString valstr(cx, valsource->ensureLinear(cx));
        if (!valstr)
            return false;

        if (comma && !buf.append(", "))
            return false;
        comma = true;

        size_t voffset, vlength;

        // Methods and accessors can return exact syntax of source, that fits
        // into property without adding property name or "get"/"set" prefix.
        // Use the exact syntax when the following conditions are met:
        //
        //   * It's a function object
        //     (exclude proxies)
        //   * Function's kind and property's kind are same
        //     (this can be false for dynamically defined properties)
        //   * Function has explicit name
        //     (this can be false for computed property and dynamically defined
        //      properties)
        //   * Function's name and property's name are same
        //     (this can be false for dynamically defined properties)
        if (kind == PropertyKind::Getter || kind == PropertyKind::Setter ||
            kind == PropertyKind::Method)
        {
            RootedFunction fun(cx);
            if (val.toObject().is<JSFunction>()) {
                fun = &val.toObject().as<JSFunction>();
                // Method's case should be checked on caller.
                if (((fun->isGetter() && kind == PropertyKind::Getter) ||
                     (fun->isSetter() && kind == PropertyKind::Setter) ||
                     kind == PropertyKind::Method) &&
                    fun->explicitName())
                {
                    bool result;
                    if (!EqualStrings(cx, fun->explicitName(), idstr, &result))
                        return false;

                    if (result)  {
                        if (!buf.append(valstr))
                            return false;
                        return true;
                    }
                }
            }

            {
                // When falling back try to generate a better string
                // representation by skipping the prelude, and also removing
                // the enclosing parentheses.
                bool success;
                JS::AutoCheckCannotGC nogc;
                if (valstr->hasLatin1Chars())
                    success = ArgsAndBodySubstring(valstr->latin1Range(nogc), &voffset, &vlength);
                else
                    success = ArgsAndBodySubstring(valstr->twoByteRange(nogc), &voffset, &vlength);
                if (!success)
                    kind = PropertyKind::Normal;
            }

            if (kind == PropertyKind::Getter) {
                if (!buf.append("get "))
                    return false;
            } else if (kind == PropertyKind::Setter) {
                if (!buf.append("set "))
                    return false;
            } else if (kind == PropertyKind::Method && fun) {
                if (IsWrappedAsyncFunction(fun)) {
                    if (!buf.append("async "))
                        return false;
                }

                if (fun->isStarGenerator()) {
                    if (!buf.append('*'))
                        return false;
                }
            }
        }

        bool needsBracket = JSID_IS_SYMBOL(id);
        if (needsBracket && !buf.append('['))
            return false;
        if (!buf.append(idstr))
            return false;
        if (needsBracket && !buf.append(']'))
            return false;

        if (kind == PropertyKind::Getter || kind == PropertyKind::Setter ||
            kind == PropertyKind::Method)
        {
            if (!buf.appendSubstring(valstr, voffset, vlength))
                return false;
        } else {
            if (!buf.append(':'))
                return false;
            if (!buf.append(valstr))
                return false;
        }
        return true;
    };

    RootedId id(cx);
    Rooted<PropertyDescriptor> desc(cx);
    RootedValue val(cx);
    RootedFunction fun(cx);
    for (size_t i = 0; i < idv.length(); ++i) {
        id = idv[i];
        if (!GetOwnPropertyDescriptor(cx, obj, id, &desc))
            return nullptr;

        if (!desc.object())
            continue;

        if (desc.isAccessorDescriptor()) {
            if (desc.hasGetterObject() && desc.getterObject()) {
                val.setObject(*desc.getterObject());
                if (!AddProperty(id, val, PropertyKind::Getter))
                    return nullptr;
            }
            if (desc.hasSetterObject() && desc.setterObject()) {
                val.setObject(*desc.setterObject());
                if (!AddProperty(id, val, PropertyKind::Setter))
                    return nullptr;
            }
            continue;
        }

        val.set(desc.value());
        if (IsFunctionObject(val, fun.address())) {
            if (IsWrappedAsyncFunction(fun))
                fun = GetUnwrappedAsyncFunction(fun);

            if (fun->isMethod()) {
                if (!AddProperty(id, val, PropertyKind::Method))
                    return nullptr;
                continue;
            }
        }

        if (!AddProperty(id, val, PropertyKind::Normal))
            return nullptr;
    }

    if (!buf.append('}'))
        return nullptr;
    if (outermost && !buf.append(')'))
        return nullptr;

    return buf.finishString();
}
#endif /* JS_HAS_TOSOURCE */

static bool
GetBuiltinTagSlow(JSContext* cx, HandleObject obj, MutableHandleString builtinTag)
{
    // Step 4.
    bool isArray;
    if (!IsArray(cx, obj, &isArray))
        return false;

    // Step 5.
    if (isArray) {
        builtinTag.set(cx->names().objectArray);
        return true;
    }

    // Steps 6-13.
    ESClass cls;
    if (!GetBuiltinClass(cx, obj, &cls))
        return false;

    switch (cls) {
      case ESClass::String:
        builtinTag.set(cx->names().objectString);
        return true;
      case ESClass::Arguments:
        builtinTag.set(cx->names().objectArguments);
        return true;
      case ESClass::Error:
        builtinTag.set(cx->names().objectError);
        return true;
      case ESClass::Boolean:
        builtinTag.set(cx->names().objectBoolean);
        return true;
      case ESClass::Number:
        builtinTag.set(cx->names().objectNumber);
        return true;
      case ESClass::Date:
        builtinTag.set(cx->names().objectDate);
        return true;
      case ESClass::RegExp:
        builtinTag.set(cx->names().objectRegExp);
        return true;
      default:
        if (obj->isCallable()) {
            // Non-standard: Prevent <object> from showing up as Function.
            RootedObject unwrapped(cx, CheckedUnwrap(obj));
            if (!unwrapped || !unwrapped->getClass()->isDOMClass()) {
                builtinTag.set(cx->names().objectFunction);
                return true;
            }
        }
        builtinTag.set(nullptr);
        return true;
    }
}

static MOZ_ALWAYS_INLINE JSString*
GetBuiltinTagFast(JSObject* obj, const Class* clasp, JSContext* cx)
{
    MOZ_ASSERT(clasp == obj->getClass());
    MOZ_ASSERT(!clasp->isProxy());

    // Optimize the non-proxy case to bypass GetBuiltinClass.
    if (clasp == &PlainObject::class_ || clasp == &UnboxedPlainObject::class_) {
        // This is not handled by GetBuiltinTagSlow, but this case is by far
        // the most common so we optimize it here.
        return cx->names().objectObject;
    }

    if (clasp == &ArrayObject::class_ || clasp == &UnboxedArrayObject::class_)
        return cx->names().objectArray;

    if (clasp == &JSFunction::class_)
        return cx->names().objectFunction;

    if (clasp == &StringObject::class_)
        return cx->names().objectString;

    if (clasp == &NumberObject::class_)
        return cx->names().objectNumber;

    if (clasp == &BooleanObject::class_)
        return cx->names().objectBoolean;

    if (clasp == &DateObject::class_)
        return cx->names().objectDate;

    if (clasp == &RegExpObject::class_)
        return cx->names().objectRegExp;

    if (obj->is<ArgumentsObject>())
        return cx->names().objectArguments;

    if (obj->is<ErrorObject>())
        return cx->names().objectError;

    if (obj->isCallable() && !obj->getClass()->isDOMClass()) {
        // Non-standard: Prevent <object> from showing up as Function.
        return cx->names().objectFunction;
    }

    return nullptr;
}

// ES6 19.1.3.6
bool
js::obj_toString(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Step 1.
    if (args.thisv().isUndefined()) {
        args.rval().setString(cx->names().objectUndefined);
        return true;
    }

    // Step 2.
    if (args.thisv().isNull()) {
        args.rval().setString(cx->names().objectNull);
        return true;
    }

    // Step 3.
    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    RootedString builtinTag(cx);
    const Class* clasp = obj->getClass();
    if (MOZ_UNLIKELY(clasp->isProxy())) {
        if (!GetBuiltinTagSlow(cx, obj, &builtinTag))
            return false;
    } else {
        builtinTag = GetBuiltinTagFast(obj, clasp, cx);
#ifdef DEBUG
        // Assert this fast path is correct and matches BuiltinTagSlow. The
        // only exception is the PlainObject case: we special-case it here
        // because it's so common, but BuiltinTagSlow doesn't handle this.
        RootedString builtinTagSlow(cx);
        if (!GetBuiltinTagSlow(cx, obj, &builtinTagSlow))
            return false;
        if (clasp == &PlainObject::class_ || clasp == &UnboxedPlainObject::class_)
            MOZ_ASSERT(!builtinTagSlow);
        else
            MOZ_ASSERT(builtinTagSlow == builtinTag);
#endif
    }

    // Step 14.
    // Currently omitted for non-standard fallback.

    // Step 15.
    RootedValue tag(cx);
    if (!GetInterestingSymbolProperty(cx, obj, cx->wellKnownSymbols().toStringTag, &tag))
        return false;

    // Step 16.
    if (!tag.isString()) {
        // Non-standard (bug 1277801): Use ClassName as a fallback in the interim
        if (!builtinTag) {
            const char* className = GetObjectClassName(cx, obj);
            StringBuffer sb(cx);
            if (!sb.append("[object ") || !sb.append(className, strlen(className)) ||
                !sb.append(']'))
            {
                return false;
            }

            builtinTag = sb.finishAtom();
            if (!builtinTag)
                return false;
        }

        args.rval().setString(builtinTag);
        return true;
    }

    // Step 17.
    StringBuffer sb(cx);
    if (!sb.append("[object ") || !sb.append(tag.toString()) || !sb.append(']'))
        return false;

    JSString* str = sb.finishAtom();
    if (!str)
        return false;

    args.rval().setString(str);
    return true;
}

JSString*
js::ObjectClassToString(JSContext* cx, HandleObject obj)
{
    const Class* clasp = obj->getClass();

    if (JSString* tag = GetBuiltinTagFast(obj, clasp, cx))
        return tag;

    const char* className = clasp->name;
    StringBuffer sb(cx);
    if (!sb.append("[object ") ||
        !sb.append(className, strlen(className)) ||
        !sb.append(']'))
    {
        return nullptr;
    }

    return sb.finishAtom();
}

static bool
obj_setPrototypeOf(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    if (args.length() < 2) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_MORE_ARGS_NEEDED,
                                  "Object.setPrototypeOf", "1", "");
        return false;
    }

    /* Step 1-2. */
    if (args[0].isNullOrUndefined()) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_CANT_CONVERT_TO,
                                  args[0].isNull() ? "null" : "undefined", "object");
        return false;
    }

    /* Step 3. */
    if (!args[1].isObjectOrNull()) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_NOT_EXPECTED_TYPE,
                                  "Object.setPrototypeOf", "an object or null",
                                  InformalValueTypeName(args[1]));
        return false;
    }

    /* Step 4. */
    if (!args[0].isObject()) {
        args.rval().set(args[0]);
        return true;
    }

    /* Step 5-7. */
    RootedObject obj(cx, &args[0].toObject());
    RootedObject newProto(cx, args[1].toObjectOrNull());
    if (!SetPrototype(cx, obj, newProto))
        return false;

    /* Step 8. */
    args.rval().set(args[0]);
    return true;
}

static bool
PropertyIsEnumerable(JSContext* cx, HandleObject obj, HandleId id, bool* enumerable)
{
    PropertyResult prop;
    if (obj->isNative() &&
        NativeLookupOwnProperty<NoGC>(cx, &obj->as<NativeObject>(), id, &prop))
    {
        if (!prop) {
            *enumerable = false;
            return true;
        }

        unsigned attrs = GetPropertyAttributes(obj, prop);
        *enumerable = (attrs & JSPROP_ENUMERATE) != 0;
        return true;
    }

    Rooted<PropertyDescriptor> desc(cx);
    if (!GetOwnPropertyDescriptor(cx, obj, id, &desc))
        return false;

    *enumerable = desc.object() && desc.enumerable();
    return true;
}

static bool
TryAssignNative(JSContext* cx, HandleObject to, HandleObject from, bool* optimized)
{
    *optimized = false;

    if (!from->isNative() || !to->isNative())
        return true;

    // Don't use the fast path if |from| may have extra indexed or lazy
    // properties.
    NativeObject* fromNative = &from->as<NativeObject>();
    if (fromNative->getDenseInitializedLength() > 0 ||
        fromNative->isIndexed() ||
        fromNative->is<TypedArrayObject>() ||
        fromNative->getClass()->getNewEnumerate() ||
        fromNative->getClass()->getEnumerate())
    {
        return true;
    }

    // Get a list of |from| shapes. As long as from->lastProperty() == fromShape
    // we can use this to speed up both the enumerability check and the GetProp.

    using ShapeVector = GCVector<Shape*, 8>;
    Rooted<ShapeVector> shapes(cx, ShapeVector(cx));

    RootedShape fromShape(cx, fromNative->lastProperty());
    for (Shape::Range<NoGC> r(fromShape); !r.empty(); r.popFront()) {
        // Symbol properties need to be assigned last. For now fall back to the
        // slow path if we see a symbol property.
        if (MOZ_UNLIKELY(JSID_IS_SYMBOL(r.front().propidRaw())))
            return true;
        if (MOZ_UNLIKELY(!shapes.append(&r.front())))
            return false;
    }

    *optimized = true;

    RootedShape shape(cx);
    RootedValue propValue(cx);
    RootedId nextKey(cx);
    RootedValue toReceiver(cx, ObjectValue(*to));

    for (size_t i = shapes.length(); i > 0; i--) {
        shape = shapes[i - 1];
        nextKey = shape->propid();

        // Ensure |from| is still native: a getter/setter might have turned
        // |from| or |to| into an unboxed object or it could have been swapped
        // with a non-native object.
        if (MOZ_LIKELY(from->isNative() &&
                       from->as<NativeObject>().lastProperty() == fromShape &&
                       shape->hasDefaultGetter() &&
                       shape->hasSlot()))
        {
            if (!shape->enumerable())
                continue;
            propValue = from->as<NativeObject>().getSlot(shape->slot());
        } else {
            // |from| changed shape or the property is not a data property, so
            // we have to do the slower enumerability check and GetProp.
            bool enumerable;
            if (!PropertyIsEnumerable(cx, from, nextKey, &enumerable))
                return false;
            if (!enumerable)
                continue;
            if (!GetProperty(cx, from, from, nextKey, &propValue))
                return false;
        }

        ObjectOpResult result;
        if (MOZ_UNLIKELY(!SetProperty(cx, to, nextKey, propValue, toReceiver, result)))
            return false;
        if (MOZ_UNLIKELY(!result.checkStrict(cx, to, nextKey)))
            return false;
    }

    return true;
}

static bool
AssignSlow(JSContext* cx, HandleObject to, HandleObject from)
{
    // Step 4.b.ii.
    AutoIdVector keys(cx);
    if (!GetPropertyKeys(cx, from, JSITER_OWNONLY | JSITER_HIDDEN | JSITER_SYMBOLS, &keys))
        return false;

    // Step 4.c.
    RootedId nextKey(cx);
    RootedValue propValue(cx);
    for (size_t i = 0, len = keys.length(); i < len; i++) {
        nextKey = keys[i];

        // Step 4.c.i.
        bool enumerable;
        if (MOZ_UNLIKELY(!PropertyIsEnumerable(cx, from, nextKey, &enumerable)))
            return false;
        if (!enumerable)
            continue;

        // Step 4.c.ii.1.
        if (MOZ_UNLIKELY(!GetProperty(cx, from, from, nextKey, &propValue)))
            return false;

        // Step 4.c.ii.2.
        if (MOZ_UNLIKELY(!SetProperty(cx, to, nextKey, propValue)))
            return false;
    }

    return true;
}

// ES2018 draft rev 48ad2688d8f964da3ea8c11163ef20eb126fb8a4
// 19.1.2.1 Object.assign(target, ...sources)
static bool
obj_assign(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Step 1.
    RootedObject to(cx, ToObject(cx, args.get(0)));
    if (!to)
        return false;

    // Note: step 2 is implicit. If there are 0 arguments, ToObject throws. If
    // there's 1 argument, the loop below is a no-op.

    // Step 4.
    RootedObject from(cx);
    for (size_t i = 1; i < args.length(); i++) {
        // Step 4.a.
        if (args[i].isNullOrUndefined())
            continue;

        // Step 4.b.i.
        from = ToObject(cx, args[i]);
        if (!from)
            return false;

        // Steps 4.b.ii, 4.c.
        bool optimized;
        if (!TryAssignNative(cx, to, from, &optimized))
            return false;
        if (optimized)
            continue;

        if (!AssignSlow(cx, to, from))
            return false;
    }

    // Step 5.
    args.rval().setObject(*to);
    return true;
}

#if JS_HAS_OBJ_WATCHPOINT

bool
js::WatchHandler(JSContext* cx, JSObject* obj_, jsid id_, const JS::Value& old,
                 JS::Value* nvp, void* closure)
{
    RootedObject obj(cx, obj_);
    RootedId id(cx, id_);

    /* Avoid recursion on (obj, id) already being watched on cx. */
    AutoResolving resolving(cx, obj, id, AutoResolving::WATCH);
    if (resolving.alreadyStarted())
        return true;

    FixedInvokeArgs<3> args(cx);

    args[0].set(IdToValue(id));
    args[1].set(old);
    args[2].set(*nvp);

    RootedValue callable(cx, ObjectValue(*static_cast<JSObject*>(closure)));
    RootedValue thisv(cx, ObjectValue(*obj));
    RootedValue rv(cx);
    if (!Call(cx, callable, thisv, args, &rv))
        return false;

    *nvp = rv;
    return true;
}

static bool
obj_watch(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    if (!GlobalObject::warnOnceAboutWatch(cx, obj))
        return false;

    if (args.length() <= 1) {
        ReportMissingArg(cx, args.calleev(), 1);
        return false;
    }

    RootedObject callable(cx, ValueToCallable(cx, args[1], args.length() - 2));
    if (!callable)
        return false;

    RootedId propid(cx);
    if (!ValueToId<CanGC>(cx, args[0], &propid))
        return false;

    if (!WatchProperty(cx, obj, propid, callable))
        return false;

    args.rval().setUndefined();
    return true;
}

static bool
obj_unwatch(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    if (!GlobalObject::warnOnceAboutWatch(cx, obj))
        return false;

    RootedId id(cx);
    if (args.length() != 0) {
        if (!ValueToId<CanGC>(cx, args[0], &id))
            return false;
    } else {
        id = JSID_VOID;
    }

    if (!UnwatchProperty(cx, obj, id))
        return false;

    args.rval().setUndefined();
    return true;
}

#endif /* JS_HAS_OBJ_WATCHPOINT */

/* ES5 15.2.4.6. */
static bool
obj_isPrototypeOf(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    /* Step 1. */
    if (args.length() < 1 || !args[0].isObject()) {
        args.rval().setBoolean(false);
        return true;
    }

    /* Step 2. */
    RootedObject obj(cx, ToObject(cx, args.thisv()));
    if (!obj)
        return false;

    /* Step 3. */
    bool isDelegate;
    if (!IsDelegate(cx, obj, args[0], &isDelegate))
        return false;
    args.rval().setBoolean(isDelegate);
    return true;
}

PlainObject*
js::ObjectCreateImpl(JSContext* cx, HandleObject proto, NewObjectKind newKind,
                     HandleObjectGroup group)
{
    // Give the new object a small number of fixed slots, like we do for empty
    // object literals ({}).
    gc::AllocKind allocKind = GuessObjectGCKind(0);

    if (!proto) {
        // Object.create(null) is common, optimize it by using an allocation
        // site specific ObjectGroup. Because GetCallerInitGroup is pretty
        // slow, the caller can pass in the group if it's known and we use that
        // instead.
        RootedObjectGroup ngroup(cx, group);
        if (!ngroup) {
            ngroup = ObjectGroup::callingAllocationSiteGroup(cx, JSProto_Null);
            if (!ngroup)
                return nullptr;
        }

        MOZ_ASSERT(!ngroup->proto().toObjectOrNull());

        return NewObjectWithGroup<PlainObject>(cx, ngroup, allocKind, newKind);
    }

    return NewObjectWithGivenProto<PlainObject>(cx, proto, allocKind, newKind);
}

PlainObject*
js::ObjectCreateWithTemplate(JSContext* cx, HandlePlainObject templateObj)
{
    RootedObject proto(cx, templateObj->staticPrototype());
    RootedObjectGroup group(cx, templateObj->group());
    return ObjectCreateImpl(cx, proto, GenericObject, group);
}

// ES 2017 draft 19.1.2.3.1
static bool
ObjectDefineProperties(JSContext* cx, HandleObject obj, HandleValue properties)
{
    // Step 1. implicit
    // Step 2.
    RootedObject props(cx, ToObject(cx, properties));
    if (!props)
        return false;

    // Step 3.
    AutoIdVector keys(cx);
    if (!GetPropertyKeys(cx, props, JSITER_OWNONLY | JSITER_SYMBOLS | JSITER_HIDDEN, &keys))
        return false;

    RootedId nextKey(cx);
    Rooted<PropertyDescriptor> desc(cx);
    RootedValue descObj(cx);

    // Step 4.
    Rooted<PropertyDescriptorVector> descriptors(cx, PropertyDescriptorVector(cx));
    AutoIdVector descriptorKeys(cx);

    // Step 5.
    for (size_t i = 0, len = keys.length(); i < len; i++) {
        nextKey = keys[i];

        // Step 5.a.
        if (!GetOwnPropertyDescriptor(cx, props, nextKey, &desc))
            return false;

        // Step 5.b.
        if (desc.object() && desc.enumerable()) {
            if (!GetProperty(cx, props, props, nextKey, &descObj) ||
                !ToPropertyDescriptor(cx, descObj, true, &desc) ||
                !descriptors.append(desc) ||
                !descriptorKeys.append(nextKey))
            {
                return false;
            }
        }
    }

    // Step 6.
    for (size_t i = 0, len = descriptors.length(); i < len; i++) {
        if (!DefineProperty(cx, obj, descriptorKeys[i], descriptors[i]))
            return false;
    }

    return true;
}

// ES6 draft rev34 (2015/02/20) 19.1.2.2 Object.create(O [, Properties])
bool
js::obj_create(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Step 1.
    if (args.length() == 0) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_MORE_ARGS_NEEDED,
                                  "Object.create", "0", "s");
        return false;
    }

    if (!args[0].isObjectOrNull()) {
        RootedValue v(cx, args[0]);
        UniqueChars bytes = DecompileValueGenerator(cx, JSDVG_SEARCH_STACK, v, nullptr);
        if (!bytes)
            return false;

        JS_ReportErrorNumberLatin1(cx, GetErrorMessage, nullptr, JSMSG_UNEXPECTED_TYPE,
                                   bytes.get(), "not an object or null");
        return false;
    }

    // Step 2.
    RootedObject proto(cx, args[0].toObjectOrNull());
    RootedPlainObject obj(cx, ObjectCreateImpl(cx, proto));
    if (!obj)
        return false;

    // Step 3.
    if (args.hasDefined(1)) {
        if (!ObjectDefineProperties(cx, obj, args[1]))
            return false;
    }

    // Step 4.
    args.rval().setObject(*obj);
    return true;
}

// ES6 draft rev27 (2014/08/24) 19.1.2.6 Object.getOwnPropertyDescriptor(O, P)
bool
js::obj_getOwnPropertyDescriptor(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Steps 1-2.
    RootedObject obj(cx, ToObject(cx, args.get(0)));
    if (!obj)
        return false;

    // Steps 3-4.
    RootedId id(cx);
    if (!ToPropertyKey(cx, args.get(1), &id))
        return false;

    // Steps 5-7.
    Rooted<PropertyDescriptor> desc(cx);
    return GetOwnPropertyDescriptor(cx, obj, id, &desc) &&
           JS::FromPropertyDescriptor(cx, desc, args.rval());
}

enum EnumerableOwnPropertiesKind {
    Keys,
    Values,
    KeysAndValues
};

// ES7 proposal 2015-12-14
// http://tc39.github.io/proposal-object-values-entries/#EnumerableOwnProperties
static bool
EnumerableOwnProperties(JSContext* cx, const JS::CallArgs& args, EnumerableOwnPropertiesKind kind)
{
    // Step 1. (Step 1 of Object.{keys,values,entries}, really.)
    RootedObject obj(cx, ToObject(cx, args.get(0)));
    if (!obj)
        return false;

    // Step 2.
    AutoIdVector ids(cx);
    if (!GetPropertyKeys(cx, obj, JSITER_OWNONLY | JSITER_HIDDEN, &ids))
        return false;

    // Step 3.
    AutoValueVector properties(cx);
    size_t len = ids.length();
    if (!properties.resize(len))
        return false;

    RootedId id(cx);
    RootedValue key(cx);
    RootedValue value(cx);
    RootedNativeObject nobj(cx);
    if (obj->is<NativeObject>())
        nobj = &obj->as<NativeObject>();
    RootedShape shape(cx);
    Rooted<PropertyDescriptor> desc(cx);
    // Step 4.
    size_t out = 0;
    for (size_t i = 0; i < len; i++) {
        id = ids[i];

        // Step 4.a. (Symbols were filtered out in step 2.)
        MOZ_ASSERT(!JSID_IS_SYMBOL(id));

        if (kind != Values) {
            if (!IdToStringOrSymbol(cx, id, &key))
                return false;
        }

        // Step 4.a.i.
        if (nobj) {
            if (JSID_IS_INT(id) && nobj->containsDenseElement(JSID_TO_INT(id))) {
                value = nobj->getDenseOrTypedArrayElement(JSID_TO_INT(id));
            } else {
                shape = nobj->lookup(cx, id);
                if (!shape || !(shape->attributes() & JSPROP_ENUMERATE))
                    continue;
                if (!shape->isAccessorShape()) {
                    if (!NativeGetExistingProperty(cx, nobj, nobj, shape, &value))
                        return false;
                } else if (!GetProperty(cx, obj, obj, id, &value)) {
                    return false;
                }
            }
        } else {
            if (!GetOwnPropertyDescriptor(cx, obj, id, &desc))
                return false;

            // Step 4.a.ii. (inverted.)
            if (!desc.object() || !desc.enumerable())
                continue;

            // Step 4.a.ii.1.
            // (Omitted because Object.keys doesn't use this implementation.)

            // Step 4.a.ii.2.a.
            if (!GetProperty(cx, obj, obj, id, &value))
                return false;
        }

        // Steps 4.a.ii.2.b-c.
        if (kind == Values)
            properties[out++].set(value);
        else if (!NewValuePair(cx, key, value, properties[out++]))
            return false;
    }

    // Step 5.
    // (Implemented in step 2.)

    // Step 3 of Object.{keys,values,entries}
    JSObject* aobj = NewDenseCopiedArray(cx, out, properties.begin());
    if (!aobj)
        return false;

    args.rval().setObject(*aobj);
    return true;
}

// ES7 proposal 2015-12-14
// http://tc39.github.io/proposal-object-values-entries/#Object.keys
static bool
obj_keys(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    return GetOwnPropertyKeys(cx, args, JSITER_OWNONLY);
}

// ES7 proposal 2015-12-14
// http://tc39.github.io/proposal-object-values-entries/#Object.values
static bool
obj_values(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    return EnumerableOwnProperties(cx, args, Values);
}

// ES7 proposal 2015-12-14
// http://tc39.github.io/proposal-object-values-entries/#Object.entries
static bool
obj_entries(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    return EnumerableOwnProperties(cx, args, KeysAndValues);
}

/* ES6 draft 15.2.3.16 */
static bool
obj_is(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    bool same;
    if (!SameValue(cx, args.get(0), args.get(1), &same))
        return false;

    args.rval().setBoolean(same);
    return true;
}

bool
js::IdToStringOrSymbol(JSContext* cx, HandleId id, MutableHandleValue result)
{
    if (JSID_IS_INT(id)) {
        JSString* str = Int32ToString<CanGC>(cx, JSID_TO_INT(id));
        if (!str)
            return false;
        result.setString(str);
    } else if (JSID_IS_ATOM(id)) {
        result.setString(JSID_TO_STRING(id));
    } else {
        result.setSymbol(JSID_TO_SYMBOL(id));
    }
    return true;
}

/* ES6 draft rev 25 (2014 May 22) 19.1.2.8.1 */
bool
js::GetOwnPropertyKeys(JSContext* cx, const JS::CallArgs& args, unsigned flags)
{
    // Steps 1-2.
    RootedObject obj(cx, ToObject(cx, args.get(0)));
    if (!obj)
        return false;

    // Steps 3-10.
    AutoIdVector keys(cx);
    if (!GetPropertyKeys(cx, obj, flags, &keys))
        return false;

    // Step 11.
    AutoValueVector vals(cx);
    if (!vals.resize(keys.length()))
        return false;

    for (size_t i = 0, len = keys.length(); i < len; i++) {
        MOZ_ASSERT_IF(JSID_IS_SYMBOL(keys[i]), flags & JSITER_SYMBOLS);
        MOZ_ASSERT_IF(!JSID_IS_SYMBOL(keys[i]), !(flags & JSITER_SYMBOLSONLY));
        if (!IdToStringOrSymbol(cx, keys[i], vals[i]))
            return false;
    }

    JSObject* aobj = NewDenseCopiedArray(cx, vals.length(), vals.begin());
    if (!aobj)
        return false;

    args.rval().setObject(*aobj);
    return true;
}

bool
js::obj_getOwnPropertyNames(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    return GetOwnPropertyKeys(cx, args, JSITER_OWNONLY | JSITER_HIDDEN);
}

/* ES6 draft rev 25 (2014 May 22) 19.1.2.8 */
static bool
obj_getOwnPropertySymbols(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    return GetOwnPropertyKeys(cx, args,
                              JSITER_OWNONLY | JSITER_HIDDEN | JSITER_SYMBOLS | JSITER_SYMBOLSONLY);
}

/* ES6 draft rev 32 (2015 Feb 2) 19.1.2.4: Object.defineProperty(O, P, Attributes) */
bool
js::obj_defineProperty(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Steps 1-3.
    RootedObject obj(cx);
    if (!GetFirstArgumentAsObject(cx, args, "Object.defineProperty", &obj))
        return false;
    RootedId id(cx);
    if (!ToPropertyKey(cx, args.get(1), &id))
        return false;

    // Steps 4-5.
    Rooted<PropertyDescriptor> desc(cx);
    if (!ToPropertyDescriptor(cx, args.get(2), true, &desc))
        return false;

    // Steps 6-8.
    if (!DefineProperty(cx, obj, id, desc))
        return false;
    args.rval().setObject(*obj);
    return true;
}

/* ES5 15.2.3.7: Object.defineProperties(O, Properties) */
static bool
obj_defineProperties(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    /* Steps 1 and 7. */
    RootedObject obj(cx);
    if (!GetFirstArgumentAsObject(cx, args, "Object.defineProperties", &obj))
        return false;
    args.rval().setObject(*obj);

    /* Step 2. */
    if (args.length() < 2) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_MORE_ARGS_NEEDED,
                                  "Object.defineProperties", "0", "s");
        return false;
    }

    /* Steps 3-6. */
    return ObjectDefineProperties(cx, obj, args[1]);
}

// ES6 20141014 draft 19.1.2.15 Object.preventExtensions(O)
static bool
obj_preventExtensions(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(args.get(0));

    // Step 1.
    if (!args.get(0).isObject())
        return true;

    // Steps 2-5.
    RootedObject obj(cx, &args.get(0).toObject());
    return PreventExtensions(cx, obj);
}

// ES6 draft rev27 (2014/08/24) 19.1.2.5 Object.freeze(O)
static bool
obj_freeze(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(args.get(0));

    // Step 1.
    if (!args.get(0).isObject())
        return true;

    // Steps 2-5.
    RootedObject obj(cx, &args.get(0).toObject());
    return SetIntegrityLevel(cx, obj, IntegrityLevel::Frozen);
}

// ES6 draft rev27 (2014/08/24) 19.1.2.12 Object.isFrozen(O)
static bool
obj_isFrozen(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Step 1.
    bool frozen = true;

    // Step 2.
    if (args.get(0).isObject()) {
        RootedObject obj(cx, &args.get(0).toObject());
        if (!TestIntegrityLevel(cx, obj, IntegrityLevel::Frozen, &frozen))
            return false;
    }
    args.rval().setBoolean(frozen);
    return true;
}

// ES6 draft rev27 (2014/08/24) 19.1.2.17 Object.seal(O)
static bool
obj_seal(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(args.get(0));

    // Step 1.
    if (!args.get(0).isObject())
        return true;

    // Steps 2-5.
    RootedObject obj(cx, &args.get(0).toObject());
    return SetIntegrityLevel(cx, obj, IntegrityLevel::Sealed);
}

// ES6 draft rev27 (2014/08/24) 19.1.2.13 Object.isSealed(O)
static bool
obj_isSealed(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    // Step 1.
    bool sealed = true;

    // Step 2.
    if (args.get(0).isObject()) {
        RootedObject obj(cx, &args.get(0).toObject());
        if (!TestIntegrityLevel(cx, obj, IntegrityLevel::Sealed, &sealed))
            return false;
    }
    args.rval().setBoolean(sealed);
    return true;
}

static bool
ProtoGetter(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedValue thisv(cx, args.thisv());
    if (thisv.isPrimitive()) {
        if (thisv.isNullOrUndefined()) {
            ReportIncompatible(cx, args);
            return false;
        }

        if (!BoxNonStrictThis(cx, thisv, &thisv))
            return false;
    }

    RootedObject obj(cx, &thisv.toObject());
    RootedObject proto(cx);
    if (!GetPrototype(cx, obj, &proto))
        return false;

    args.rval().setObjectOrNull(proto);
    return true;
}

namespace js {
size_t sSetProtoCalled = 0;
} // namespace js

static bool
ProtoSetter(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    HandleValue thisv = args.thisv();
    if (thisv.isNullOrUndefined()) {
        ReportIncompatible(cx, args);
        return false;
    }
    if (thisv.isPrimitive()) {
        // Mutating a boxed primitive's [[Prototype]] has no side effects.
        args.rval().setUndefined();
        return true;
    }

    if (!cx->runningWithTrustedPrincipals())
        ++sSetProtoCalled;

    Rooted<JSObject*> obj(cx, &args.thisv().toObject());

    /* Do nothing if __proto__ isn't being set to an object or null. */
    if (args.length() == 0 || !args[0].isObjectOrNull()) {
        args.rval().setUndefined();
        return true;
    }

    Rooted<JSObject*> newProto(cx, args[0].toObjectOrNull());
    if (!SetPrototype(cx, obj, newProto))
        return false;

    args.rval().setUndefined();
    return true;
}

static const JSFunctionSpec object_methods[] = {
#if JS_HAS_TOSOURCE
    JS_FN(js_toSource_str,             obj_toSource,                0,0),
#endif
    JS_INLINABLE_FN(js_toString_str,   obj_toString,                0,0, ObjectToString),
    JS_SELF_HOSTED_FN(js_toLocaleString_str, "Object_toLocaleString", 0, 0),
    JS_SELF_HOSTED_FN(js_valueOf_str,  "Object_valueOf",            0,0),
#if JS_HAS_OBJ_WATCHPOINT
    JS_FN(js_watch_str,                obj_watch,                   2,0),
    JS_FN(js_unwatch_str,              obj_unwatch,                 1,0),
#endif
    JS_SELF_HOSTED_FN(js_hasOwnProperty_str, "Object_hasOwnProperty", 1,0),
    JS_FN(js_isPrototypeOf_str,        obj_isPrototypeOf,           1,0),
    JS_FN(js_propertyIsEnumerable_str, obj_propertyIsEnumerable,    1,0),
#if JS_OLD_GETTER_SETTER_METHODS
    JS_SELF_HOSTED_FN(js_defineGetter_str, "ObjectDefineGetter",    2,0),
    JS_SELF_HOSTED_FN(js_defineSetter_str, "ObjectDefineSetter",    2,0),
    JS_SELF_HOSTED_FN(js_lookupGetter_str, "ObjectLookupGetter",    1,0),
    JS_SELF_HOSTED_FN(js_lookupSetter_str, "ObjectLookupSetter",    1,0),
#endif
    JS_FS_END
};

static const JSPropertySpec object_properties[] = {
#if JS_HAS_OBJ_PROTO_PROP
    JS_PSGS("__proto__", ProtoGetter, ProtoSetter, 0),
#endif
    JS_PS_END
};

static const JSFunctionSpec object_static_methods[] = {
    JS_FN("assign",                    obj_assign,                  2, 0),
    JS_SELF_HOSTED_FN("getPrototypeOf", "ObjectGetPrototypeOf",     1, 0),
    JS_FN("setPrototypeOf",            obj_setPrototypeOf,          2, 0),
    JS_FN("getOwnPropertyDescriptor",  obj_getOwnPropertyDescriptor,2, 0),
    JS_SELF_HOSTED_FN("getOwnPropertyDescriptors", "ObjectGetOwnPropertyDescriptors", 1, 0),
    JS_FN("keys",                      obj_keys,                    1, 0),
    JS_FN("values",                    obj_values,                  1, 0),
    JS_FN("entries",                   obj_entries,                 1, 0),
    JS_FN("is",                        obj_is,                      2, 0),
    JS_FN("defineProperty",            obj_defineProperty,          3, 0),
    JS_FN("defineProperties",          obj_defineProperties,        2, 0),
    JS_INLINABLE_FN("create",          obj_create,                  2, 0, ObjectCreate),
    JS_FN("getOwnPropertyNames",       obj_getOwnPropertyNames,     1, 0),
    JS_FN("getOwnPropertySymbols",     obj_getOwnPropertySymbols,   1, 0),
    JS_SELF_HOSTED_FN("isExtensible",  "ObjectIsExtensible",        1, 0),
    JS_FN("preventExtensions",         obj_preventExtensions,       1, 0),
    JS_FN("freeze",                    obj_freeze,                  1, 0),
    JS_FN("isFrozen",                  obj_isFrozen,                1, 0),
    JS_FN("seal",                      obj_seal,                    1, 0),
    JS_FN("isSealed",                  obj_isSealed,                1, 0),
    JS_FS_END
};

static JSObject*
CreateObjectConstructor(JSContext* cx, JSProtoKey key)
{
    Rooted<GlobalObject*> self(cx, cx->global());
    if (!GlobalObject::ensureConstructor(cx, self, JSProto_Function))
        return nullptr;

    /* Create the Object function now that we have a [[Prototype]] for it. */
    return NewNativeConstructor(cx, obj_construct, 1, HandlePropertyName(cx->names().Object),
                                gc::AllocKind::FUNCTION, SingletonObject);
}

static JSObject*
CreateObjectPrototype(JSContext* cx, JSProtoKey key)
{
    MOZ_ASSERT(!cx->runtime()->isAtomsCompartment(cx->compartment()));
    MOZ_ASSERT(cx->global()->isNative());

    /*
     * Create |Object.prototype| first, mirroring CreateBlankProto but for the
     * prototype of the created object.
     */
    RootedPlainObject objectProto(cx, NewObjectWithGivenProto<PlainObject>(cx, nullptr,
                                                                           SingletonObject));
    if (!objectProto)
        return nullptr;

    bool succeeded;
    if (!SetImmutablePrototype(cx, objectProto, &succeeded))
        return nullptr;
    MOZ_ASSERT(succeeded,
               "should have been able to make a fresh Object.prototype's "
               "[[Prototype]] immutable");

    /*
     * The default 'new' type of Object.prototype is required by type inference
     * to have unknown properties, to simplify handling of e.g. heterogenous
     * objects in JSON and script literals.
     */
    if (!JSObject::setNewGroupUnknown(cx, &PlainObject::class_, objectProto))
        return nullptr;

    return objectProto;
}

static bool
FinishObjectClassInit(JSContext* cx, JS::HandleObject ctor, JS::HandleObject proto)
{
    Rooted<GlobalObject*> global(cx, cx->global());

    /* ES5 15.1.2.1. */
    RootedId evalId(cx, NameToId(cx->names().eval));
    JSObject* evalobj = DefineFunction(cx, global, evalId, IndirectEval, 1,
                                       JSFUN_STUB_GSOPS | JSPROP_RESOLVING);
    if (!evalobj)
        return false;
    global->setOriginalEval(evalobj);

#ifdef FUZZING
    if (cx->options().fuzzing()) {
        if (!DefineTestingFunctions(cx, global, /* fuzzingSafe = */ true,
                                    /* disableOOMFunctions = */ false))
        {
            return false;
        }
    }
#endif

    Rooted<NativeObject*> holder(cx, GlobalObject::getIntrinsicsHolder(cx, global));
    if (!holder)
        return false;

    /*
     * The global object should have |Object.prototype| as its [[Prototype]].
     * Eventually we'd like to have standard classes be there from the start,
     * and thus we would know we were always setting what had previously been a
     * null [[Prototype]], but right now some code assumes it can set the
     * [[Prototype]] before standard classes have been initialized.  For now,
     * only set the [[Prototype]] if it hasn't already been set.
     */
    Rooted<TaggedProto> tagged(cx, TaggedProto(proto));
    if (global->shouldSplicePrototype()) {
        if (!JSObject::splicePrototype(cx, global, global->getClass(), tagged))
            return false;
    }
    return true;
}

static const ClassSpec PlainObjectClassSpec = {
    CreateObjectConstructor,
    CreateObjectPrototype,
    object_static_methods,
    nullptr,
    object_methods,
    object_properties,
    FinishObjectClassInit
};

const Class PlainObject::class_ = {
    js_Object_str,
    JSCLASS_HAS_CACHED_PROTO(JSProto_Object),
    JS_NULL_CLASS_OPS,
    &PlainObjectClassSpec
};

const Class* const js::ObjectClassPtr = &PlainObject::class_;
