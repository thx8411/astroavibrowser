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

#include <iostream>

#include <QtGui/QProgressDialog>

#include "Qframelist.moc"

#include "bayer.hpp"

using namespace std;

FrameList::FrameList(QWidget* parent) : QListWidget(parent) {
   // init
   img_convert_ctx=NULL;
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
   // init frame number
   frameNumber=0;
   // no file
   fileOpened=false;
   // no frame
   frameOk=false;
}

FrameList::~FrameList() {
   // free scale context
   if(img_convert_ctx!=NULL)
      sws_freeContext(img_convert_ctx);
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
   if(buffer!=NULL)
      av_free(buffer);
   // buffer and frameRGB settings
   numBytes=avpicture_get_size(PIX_FMT_RGB24, codecContext->width,codecContext->height);
   buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
   avpicture_fill((AVPicture *)frameRGB, buffer, PIX_FMT_RGB24,codecContext->width, codecContext->height);
   // setting img_convert_ctx
   if(img_convert_ctx!=NULL)
      sws_freeContext(img_convert_ctx);
   img_convert_ctx = sws_getContext(codecContext->width,codecContext->height,codecContext->pix_fmt,codecContext->width,codecContext->height
      ,PIX_FMT_RGB24,SWS_FAST_BILINEAR|SWS_CPU_CAPS_MMX,NULL, NULL, NULL);
}

void FrameList::setStreamNumber(int sn) {
   streamNumber=sn;
}

void FrameList::setFrameDisplay(FrameDisplay* fd) {
   frameDisplay=fd;
}

void FrameList::fill() {
   // init
   int i=0;
   int res=0;
   int frameNumberEstimation;
   QListWidgetItem* current;
   QProgressDialog* progress;
   // base frame name
   QString itemName("Frame ");
   // stop display on change
   connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(nop(int)));
   // clears the list
   reset();
   // var init
   frameNumber=0;
   framePosition=0;
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
   // read a new frame
   av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
   res=av_read_frame(formatContext, pkt);
   while (res==0) {
      i++;
      progress->setValue(i);
      // add item...
      current= new QListWidgetItem(itemName+QString::number(i),this);
      // and check it
      current->setFlags(current->flags()|Qt::ItemIsUserCheckable);
      current->setCheckState(Qt::Checked);
      // read new frame
      res=av_read_frame(formatContext, pkt);
   }
   // progress closing
   progress->close();
   delete progress;
   // back to stream beginning
   av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
   // compute frame number
   frameNumber=i;
   // set the slot back
   connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(displayFrame(int)));
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // file is open
   fileOpened=true;
}

void FrameList::reset() {
   // remove all items
   if(0!=count())
      clear();
}

bool FrameList::seekFrame(int number) {
   int frameDecoded;
   int res=0;

   if(number>framePosition) {
      // go forward
      res=av_read_frame(formatContext, pkt);
      if(res==0)
         framePosition++;
      while ((res==0)&&(framePosition!=number)) {
         res=av_read_frame(formatContext, pkt);
         framePosition++;
      }
   } else {
      // we must start at stream beginning to get the real frame num index
      av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
      framePosition=0;
      res=av_read_frame(formatContext, pkt);
      if(res==0)
         framePosition++;
      // counting
      while ((res==0)&&(framePosition!=number)) {
         res=av_read_frame(formatContext, pkt);
         framePosition++;
      }
   }
   // decode frame
   res=avcodec_decode_video(codecContext, frame, &frameDecoded, pkt->data, pkt->size);
   //cerr << frameDecoded << endl;
   frameOk=(res>0)&&(frameDecoded!=0);
   return(frameOk);
}

void getPlan(unsigned char* dest, unsigned char* source, int size, int colorPlan) {
   // red plan is RED=1
   colorPlan--;
   for(int i=0;i<size;i++) {
      dest[i]=source[(i*3)+colorPlan];
   }
}

void FrameList::dump(IAviVideoWriteStream* stream, BITMAPINFOHEADER* bi, int colorPlan) {
   unsigned char* datas;
   unsigned char* plan;
   int size;
   AVFrame* savedFrame;
   QProgressDialog* progress;
   QListWidgetItem* current;

   // setting progress dialog
   switch(colorPlan) {
      case ALL :
         progress = new QProgressDialog(QString("File saving..."),QString(),0,frameNumber);
         break;
      case RED :
         progress = new QProgressDialog(QString("Saving red plan..."),QString(),0,frameNumber);
         break;
      case GREEN :
         progress = new QProgressDialog(QString("Saving green plan..."),QString(),0,frameNumber);
         break;
      case BLUE :
         progress = new QProgressDialog(QString("Saving blue plan..."),QString(),0,frameNumber);
         break;
   }
   // alloc datas if needed
   size=codecContext->width*codecContext->height;
   if(colorPlan!=ALL)
      plan=(unsigned char*)malloc(size);
   datas=(unsigned char*)malloc(size*3);
   // loop
   for(int i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         CImage* img;
         BitmapInfo info(*bi);
         // if we have a frame
         if(getFrame(i)) {
            savedFrame=frameRGB;
            // apply raw filter
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(datas,savedFrame->data[0],size*3);
            else
               raw2rgb(datas,savedFrame->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(datas,codecContext->width,codecContext->height);
            // get frame datas
            switch(colorPlan) {
               case ALL :
                  plan=datas;
                  break;
               case RED :
                  // get red plan
                  getPlan(plan,datas,size,RED);
                  break;
               case GREEN :
                  // get green plan
                  getPlan(plan,datas,size,GREEN);
                  break;
               case BLUE :
                  // get blue plan
                  getPlan(plan,datas,size,BLUE);
                  break;
            }
            // add frame
            img= new CImage(&info, plan, true);
            stream->AddFrame(img);
            // free image
            delete img;
         }
      }
   }
   // free datas
   if(colorPlan!=ALL)
      free(plan);
   free(datas);
   // back to the beginning
   seekFrame(0);
   // closing progress window
   progress->close();
   delete progress;
}

void FrameList::refreshFrame() {
   if(fileOpened) {
      // if we have a frame
      if(frameOk) {
         // convert and display
         sws_scale(img_convert_ctx, frame->data,frame->linesize,0,codecContext->height,frameRGB->data, frameRGB->linesize);
         frameDisplay->setFrame(codecContext->width,codecContext->height,frameRGB);
      } else
         // or clear the display
         frameDisplay->setFrame(codecContext->width,codecContext->height,NULL);
      frameDisplay->update();
   }
}

void FrameList::displayFrame(int number) {
   // seek the frame
   if(seekFrame(number)) {
      // display it
      sws_scale(img_convert_ctx, frame->data,frame->linesize,0,codecContext->height,frameRGB->data, frameRGB->linesize);
      frameDisplay->setFrame(codecContext->width,codecContext->height,frameRGB);
   } else
      // or clear the display
      frameDisplay->setFrame(codecContext->width,codecContext->height,NULL);
   frameDisplay->update();
}

bool FrameList::getFrame(int number) {
   // seek the frame
   if(seekFrame(number)) {
      // if we have one, save it
      sws_scale(img_convert_ctx, frame->data,frame->linesize,0,codecContext->height,frameRGB->data, frameRGB->linesize);
      return(true);
   } else
      return(false);
}

void FrameList::nop(int tmp) {
   // nop
}

int FrameList::getFrameNumber() {
   // returns frameNumber
   return(frameNumber);
}

int FrameList::getSelectedFrames() {
   int num=0;
   // count the number of selected frames
   QListWidgetItem* current;
   for(int i=0;i<frameNumber;i++) {
      current=item(i);
      if(current->checkState()==Qt::Checked)
         num++;
   }
   return(num);
}
