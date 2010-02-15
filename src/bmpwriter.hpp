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

#ifndef _BMPWRITER_HPP_
#define _BMPWRITER_HPP_

typedef struct {
   // file header
   char type[2];
   unsigned int total_size;
   unsigned int reserved1;
   unsigned int offset;
   // picture header
   unsigned int header_size;
   unsigned int width;
   unsigned int height;
   unsigned short plans;
   unsigned short depth;
   unsigned int compression;
   unsigned int picture_size;
   unsigned int hres;
   unsigned int vres;
   unsigned int palette_size;
   unsigned int important_colors;
} BMPHEADER;

#include "filewriter.hpp"

class BmpWriter : public FileWriter {
   public :
      BmpWriter(int codec, int plans, const char* name, int width, int height, int frameRate);
      ~BmpWriter();
      void AddFrame(unsigned char* datas);
   private:
      int frameNumber;
};

#endif
