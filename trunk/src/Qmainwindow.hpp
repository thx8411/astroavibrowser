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

/* main app window */

#ifndef _QMAINWINDOW_HPP_
#define _QMAINWINDOW_HPP_

#include <Qt/qapplication.h>
#include <Qt/qwidget.h>
#include <Qt/qaction.h>
#include <Qt/qmainwindow.h>
#include <Qt/qpushbutton.h>
#include <Qt/qradiobutton.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "Qframedisplay.hpp"
#include "Qframelist.hpp"

#include "config.h"

class MainWindow : public QMainWindow
{
      Q_OBJECT
   public :
      MainWindow(QWidget* parent=0);
      ~MainWindow();
   public slots :
      // menu slots
      void MenuOpen();
      void MenuSaveAll();
      void MenuSaveR();
      void MenuSaveG();
      void MenuSaveB();
      void MenuProperties();
      void MenuAbout();
      // input mode slots
      void setNone();
      void setBg();
      void setGb();
      void setRg();
      void setGr();
      // ouput codecs slots
      void setRawgrey();
      void setRawrgb();
      void setLossless();
      // buttons slots
      void ButtonSelectAll();
      void ButtonUnSelectAll();
      void ButtonInvert();
   private :
      // release an opened file
      void freeFile();
      // format and codec menu tools
      void createBayerMenu();
      void createCodecMenu();
      // save a file
      void MenuSaveImpl(int p);
      // menu actions
      QAction* openfile;
      QAction* save;
      QAction* rPlan;
      QAction* gPlan;
      QAction* bPlan;
      QAction* quit;
      QAction* properties;
      QMenu* bayer;
      QMenu* codec;
      QAction* about;
      // bayer menu radio buttons
      QAction* bayerNone;
      QAction* bayerBg;
      QAction* bayerGb;
      QAction* bayerRg;
      QAction* bayerGr;
      // codec menu radio buttons
      QAction* codecRawgrey;
      QAction* codecRawrgb;
      QAction* codecLossless;
      // buttons
      QPushButton* selectAll;
      QPushButton* unSelectAll;
      QPushButton* invertSelection;
      // file names
      QString inputFileName;
      QString outputFileName;
      // display and list
      FrameDisplay* frameDisplay;
      FrameList* frameList;
      // avifile datas
      int outputCodec;
      // ffmpeg datas
      AVFormatContext* inputFileFormatContext;
      AVCodecContext* inputFileCodecContext;
      AVCodec* inputFileCodec;
      // video sequence frame number
      int inputStreamNumber;
};

#endif
