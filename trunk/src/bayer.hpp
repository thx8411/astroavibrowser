/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#ifndef _BAYER_HPP_
#define _BAYER_HPP_

// vesta raw to rgb conversion
void raw2rgb(unsigned char * dest,const unsigned char * const data,
               int w, int h,int mode);

void bgr2rgb(unsigned char * datas, int w, int h);

#endif
