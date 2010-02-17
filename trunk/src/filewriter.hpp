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

#ifndef _FILEWRITER_HPP_
#define _FILEWRITER_HPP_

#include <string>

// formats
#define BMP_FILE	1
#define AVI_FILE	2

// codecs
#define CODEC_RAW	1
#define CODEC_LOSSLESS	2

using namespace std;

class FileWriter {
   public :
      FileWriter(int codec, int plans, const char* name, int width, int height, int frameRate);
      virtual ~FileWriter() {};
      virtual void AddFrame(unsigned char* datas) {};
   protected :
      int w;
      int h;
      int codec_;
      int plans_;
      string name_;
};

#endif
