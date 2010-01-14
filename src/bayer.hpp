/*
 * copyright (c) 2009 Blaise-Florentin Collin
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

#define ALL_PLANS       3
#define RED_PLAN        4
#define GREEN_PLAN      5
#define BLUE_PLAN       6
#define LUM_PLAN        7

// vesta raw to rgb conversion
void raw2rgb(unsigned char * dest,const unsigned char * const data,
               int w, int h,int mode);

// bgr to rgb plan swap
void bgr2rgb(unsigned char * datas, int w, int h);

// swap rgb24 (upside down)
void rgb24_vertical_swap(int w, int h, unsigned char* data);

// return R, G, B or luminance plan form rgb24
unsigned char* getPlan(int w, int h, unsigned char* data, int plan);

#endif
