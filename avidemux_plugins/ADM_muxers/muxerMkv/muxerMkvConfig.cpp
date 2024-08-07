/***************************************************************************
    copyright            : (C) 2007 by mean
    email                : fixounet@free.fr
    
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
#include "ADM_muxerInternal.h"
#include "muxerMkv.h"
#define ADM_MINIMAL_UI_INTERFACE
#include "DIA_factory.h"

#ifdef MUXER_IS_WEBM
bool WebmConfigure(void)
#else
bool mkvConfigure(void)
#endif
{
    diaMenuEntry aspect[]={
        {STANDARD,"4:3",NULL},
        {WIDE,"16:9",NULL},
        {UNI,"18:9",NULL},
        {CINEMA,"64:27",NULL},
        {OTHER,QT_TRANSLATE_NOOP("mkvmuxer","Derived from display width"),NULL}
    };
// see https://www.matroska.org/technical/elements.html and libavutil/pixfmt.h
    diaMenuEntry meMatrixCoeff[]={
        {AVCOL_SPC_RGB,                 QT_TRANSLATE_NOOP("mkvmuxer","0: sRGB"), NULL},
        {AVCOL_SPC_BT709,               QT_TRANSLATE_NOOP("mkvmuxer","1: ITU-R BT.709"), NULL},
        {AVCOL_SPC_UNSPECIFIED,         QT_TRANSLATE_NOOP("mkvmuxer","2: Unspecified"), NULL},
        {AVCOL_SPC_RESERVED,            QT_TRANSLATE_NOOP("mkvmuxer","3: Reserved"), NULL},
        {AVCOL_SPC_FCC,                 QT_TRANSLATE_NOOP("mkvmuxer","4: US FCC 73.682"), NULL},
        {AVCOL_SPC_BT470BG,             QT_TRANSLATE_NOOP("mkvmuxer","5: ITU-R BT.470BG"), NULL},
        {AVCOL_SPC_SMPTE170M,           QT_TRANSLATE_NOOP("mkvmuxer","6: SMPTE 170M"), NULL},
        {AVCOL_SPC_SMPTE240M,           QT_TRANSLATE_NOOP("mkvmuxer","7: SMPTE 240M"), NULL},
        {AVCOL_SPC_YCGCO,               QT_TRANSLATE_NOOP("mkvmuxer","8: YCoCg"), NULL},
        {AVCOL_SPC_BT2020_NCL,          QT_TRANSLATE_NOOP("mkvmuxer","9: BT2020 Non-constant Luminance"), NULL},
        {AVCOL_SPC_BT2020_CL,           QT_TRANSLATE_NOOP("mkvmuxer","10: BT2020 Constant Luminance"), NULL},
        {AVCOL_SPC_SMPTE2085,           QT_TRANSLATE_NOOP("mkvmuxer","11: SMPTE ST 2085"), NULL},
        {AVCOL_SPC_CHROMA_DERIVED_NCL,  QT_TRANSLATE_NOOP("mkvmuxer","12: Chroma-derived Non-constant Luminance"), NULL},
        {AVCOL_SPC_CHROMA_DERIVED_CL,   QT_TRANSLATE_NOOP("mkvmuxer","13: Chroma-derived Constant Luminance"), NULL},
        {AVCOL_SPC_ICTCP,               QT_TRANSLATE_NOOP("mkvmuxer","14: ITU-R BT.2100-0"), NULL}
    };
    diaMenuEntry meRange[]={
        {AVCOL_RANGE_UNSPECIFIED,       QT_TRANSLATE_NOOP("mkvmuxer","0: Unspecified"), NULL},
        {AVCOL_RANGE_MPEG,              QT_TRANSLATE_NOOP("mkvmuxer","1: Limited"), NULL},
        {AVCOL_RANGE_JPEG,              QT_TRANSLATE_NOOP("mkvmuxer","2: Full"), NULL}
    };
    diaMenuEntry meTransfer[]={
        {AVCOL_TRC_RESERVED0,           QT_TRANSLATE_NOOP("mkvmuxer","0: Reserved"), NULL},
        {AVCOL_TRC_BT709,               QT_TRANSLATE_NOOP("mkvmuxer","1: ITU-R BT.709"), NULL},
        {AVCOL_TRC_UNSPECIFIED,         QT_TRANSLATE_NOOP("mkvmuxer","2: Unspecified"), NULL},
        {AVCOL_TRC_RESERVED,            QT_TRANSLATE_NOOP("mkvmuxer","3: Reserved"), NULL},
        {AVCOL_TRC_GAMMA22,             QT_TRANSLATE_NOOP("mkvmuxer","4: Gamma 2.2 curve - BT.470M"), NULL},
        {AVCOL_TRC_GAMMA28,             QT_TRANSLATE_NOOP("mkvmuxer","5: Gamma 2.8 curve - BT.470BG"), NULL},
        {AVCOL_TRC_SMPTE170M,           QT_TRANSLATE_NOOP("mkvmuxer","6: SMPTE 170M"), NULL},
        {AVCOL_TRC_SMPTE240M,           QT_TRANSLATE_NOOP("mkvmuxer","7: SMPTE 240M"), NULL},
        {AVCOL_TRC_LINEAR,              QT_TRANSLATE_NOOP("mkvmuxer","8: Linear"), NULL},
        {AVCOL_TRC_LOG,                 QT_TRANSLATE_NOOP("mkvmuxer","9: Logarithmic (100:1 range)"), NULL},
        {AVCOL_TRC_LOG_SQRT,            QT_TRANSLATE_NOOP("mkvmuxer","10: Logarithmic (100 * Sqrt(10) : 1 range)"), NULL},
        {AVCOL_TRC_IEC61966_2_4,        QT_TRANSLATE_NOOP("mkvmuxer","11: IEC 61966-2-4"), NULL},
        {AVCOL_TRC_BT1361_ECG,          QT_TRANSLATE_NOOP("mkvmuxer","12: ITU-R BT.1361 Extended Colour Gamut"), NULL},
        {AVCOL_TRC_IEC61966_2_1,        QT_TRANSLATE_NOOP("mkvmuxer","13: IEC 61966-2-1"), NULL},
        {AVCOL_TRC_BT2020_10,           QT_TRANSLATE_NOOP("mkvmuxer","14: ITU-R BT.2020 10 bit"), NULL},
        {AVCOL_TRC_BT2020_12,           QT_TRANSLATE_NOOP("mkvmuxer","15: ITU-R BT.2020 12 bit"), NULL},
        {AVCOL_TRC_SMPTE2084,           QT_TRANSLATE_NOOP("mkvmuxer","16: ITU-R BT.2100 Perceptual Quantization"), NULL},
        {AVCOL_TRC_SMPTE428,            QT_TRANSLATE_NOOP("mkvmuxer","17: SMPTE ST 428-1"), NULL},
        {AVCOL_TRC_ARIB_STD_B67,        QT_TRANSLATE_NOOP("mkvmuxer","18: ARIB STD-B67 (HLG)"), NULL}
    };
    diaMenuEntry mePrimaries[]={
        {AVCOL_PRI_RESERVED0,           QT_TRANSLATE_NOOP("mkvmuxer","0: Reserved"), NULL},
        {AVCOL_PRI_BT709,               QT_TRANSLATE_NOOP("mkvmuxer","1: ITU-R BT.709"), NULL},
        {AVCOL_PRI_UNSPECIFIED,         QT_TRANSLATE_NOOP("mkvmuxer","2: Unspecified"), NULL},
        {AVCOL_PRI_RESERVED,            QT_TRANSLATE_NOOP("mkvmuxer","3: Reserved"), NULL},
        {AVCOL_PRI_BT470M,              QT_TRANSLATE_NOOP("mkvmuxer","4: ITU-R BT.470M"), NULL},
        {AVCOL_PRI_BT470BG,             QT_TRANSLATE_NOOP("mkvmuxer","5: ITU-R BT.470BG - BT.601 625"), NULL},
        {AVCOL_PRI_SMPTE170M,           QT_TRANSLATE_NOOP("mkvmuxer","6: ITU-R BT.601 525 - SMPTE 170M"), NULL},
        {AVCOL_PRI_SMPTE240M,           QT_TRANSLATE_NOOP("mkvmuxer","7: SMPTE 240M"), NULL},
        {AVCOL_PRI_FILM,                QT_TRANSLATE_NOOP("mkvmuxer","8: Film"), NULL},
        {AVCOL_PRI_BT2020,              QT_TRANSLATE_NOOP("mkvmuxer","9: ITU-R BT.2020"), NULL},
        {AVCOL_PRI_SMPTE428,            QT_TRANSLATE_NOOP("mkvmuxer","10: SMPTE ST 428-1"), NULL},
        {AVCOL_PRI_SMPTE431,            QT_TRANSLATE_NOOP("mkvmuxer","11: SMPTE RP 431-2"), NULL},
        {AVCOL_PRI_SMPTE432,            QT_TRANSLATE_NOOP("mkvmuxer","12: SMPTE EG 432-2"), NULL},
        {AVCOL_PRI_JEDEC_P22,           QT_TRANSLATE_NOOP("mkvmuxer","22: JEDEC P22 phosphors"), NULL}
    };

    mkv_muxer *cfg = &muxerConfig;

#define PX(x) &cfg->x
#define MZ(x) sizeof(x)/sizeof(diaMenuEntry)

    diaElemToggle alternate(PX(forceAspectRatio),QT_TRANSLATE_NOOP("mkvmuxer","Force aspect ratio"));
    diaElemUInteger dWidth(PX(displayWidth),QT_TRANSLATE_NOOP("mkvmuxer","Display width:"),16,65535);
    diaElemMenu menuAspect(PX(displayAspectRatio),QT_TRANSLATE_NOOP("mkvmuxer","Aspect Ratio (DAR):"),MZ(aspect),aspect);

    alternate.link(1,&menuAspect);
    menuAspect.link(aspect+4,1,&dWidth);

    diaElemToggle tColInfo(PX(addColourInfo),QT_TRANSLATE_NOOP("mkvmuxer","Add color info"));
    diaElemMenu menuColMatrix(PX(colMatrixCoeff),QT_TRANSLATE_NOOP("mkvmuxer","Matrix Coefficients:"),MZ(meMatrixCoeff),meMatrixCoeff);
    diaElemMenu menuColRange(PX(colRange),QT_TRANSLATE_NOOP("mkvmuxer","Range:"),MZ(meRange),meRange);
    diaElemMenu menuColTransfer(PX(colTransfer),QT_TRANSLATE_NOOP("mkvmuxer","Transfer Characteristic:"),MZ(meTransfer),meTransfer);
    diaElemMenu menuColPrimaries(PX(colPrimaries),QT_TRANSLATE_NOOP("mkvmuxer","Color Primaries:"),MZ(mePrimaries),mePrimaries);

    tColInfo.link(1,&menuColMatrix);
    tColInfo.link(1,&menuColRange);
    tColInfo.link(1,&menuColTransfer);
    tColInfo.link(1,&menuColPrimaries);

#undef MZ
#define MZ(x) sizeof(x)/sizeof(diaElem *)
    diaElem *basics[]={&alternate,&menuAspect,&dWidth};
    diaElemTabs tabBasic(QT_TRANSLATE_NOOP("mkvmuxer","Basic Settings"),MZ(basics),basics);

    diaElem *advanced[]={&tColInfo,&menuColMatrix,&menuColRange,&menuColTransfer,&menuColPrimaries};
    diaElemTabs tabAdvanced(QT_TRANSLATE_NOOP("mkvmuxer","Advanced Settings"),MZ(advanced),advanced);
#undef MZ
#define MZ(x) sizeof(x)/sizeof(diaElemTabs *)
    diaElemTabs *tabs[]={&tabBasic,&tabAdvanced};
    if(diaFactoryRunTabs(
#ifdef MUXER_IS_WEBM
            QT_TRANSLATE_NOOP("mkvmuxer","WebM Muxer"),
#else
            QT_TRANSLATE_NOOP("mkvmuxer","MKV Muxer"),
#endif
            MZ(tabs), tabs))
        return true;
    return false;

}

