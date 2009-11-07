/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

/* main app window */

#ifndef _QMAINWINDOW_HPP_
#define _QMAINWINDOW_HPP_

#include <Qt/qapplication.h>
#include <Qt/qwidget.h>
#include <Qt/qaction.h>
#include <Qt/qmainwindow.h>
#include <Qt/qpushbutton.h>

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
      void MenuFormat();
      void MenuCodec();
      // buttons slots
      void ButtonSelectAll();
      void ButtonUnSelectAll();
      void ButtonInvert();
   private :
      // release an opened file
      void freeFile();
      // menu actions
      QAction* open;
      QAction* save;
      QAction* quit;
      QAction* properties;
      QAction* format;
      QAction* codec;
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
      // vidoe sequence frame number
      int inputStreamNumber;
};

#endif
