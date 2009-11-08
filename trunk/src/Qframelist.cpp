/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include <iostream>

#include <QtGui/QProgressDialog>

#include "Qframelist.moc"

using namespace std;

FrameList::FrameList(QWidget* parent) : QListWidget(parent) {
   // init
   frameDisplay=NULL;
   codecContext=NULL;
   buffer=NULL;
   // mem alloc
   frame=avcodec_alloc_frame();
   frameRGB=avcodec_alloc_frame();
   // buffer and frameRGB settings
   // packet mem alloc
   pkt=(AVPacket*)av_malloc(sizeof(AVPacket));
   // set max list width
   setMaximumWidth(128);
}

FrameList::~FrameList() {
   // clears the list
   //reset();
   // free mem
   av_free(frame);
   av_free(frameRGB);
   // release packet
   av_free_packet(pkt);
   // release buffer
   if(buffer==NULL)
      av_free(buffer);
}

void FrameList::selectAll() {
   // for each item, set checked
   QListWidgetItem* current;
   for(int i=0;i<frameNumber;i++) {
      current=item(i);
      current->setCheckState(Qt::Checked);
   }
}

void FrameList::unSelectAll() {
   // for each item, set unchecked
   QListWidgetItem* current;
   for(int i=0;i<frameNumber;i++) {
      current=item(i);
      current->setCheckState(Qt::Unchecked);
   }
}

void FrameList::invertSelection() {
   // for each item, invert selection
   QListWidgetItem* current;
   for(int i=0;i<frameNumber;i++) {
      current=item(i);
      if(current->checkState()==Qt::Checked)
         current->setCheckState(Qt::Unchecked);
      else
         current->setCheckState(Qt::Checked);
   }
}

void FrameList::setFormatContext(AVFormatContext* fc) {
   formatContext=fc;
}

void FrameList::setCodecContext(AVCodecContext* cc) {
   int numBytes;

   codecContext=cc;
   // release buffer if needed
   if(buffer==NULL)
      av_free(buffer);
   // buffer and frameRGB settings
   numBytes=avpicture_get_size(PIX_FMT_RGB24, codecContext->width,codecContext->height);
   buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
   avpicture_fill((AVPicture *)frameRGB, buffer, PIX_FMT_RGB24,codecContext->width, codecContext->height);
}

void FrameList::setStreamNumber(int sn) {
   streamNumber=sn;
}

void FrameList::setFrameDisplay(FrameDisplay* fd) {
   frameDisplay=fd;
}

void FrameList::fill() {
   // init
   int i=1;
   int res=0;
   int frameNumberEstimation;
   QListWidgetItem* current;
   QProgressDialog* progress;
   // base frame name
   QString itemName("Frame ");
   // var init
   frameNumber=0;
   framePosition=0;

   // stop display on change
   connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(nop(int)));
   // clears the list
   reset();
   // set display size
   frameDisplay->setMinimumWidth(codecContext->width);
   frameDisplay->setMaximumWidth(codecContext->width);
   frameDisplay->setMinimumHeight(codecContext->height);
   frameDisplay->setMaximumHeight(codecContext->height);
   // frame number estimation
   frameNumberEstimation=formatContext->duration/AV_TIME_BASE*(formatContext->streams[streamNumber]->r_frame_rate.num/formatContext->streams[streamNumber]->r_frame_rate.den);
   // setting progress dialog
   progress = new QProgressDialog(QString("File scanning..."),QString(),0,frameNumberEstimation);
   // read each frame and add item in the list
   while (res==0) {
      progress->setValue(i);
      // read new frame
      res=av_read_frame(formatContext, pkt);
      // add item...
      current= new QListWidgetItem(itemName+QString::number(i),this);
      // and check it
      current->setFlags(current->flags()|Qt::ItemIsUserCheckable);
      current->setCheckState(Qt::Checked);
      i++;
   }
   // progress closing
   progress->close();
   // back to stream beginning
   av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
   // compute frame number
   frameNumber=i-1;
   // set the slot back
   connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(displayFrame(int)));
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
}

void FrameList::reset() {
   // remove all items
   QListWidgetItem* current;
   for(int i=0;i<frameNumber;i++) {
      current=takeItem(0);
      delete current;
   }
}

void FrameList::seekFrame(int number) {
   int res=0;
   int i=1;

   if(number>framePosition) {
      // go forward
      while ((res==0)&&(framePosition!=number)) {
         res=av_read_frame(formatContext, pkt);
         framePosition++;
      }
   } else {
      // we must start at stream beginning to get the real frame num index
      av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
      framePosition=0;
      while ((res==0)&&(framePosition!=number)) {
         res=av_read_frame(formatContext, pkt);
         framePosition++;
      }
   }
}

void FrameList::displayFrame(int number) {
   int frameDecoded;
   static struct SwsContext *img_convert_ctx;

   // item numbering starts a 0, and frames at 1
   number++;

   seekFrame(number);
   avcodec_decode_video(codecContext, frame, &frameDecoded, pkt->data, pkt->size);
   img_convert_ctx = sws_getContext(codecContext->width,codecContext->height,codecContext->pix_fmt,codecContext->width,codecContext->height,PIX_FMT_RGB24,SWS_BICUBIC,NULL, NULL, NULL);
   sws_scale(img_convert_ctx, frame->data,frame->linesize,0,codecContext->height,frameRGB->data, frameRGB->linesize);
   frameDisplay->setFrame(codecContext->width,codecContext->height,frameRGB);
   frameDisplay->update();
   //cerr << frameDecoded << endl;
}

void FrameList::nop(int tmp) {
   // nop
}

bool FrameList::skeepFrame(int number) {
   // check item state
   QListWidgetItem* current;
   current=item(number-1);
   if(current->checkState()==Qt::Checked)
      return(true);
   return(false);
}

int FrameList::getFrameNumber() {
   // returns frameNumber
   return(frameNumber);
}
