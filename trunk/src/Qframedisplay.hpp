/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

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

using namespace std;

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
