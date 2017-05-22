// Copyright 2017 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Join_Control`
info: |
  Generated by https://github.com/mathiasbynens/unicode-property-escapes-tests
  Unicode v9.0.0
  Emoji v5.0 (UTR51)
esid: sec-static-semantics-unicodematchproperty-p
features: [regexp-unicode-property-escapes]
includes: [regExpUtils.js]
---*/

const matchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x00200C, 0x00200D]
  ]
});
testPropertyEscapes(
  /^\p{Join_Control}+$/u,
  matchSymbols,
  "\\p{Join_Control}"
);
testPropertyEscapes(
  /^\p{Join_C}+$/u,
  matchSymbols,
  "\\p{Join_C}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x00200B],
    [0x00200E, 0x00DBFF],
    [0x00E000, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Join_Control}+$/u,
  nonMatchSymbols,
  "\\P{Join_Control}"
);
testPropertyEscapes(
  /^\P{Join_C}+$/u,
  nonMatchSymbols,
  "\\P{Join_C}"
);

reportCompare(0, 0);
