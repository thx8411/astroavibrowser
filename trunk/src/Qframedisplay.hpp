/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#ifndef _QFRAMEDISPLAY_HPP_
#define _QFRAMEDISPLAY_HPP_

#include <Qt/qwidget.h>
#include <Qt/qpainter.h>
#include <Qt/qpen.h>

#include <QtGui/QPaintEvent>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

class FrameDisplay : public QWidget
{
      Q_OBJECT
   public :
      FrameDisplay(QWidget* parent=0);
      // Set the frame to diplay
      void setFrame(int width, int height, AVFrame* f);
      void setRawMode(int mode);
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
