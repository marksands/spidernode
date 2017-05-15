// Copyright 2017 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script=Adlam`
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
    [0x01E900, 0x01E94A],
    [0x01E950, 0x01E959],
    [0x01E95E, 0x01E95F]
  ]
});
testPropertyEscapes(
  /^\p{Script=Adlam}+$/u,
  matchSymbols,
  "\\p{Script=Adlam}"
);
testPropertyEscapes(
  /^\p{Script=Adlm}+$/u,
  matchSymbols,
  "\\p{Script=Adlm}"
);
testPropertyEscapes(
  /^\p{sc=Adlam}+$/u,
  matchSymbols,
  "\\p{sc=Adlam}"
);
testPropertyEscapes(
  /^\p{sc=Adlm}+$/u,
  matchSymbols,
  "\\p{sc=Adlm}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x00DBFF],
    [0x00E000, 0x01E8FF],
    [0x01E94B, 0x01E94F],
    [0x01E95A, 0x01E95D],
    [0x01E960, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script=Adlam}+$/u,
  nonMatchSymbols,
  "\\P{Script=Adlam}"
);
testPropertyEscapes(
  /^\P{Script=Adlm}+$/u,
  nonMatchSymbols,
  "\\P{Script=Adlm}"
);
testPropertyEscapes(
  /^\P{sc=Adlam}+$/u,
  nonMatchSymbols,
  "\\P{sc=Adlam}"
);
testPropertyEscapes(
  /^\P{sc=Adlm}+$/u,
  nonMatchSymbols,
  "\\P{sc=Adlm}"
);

reportCompare(0, 0);
