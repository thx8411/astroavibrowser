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

#include "Qframedisplay.hpp"
#include "Qframelist.hpp"

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
      void MenuBayer();
      void MenuCodec();
      void MenuAbout();
      // buttons slots
      void ButtonSelectAll();
      void ButtonUnSelectAll();
      void ButtonInvert();
   private :
      // format and codec menu tools
      void createBayerMenu();
      void createCodecMenu();
      // release an opened file
      void freeFile();
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
      QAction* bayerGrey;
      QAction* bayerBg;
      QAction* bayerGb;
      QAction* bayerRg;
      QAction* bayerGr;
      // codec menu radio buttons
      QAction* codecSame;
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
      // ffmpeg datas
      AVFormatContext* inputFileFormatContext;
      AVCodecContext* inputFileCodecContext;
      AVCodec* inputFileCodec;
      // video sequence frame number
      int inputStreamNumber;
};

#endif
