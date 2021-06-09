/***************************************************************************
                          Motion interpolation
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
 
#pragma once

#include <cmath>
#include "ADM_image.h"
#include "ADM_default.h"

#define MOTIN_MAX_PYRAMID_LEVELS  (7)
#define MOTIN_SEARCH_RADIUS       (2)

class  motin
{

  protected:
    int         frameW, frameH;
    int         pyramidLevels;
    ADMImage *  frameA;
    ADMImage *  frameB;
    ADMImage ** pyramidA;
    ADMImage ** pyramidB;
    ADMImage ** pyramidWA;
    ADMImage ** pyramidWB;
    ADMColorScalerFull ** upScalers;
    ADMColorScalerFull ** downScalers;
    
    typedef struct {
        int lv;
        uint8_t * plA[3];
        uint8_t * plB[3];
        uint8_t * plW[3];
        int strides[3];
        uint32_t w,h;
    } worker_thread_arg;
    
    static void *worker_thread( void *ptr );
    
    static int sad(uint8_t * p1, uint8_t * p2, int stride, int x1, int y1, int x2, int y2);

  public:
    motin(int width, int height);
    ~motin();
    
    void createPyramids(ADMImage * imgA, ADMImage * imgB);
    void estimateMotion();
    void interpolate(ADMImage * dst, int alpha);
};
