/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

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

#include <avifile/avifile.h>
#include <avifile/avm_fourcc.h>

#include "Qframedisplay.hpp"
#include "Qframelist.hpp"

#define	CODEC_SAME	0
#define CODEC_RAWGREY	1
#define CODEC_RAWRGB	2
#define CODEC_LOSSLESS	3

class MainWindow : public QMainWindow
{
      Q_OBJECT
   public :
      MainWindow(QWidget* parent=0);
      ~MainWindow();
   public slots :
      // menu slots
      void MenuOpen();
      void MenuSave();
      void MenuProperties();
      void MenuAbout();
      // input mode slots
      void setNone();
      void setBg();
      void setGb();
      void setRg();
      void setGr();
      // rgb sepearation slot
      void setSeparate();
      // ouput codecs slots
      //void setSame();
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
      // menu actions
      QAction* open;
      QAction* save;
      QAction* quit;
      QAction* properties;
      QAction* separateRgb;
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
      //QAction* codecSame;
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
      fourcc_t outputCodec;
      // ffmpeg datas
      AVFormatContext* inputFileFormatContext;
      AVCodecContext* inputFileCodecContext;
      AVCodec* inputFileCodec;
      // video sequence frame number
      int inputStreamNumber;
};

#endif
