/***************************************************************************
    \file T_vumeter.cpp
    \brief Class to draw volume unit audio graphs
    \author mean (c) 2009

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFrame>
#include <QImage>
#include <QPainter>

/* Probably on unix/X11 ..*/
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#elif !defined(_WIN32)
#if QT_VERSION < QT_VERSION_CHECK(5,0,0) 
#include <QX11Info>
#endif
#endif
#include "ADM_default.h"
#include "T_vumeter.h"

static ADM_Qvumeter *vuWidget = NULL;

//  #warning Assume rgb bbuffer is 32bits aligned...
#define GREEN   0xFF00FF00
#define DGREEN  0xFF00BF00
#define YELLOW  0xFFA0C000
#define DYELLOW 0xFF789000
#define RED     0xFFFF0000
#define DRED    0xFFBF0000
#define BLACK   0xFF000000

#define VU_BAR_WIDTH  5
#define VU_PEAK_WIDTH 7
#define VU_BOTTOM_SCALE -60.0	// dBFS, must be negative
#define VU_DECAY      1.0

ADM_Qvumeter::ADM_Qvumeter(QWidget *z, int width, int height) : QWidget(z)
{
    this->resize(width, height);
    z->resize(width + 2, height + 2);

    rgbDataBuffer = new uint8_t[width * height * 4];
    for(int i=0;i<88;i++)
    {
        uint8_t *ptr = rgbDataBuffer + (width * i * 4);
        uint32_t *data = (uint32_t *)ptr;
        for(int j=0;j<64;j++)
        {
            *data++=BLACK;
        }
    }
    for (int i=0; i<8; i++)
        peaks[i].maxPos = 0;
}

ADM_Qvumeter::~ADM_Qvumeter()
{
    delete [] rgbDataBuffer;
}

void ADM_Qvumeter::paintEvent(QPaintEvent *ev)
{
    QImage image(this->rgbDataBuffer, this->width(), this->height(), QImage::Format_RGB32);
    QPainter painter(this);
    painter.drawImage(QPoint(1, 1), image);
    painter.end();
}

/**
    \fn UI_InitVUMeter
*/
bool UI_InitVUMeter(QFrame *host)
{
    vuWidget = new ADM_Qvumeter(host, 64, 88);
    vuWidget->show();

    return true;
}
/**
    \fn UI_vuUpdate
    \brief Update vumeter, input is volume per channel as integer dBFS (valid range -100 to +3), mark inactive channel with +255 (> 3)
*/
bool UI_vuUpdate(int32_t volume[8])
{
    int width = vuWidget->width();
    uint8_t * basePtr = vuWidget->rgbDataBuffer;
    
    int active_channel_count = 0;
    int active_channels[8] = {0};
    for(int i=0;i<8;i++)
    {
        if (volume[i] <= 3)
        {
            active_channel_count++;
            active_channels[i] = 1;
        }
    }
    
    if (active_channel_count == 0)	// just clear
    {
        for(int i=0;i<88;i++)
        {
            uint8_t *ptr = basePtr + (width * i * 4);
            uint32_t *data = (uint32_t *)ptr;
            for(int j=0;j<64;j++)
            {
                *data++=BLACK;
            }
        }
        for(int i=0;i<8;i++)
            vuWidget->peaks[i].maxPos = 0;
        vuWidget->update();
        return true;
    }

    // Update max, adjust volume scale
    for(int i=0;i<8;i++)
    {
        // +3 -> 87
        // VU_BOTTOM_SCALE -> 0
        float v = volume[i];
        v -= VU_BOTTOM_SCALE;
        v /= (3.0 - VU_BOTTOM_SCALE);
        v *= 87.0;
        volume[i] = v+0.49;
        if (volume[i] > 87)
            volume[i] = 87;
        if (volume[i] < 0)
            volume[i] = 0;
        if (active_channels[i])
        {
            if (volume[i] > vuWidget->peaks[i].maxPos)
            {
                vuWidget->peaks[i].maxPos = volume[i];
                if(volume[i]<66) vuWidget->peaks[i].maxColor = GREEN;
                else if(volume[i]<84) vuWidget->peaks[i].maxColor = YELLOW;		// headroom -18 dBFS (Digital broadcasts and ordinary digital recordings) (<84 & >= 66)
                else vuWidget->peaks[i].maxColor = RED;		// overdrive >= 0 dBFS (>= 84)
            }
        }
        else
            vuWidget->peaks[i].maxPos = 0;
        if (volume[i] < 1)
            volume[i] = 1;
    }

    // Draw
    for(int i=0;i<88;i++)
    {
        uint8_t *ptr = basePtr + (width * i * 4);
        uint32_t *data = (uint32_t *)ptr;
        int h = 87-i;
        for(int j=0;j<64;j++)
        {
            *data=BLACK;
            int v = j/8;
            int w = j%8;
            if (active_channels[v])
            {
                unsigned int maxpos = vuWidget->peaks[v].maxPos;
                if (maxpos == h)
                {
                    if ((w>=(4-VU_PEAK_WIDTH/2))&(w<(4+VU_PEAK_WIDTH/2)))
                        *data = vuWidget->peaks[v].maxColor;
                } else {
                    if (volume[v] >= h)
                        if ((w>=(4-VU_BAR_WIDTH/2))&(w<(4+VU_BAR_WIDTH/2)))
                        {
                            if(h<66) *data=(i%4==3)?DGREEN:GREEN;
                            else if(h<84) *data=(i%4==3)?DYELLOW:YELLOW;
                            else *data=(i%4==3)?DRED:RED;
                        }
                }
            }
            data++;
        }
    }

    // Decay
    for(int i=0;i<8;i++)
    {
        if (vuWidget->peaks[i].maxPos > 0)
        {
            if (vuWidget->peaks[i].maxPos > VU_DECAY)
                vuWidget->peaks[i].maxPos -= VU_DECAY;
            else
                vuWidget->peaks[i].maxPos = 0;
        }
    }

#if 0
      ADM_info("VU : LEFT %"PRIu32" CENTER %"PRIu32" RIGHT %"PRIu32" REARLEFT %"PRIu32" REARRIGHT %"PRIu32"\n",
                    volume[0],volume[1],volume[2],volume[3],volume[4]);
#endif
    vuWidget->update();
    return true;
}
//EOF
