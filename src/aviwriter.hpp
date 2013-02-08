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

#ifndef _AVIWRITER_HPP_
#define _AVIWRITER_HPP_

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "filewriter.hpp"

class AviWriter : public FileWriter {
   public :
      AviWriter(int codec, int plans, const char* name, int width, int height, int frameRate);
      ~AviWriter();
      void AddFrame(unsigned char* datas);
   private:
      AVOutputFormat* output_format;
      AVFormatContext* output_format_cx;
      AVCodec* output_codec;
      AVCodecContext* output_codec_cx;
      AVStream* output_video_stream;
      AVFrame* picture;
      uint8_t* video_outbuf;
      int video_outbuf_size;
};

#endif
