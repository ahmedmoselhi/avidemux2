/***************************************************************************
                          Color Effect filter ported from frei0r
    Algorithm:
        Copyright 2012 Janne Liljeblad
    Ported to Avidemux:
        Copyright 2021 szlldm
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "ADM_default.h"
#include "ADM_coreVideoFilter.h"
#include "ADM_coreVideoFilterInternal.h"
#include "ADM_vidArtColorEffect.h"
#include "artColorEffect_desc.cpp"
#include "DIA_factory.h"

extern uint8_t DIA_getArtColorEffect(artColorEffect *param, ADM_coreVideoFilter *in);

// Add the hook to make it valid plugin
//DECLARE_VIDEO_FILTER(   ADMVideoArtColorEffect,   // Class
DECLARE_VIDEO_FILTER_PARTIALIZABLE(   ADMVideoArtColorEffect,   // Class
                                      1,0,0,              // Version
                                      ADM_UI_TYPE_BUILD,         // UI
                                      VF_ART,            // Category
                                      "artColorEffect",            // internal name (must be uniq!)
                                      QT_TRANSLATE_NOOP("artColorEffect","Color Effect"),            // Display name
                                      QT_TRANSLATE_NOOP("artColorEffect","Ported from frei0r.") // Description
                                  );

/*
 * Tables were produced roughly this way:
 *  - take a sample image
 *  - open with the gimp and play with the Curves tool until you find the desired effect
 *  - save the curve
 *  - create a new 256x1 rgb image and paint a black to white gradient on it
 *  - apply the curve
 *  - save as "C source"
 *  - the array in the saved file is the lookup table
 */
static const uint8_t sepia_table[769] =
    "\0\0\0\0\0\0\0\0\0\0\1\0\1\1\0\1\1\0\1\1\1\2\1\1\2\2\1\3\2\1\3\2\1\3\2\1"
    "\4\3\2\4\3\2\4\3\2\6\4\2\6\4\2\6\4\2\7\5\2\7\5\3\11\6\3\11\6\3\12\7\3\13"
    "\10\3\15\10\4\16\11\4\17\11\4\21\12\4\22\13\4\22\13\5\23\14\5\24\15\5\26"
    "\16\6\31\20\6\31\21\6\32\22\7\34\22\7\35\23\7\40\24\10\40\26\10!\26\11#\30"
    "\11&\31\12&\32\12'\34\13)\34\13*\37\13,\37\13-\40\14.\"\15" "0\"\15"
    "2#\17" "" "3&\17" "4&\17" "5'\20" "8(\21"
    "9)\21:*\23<,\23=-\23A.\24A0\25B0\25C2\26D3"
    "\30H4\30H7\31K7\32K8\32L9\33M:\34P<\34Q=\35S>\37T?\37UA\40VB!XC!ZD#\\F#^"
    "G#^J$`J&bK'bM'eM(fO)gP)iQ*kS,mT-mU-nV.oX/rY0sZ2u]2v]3w^3x`4za5{c7|c8~e8\177"
    "f9\200i:\203i<\204j<\206k=\207m>\210n?\211o?\213qA\214rC\215sC\217uD\220"
    "vD\221wF\223xG\224zH\225{J\227|K\230~K\231\177L\232\200M\234\202O\235\203"
    "P\236\204Q\240\206Q\241\207S\242\210T\243\211U\245\213V\246\214X\247\215"
    "Y\250\217Y\252\220Z\253\221\\\254\223]\254\224^\255\225`\257\227a\260\230"
    "b\261\231c\262\232e\264\234e\265\235f\266\236g\267\240i\267\241i\272\242"
    "k\273\243m\274\245n\274\246o\276\247q\277\250r\300\252s\301\253u\302\254"
    "v\304\255w\305\257x\306\257z\306\261{\307\262|\310\264~\310\265\177\313\266"
    "\200\314\267\202\315\267\203\316\272\204\317\273\206\317\274\207\320\276"
    "\210\322\277\211\323\277\213\324\301\214\325\302\215\326\304\217\326\305"
    "\220\327\306\221\327\307\223\331\310\224\333\311\225\334\311\227\334\313"
    "\227\335\315\231\335\316\231\337\317\234\340\320\235\341\320\235\341\323"
    "\240\342\324\241\343\324\242\343\326\243\345\327\245\345\330\245\346\331"
    "\250\346\333\252\347\334\253\351\335\254\351\335\255\351\337\257\352\340"
    "\260\353\341\260\354\342\262\355\343\264\355\344\265\355\345\266\356\346"
    "\266\356\347\272\357\350\273\360\351\274\360\351\276\361\352\277\361\353"
    "\300\362\353\301\362\354\302\362\355\304\362\356\305\364\357\305\364\357"
    "\310\364\360\311\365\361\313\365\361\314\366\362\315\366\362\316\366\363"
    "\316\367\364\320\367\364\320\367\365\324\367\365\324\370\366\326\370\366"
    "\327\371\366\330\371\367\331\371\367\333\371\370\333\372\370\336\372\370"
    "\336\372\371\340\373\371\341\373\372\342\373\372\343\374\372\344\374\373"
    "\344\374\373\347\374\374\350\375\374\351\375\374\351\375\374\352\375\375"
    "\352\376\375\353\376\376\355\376\376\356\376\376\357\377\377\357";

static const uint8_t heat_table[769] =
    "\0\0\0\0\0\0\0\1\0\0\1\0\0\1\1\0\2\1\0\2\1\1\2\1\1\2\2\1\2\2\1\3\2\1\3\3"
    "\1\3\3\1\4\3\1\4\4\1\5\4\1\5\5\2\5\6\2\6\6\2\6\7\2\6\7\2\7\7\2\7\11\2\10"
    "\11\2\10\12\3\11\13\3\11\13\3\11\14\3\12\15\3\12\17\3\13\17\3\14\20\3\14"
    "\22\4\15\23\4\16\24\4\16\26\4\16\27\4\17\31\4\20\34\4\21\34\5\21\40\5\22"
    "\40\5\22$\5\23$\5\25&\6\25(\6\26-\6\26-\6\27" "0\6\31" "2\7\31"
    "5\7\32;\7\34"
    ";\7\34?\10\35C\10\36G\10\37L\10\40V\11!V\11\"[\11$a\11&l\12&l\12'r\12(~\13"
    "*~\13,\204\14,\213\14.\221\14/\227\14" "1\236\15" "2\244\15" "4\252\15"
    "5\260" "\16" "7\267\16"
    "8\275\17:\302\17;\310\17=\323\20?\323\21@\330\21D\335\21D"
    "\342\22E\346\22I\353\23I\356\23K\362\24M\365\24N\370\25P\372\26R\374\26T"
    "\376\26V\377\27X\377\27Z\377\30\\\376\31`\376\31`\375\32b\373\32d\371\33"
    "f\366\34j\363\34j\360\35l\354\36n\350\36r\344\37r\337\40t\333\40w\326!y\321"
    "\"|\314#~\307$\201\301$\204\267%\207\267&\212\261'\214\254(\217\247(\222"
    "\241)\226\234*\231\227+\234\222,\237\216-\242\211.\245\205/\251\2010\254"
    "}1\257z2\262w3\266t4\271p5\274m6\277j7\302f8\305c9\310`:\314\\;\317Y<\321"
    "V>\324S?\327P@\332LA\335IB\337FC\342CE\344@F\347=G\351;I\3538I\3558M\357"
    "3P\3610S\363.V\365+Y\366)\\\370'`\371%d\372#g\373\"l\374\40p\374\37t\374"
    "\35t\375\34}\376\33\202\376\32\202\375\31\213\375\30\220\375\27\225\375\27"
    "\232\373\26\237\372\25\244\371\24\251\370\23\256\367\23\262\367\22\267\364"
    "\21\274\362\20\300\361\20\305\357\17\311\355\16\311\353\16\322\351\15\326"
    "\346\15\332\346\14\336\344\14\341\337\13\341\335\13\350\332\12\353\330\11"
    "\356\330\11\360\322\10\362\320\10\364\320\10\364\312\7\366\307\7\366\304"
    "\7\367\302\6\367\277\6\370\274\5\367\271\5\367\271\5\367\263\4\365\260\4"
    "\364\255\4\363\253\3\362\250\3\361\245\3\360\242\3\357\240\3\357\235\2\355"
    "\232\2\355\227\2\354\225\2\353\221\1\353\216\1\353\216\1\353\213\1\353\204"
    "\1\353\201\1\354}\1\354y\0\354v\0\355r\0\355n\0\355j\0\356f\0\356b\0\357"
    "_\0\357[\0\357W\0\357S\0\360O\0\360O\0\361K\0\361C\0\362@\0\363<\0\3638\0"
    "\3648\0\3641\0\365.\0\366+\0\366'\0\367'\0\370!\0\370\36\0\370\33\0\371\30"
    "\0\371\26\0\373\26\0\373\23\0\374\15\0\374\13\0\375\10\0\375\5\0\376\3\0";

static const uint8_t red_green_table[769] =
    "\13\0\2\17\0\2\23\0\3\27\0\3\33\1\4\37\1\5\"\1\5&\1\5&\1\6-\1\7""1\2\7""4"
    "\2\10""8\2\10;\2\11>\2\12A\3\12D\3\13G\3\13G\3\14L\4\14O\4\14Q\4\16Q\4\16"
    "V\5\17V\5\17Y\5\17[\5\20]\6\21^\6\22_\6\22`\7\22`\7\23a\7\23b\10\24b\10\24"
    "b\10\25c\11\26c\11\26d\11\27e\12\27e\12\30e\13\31e\13\31f\14\32f\15\32g\15"
    "\34g\15\34g\16\34g\17\35g\17\35h\20\36h\20\37h\21\37h\22!i\22!i\23!i\24\""
    "i\25\"i\25#i\26$j\27$j\30%j\31&j\31&j\32'j\33(j\34(j\35)j\36)j\37*j\40+j"
    "!+j\",j#,j$-j&.j'/j(/j)0j)0j,1i-2i-2i/4i14i15i45i47i77i97h;8h=9h?:hA:hC<"
    "hE<gH<gJ=gJ>gO?gQ?fQ@fVAfXBfXBf[Ce^DecFefFefFekGdnHdqIdqJdsKdvKc|Lc|Mc\177"
    "Nc\202Oc\204Pb\212Pb\215Qb\215Ra\220Sa\223Ta\225Ua\230Va\233Va\236W`\240"
    "W`\243X`\246Z`\250[_\253[_\256\\_\260]_\263__\265_^\270a^\272b^\274c^\277"
    "d^\301d^\303e^\305e]\307f]\311g]\313i]\315i]\317k]\321l]\322m]\324n]\325"
    "o\\\327p\\\330q\\\331r\\\333s\\\333t\\\335u\\\336x\\\337x\\\340y\\\341z\\"
    "\342{\\\343|\\\343}\\\345\177\\\345\200\\\347\201\\\347\202\\\350\204\\\352"
    "\205]\353\206]\354\207]\354\211]\355\212]\356\212]\356\215]\357\216^\357"
    "\217^\361\221^\361\222^\362\223^\363\223_\363\225_\363\227_\364\231_\365"
    "\232`\365\232`\365\235a\366\236a\366\240a\367\241a\367\243b\370\243b\370"
    "\244b\371\246c\371\250c\372\252d\372\253d\372\253e\373\255e\373\256f\373"
    "\261f\373\261g\374\263h\374\264h\374\266i\375\267j\375\271j\375\273k\375"
    "\274k\375\276l\376\301m\376\301o\376\302p\376\304r\376\305s\376\307u\377"
    "\311x\377\312z\377\314|\377\315\177\377\317\202\377\320\202\377\322\205\377"
    "\324\213\377\325\216\377\327\222\377\330\225\377\332\231\377\334\235\377"
    "\335\241\377\337\241\377\337\251\377\342\251\377\344\262\377\345\266\377"
    "\347\272\377\351\277\377\352\304\377\354\310\377\356\315\377\356\322\377"
    "\361\327\377\362\334\377\364\341\377\366\341\377\367\352\377\367\357\377"
    "\371\364\377\374\364\377\376\371\377\377";

static const uint8_t old_photo_table[769] =
    "&\3@'\4@(\5A)\6A*\7A*\10A+\11A,\12A,\13B.\13B/\15B0\16B0\17B1\20C2\21C3\22"
    "C4\23C5\24C5\25C6\26D7\27D8\30D8\31D:\32D:\33E<\34E<\35E=\36E>\37E?\40F@"
    "!F@!FA\"FB$FC$FC%GE&GE'GF(GG*GG*HH+HI,HK-HK/HM0IM0IM1IO2IO3JQ4JQ5JR6JR8K"
    "S8KT9KU:KW;KW<LW=LY>LY?LZ@M[AM[BM\\CM]DN^EN_FN`GO`HOaIObJOcKOdLPdMPeNPfO"
    "PgPQhPQhRQiSRjSRjUSkVSlVSmXSnXToZTo[Tp\\Up]Uq^Vr_Vs`VsaVubWucWvcWveXwfXy"
    "fXyhYyiYziZ{jZ|kZ}m[}n[~n\\\177p\\\200q\\\201r]\201r]\202s^\202t^\203v_\203"
    "v_\204w_\205x`\205y`\206{a\207|a\207|b\211}b\212~c\212\177c\212\200d\213"
    "\201d\214\202d\214\203d\215\204e\216\205f\216\206g\217\207g\220\210g\221"
    "\211h\221\212i\222\213i\223\214j\223\215j\224\216k\225\217k\225\220k\226"
    "\221l\227\222m\227\223m\230\224n\231\225o\231\226o\232\227o\232\230p\233"
    "\231q\234\232q\235\233r\235\234s\236\235s\237\236t\237\237t\240\237u\241"
    "\241v\241\242w\242\243w\242\244x\243\245x\244\246y\244\247z\244\247z\246"
    "\251{\246\251|\247\253|\250\253}\250\254~\251\256~\252\257\177\252\260\200"
    "\253\260\200\253\262\201\254\263\201\255\263\203\255\265\203\256\265\204"
    "\257\267\205\257\267\205\260\271\206\260\272\206\261\273\207\262\273\210"
    "\262\275\211\262\276\212\264\277\212\264\277\213\265\300\214\265\301\215"
    "\266\303\216\266\303\216\267\304\216\267\305\217\270\306\220\271\307\221"
    "\271\311\222\272\311\223\272\312\223\273\313\224\274\314\225\274\315\226"
    "\275\316\226\275\317\227\276\320\230\277\321\230\277\322\231\300\323\232"
    "\301\324\233\301\325\234\302\326\234\302\327\236\303\331\236\304\331\237"
    "\304\332\240\305\333\240\305\334\241\306\335\242\307\336\243\307\337\244"
    "\310\340\244\310\341\245\311\341\246\311\343\247\312\344\250\313\345\251"
    "\313\346\251\314\347\252\315\347\253\315\351\254\316\352\255\316\353\256"
    "\316\353\256\320\354\257\320\356\260\321\356\261\321\360\262\322\360\262"
    "\323\362\263\323\362\264\324\363\265\324\365\265\325\366\267\326\367\267"
    "\326\370\270\327\371\271\327\371\272\330\372\272\330\374\273\331\374\275"
    "\331\375\275\332\377\276";

static const uint8_t xray_table[769] =
    "\377\377\377\377\377\377\376\376\376\375\375\376\374\375\375\373\374\375"
    "\372\374\374\371\374\374\370\373\373\366\373\372\366\372\372\365\372\371"
    "\363\371\371\363\371\370\362\370\370\360\370\367\360\367\366\357\367\365"
    "\356\366\365\355\366\364\353\365\363\353\365\363\352\364\362\351\363\362"
    "\347\363\361\346\362\361\345\362\361\344\362\360\343\361\357\343\361\356"
    "\342\360\356\341\360\356\340\357\355\336\356\354\336\356\354\335\355\353"
    "\334\355\353\333\355\352\331\354\351\331\353\351\330\353\350\327\353\350"
    "\325\352\347\325\351\347\324\350\346\323\350\345\322\347\344\321\347\344"
    "\320\347\344\317\346\343\316\346\342\315\345\341\314\344\341\313\344\340"
    "\312\344\340\311\343\337\310\342\337\307\342\335\306\341\335\305\341\335"
    "\303\340\334\303\337\333\302\337\333\301\337\332\300\336\331\276\335\331"
    "\276\334\330\274\334\330\274\334\327\273\333\327\272\333\326\271\332\325"
    "\270\332\325\267\331\324\266\330\323\265\330\323\264\327\322\263\327\321"
    "\262\326\320\261\325\320\257\325\317\257\324\317\256\324\316\254\323\315"
    "\254\322\315\253\322\314\252\321\313\251\321\313\250\320\312\246\317\311"
    "\245\317\311\245\316\310\244\316\307\243\315\307\242\314\306\241\314\305"
    "\240\312\305\237\312\304\236\312\303\235\311\303\234\311\302\233\307\301"
    "\232\307\300\231\307\300\230\306\277\227\305\276\226\305\276\225\304\275"
    "\224\303\274\223\303\273\222\302\273\221\301\272\220\301\271\217\300\270"
    "\216\277\270\215\277\267\214\276\266\213\275\265\212\275\265\211\274\264"
    "\210\273\263\207\273\262\206\272\262\205\271\261\204\270\260\203\270\257"
    "\202\267\257\201\266\256\200\266\255\177\265\254~\264\253}\263\253|\263\252"
    "{\262\251z\261\250y\260\247x\260\247w\257\246v\256\245u\255\244t\255\243"
    "s\254\243r\253\242q\252\241p\252\240o\251\237n\250\236m\247\235l\246\235"
    "l\246\235j\245\233i\244\232h\243\231g\242\230f\242\227e\241\226d\240\226"
    "c\237\225b\236\224a\235\223`\234\222_\234\221_\233\220]\232\217\\\231\216"
    "\\\230\215Z\227\214Y\226\214X\226\213W\225\212V\224\211U\223\210T\222\207"
    "S\221\206R\221\205Q\217\204P\216\203O\215\202N\215\201M\214\200M\213\177"
    "K\212~J\211}I\211|H\210|G\206zG\205zE\204xD\203vC\203vB\201tA\200s@\200q"
    "@~p>}o>|o<{l<yk;xi9wh8wg8te6sd5qd4pa3n_2m]1k\\0j\\0hY.fW-dU,cT+aR*_P)_O("
    "]M'YK'XI%VI$TF$RD\"OB!M@\40K?\37I=\37G=\35E9\34C9\34A5\33>5\31<2\31<0\27"
    ":.\27" "5,\26" "3*\24"
    "1*\23.&\22.&\22*\"\21'\40\17%\36\16\"\34\15\"\32\14"
    "\36\32\13\33\26\13\31\24\11\26\22\11\24\20\7\24\16\6\21\16\5\14\14\4\12\10"
    "\3\7\6\3\5\4\1\2\2";

static const uint8_t xpro_table[769] =
    "\0\0\37\0\0\37\0\1\40\0\2!\0\2\"\0\3\"\1\4%\1\4%\1\5%\1\5'\1\7'\1\7(\1\7"
    "(\1\10*\1\11+\1\11,\1\12,\1\13/\1\14/\1\14" "1\2\15" "1\2\15" "1\2\16"
    "4\2\17" "" "4\3\17" "5\3\22" "7\3\22" "7\3\23" "8\3\24"
    "9\3\25;\3\26;\3\27<\3\27=\4\31"
    "=\4\33?\4\34@\5\34B\5\35C\5\36D\5\40D\5\40G\5!G\6\"H\6$H\7&J\7&K\7*M\7*M"
    "\10+N\10-P\11-P\11/R\11" "3R\11" "3T\12" "4U\12" "5U\13" "7W\14" "8Y\14"
    "9Y\14"
    "<Y\16=[\16@^\16@^\17C^\17D`\20F`\20Jb\22Jb\22Kc\23Me\24Nf\25Qg\26Rg\27Ti"
    "\27Wj\30Xl\31Yl\33\\m\34^p\35`p\40bp\40fq!fr$gt$lt%lu'mv(px*qy-ty/uz/x|0"
    "y}3|}4}~5\177\2018\203\2019\203\201;\204\202=\207\203?\210\204@\214\204C"
    "\214\206D\216\207G\217\210H\223\211K\223\211M\225\212P\226\214Q\231\215T"
    "\232\215U\234\216X\235\217Y\240\220\\\241\220^\243\221`\244\223b\246\224"
    "e\250\224f\252\225i\253\226l\255\227m\256\231p\261\231q\262\232t\264\233"
    "v\265\234x\267\234z\270\235|\271\236~\274\240\201\275\240\202\277\241\204"
    "\300\242\207\302\243\210\303\243\212\305\244\214\306\245\216\307\246\220"
    "\311\250\221\313\250\224\315\251\226\316\252\227\317\253\232\321\253\234"
    "\322\254\235\323\255\240\325\256\242\326\256\242\330\256\245\331\261\250"
    "\331\262\251\332\262\253\334\263\255\335\264\256\336\265\261\340\266\263"
    "\341\266\264\342\267\266\343\270\270\344\271\271\344\271\271\346\273\276"
    "\347\274\277\350\275\277\351\275\302\352\276\304\353\277\306\353\300\307"
    "\355\300\311\356\301\314\356\302\315\357\303\317\360\304\320\360\304\322"
    "\361\305\323\362\306\325\362\307\327\363\307\330\363\310\330\364\311\333"
    "\364\313\334\365\313\336\365\314\340\365\314\342\366\316\342\366\316\346"
    "\367\317\347\367\320\351\367\320\353\370\322\354\370\322\356\370\323\356"
    "\370\324\360\371\325\360\371\325\363\371\326\363\371\327\363\372\330\365"
    "\372\330\366\372\331\366\372\331\370\372\332\371\373\332\371\373\333\372"
    "\373\334\373\373\335\373\373\336\374\373\336\374\374\337\374\374\340\375"
    "\374\341\375\374\341\376\374\342\376\374\343\376\374\344\376\374\344\377"
    "\374\345\377\374\346\377\375\346\377\375\346\377\375\347\377\375\350\377"
    "\375\351\377\375\352\377\375\352\377\375\352\377\375\353\377\375\353\377"
    "\376\354\377\376\354\377\376\356\377\376\356\377\376\356\377\376\357\377"
    "\376\360\377\376\360\377\376\360\377\376\360\377\376\362\377\376\362\377"
    "\376\363\377\376\363\377\376\363\377\376\363\377\376\364\377\376\364\377"
    "\376\365\377\377\365\377\377\366\377\377\366\377\377\366\377\377\367\377"
    "\377\367\377\377\367\377\377\370";

/*Used for a video magnifer emulator in gnome-video-effects*/
static const uint8_t yellowblue_table[769] =
    "\0\0\377\1\1\376\2\2\375\3\3\374\4\4\373\5\5\372\6\6\371\7\7\370\10\10\367"
    "\11\11\367\12\12\365\13\13\364\14\14\363\15\14\362\16\16\361\17\17\360\20"
    "\20\357\20\21\356\22\22\355\23\23\354\24\24\354\24\25\352\26\26\351\27\27"
    "\350\27\30\347\31\31\346\32\32\345\33\32\344\34\34\343\34\34\342\36\36\341"
    "\37\36\340\40\40\337!!\336!!\335##\334$#\334%%\332&%\331'&\330((\327()\326"
    "*)\325++\324,,\323--\322..\321//\320/0\31711\31722\31522\31444\31445\313"
    "55\31276\31188\30799\3069:\305;;\305<<\304==\302>>\301>>\300@@\300@A\276"
    "AB\275BC\274CD\273DE\272EE\272FF\270HH\270HI\266IJ\265KK\264KL\263MM\262"
    "NN\262NN\261OO\257QP\256RQ\256RR\254TT\253UU\253VU\251VW\250XX\247XY\246"
    "YZ\245[[\245[[\243]]\243^^\242^_\240_`\237`a\236aa\235bb\235dc\233de\233"
    "ff\232gf\231hg\230hi\227ji\226kj\225lk\223lm\223nm\222nn\221op\217qq\216"
    "rr\215ss\214st\213uu\213uu\211wv\210ww\207xx\207yz\205z{\205{{\204||\203"
    "}}\202\177~\201\177\200\177\200\201\177\202\202~\203\202|\204\203|\204\204"
    "{\205\206z\207\206x\207\207w\211\210w\211\211v\212\212u\213\214s\214\214"
    "r\215\215r\216\217q\217\217p\221\220o\221\222n\223\222l\224\223k\224\224"
    "k\225\225j\226\226i\227\227h\230\231f\231\231f\233\232e\233\233c\234\234"
    "c\235\235b\236\236a\237\237`\241\240_\242\241^\242\242]\243\244\\\244\244"
    "[\245\245Y\246\246Y\250\247X\250\250W\251\251V\252\252T\253\253T\254\255"
    "S\256\255R\257\256Q\257\260P\260\261O\261\261N\262\262M\263\263L\264\265"
    "K\265\265J\266\266I\267\270H\270\270G\271\271F\272\272E\273\273C\274\274"
    "B\275\275B\276\276A\277\277@\300\300?\301\301>\302\302=\303\303<\304\304"
    ";\305\305:\306\3069\307\3078\310\3107\311\3116\312\3125\313\3134\314\314"
    "3\315\3152\316\3161\317\3170\320\320/\321\321.\322\322-\323\323,\323\324"
    "+\325\325*\326\326)\327\327(\330\330'\331\331&\332\331%\333\332$\334\334"
    "#\334\335\"\336\336!\337\337\40\340\340\37\341\341\36\342\342\35\343\343"
    "\34\344\344\33\345\345\32\345\346\31\347\347\30\350\350\27\351\351\26\352"
    "\352\25\353\353\24\354\354\23\354\355\22\356\356\21\357\357\20\360\360\17"
    "\361\361\16\362\362\15\363\362\14\364\364\13\365\365\12\365\366\11\367\367"
    "\11\370\370\7\371\371\6\372\371\5\373\373\4\374\374\4\375\375\3\375\376\1";

static const uint8_t esses_table[769] =
    "\252\252\245\245\245\240\240\240\233\233\233\226\226\226\220\220\220\213"
    "\213\213\213\206\206\206\206\202}}\202xxxsssnnnjjjeeeaaaa\\\\XXXTXTPPPLL"
    "LLHHDHDAAA=A=::::66333000---++++++(((&$$\"\"$\"\"\"\40\36\40\36\36\36\34"
    "\34\34\32\32\32\32\32\32\31\31\31\30\30\30\30\27\30\27\27\27\27\27\27\30"
    "\27\27\30\30\30\30\31\30\31\31\31\31\31\31\31\31\31\32\32\32\33\33\33\34"
    "\33\33\34\34\34\35\35\35\35\36\35\36\37\36\37\37\37\40!\40!!!\"\"\"###$$"
    "$%%%&&&'''((()))***,,,---....001112224444557757888:::;:;;=????@?BBBBDBEE"
    "EGGGGIIIIKKKLNNNNPPRPRRTTTVVVWVWWYYY[[][]]___aaaacecegegiikikkkkmoooqqqs"
    "ssusuwuwwwy{{{{{}}}\177\177\177\201\201\201\203\203\205\205\205\210\210\210"
    "\210\212\214\212\214\216\214\216\220\216\220\220\220\222\222\222\222\224"
    "\224\224\226\226\226\230\230\230\232\232\234\234\234\236\236\236\236\240"
    "\240\240\242\242\242\244\244\246\246\246\246\252\250\252\252\252\254\254"
    "\254\256\256\256\260\260\260\260\261\261\261\263\263\263\265\265\265\267"
    "\267\267\271\271\273\273\273\273\274\274\274\274\276\276\300\300\300\302"
    "\302\302\303\303\303\305\305\305\307\307\307\311\311\311\312\312\312\314"
    "\312\314\315\315\315\317\317\320\320\320\320\322\322\322\323\323\323\325"
    "\325\325\326\326\326\326\326\330\331\331\331\332\331\332\334\334\334\335"
    "\334\335\336\335\336\337\337\337\341\341\341\341\342\342\343\342\343\344"
    "\344\344\345\345\344\346\345\346\347\347\347\350\347\350\351\351\351\351"
    "\351\352\352\352\352\352\353\352\354\354\353\355\354\355\355\355\355\355"
    "\355\355\356\356\355\356\356\356\356\356\356\356\356\355\354\354\354\354"
    "\354\353\353\353\353\351\351\351\347\347\347\345\345\342\342\340\340\340"
    "\340\335\335\335\335\332\332\332\327\327\327\323\323\320\320\320\320\314"
    "\314\314\310\310\310\305\305\305\301\301\301\275\275\275\271\271\271\265"
    "\265\265\261\261\261\255\255\251\251\245\251\245\245\245\241\241\241\235"
    "\235\235\231\231\231\226\226\226\222\222\222\217\217\217\214\214\214\211"
    "\211\211\206\211\206\206\202\202\202\200\200~~~|||yyywywuuusssqqqqqqnonn"
    "lljljhhhghgeeececbcb``b___]]]\\\\\\ZZZZZYWYYVVWTVTTTSSQQ";

/**
    \fn ArtColorEffectCreateBuffers
*/
void ADMVideoArtColorEffect::ArtColorEffectCreateBuffers(int w, int h, int * rgbBufStride, ADM_byteBuffer ** rgbBufRaw, ADMImageRef ** rgbBufImage, ADMColorScalerFull ** convertYuvToRgb, ADMColorScalerFull ** convertRgbToYuv)
{
    *rgbBufStride = ADM_IMAGE_ALIGN(w * 4);
    *rgbBufRaw = new ADM_byteBuffer();
    if (*rgbBufRaw)
        (*rgbBufRaw)->setSize(*rgbBufStride * h);
    *convertYuvToRgb = new ADMColorScalerFull(ADM_CS_BICUBIC,w,h,w,h,ADM_PIXFRMT_YV12,ADM_PIXFRMT_RGB32A);
    *convertRgbToYuv = new ADMColorScalerFull(ADM_CS_BICUBIC,w,h,w,h,ADM_PIXFRMT_RGB32A,ADM_PIXFRMT_YV12);
    *rgbBufImage = new ADMImageRef(w,h);
    if (*rgbBufImage)
    {
        (*rgbBufImage)->_pixfrmt = ADM_PIXFRMT_RGB32A;
        (*rgbBufImage)->_planes[0] = (*rgbBufRaw)->at(0);
        (*rgbBufImage)->_planes[1] = (*rgbBufImage)->_planes[2] = NULL;
        (*rgbBufImage)->_planeStride[0] = *rgbBufStride;
        (*rgbBufImage)->_planeStride[1] = (*rgbBufImage)->_planeStride[2] = 0;
    }
}
/**
    \fn ArtColorEffectDestroyBuffers
*/
void ADMVideoArtColorEffect::ArtColorEffectDestroyBuffers(ADM_byteBuffer * rgbBufRaw, ADMImageRef * rgbBufImage, ADMColorScalerFull * convertYuvToRgb, ADMColorScalerFull * convertRgbToYuv)
{
    if (convertYuvToRgb) delete convertYuvToRgb;
    if (convertRgbToYuv) delete convertRgbToYuv;
    if (rgbBufRaw) rgbBufRaw->clean();
    if (rgbBufImage) delete rgbBufImage;
    if (rgbBufRaw) delete rgbBufRaw;
}
/**
    \fn ArtColorEffectProcess_C
*/
void ADMVideoArtColorEffect::ArtColorEffectProcess_C(ADMImage *img, int w, int h, int effect, int rgbBufStride, ADM_byteBuffer * rgbBufRaw, ADMImageRef * rgbBufImage, ADMColorScalerFull * convertYuvToRgb, ADMColorScalerFull * convertRgbToYuv)
{
    if (!img || !rgbBufRaw || !rgbBufImage || !convertYuvToRgb || !convertRgbToYuv) return;
    uint8_t * line;
    const uint8_t * table;
    bool bw;

    bw = ((effect % 2) == 0);
    switch(effect / 2) {
        default:
        case 0: table = sepia_table;
            break;
        case 1: table = heat_table;
            break;
        case 2: table = red_green_table;
            break;
        case 3: table = old_photo_table;
            break;
        case 4: table = xray_table;
            break;
        case 5: table = xpro_table;
            break;
        case 6: table = yellowblue_table;
            break;
        case 7: table = esses_table;
            break;
    }

    if (bw) {
        int      pitches[3];
        uint8_t *ptr[3];
        img->GetPitches(pitches);
        img->GetWritePlanes((uint8_t **)ptr);
        for(int i=1;i<3;i++)
        {
            uint8_t *p=ptr[i];
            uint32_t d=pitches[i];
            for(int y=0;y<(h/2);y++)
            {
                memset(p,128,(w/2));
                p+=d;
            }
        }
    }

    convertYuvToRgb->convertImage(img,rgbBufRaw->at(0));

    for(int y=0; y<h; y++) {
        line = rgbBufRaw->at(y*rgbBufStride);
        for(int x=0; x<w; x++) {
            line[x*4 + 0] = table[line[x*4 + 0]*3 + 0];
            line[x*4 + 1] = table[line[x*4 + 1]*3 + 1];
            line[x*4 + 2] = table[line[x*4 + 2]*3 + 2];
        }
    }

    convertRgbToYuv->convertImage(rgbBufImage,img);

}

/**
    \fn configure
*/
bool ADMVideoArtColorEffect::configure()
{
    if(DIA_getArtColorEffect(&_param, previousFilter))
    {
        update();
        return true;
    }
    return false;
}
/**
    \fn getConfiguration
*/

const char   *ADMVideoArtColorEffect::getConfiguration(void)
{
    static char s[256];
    const char * effect_name;
    switch(_param.effect) {
        case 0: effect_name = "Sepia";
            break;
        case 1: effect_name = "Vivid";
            break;
        case 2: effect_name = "RGB";
            break;
        case 3: effect_name = "Heat";
            break;
        case 4: effect_name = "Radium";
            break;
        case 5: effect_name = "Red-green";
            break;
        case 6: effect_name = "Monochrome";
            break;
        case 7: effect_name = "Old photo";
            break;
        case 8: effect_name = "Night vision";
            break;
        case 9: effect_name = "X-ray";
            break;
        case 10: effect_name = "Shades";
            break;
        case 11: effect_name = "XPRO";
            break;
        case 12: effect_name = "Yellow-blue";
            break;
        case 13: effect_name = "Yellow-bluish";
            break;
        case 14: effect_name = "Chrome";
            break;
        case 15: effect_name = "Esses";
            break;
        default: effect_name = "invalid";
            break;
    }
    snprintf(s,255," Effect: %s",effect_name);
    return s;
}
/**
    \fn ctor
*/
ADMVideoArtColorEffect::ADMVideoArtColorEffect(  ADM_coreVideoFilter *in,CONFcouple *couples)  :ADM_coreVideoFilter(in,couples)
{
    if(!couples || !ADM_paramLoad(couples,artColorEffect_param,&_param))
    {
        _param.effect = 0;
    }
    ArtColorEffectCreateBuffers(info.width,info.height, &(_rgbBufStride), &(_rgbBufRaw), &(_rgbBufImage), &(_convertYuvToRgb), &(_convertRgbToYuv));
    update();
}
/**
    \fn update
*/
void ADMVideoArtColorEffect::update(void)
{
    _effect=_param.effect;
}
/**
    \fn dtor
*/
ADMVideoArtColorEffect::~ADMVideoArtColorEffect()
{
    ArtColorEffectDestroyBuffers(_rgbBufRaw, _rgbBufImage, _convertYuvToRgb, _convertRgbToYuv);
}
/**
    \fn getCoupledConf
*/
bool ADMVideoArtColorEffect::getCoupledConf(CONFcouple **couples)
{
    return ADM_paramSave(couples, artColorEffect_param,&_param);
}
/**
    \fn setCoupledConf
*/
void ADMVideoArtColorEffect::setCoupledConf(CONFcouple *couples)
{
    ADM_paramLoad(couples, artColorEffect_param, &_param);
}



/**
    \fn getNextFrame
    \brief
*/
bool ADMVideoArtColorEffect::getNextFrame(uint32_t *fn,ADMImage *image)
{
    /*
    ADMImage *src;
    src=vidCache->getImage(nextFrame);
    if(!src)
        return false; // EOF
    *fn=nextFrame++;
    image->copyInfo(src);
    image->copyPlane(src,image,PLANAR_Y); // Luma is untouched
    src = image;

    DoFilter(...);

    vidCache->unlockAll();
    */

    if(!previousFilter->getNextFrame(fn,image)) return false;

    ArtColorEffectProcess_C(image,info.width,info.height,_effect,_rgbBufStride,_rgbBufRaw, _rgbBufImage, _convertYuvToRgb, _convertRgbToYuv);
 
    return 1;
}

