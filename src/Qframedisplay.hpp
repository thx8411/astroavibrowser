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

#ifndef _QFRAMEDISPLAY_HPP_
#define _QFRAMEDISPLAY_HPP_

#include <iostream>

#include <Qt/qwidget.h>
#include <Qt/qpainter.h>
#include <Qt/qpen.h>

#include <QtGui/QPaintEvent>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "config.h"

using namespace std;

// raw modes
#define RAW_NONE        0
#define RAW_BG          1
#define RAW_GB          2
#define RAW_RG		3
#define RAW_GR		4

class FrameDisplay : public QWidget
{
      Q_OBJECT
   public :
      FrameDisplay(QWidget* parent=0);
      ~FrameDisplay();
      // set the frame to display
      void setFrame(int width, int height, AVFrame* f);
      // get the frame
      AVFrame* getFrame();
      // set raw mode
      void setRawmode(int mode);
      // get used raw mode
      int getRawmode();
   protected:
      void paintEvent(QPaintEvent * ev);
   private :
      // attributs
      int rawMode;
      int frameWidth;
      int frameHeight;
      AVFrame* frameData;
      QPainter * painter_;
      // functions
      void applyRawMode();
};

#endif
