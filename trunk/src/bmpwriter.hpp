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

#ifndef _BMPWRITER_HPP_
#define _BMPWRITER_HPP_

#include <stdint.h>
#include <stdio.h>

#include "filewriter.hpp"

// works only on little endian archs
typedef struct {
   // file header
   char type[2];
   uint32_t total_size;
   uint32_t reserved;
   uint32_t offset;
   // picture header
   uint32_t header_size;
   uint32_t width;
   uint32_t height;
   uint16_t plans;
   uint16_t depth;
   uint32_t compression;
   uint32_t picture_size;
   uint32_t hres;
   uint32_t vres;
   uint32_t palette_size;
   uint32_t important_colors;
} BMPHEADER;

class BmpWriter : public FileWriter {
   public :
      BmpWriter(int codec, int plans, const char* name, int width, int height, int frameRate, bool count=true);
      ~BmpWriter();
      void AddFrame(unsigned char* datas);
   private:
      bool count_;
      int frameNumber;
};

#endif
