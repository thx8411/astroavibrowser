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

#ifndef _AVIWRITER_HPP_
#define _AVIWRITER_HPP_

#include <avifile/avifile.h>
#include <avifile/image.h>
#include <avifile/avm_fourcc.h>

// codecs
#define CODEC_RAWGREY           0
#define CODEC_RAWRGB            1
#define CODEC_LOSSLESSRGB       2

class AviWriter {
   public :
      AviWriter(int codec, int plans, const char* name, int width, int height, int frameRate);
      ~AviWriter();
      void AddFrame(unsigned char* datas);
   private:
      avm::IWriteFile* aviFile_;
      avm::IWriteStream* aviChunkStream_;
      avm::IVideoWriteStream* aviFrameStream_;
      int w;
      int h;
      int codec_;
      int plans_;
};

#endif
