// Copyright 2017 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `General_Category=Punctuation`
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
    0x00005F,
    0x00007B,
    0x00007D,
    0x0000A1,
    0x0000A7,
    0x0000AB,
    0x0000BB,
    0x0000BF,
    0x00037E,
    0x000387,
    0x0005BE,
    0x0005C0,
    0x0005C3,
    0x0005C6,
    0x00061B,
    0x0006D4,
    0x00085E,
    0x000970,
    0x000AF0,
    0x000DF4,
    0x000E4F,
    0x000F14,
    0x000F85,
    0x0010FB,
    0x001400,
    0x001CD3,
    0x002D70,
    0x003030,
    0x00303D,
    0x0030A0,
    0x0030FB,
    0x00A673,
    0x00A67E,
    0x00A8FC,
    0x00A95F,
    0x00ABEB,
    0x00FE63,
    0x00FE68,
    0x00FF3F,
    0x00FF5B,
    0x00FF5D,
    0x01039F,
    0x0103D0,
    0x01056F,
    0x010857,
    0x01091F,
    0x01093F,
    0x010A7F,
    0x0111CD,
    0x0111DB,
    0x0112A9,
    0x01145B,
    0x01145D,
    0x0114C6,
    0x016AF5,
    0x016B44,
    0x01BC9F
  ],
  ranges: [
    [0x000021, 0x000023],
    [0x000025, 0x00002A],
    [0x00002C, 0x00002F],
    [0x00003A, 0x00003B],
    [0x00003F, 0x000040],
    [0x00005B, 0x00005D],
    [0x0000B6, 0x0000B7],
    [0x00055A, 0x00055F],
    [0x000589, 0x00058A],
    [0x0005F3, 0x0005F4],
    [0x000609, 0x00060A],
    [0x00060C, 0x00060D],
    [0x00061E, 0x00061F],
    [0x00066A, 0x00066D],
    [0x000700, 0x00070D],
    [0x0007F7, 0x0007F9],
    [0x000830, 0x00083E],
    [0x000964, 0x000965],
    [0x000E5A, 0x000E5B],
    [0x000F04, 0x000F12],
    [0x000F3A, 0x000F3D],
    [0x000FD0, 0x000FD4],
    [0x000FD9, 0x000FDA],
    [0x00104A, 0x00104F],
    [0x001360, 0x001368],
    [0x00166D, 0x00166E],
    [0x00169B, 0x00169C],
    [0x0016EB, 0x0016ED],
    [0x001735, 0x001736],
    [0x0017D4, 0x0017D6],
    [0x0017D8, 0x0017DA],
    [0x001800, 0x00180A],
    [0x001944, 0x001945],
    [0x001A1E, 0x001A1F],
    [0x001AA0, 0x001AA6],
    [0x001AA8, 0x001AAD],
    [0x001B5A, 0x001B60],
    [0x001BFC, 0x001BFF],
    [0x001C3B, 0x001C3F],
    [0x001C7E, 0x001C7F],
    [0x001CC0, 0x001CC7],
    [0x002010, 0x002027],
    [0x002030, 0x002043],
    [0x002045, 0x002051],
    [0x002053, 0x00205E],
    [0x00207D, 0x00207E],
    [0x00208D, 0x00208E],
    [0x002308, 0x00230B],
    [0x002329, 0x00232A],
    [0x002768, 0x002775],
    [0x0027C5, 0x0027C6],
    [0x0027E6, 0x0027EF],
    [0x002983, 0x002998],
    [0x0029D8, 0x0029DB],
    [0x0029FC, 0x0029FD],
    [0x002CF9, 0x002CFC],
    [0x002CFE, 0x002CFF],
    [0x002E00, 0x002E2E],
    [0x002E30, 0x002E44],
    [0x003001, 0x003003],
    [0x003008, 0x003011],
    [0x003014, 0x00301F],
    [0x00A4FE, 0x00A4FF],
    [0x00A60D, 0x00A60F],
    [0x00A6F2, 0x00A6F7],
    [0x00A874, 0x00A877],
    [0x00A8CE, 0x00A8CF],
    [0x00A8F8, 0x00A8FA],
    [0x00A92E, 0x00A92F],
    [0x00A9C1, 0x00A9CD],
    [0x00A9DE, 0x00A9DF],
    [0x00AA5C, 0x00AA5F],
    [0x00AADE, 0x00AADF],
    [0x00AAF0, 0x00AAF1],
    [0x00FD3E, 0x00FD3F],
    [0x00FE10, 0x00FE19],
    [0x00FE30, 0x00FE52],
    [0x00FE54, 0x00FE61],
    [0x00FE6A, 0x00FE6B],
    [0x00FF01, 0x00FF03],
    [0x00FF05, 0x00FF0A],
    [0x00FF0C, 0x00FF0F],
    [0x00FF1A, 0x00FF1B],
    [0x00FF1F, 0x00FF20],
    [0x00FF3B, 0x00FF3D],
    [0x00FF5F, 0x00FF65],
    [0x010100, 0x010102],
    [0x010A50, 0x010A58],
    [0x010AF0, 0x010AF6],
    [0x010B39, 0x010B3F],
    [0x010B99, 0x010B9C],
    [0x011047, 0x01104D],
    [0x0110BB, 0x0110BC],
    [0x0110BE, 0x0110C1],
    [0x011140, 0x011143],
    [0x011174, 0x011175],
    [0x0111C5, 0x0111C9],
    [0x0111DD, 0x0111DF],
    [0x011238, 0x01123D],
    [0x01144B, 0x01144F],
    [0x0115C1, 0x0115D7],
    [0x011641, 0x011643],
    [0x011660, 0x01166C],
    [0x01173C, 0x01173E],
    [0x011C41, 0x011C45],
    [0x011C70, 0x011C71],
    [0x012470, 0x012474],
    [0x016A6E, 0x016A6F],
    [0x016B37, 0x016B3B],
    [0x01DA87, 0x01DA8B],
    [0x01E95E, 0x01E95F]
  ]
});
testPropertyEscapes(
  /^\p{General_Category=Punctuation}+$/u,
  matchSymbols,
  "\\p{General_Category=Punctuation}"
);
testPropertyEscapes(
  /^\p{General_Category=P}+$/u,
  matchSymbols,
  "\\p{General_Category=P}"
);
testPropertyEscapes(
  /^\p{General_Category=punct}+$/u,
  matchSymbols,
  "\\p{General_Category=punct}"
);
testPropertyEscapes(
  /^\p{gc=Punctuation}+$/u,
  matchSymbols,
  "\\p{gc=Punctuation}"
);
testPropertyEscapes(
  /^\p{gc=P}+$/u,
  matchSymbols,
  "\\p{gc=P}"
);
testPropertyEscapes(
  /^\p{gc=punct}+$/u,
  matchSymbols,
  "\\p{gc=punct}"
);
testPropertyEscapes(
  /^\p{Punctuation}+$/u,
  matchSymbols,
  "\\p{Punctuation}"
);
testPropertyEscapes(
  /^\p{P}+$/u,
  matchSymbols,
  "\\p{P}"
);
testPropertyEscapes(
  /^\p{punct}+$/u,
  matchSymbols,
  "\\p{punct}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [
    0x000024,
    0x00002B,
    0x00005E,
    0x00007C,
    0x0005BF,
    0x00060B,
    0x000F13,
    0x0017D7,
    0x001AA7,
    0x002044,
    0x002052,
    0x002CFD,
    0x002E2F,
    0x00A8FB,
    0x00FE53,
    0x00FE62,
    0x00FE69,
    0x00FF04,
    0x00FF0B,
    0x00FF3E,
    0x00FF5C,
    0x00FF5E,
    0x0110BD,
    0x0111DC,
    0x01145C
  ],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x000020],
    [0x000030, 0x000039],
    [0x00003C, 0x00003E],
    [0x000041, 0x00005A],
    [0x000060, 0x00007A],
    [0x00007E, 0x0000A0],
    [0x0000A2, 0x0000A6],
    [0x0000A8, 0x0000AA],
    [0x0000AC, 0x0000B5],
    [0x0000B8, 0x0000BA],
    [0x0000BC, 0x0000BE],
    [0x0000C0, 0x00037D],
    [0x00037F, 0x000386],
    [0x000388, 0x000559],
    [0x000560, 0x000588],
    [0x00058B, 0x0005BD],
    [0x0005C1, 0x0005C2],
    [0x0005C4, 0x0005C5],
    [0x0005C7, 0x0005F2],
    [0x0005F5, 0x000608],
    [0x00060E, 0x00061A],
    [0x00061C, 0x00061D],
    [0x000620, 0x000669],
    [0x00066E, 0x0006D3],
    [0x0006D5, 0x0006FF],
    [0x00070E, 0x0007F6],
    [0x0007FA, 0x00082F],
    [0x00083F, 0x00085D],
    [0x00085F, 0x000963],
    [0x000966, 0x00096F],
    [0x000971, 0x000AEF],
    [0x000AF1, 0x000DF3],
    [0x000DF5, 0x000E4E],
    [0x000E50, 0x000E59],
    [0x000E5C, 0x000F03],
    [0x000F15, 0x000F39],
    [0x000F3E, 0x000F84],
    [0x000F86, 0x000FCF],
    [0x000FD5, 0x000FD8],
    [0x000FDB, 0x001049],
    [0x001050, 0x0010FA],
    [0x0010FC, 0x00135F],
    [0x001369, 0x0013FF],
    [0x001401, 0x00166C],
    [0x00166F, 0x00169A],
    [0x00169D, 0x0016EA],
    [0x0016EE, 0x001734],
    [0x001737, 0x0017D3],
    [0x0017DB, 0x0017FF],
    [0x00180B, 0x001943],
    [0x001946, 0x001A1D],
    [0x001A20, 0x001A9F],
    [0x001AAE, 0x001B59],
    [0x001B61, 0x001BFB],
    [0x001C00, 0x001C3A],
    [0x001C40, 0x001C7D],
    [0x001C80, 0x001CBF],
    [0x001CC8, 0x001CD2],
    [0x001CD4, 0x00200F],
    [0x002028, 0x00202F],
    [0x00205F, 0x00207C],
    [0x00207F, 0x00208C],
    [0x00208F, 0x002307],
    [0x00230C, 0x002328],
    [0x00232B, 0x002767],
    [0x002776, 0x0027C4],
    [0x0027C7, 0x0027E5],
    [0x0027F0, 0x002982],
    [0x002999, 0x0029D7],
    [0x0029DC, 0x0029FB],
    [0x0029FE, 0x002CF8],
    [0x002D00, 0x002D6F],
    [0x002D71, 0x002DFF],
    [0x002E45, 0x003000],
    [0x003004, 0x003007],
    [0x003012, 0x003013],
    [0x003020, 0x00302F],
    [0x003031, 0x00303C],
    [0x00303E, 0x00309F],
    [0x0030A1, 0x0030FA],
    [0x0030FC, 0x00A4FD],
    [0x00A500, 0x00A60C],
    [0x00A610, 0x00A672],
    [0x00A674, 0x00A67D],
    [0x00A67F, 0x00A6F1],
    [0x00A6F8, 0x00A873],
    [0x00A878, 0x00A8CD],
    [0x00A8D0, 0x00A8F7],
    [0x00A8FD, 0x00A92D],
    [0x00A930, 0x00A95E],
    [0x00A960, 0x00A9C0],
    [0x00A9CE, 0x00A9DD],
    [0x00A9E0, 0x00AA5B],
    [0x00AA60, 0x00AADD],
    [0x00AAE0, 0x00AAEF],
    [0x00AAF2, 0x00ABEA],
    [0x00ABEC, 0x00DBFF],
    [0x00E000, 0x00FD3D],
    [0x00FD40, 0x00FE0F],
    [0x00FE1A, 0x00FE2F],
    [0x00FE64, 0x00FE67],
    [0x00FE6C, 0x00FF00],
    [0x00FF10, 0x00FF19],
    [0x00FF1C, 0x00FF1E],
    [0x00FF21, 0x00FF3A],
    [0x00FF40, 0x00FF5A],
    [0x00FF66, 0x0100FF],
    [0x010103, 0x01039E],
    [0x0103A0, 0x0103CF],
    [0x0103D1, 0x01056E],
    [0x010570, 0x010856],
    [0x010858, 0x01091E],
    [0x010920, 0x01093E],
    [0x010940, 0x010A4F],
    [0x010A59, 0x010A7E],
    [0x010A80, 0x010AEF],
    [0x010AF7, 0x010B38],
    [0x010B40, 0x010B98],
    [0x010B9D, 0x011046],
    [0x01104E, 0x0110BA],
    [0x0110C2, 0x01113F],
    [0x011144, 0x011173],
    [0x011176, 0x0111C4],
    [0x0111CA, 0x0111CC],
    [0x0111CE, 0x0111DA],
    [0x0111E0, 0x011237],
    [0x01123E, 0x0112A8],
    [0x0112AA, 0x01144A],
    [0x011450, 0x01145A],
    [0x01145E, 0x0114C5],
    [0x0114C7, 0x0115C0],
    [0x0115D8, 0x011640],
    [0x011644, 0x01165F],
    [0x01166D, 0x01173B],
    [0x01173F, 0x011C40],
    [0x011C46, 0x011C6F],
    [0x011C72, 0x01246F],
    [0x012475, 0x016A6D],
    [0x016A70, 0x016AF4],
    [0x016AF6, 0x016B36],
    [0x016B3C, 0x016B43],
    [0x016B45, 0x01BC9E],
    [0x01BCA0, 0x01DA86],
    [0x01DA8C, 0x01E95D],
    [0x01E960, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{General_Category=Punctuation}+$/u,
  nonMatchSymbols,
  "\\P{General_Category=Punctuation}"
);
testPropertyEscapes(
  /^\P{General_Category=P}+$/u,
  nonMatchSymbols,
  "\\P{General_Category=P}"
);
testPropertyEscapes(
  /^\P{General_Category=punct}+$/u,
  nonMatchSymbols,
  "\\P{General_Category=punct}"
);
testPropertyEscapes(
  /^\P{gc=Punctuation}+$/u,
  nonMatchSymbols,
  "\\P{gc=Punctuation}"
);
testPropertyEscapes(
  /^\P{gc=P}+$/u,
  nonMatchSymbols,
  "\\P{gc=P}"
);
testPropertyEscapes(
  /^\P{gc=punct}+$/u,
  nonMatchSymbols,
  "\\P{gc=punct}"
);
testPropertyEscapes(
  /^\P{Punctuation}+$/u,
  nonMatchSymbols,
  "\\P{Punctuation}"
);
testPropertyEscapes(
  /^\P{P}+$/u,
  nonMatchSymbols,
  "\\P{P}"
);
testPropertyEscapes(
  /^\P{punct}+$/u,
  nonMatchSymbols,
  "\\P{punct}"
);

reportCompare(0, 0);
