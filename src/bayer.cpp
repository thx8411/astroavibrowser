/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include "Qframedisplay.hpp"

#include "bayer.hpp"

// vesta raw to rgb conversion
void raw2rgb(unsigned char * dest,const unsigned char * const data,int w,
             int h,int mode) {
   int redShiftX,redShiftY;
   int blueShiftX, blueShiftY;
   int greenShiftX,greenShiftY;

   switch (mode) {
   case RAW_GR:
      break;
   case RAW_RG:
      break;
   case RAW_BG:
      break;
   case RAW_GB:
      break;
   }
   for(int i=0;i<w*h*3;i+=3) {
      //
      // temp, just for tests
      //
      dest[i]=0/*data[i+1]*/;
      dest[i+1]=data[i+1];
      dest[i+2]=0/*data[i+1]*/;
      //
   }
}
