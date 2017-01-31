# Copyright 2013 the V8 project authors. All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of Google Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# This file is automatically generated from the V8 source and should not
# be modified manually, run 'make grokdump' instead to update this file.

# List of known V8 instance types.
INSTANCE_TYPES = {
  64: "STRING_TYPE",
  68: "ONE_BYTE_STRING_TYPE",
  65: "CONS_STRING_TYPE",
  69: "CONS_ONE_BYTE_STRING_TYPE",
  67: "SLICED_STRING_TYPE",
  71: "SLICED_ONE_BYTE_STRING_TYPE",
  66: "EXTERNAL_STRING_TYPE",
  70: "EXTERNAL_ONE_BYTE_STRING_TYPE",
  74: "EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE",
  82: "SHORT_EXTERNAL_STRING_TYPE",
  86: "SHORT_EXTERNAL_ONE_BYTE_STRING_TYPE",
  90: "SHORT_EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE",
  0: "INTERNALIZED_STRING_TYPE",
  4: "ONE_BYTE_INTERNALIZED_STRING_TYPE",
  2: "EXTERNAL_INTERNALIZED_STRING_TYPE",
  6: "EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  10: "EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE",
  18: "SHORT_EXTERNAL_INTERNALIZED_STRING_TYPE",
  22: "SHORT_EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  26: "SHORT_EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE",
  128: "SYMBOL_TYPE",
  130: "SIMD128_VALUE_TYPE",
  132: "MAP_TYPE",
  133: "CODE_TYPE",
  131: "ODDBALL_TYPE",
  169: "CELL_TYPE",
  172: "PROPERTY_CELL_TYPE",
  129: "HEAP_NUMBER_TYPE",
  134: "MUTABLE_HEAP_NUMBER_TYPE",
  135: "FOREIGN_TYPE",
  136: "BYTE_ARRAY_TYPE",
  137: "BYTECODE_ARRAY_TYPE",
  138: "FREE_SPACE_TYPE",
  139: "FIXED_INT8_ARRAY_TYPE",
  140: "FIXED_UINT8_ARRAY_TYPE",
  141: "FIXED_INT16_ARRAY_TYPE",
  142: "FIXED_UINT16_ARRAY_TYPE",
  143: "FIXED_INT32_ARRAY_TYPE",
  144: "FIXED_UINT32_ARRAY_TYPE",
  145: "FIXED_FLOAT32_ARRAY_TYPE",
  146: "FIXED_FLOAT64_ARRAY_TYPE",
  147: "FIXED_UINT8_CLAMPED_ARRAY_TYPE",
  149: "FILLER_TYPE",
  150: "ACCESSOR_INFO_TYPE",
  151: "ACCESSOR_PAIR_TYPE",
  152: "ACCESS_CHECK_INFO_TYPE",
  153: "INTERCEPTOR_INFO_TYPE",
  154: "CALL_HANDLER_INFO_TYPE",
  155: "FUNCTION_TEMPLATE_INFO_TYPE",
  156: "OBJECT_TEMPLATE_INFO_TYPE",
  157: "SIGNATURE_INFO_TYPE",
  158: "TYPE_SWITCH_INFO_TYPE",
  160: "ALLOCATION_MEMENTO_TYPE",
  159: "ALLOCATION_SITE_TYPE",
  161: "SCRIPT_TYPE",
  162: "TYPE_FEEDBACK_INFO_TYPE",
  163: "ALIASED_ARGUMENTS_ENTRY_TYPE",
  164: "BOX_TYPE",
  173: "PROTOTYPE_INFO_TYPE",
  174: "CONTEXT_EXTENSION_TYPE",
  167: "FIXED_ARRAY_TYPE",
  148: "FIXED_DOUBLE_ARRAY_TYPE",
  168: "SHARED_FUNCTION_INFO_TYPE",
  170: "WEAK_CELL_TYPE",
  171: "TRANSITION_ARRAY_TYPE",
  180: "JS_MESSAGE_OBJECT_TYPE",
  179: "JS_VALUE_TYPE",
  181: "JS_DATE_TYPE",
  183: "JS_OBJECT_TYPE",
  184: "JS_ARGUMENTS_TYPE",
  185: "JS_CONTEXT_EXTENSION_OBJECT_TYPE",
  186: "JS_GENERATOR_OBJECT_TYPE",
  187: "JS_MODULE_TYPE",
  176: "JS_GLOBAL_OBJECT_TYPE",
  177: "JS_GLOBAL_PROXY_TYPE",
  182: "JS_API_OBJECT_TYPE",
  178: "JS_SPECIAL_API_OBJECT_TYPE",
  188: "JS_ARRAY_TYPE",
  189: "JS_ARRAY_BUFFER_TYPE",
  190: "JS_TYPED_ARRAY_TYPE",
  191: "JS_DATA_VIEW_TYPE",
  175: "JS_PROXY_TYPE",
  192: "JS_SET_TYPE",
  193: "JS_MAP_TYPE",
  194: "JS_SET_ITERATOR_TYPE",
  195: "JS_MAP_ITERATOR_TYPE",
  196: "JS_WEAK_MAP_TYPE",
  197: "JS_WEAK_SET_TYPE",
  198: "JS_PROMISE_TYPE",
  199: "JS_REGEXP_TYPE",
  200: "JS_ERROR_TYPE",
  201: "JS_BOUND_FUNCTION_TYPE",
  202: "JS_FUNCTION_TYPE",
  165: "DEBUG_INFO_TYPE",
  166: "BREAK_POINT_INFO_TYPE",
}

# List of known V8 maps.
KNOWN_MAPS = {
  0x08101: (138, "FreeSpaceMap"),
  0x0812d: (132, "MetaMap"),
  0x08159: (131, "NullMap"),
  0x08185: (167, "FixedArrayMap"),
  0x081b1: (4, "OneByteInternalizedStringMap"),
  0x081dd: (149, "OnePointerFillerMap"),
  0x08209: (149, "TwoPointerFillerMap"),
  0x08235: (131, "UninitializedMap"),
  0x08261: (131, "UndefinedMap"),
  0x0828d: (129, "HeapNumberMap"),
  0x082b9: (131, "TheHoleMap"),
  0x082e5: (131, "BooleanMap"),
  0x08311: (136, "ByteArrayMap"),
  0x0833d: (167, "FixedCOWArrayMap"),
  0x08369: (167, "HashTableMap"),
  0x08395: (128, "SymbolMap"),
  0x083c1: (68, "OneByteStringMap"),
  0x083ed: (167, "ScopeInfoMap"),
  0x08419: (168, "SharedFunctionInfoMap"),
  0x08445: (133, "CodeMap"),
  0x08471: (167, "FunctionContextMap"),
  0x0849d: (169, "CellMap"),
  0x084c9: (170, "WeakCellMap"),
  0x084f5: (172, "GlobalPropertyCellMap"),
  0x08521: (135, "ForeignMap"),
  0x0854d: (171, "TransitionArrayMap"),
  0x08579: (131, "NoInterceptorResultSentinelMap"),
  0x085a5: (131, "ArgumentsMarkerMap"),
  0x085d1: (167, "NativeContextMap"),
  0x085fd: (167, "ModuleContextMap"),
  0x08629: (167, "ScriptContextMap"),
  0x08655: (167, "BlockContextMap"),
  0x08681: (167, "CatchContextMap"),
  0x086ad: (167, "WithContextMap"),
  0x086d9: (148, "FixedDoubleArrayMap"),
  0x08705: (134, "MutableHeapNumberMap"),
  0x08731: (167, "OrderedHashTableMap"),
  0x0875d: (167, "SloppyArgumentsElementsMap"),
  0x08789: (180, "JSMessageObjectMap"),
  0x087b5: (183, "NeanderMap"),
  0x087e1: (137, "BytecodeArrayMap"),
  0x0880d: (64, "StringMap"),
  0x08839: (69, "ConsOneByteStringMap"),
  0x08865: (65, "ConsStringMap"),
  0x08891: (67, "SlicedStringMap"),
  0x088bd: (71, "SlicedOneByteStringMap"),
  0x088e9: (66, "ExternalStringMap"),
  0x08915: (74, "ExternalStringWithOneByteDataMap"),
  0x08941: (70, "ExternalOneByteStringMap"),
  0x0896d: (82, "ShortExternalStringMap"),
  0x08999: (90, "ShortExternalStringWithOneByteDataMap"),
  0x089c5: (0, "InternalizedStringMap"),
  0x089f1: (2, "ExternalInternalizedStringMap"),
  0x08a1d: (10, "ExternalInternalizedStringWithOneByteDataMap"),
  0x08a49: (6, "ExternalOneByteInternalizedStringMap"),
  0x08a75: (18, "ShortExternalInternalizedStringMap"),
  0x08aa1: (26, "ShortExternalInternalizedStringWithOneByteDataMap"),
  0x08acd: (22, "ShortExternalOneByteInternalizedStringMap"),
  0x08af9: (86, "ShortExternalOneByteStringMap"),
  0x08b25: (130, "Float32x4Map"),
  0x08b51: (130, "Int32x4Map"),
  0x08b7d: (130, "Uint32x4Map"),
  0x08ba9: (130, "Bool32x4Map"),
  0x08bd5: (130, "Int16x8Map"),
  0x08c01: (130, "Uint16x8Map"),
  0x08c2d: (130, "Bool16x8Map"),
  0x08c59: (130, "Int8x16Map"),
  0x08c85: (130, "Uint8x16Map"),
  0x08cb1: (130, "Bool8x16Map"),
  0x08cdd: (131, "ExceptionMap"),
  0x08d09: (131, "TerminationExceptionMap"),
  0x08d35: (131, "OptimizedOutMap"),
  0x08d61: (131, "StaleRegisterMap"),
  0x08d8d: (167, "DebugEvaluateContextMap"),
  0x08db9: (167, "ScriptContextTableMap"),
  0x08de5: (167, "UnseededNumberDictionaryMap"),
  0x08e11: (183, "ExternalMap"),
  0x08e3d: (86, "NativeSourceStringMap"),
  0x08e69: (140, "FixedUint8ArrayMap"),
  0x08e95: (139, "FixedInt8ArrayMap"),
  0x08ec1: (142, "FixedUint16ArrayMap"),
  0x08eed: (141, "FixedInt16ArrayMap"),
  0x08f19: (144, "FixedUint32ArrayMap"),
  0x08f45: (143, "FixedInt32ArrayMap"),
  0x08f71: (145, "FixedFloat32ArrayMap"),
  0x08f9d: (146, "FixedFloat64ArrayMap"),
  0x08fc9: (147, "FixedUint8ClampedArrayMap"),
  0x08ff5: (161, "ScriptMap"),
  0x09021: (159, "AllocationSiteMap"),
  0x0904d: (160, "AllocationMementoMap"),
  0x09079: (150, "AccessorInfoMap"),
  0x090a5: (164, "BoxMap"),
  0x090d1: (151, "AccessorPairMap"),
  0x090fd: (152, "AccessCheckInfoMap"),
  0x09129: (153, "InterceptorInfoMap"),
  0x09155: (154, "CallHandlerInfoMap"),
  0x09181: (155, "FunctionTemplateInfoMap"),
  0x091ad: (156, "ObjectTemplateInfoMap"),
  0x091d9: (162, "TypeFeedbackInfoMap"),
  0x09205: (163, "AliasedArgumentsEntryMap"),
  0x09231: (165, "DebugInfoMap"),
  0x0925d: (166, "BreakPointInfoMap"),
  0x09289: (173, "PrototypeInfoMap"),
  0x092b5: (174, "ContextExtensionMap"),
}

# List of known V8 objects.
KNOWN_OBJECTS = {
  ("OLD_SPACE", 0x08101): "NullValue",
  ("OLD_SPACE", 0x0811d): "EmptyDescriptorArray",
  ("OLD_SPACE", 0x08125): "EmptyFixedArray",
  ("OLD_SPACE", 0x08151): "UninitializedValue",
  ("OLD_SPACE", 0x081a1): "UndefinedValue",
  ("OLD_SPACE", 0x081bd): "NanValue",
  ("OLD_SPACE", 0x081cd): "TheHoleValue",
  ("OLD_SPACE", 0x081f9): "TrueValue",
  ("OLD_SPACE", 0x08239): "FalseValue",
  ("OLD_SPACE", 0x08269): "empty_string",
  ("OLD_SPACE", 0x08275): "NoInterceptorResultSentinel",
  ("OLD_SPACE", 0x082bd): "ArgumentsMarker",
  ("OLD_SPACE", 0x082f5): "EmptyByteArray",
  ("OLD_SPACE", 0x082fd): "EmptyWeakCell",
  ("OLD_SPACE", 0x0830d): "InfinityValue",
  ("OLD_SPACE", 0x0831d): "MinusZeroValue",
  ("OLD_SPACE", 0x0832d): "MinusInfinityValue",
  ("OLD_SPACE", 0x09961): "EmptyLiteralsArray",
  ("OLD_SPACE", 0x0996d): "ClearedOptimizedCodeMap",
  ("OLD_SPACE", 0x09979): "Exception",
  ("OLD_SPACE", 0x099ad): "TerminationException",
  ("OLD_SPACE", 0x099ed): "OptimizedOut",
  ("OLD_SPACE", 0x09a25): "StaleRegister",
  ("OLD_SPACE", 0x09a5d): "EmptyFixedUint8Array",
  ("OLD_SPACE", 0x09a6d): "EmptyFixedInt8Array",
  ("OLD_SPACE", 0x09a7d): "EmptyFixedUint16Array",
  ("OLD_SPACE", 0x09a8d): "EmptyFixedInt16Array",
  ("OLD_SPACE", 0x09a9d): "EmptyFixedUint32Array",
  ("OLD_SPACE", 0x09aad): "EmptyFixedInt32Array",
  ("OLD_SPACE", 0x09abd): "EmptyFixedFloat32Array",
  ("OLD_SPACE", 0x09acd): "EmptyFixedFloat64Array",
  ("OLD_SPACE", 0x09add): "EmptyFixedUint8ClampedArray",
  ("OLD_SPACE", 0x09aed): "EmptyScript",
  ("OLD_SPACE", 0x09b2d): "UndefinedCell",
  ("OLD_SPACE", 0x09b35): "EmptySloppyArgumentsElements",
  ("OLD_SPACE", 0x09b45): "EmptySlowElementDictionary",
  ("OLD_SPACE", 0x09b91): "DummyVector",
  ("OLD_SPACE", 0x09c09): "EmptyPropertyCell",
  ("OLD_SPACE", 0x09c19): "ArrayProtector",
  ("OLD_SPACE", 0x09c29): "IsConcatSpreadableProtector",
  ("OLD_SPACE", 0x09c31): "HasInstanceProtector",
  ("OLD_SPACE", 0x09c41): "SpeciesProtector",
  ("OLD_SPACE", 0x09c49): "NumberStringCache",
  ("OLD_SPACE", 0x0a451): "SingleCharacterStringCache",
  ("OLD_SPACE", 0x0a909): "StringSplitCache",
  ("OLD_SPACE", 0x0ad11): "RegExpMultipleCache",
  ("OLD_SPACE", 0x0b119): "NativesSourceCache",
  ("OLD_SPACE", 0x0b2e5): "ExperimentalNativesSourceCache",
  ("OLD_SPACE", 0x0b309): "ExtraNativesSourceCache",
  ("OLD_SPACE", 0x0b325): "ExperimentalExtraNativesSourceCache",
  ("OLD_SPACE", 0x0b331): "IntrinsicFunctionNames",
  ("OLD_SPACE", 0x244bd): "EmptyPropertiesDictionary",
  ("OLD_SPACE", 0x24509): "ScriptList",
  ("OLD_SPACE", 0x3fd85): "CodeStubs",
  ("OLD_SPACE", 0x49285): "WeakObjectToCodeTable",
  ("OLD_SPACE", 0x49399): "WeakNewSpaceObjectToCodeList",
  ("OLD_SPACE", 0x493e1): "NoScriptSharedFunctionInfos",
  ("OLD_SPACE", 0x50cf9): "MessageListeners",
  ("OLD_SPACE", 0x5494d): "StringTable",
  ("CODE_SPACE", 0x184a1): "JsConstructEntryCode",
  ("CODE_SPACE", 0x23fe1): "JsEntryCode",
}
