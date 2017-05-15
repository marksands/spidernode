// This file was procedurally generated from the following sources:
// - src/spread/mult-obj-null-empty-undefined.case
// - src/spread/default/array.template
/*---
description: Object Spread operator following other arguments with null, undefined and empty object (Array initializer)
esid: sec-runtime-semantics-arrayaccumulation
es6id: 12.2.5.2
features: [object-spread]
flags: [generated]
includes: [compareArray.js]
info: |
    SpreadElement : ...AssignmentExpression

    1. Let spreadRef be the result of evaluating AssignmentExpression.
    2. Let spreadObj be ? GetValue(spreadRef).
    3. Let iterator be ? GetIterator(spreadObj).
    4. Repeat
       a. Let next be ? IteratorStep(iterator).
       b. If next is false, return nextIndex.
       c. Let nextValue be ? IteratorValue(next).
       d. Let status be CreateDataProperty(array, ToString(ToUint32(nextIndex)),
          nextValue).
       e. Assert: status is true.
       f. Let nextIndex be nextIndex + 1.

    Pending Runtime Semantics: PropertyDefinitionEvaluation

    PropertyDefinition:...AssignmentExpression

    1. Let exprValue be the result of evaluating AssignmentExpression.
    2. Let fromValue be GetValue(exprValue).
    3. ReturnIfAbrupt(fromValue).
    4. Let excludedNames be a new empty List.
    5. Return CopyDataProperties(object, fromValue, excludedNames).

---*/

var callCount = 0;

(function(obj) {
  assert.sameValue(obj.a, 1);
  assert.sameValue(obj.b, 2);
  assert.sameValue(obj.c, 3);
  assert.sameValue(obj.d, 4);
  assert(compareArray(Object.keys(obj), ["a", "b", "c", "d"]));
  callCount += 1;
}.apply(null, [{a: 1, ...null, b: 2, ...undefined, c: 3, ...{}, ...{...{}}, d: 4}]));

assert.sameValue(callCount, 1);

reportCompare(0, 0);
