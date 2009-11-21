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

#include <iostream>

#include "Qframedisplay.hpp"

#include "bayer.hpp"

using namespace std;

// pixel types
#define	RED	0
#define GREEN1	1	// green pixel on a red line
#define GREEN2	2	// green pixel on a blue line
#define	BLUE	3

// color offsets
#define	RED_OFFSET	0
#define	GREEN_OFFSET	1
#define	BLUE_OFFSET	2

// get the bayer pixel color depending on its position
// and bayer pattern
int getPixelColor(int x,int y,int mode) {
   int color;
   int x_odd = x % 2;
   int y_odd = y % 2;
   switch (mode) {
   case RAW_GR:
      if(x_odd==y_odd) {
         // green pixel
         if(y_odd)
            // blue line
            return(GREEN2);
         else
            // red line
            return(GREEN1);
      } else {
         if(y_odd)
            // blue pixel
            return(BLUE);
         else
            // red pixel
            return(RED);
      }
      break;
   case RAW_RG:
      if(x_odd!=y_odd) {
         // green pixel
         if(y_odd)
            // blue line
            return(GREEN2);
         else
            // red line
            return(GREEN1);
      } else {
         if(y_odd)
            // blue pixel
            return(BLUE);
         else
            // red pixel
            return(RED);
      }
      break;
   case RAW_BG:
      if(x_odd!=y_odd) {
         // green pixel
         if(y_odd)
            // red line
            return(GREEN1);
         else
            // blue line
            return(GREEN2);
      } else {
         if(y_odd)
            // red pixel
            return(RED);
         else
            // blue pixel
            return(BLUE);
      }
      break;
   case RAW_GB:
      if(x_odd==y_odd) {
         // green pixel
         if(y_odd)
            // red line
            return(GREEN1);
         else
            // blue line
            return(GREEN2);
      } else {
         if(y_odd)
            // red pixel
            return(RED);
         else
            // blue pixel
            return(BLUE);
      }
      break;
   }
   return(color);
}

// rgb raw to rgb color conversion
// we use the green plan (close to luminance on most codecs)
void raw2rgb(unsigned char * dest,const unsigned char * const data,int w,int h,int mode) {
   int pixelOffset;
   int rowOffset=3;
   int lineOffset=w*3;
   for(int x=0;x<w;x++) {
      for(int y=0;y<h;y++) {
         pixelOffset=(y*w+x)*3;
         // manage edges
         if((x==0)||(x==w-1)||(y==0)||(y==h-1)) {
            dest[pixelOffset+RED_OFFSET]=0;
            dest[pixelOffset+GREEN_OFFSET]=0;
            dest[pixelOffset+BLUE_OFFSET]=0;
         } else {
            switch(getPixelColor(x,y,mode)) {
               case RED :
                  dest[pixelOffset+RED_OFFSET]=data[pixelOffset+GREEN_OFFSET];
                  dest[pixelOffset+GREEN_OFFSET]=(data[pixelOffset-rowOffset+GREEN_OFFSET]
                     +data[pixelOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset+GREEN_OFFSET]
                     +data[pixelOffset+lineOffset+GREEN_OFFSET])/4;
                  dest[pixelOffset+BLUE_OFFSET]=(data[pixelOffset+lineOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset+lineOffset-rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset-rowOffset+GREEN_OFFSET])/4;
                  break;
               case GREEN1 :
                  dest[pixelOffset+RED_OFFSET]=(data[pixelOffset+rowOffset+GREEN_OFFSET]+data[pixelOffset-rowOffset+GREEN_OFFSET])/2;
                  dest[pixelOffset+GREEN_OFFSET]=data[pixelOffset+GREEN_OFFSET];
                  dest[pixelOffset+BLUE_OFFSET]=(data[pixelOffset+lineOffset+GREEN_OFFSET]+data[pixelOffset-lineOffset+GREEN_OFFSET])/2;
                  break;
               case GREEN2 :
                  dest[pixelOffset+RED_OFFSET]=(data[pixelOffset+lineOffset+GREEN_OFFSET]+data[pixelOffset-lineOffset+GREEN_OFFSET])/2;
                  dest[pixelOffset+GREEN_OFFSET]=data[pixelOffset+GREEN_OFFSET];
                  dest[pixelOffset+BLUE_OFFSET]=(data[pixelOffset+rowOffset+GREEN_OFFSET]+data[pixelOffset-rowOffset+GREEN_OFFSET])/2;
                  break;
               case BLUE :
                  dest[pixelOffset+RED_OFFSET]=(data[pixelOffset+lineOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset+lineOffset-rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset-rowOffset+GREEN_OFFSET])/4;
                  dest[pixelOffset+GREEN_OFFSET]=(data[pixelOffset-rowOffset+GREEN_OFFSET]
                     +data[pixelOffset+rowOffset+GREEN_OFFSET]
                     +data[pixelOffset-lineOffset+GREEN_OFFSET]
                     +data[pixelOffset+lineOffset+GREEN_OFFSET])/4;
                  dest[pixelOffset+BLUE_OFFSET]=data[pixelOffset+GREEN_OFFSET];
                  break;
            }
         }
         // cerr << x << " : " << y << endl;
      }
   }
}

void bgr2rgb(unsigned char * datas, int w, int h) {
   char tmp;
   for(int i=0;i<w*h;i++) {
      tmp=datas[i*3];
      datas[i*3]=datas[i*3+2];
      datas[i*3+2]=tmp;
   }
}
