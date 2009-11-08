/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#ifndef _QFRAMELIST_HPP_
#define _QFRAMELIST_HPP_

#include <Qt/qlistwidget.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "Qframedisplay.hpp"

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
      // when saving output stream, says if a frame shoudld be
      // skeeped or notm depanding on the frame selection
      // starts at frame 1
      bool skeepFrame(int number);
      // returns the total stream frame number
      // real number, no approx.
      int getFrameNumber();
   public slots :
      // display the frame (CAUTION, starts at frame 0, not 1)
      void displayFrame(int number);
      // dummy slot, to do nothing
      void nop(int tmp);
   private :
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
      // converted frame
      AVFrame* frameRGB;
      // av packet
      AVPacket* pkt;
      // video buffer
      uint8_t* buffer;
      // go to frame a given frame
      // exact number, no approx.
      void seekFrame(int number);
      // clears the list
      void reset();
};

#endif
