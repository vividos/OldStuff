// keycode.h - keycode definitions
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#ifndef __keycode_h_
#define __keycode_h_

// tastatur-konstanten
typedef enum keycode
{
  kbReturn = 0xd,
  kbEnter = 0xd,
  kbEsc = 0x1b,
  kbSpace = 0x20,
  kbTab = 0x9,
  kbPlus = 0x2b,
  kbMinus = 0x2d,
  kbUp = 0x4800,
  kbDown = 0x5000,
  kbLeft = 0x4b00,
  kbRight = 0x4d00,
  kbPos1 = 0x4700,
  kbEnd = 0x4f00,
  kbPageUp = 0x4900,
  kbPageDown = 0x5100,
  kbDel = 0x5300,
  kbIns = 0x5200,
  kbBackspace = 0x8,
  kbF1 = 0x3b00,
  kbF2 = 0x3c00,
  kbF3 = 0x3d00,
  kbF4 = 0x3e00,
  kbF5 = 0x3f00,
  kbF6 = 0x4000,
  kbF7 = 0x4100,
  kbF8 = 0x4200,
  kbF9 = 0x4300,
  kbF10 = 0x4400,
  kbStrgPageUp = 0x8400,
  kbStrgPageDown = 0x7600,
  kbAlt1 = 0x7800,
  kbAlt2 = 0x7900,
  kbAlt3 = 0x7a00,
  kbAlt4 = 0x7b00,
  kbAlt5 = 0x7c00,
  kbAlt6 = 0x7d00,
  kbAlt7 = 0x7e00,
  kbAlt8 = 0x7f00,
  kbAlt9 = 0x8000,
  kbAlt0 = 0x8100,
  kb0 = 0x30, kb1, kb2, kb3, kb4, kb5, kb6, kb7, kb8, kb9,
  kbA = 0x41, kbB, kbC, kbD, kbE, kbF, kbG, kbH, kbI, kbJ, kbK, kbL, kbM,
  kbN, kbO, kbP, kbQ, kbR, kbS, kbT, kbU, kbV, kbW, kbX, kbY, kbZ,
  kba = 0x61, kbb, kbc, kbd, kbe, kbf, kbg, kbh, kbi, kbj, kbk, kbl, kbm,
  kbn, kbo, kbp, kbq, kbr, kbs, kbt, kbu, kbv, kbw, kbx, kby, kbz,
  kbStrgA = 0x1, kbStrgB, kbStrgC, kbStrgD, kbStrgE, kbStrgF, kbStrgG, kbStrgH,
  kbStrgI, kbStrgJ, kbStrgK, kbStrgL, kbStrgM, kbStrgN, kbStrgO, kbStrgP, kbStrgQ,
  kbStrgR, kbStrgS, kbStrgT, kbStrgU, kbStrgV, kbStrgW, kbStrgX, kbStrgY, kbStrgZ
};

typedef enum specialKeys
{
  skShift = 0x3,
  skShiftLeft = 0x1,
  skShiftRight = 0x2,
  skCtrl = 0x4,
  skStrg = 0x4,
  skAlt = 0x8
};

typedef enum scancode
{
  scNoKey = 0,
  scEsc = 1,
  scLeft = 75,
  scRight = 77,
  scUp = 72,
  scDown = 80,
  scInsert = 82,
  scDelete = 83,
  scPos1 = 71,
  scEnd = 79,
  scPageUp = 73,
  scPageDown = 81,
  scF1 = 59, scF2, scF3, scF4, scF5, scF6, scF7, scF8, scF9, scF10,
  scF11 = 87, scF12 = 88,
  scPrint = 55,
  scScroll = 70,
  scBreak = 69,
  scHoch = 41,
  sc1 = 2, sc2, sc3, sc4, sc5, sc6, sc7, sc8, sc9, sc0, scSS, scHochkomma, scBackspace,
  scTab = 15, scQ, scW, scE, scR, scT, scZ, scU, scI, scO, scP, scUe, scPlus,
  scEnter = 28, scReturn = 28,
  scCaps = 58, scA = 30, scS, scD, scF, scG, scH, scJ, scK, scL, scOe, scAe,
  scNo = 43,
  scShiftLeft = 42,
  scKleinerGroesser = 86,
  scY = 44, scX, scC, scV, scB, scN, scM,
  scKomma, scDoppelpunkt, scStrich, scShiftRight,
  scCtrl = 29,
  scAlt = 56,
  scSpace = 57,
  scNumLock = 69,
  scNumDiv = 53,
  scNumMul = 55,
  scNum7 = 71, scNum8, scNum9, scNumMinus,
  scNum4, scNum5, scNum6, scNumPlus,
  scNum1, scNum2, scNum3, scNum0, scNumKomma
};

#endif
