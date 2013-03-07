/*
 * copyright (c) 2009-2013 Blaise-Florentin Collin
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

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include "processing.hpp"

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

// clips a value between 0 and 255
unsigned char clip(double v) {
   if(v>255)
      return 255;
   if(v<0)
      return 0;
   return((unsigned char)v);
}

unsigned char clip(long v) {
   if(v>255)
      return 255;
   if(v<0)
      return 0;
   return((unsigned char)v);
}

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
void raw2rgb(unsigned char * dest, unsigned char * const data,int w,int h,int mode) {
   unsigned char* lum;
   int pixelOffset;
   int pixelOffsetLum;
   int rowOffset=1;
   int lineOffset=w;
   // compute luminance first
   lum=getPlan(w,h,data,LUM_PLAN);
   for(int x=0;x<w;x++) {
      for(int y=0;y<h;y++) {
         pixelOffsetLum=y*w+x;
         pixelOffset=pixelOffsetLum*3;
         // manage edges
         if((x==0)||(x==w-1)||(y==0)||(y==h-1)) {
            dest[pixelOffset+RED_OFFSET]=0;
            dest[pixelOffset+GREEN_OFFSET]=0;
            dest[pixelOffset+BLUE_OFFSET]=0;
         } else {
            switch(getPixelColor(x,y,mode)) {
               case RED :
                  dest[pixelOffset+RED_OFFSET]=lum[pixelOffsetLum];
                  dest[pixelOffset+GREEN_OFFSET]=(lum[pixelOffsetLum-rowOffset]
                     +lum[pixelOffsetLum+rowOffset]
                     +lum[pixelOffsetLum-lineOffset]
                     +lum[pixelOffsetLum+lineOffset])/4;
                  dest[pixelOffset+BLUE_OFFSET]=(lum[pixelOffsetLum+lineOffset+rowOffset]
                     +lum[pixelOffsetLum+lineOffset-rowOffset]
                     +lum[pixelOffsetLum-lineOffset+rowOffset]
                     +lum[pixelOffsetLum-lineOffset-rowOffset])/4;
                  break;
               case GREEN1 :
                  dest[pixelOffset+RED_OFFSET]=(lum[pixelOffsetLum+rowOffset]+lum[pixelOffsetLum-rowOffset])/2;
                  dest[pixelOffset+GREEN_OFFSET]=lum[pixelOffsetLum];
                  dest[pixelOffset+BLUE_OFFSET]=(lum[pixelOffsetLum+lineOffset]+lum[pixelOffsetLum-lineOffset])/2;
                  break;
               case GREEN2 :
                  dest[pixelOffset+RED_OFFSET]=(lum[pixelOffsetLum+lineOffset]+lum[pixelOffsetLum-lineOffset])/2;
                  dest[pixelOffset+GREEN_OFFSET]=lum[pixelOffsetLum];
                  dest[pixelOffset+BLUE_OFFSET]=(lum[pixelOffsetLum+rowOffset]+lum[pixelOffsetLum-rowOffset])/2;
                  break;
               case BLUE :
                  dest[pixelOffset+RED_OFFSET]=(lum[pixelOffsetLum+lineOffset+rowOffset]
                     +lum[pixelOffsetLum+lineOffset-rowOffset]
                     +lum[pixelOffsetLum-lineOffset+rowOffset]
                     +lum[pixelOffsetLum-lineOffset-rowOffset])/4;
                  dest[pixelOffset+GREEN_OFFSET]=(lum[pixelOffsetLum-rowOffset]
                     +lum[pixelOffsetLum+rowOffset]
                     +lum[pixelOffsetLum-lineOffset]
                     +lum[pixelOffsetLum+lineOffset])/4;
                  dest[pixelOffset+BLUE_OFFSET]=lum[pixelOffsetLum];
                  break;
            }
         }
      }
   }
   free(lum);
}

// swap blue and green in rgb24 frame
void bgr2rgb(unsigned char * datas, int w, int h) {
   char tmp;
   for(int i=0;i<w*h;i++) {
      tmp=datas[i*3];
      datas[i*3]=datas[i*3+2];
      datas[i*3+2]=tmp;
   }
}

// swap rgb24 (upside down)
void rgb24_vertical_swap(int w, int h, unsigned char* data){
   unsigned char* tmp;
   tmp=(unsigned char*)malloc(w*h*3);
   memcpy(tmp,data,w*h*3);
   for(int i=0;i<h;i++) {
      memcpy(&data[i*w*3],&tmp[(h-i-1)*w*3],w*3);
   }
   free(tmp);
}

// swap 8 bit gray (upside down)
void grey_vertical_swap(int w, int h, unsigned char* data){
   unsigned char* tmp;
   static int i;
   tmp=(unsigned char*)malloc(w*h);
   memcpy(tmp,data,w*h);
   for(i=0;i<h;i++) {
      memcpy(&data[i*w],&tmp[(h-i-1)*w],w);
   }
   free(tmp);
}

// return R, G, B or luminance plan form bgr24
unsigned char* getPlan(int w, int h, unsigned char* data, int plan) {
   unsigned char* buffer;
   buffer=(unsigned char*)malloc(w*h);
   for(int i=0;i<(w*h);i++){
      switch(plan){
         case RED_PLAN :
            buffer[i]=data[i*3+2];
            break;
         case GREEN_PLAN :
            buffer[i]=data[i*3+1];
            break;
         case BLUE_PLAN :
            buffer[i]=data[i*3];
            break;
         case LUM_PLAN :
            buffer[i]=clip(0.299*(double)data[i*3+2]+0.587*(double)data[i*3+1]+0.114*(double)data[i*3]);
            break;
         default :
            return(NULL);
      }
   }
   return(buffer);
}

// returns an array of 256 ints, histogram of the rgb frame luminance
// (log2)^2 base
int* getHistogram(int w, int h, unsigned char* data) {
   int i;
   int* tab;
   unsigned char* buffer;
   if(data==NULL)
      return(NULL);
   tab=(int*)malloc(256*sizeof(int));
   memset(tab,0,256*sizeof(int));
   buffer=getPlan(w,h,data,LUM_PLAN);
   for(i=0;i<(w*h);i++) {
      tab[buffer[i]]++;
   }
   for(i=0;i<256;i++)
      if(tab[i]!=0)
         tab[i]=log2(tab[i])*log2(tab[i]);
   return(tab);
}

// 8 bits grey to yuyv
void grey_to_yuy2(int w, int h, const unsigned char* src, unsigned char* dst) {
   int i;
   int size=w*h/2;
   for(i=0;i<size;i++) {
      dst[i*4]=src[i*2];
      dst[i*4+1]=128;
      dst[i*4+2]=src[i*2+1];
      dst[i*4+3]=128;
   }
}
