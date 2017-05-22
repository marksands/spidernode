// Copyright 2017 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script=Telugu`
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
    [0x000C00, 0x000C03],
    [0x000C05, 0x000C0C],
    [0x000C0E, 0x000C10],
    [0x000C12, 0x000C28],
    [0x000C2A, 0x000C39],
    [0x000C3D, 0x000C44],
    [0x000C46, 0x000C48],
    [0x000C4A, 0x000C4D],
    [0x000C55, 0x000C56],
    [0x000C58, 0x000C5A],
    [0x000C60, 0x000C63],
    [0x000C66, 0x000C6F],
    [0x000C78, 0x000C7F]
  ]
});
testPropertyEscapes(
  /^\p{Script=Telugu}+$/u,
  matchSymbols,
  "\\p{Script=Telugu}"
);
testPropertyEscapes(
  /^\p{Script=Telu}+$/u,
  matchSymbols,
  "\\p{Script=Telu}"
);
testPropertyEscapes(
  /^\p{sc=Telugu}+$/u,
  matchSymbols,
  "\\p{sc=Telugu}"
);
testPropertyEscapes(
  /^\p{sc=Telu}+$/u,
  matchSymbols,
  "\\p{sc=Telu}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [
    0x000C04,
    0x000C0D,
    0x000C11,
    0x000C29,
    0x000C45,
    0x000C49,
    0x000C57
  ],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x000BFF],
    [0x000C3A, 0x000C3C],
    [0x000C4E, 0x000C54],
    [0x000C5B, 0x000C5F],
    [0x000C64, 0x000C65],
    [0x000C70, 0x000C77],
    [0x000C80, 0x00DBFF],
    [0x00E000, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script=Telugu}+$/u,
  nonMatchSymbols,
  "\\P{Script=Telugu}"
);
testPropertyEscapes(
  /^\P{Script=Telu}+$/u,
  nonMatchSymbols,
  "\\P{Script=Telu}"
);
testPropertyEscapes(
  /^\P{sc=Telugu}+$/u,
  nonMatchSymbols,
  "\\P{sc=Telugu}"
);
testPropertyEscapes(
  /^\P{sc=Telu}+$/u,
  nonMatchSymbols,
  "\\P{sc=Telu}"
);

reportCompare(0, 0);
