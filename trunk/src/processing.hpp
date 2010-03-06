/*
 * copyright (c) 2009-2010 Blaise-Florentin Collin
 *
 * This file is part of AstroAviBrowser.
 *
 * AstroAviBrowser is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3
 * as published by the Free Software Foundation
 *
 * AstroAviBrowser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * v3 along with AstroAviBrowser; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _BAYER_HPP_
#define _BAYER_HPP_

// plans def.
#define ALL_PLANS       3
#define RED_PLAN        4
#define GREEN_PLAN      5
#define BLUE_PLAN       6
#define LUM_PLAN        7

// raw modes
#define RAW_NONE        0
#define RAW_BG          1
#define RAW_GB          2
#define RAW_RG          3
#define RAW_GR          4

// tools

// clips a value between 0 and 255
unsigned char clip(double v);
unsigned char clip(long v);

// vesta raw to rgb conversion
void raw2rgb(unsigned char * dest, unsigned char * const data,
               int w, int h,int mode);

// bgr to rgb plan swap
void bgr2rgb(unsigned char * datas, int w, int h);

// swap rgb24 (upside down)
void rgb24_vertical_swap(int w, int h, unsigned char* data);

// return R, G, B or luminance plan from rgb24
unsigned char* getPlan(int w, int h, unsigned char* data, int plan);

// 8 bits grey to yuy2
void grey_to_yuy2(int w, int h, const unsigned char* src, unsigned char* dst);

// returns an array of 256 ints, histograme of the rgb frame luminance
int* getHistogram(int w, int h, unsigned char* data);

#endif
