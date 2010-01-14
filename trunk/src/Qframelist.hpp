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

#ifndef _QFRAMELIST_HPP_
#define _QFRAMELIST_HPP_

#include <Qt/qlistwidget.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "Qframedisplay.hpp"

#include "config.h"

#include "aviwriter.hpp"

// color plans
//#define ALL	0
//#define RED	1
//#define GREEN	2
//#define BLUE	3

class FrameList : public QListWidget
{
      Q_OBJECT
   public :
      FrameList(QWidget* parent=0);
      ~FrameList();
      // handles frames selection
      void selectAll();
      void unSelectAll();
      void invertSelection();
      // init the list with the stream handlers
      void setFormatContext(AVFormatContext* fc);
      void setCodecContext(AVCodecContext* cc);
      void setStreamNumber(int sn);
      // set the display widget
      void setFrameDisplay(FrameDisplay* fd);
      // scans the stream and fill the list
      void fill();
      // dump selected frames in the given codec
      void dump(AviWriter* file);
      // returns the total stream frame number
      // real number, no approx.
      int getFrameNumber();
      // number of frame selected
      int getSelectedFrames();
   public slots :
      // look for a frame
      bool getFrame(int number);
      // display the frame (CAUTION, starts at frame 0, not 1)
      void displayFrame(int number);
      // refresh frame
      void refreshFrame();
      // dummy slot, to do nothing
      void nop(int tmp);
   private :
      // is a file open ?
      bool fileOpened;
      // scale context
      SwsContext* img_convert_ctx;
      // total stream frame number
      int frameNumber;
      // actual frame
      int framePosition;
      // display widget
      FrameDisplay* frameDisplay;
      // ffmpeg stuff
      AVFormatContext* formatContext;
      AVCodecContext* codecContext;
      int streamNumber;
      // original frame
      AVFrame* frame;
      // we have got a frame
      bool frameOk;
      // converted frame
      AVFrame* frameRGB;
      // av packet
      AVPacket* pkt;
      // video buffer
      uint8_t* buffer;
      // go to frame a given frame
      // exact number, no approx.
      bool seekFrame(int number);
      // clears the list
      void reset();
};

#endif
