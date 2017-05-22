// Copyright 2017 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script=Mongolian`
info: |
  Generated by https://github.com/mathiasbynens/unicode-property-escapes-tests
  Unicode v9.0.0
  Emoji v5.0 (UTR51)
esid: sec-static-semantics-unicodematchproperty-p
features: [regexp-unicode-property-escapes]
includes: [regExpUtils.js]
---*/

const matchSymbols = buildString({
  loneCodePoints: [
    0x001804
  ],
  ranges: [
    [0x001800, 0x001801],
    [0x001806, 0x00180E],
    [0x001810, 0x001819],
    [0x001820, 0x001877],
    [0x001880, 0x0018AA],
    [0x011660, 0x01166C]
  ]
});
testPropertyEscapes(
  /^\p{Script=Mongolian}+$/u,
  matchSymbols,
  "\\p{Script=Mongolian}"
);
testPropertyEscapes(
  /^\p{Script=Mong}+$/u,
  matchSymbols,
  "\\p{Script=Mong}"
);
testPropertyEscapes(
  /^\p{sc=Mongolian}+$/u,
  matchSymbols,
  "\\p{sc=Mongolian}"
);
testPropertyEscapes(
  /^\p{sc=Mong}+$/u,
  matchSymbols,
  "\\p{sc=Mong}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [
    0x001805,
    0x00180F
  ],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x0017FF],
    [0x001802, 0x001803],
    [0x00181A, 0x00181F],
    [0x001878, 0x00187F],
    [0x0018AB, 0x00DBFF],
    [0x00E000, 0x01165F],
    [0x01166D, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script=Mongolian}+$/u,
  nonMatchSymbols,
  "\\P{Script=Mongolian}"
);
testPropertyEscapes(
  /^\P{Script=Mong}+$/u,
  nonMatchSymbols,
  "\\P{Script=Mong}"
);
testPropertyEscapes(
  /^\P{sc=Mongolian}+$/u,
  nonMatchSymbols,
  "\\P{sc=Mongolian}"
);
testPropertyEscapes(
  /^\P{sc=Mong}+$/u,
  nonMatchSymbols,
  "\\P{sc=Mong}"
);

reportCompare(0, 0);
