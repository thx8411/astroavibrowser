/*
 * copyright (c) 2009-2013 Blaise-Florentin Collin
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

#include <string.h>
#include <iostream>

#include <Qt/qmessagebox.h>
#include <QtGui/QProgressDialog>

#include "Qframelist.hpp"

#include "processing.hpp"

using namespace std;


/********************/
/* FwhmTable object */
/********************/

// build and fwhm table for 'size' elements
FwhmTable::FwhmTable(int nsize) {
   // sanity check
   if(size<1) {
      fprintf(stderr,"Fwhm table should contain at least 1 element, leaving...\n");
      exit(1);
   }

   size=nsize;
   // table allocation
   table=(FwhmNode*)malloc(size*sizeof(FwhmNode));
   if(!table) {
      fprintf(stderr,"No more memory, leaving...\n");
      exit(1);
   }
   // table init
   memset(table,0,size*sizeof(FwhmNode));
   // not sorted
   sorted=false;
}

// destructor
FwhmTable::~FwhmTable() {
   // releaseing table
   free(table);
}

// sort the table (lower to higher)
void FwhmTable::sort() {
   // basic sort at this time, will be changed

   // checks if allready sorted
   if(!sorted) {
      // vars
      int i,j;
      int minIndex;
      double min;
      FwhmNode temp;
      QProgressDialog* progress;

      // setting progress dialog
      progress = new QProgressDialog(QString("Frame sorting..."),QString(),0,size);

      // for each element...
      for(i=0;i<size;i++) {
         // init
         minIndex=i;
         min=table[minIndex].frameFwhm;
         // looking for the smallest
         for(j=i+1;j<size;j++) {
            if(table[j].frameFwhm<min) {
               minIndex=j;
               min=table[minIndex].frameFwhm;
            }
         }
         // switching the current element with the smallest
         temp.frameIndex=table[i].frameIndex;
         temp.frameFwhm=table[i].frameFwhm;
         table[i].frameIndex=table[minIndex].frameIndex;
         table[i].frameFwhm=table[minIndex].frameFwhm;
         table[minIndex].frameIndex=temp.frameIndex;
         table[minIndex].frameFwhm=temp.frameFwhm;

         // progress update
         progress->setValue(i);
      }

      // closing progress bar
      progress->close();
      delete(progress);
      sorted=true;
   }
}

// set the node
int FwhmTable::setNode(int index, int frame, double value) {
   if((index<0)||(index>=size))
      return(-1);
   table[index].frameIndex=frame;
   table[index].frameFwhm=value;
   sorted=false;
   return(0);
}

// get frame number at given index
int FwhmTable::getFrame(int index) {
   if((index<0)||(index>=size))
      return(-1);
   return(table[index].frameIndex);
}

// get fwhm value at given index
double FwhmTable::getFwhm(int index) {
   if((index<0)||(index>=size))
      return(-1.0);
   return(table[index].frameFwhm);
}

// returns the table size
int FwhmTable::getSize() {
   return(size);
}

// display, for debug purpose
void FwhmTable::display() {
   int i;

   for(i=0;i<size;i++)
      fprintf(stderr,"%i : %f\n",table[i].frameIndex,table[i].frameFwhm);
   fprintf(stderr,"\n");
}

/********************/
/* FrameList object */
/********************/

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
   pkt->data=NULL;
   // set max list width
   setMaximumWidth(135);
   // init frame number
   frameNumber=0;
   // no file
   fileOpened=false;
   // no frame
   frameOk=false;

   // fwhm init
   fwhmList=NULL;
   fwhmFeeded=false;
}

FrameList::~FrameList() {
   // free scale context
   sws_freeContext(img_convert_ctx);
   // free mem
   av_free(frame);
   av_free(frameRGB);
   // release packet
   if(pkt) {
      if(pkt->data)
         av_free_packet(pkt);
      av_free(pkt);
   }
   // release buffer
   av_free(buffer);

   // fwhm release
   if(fwhmList)
      delete(fwhmList);
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

void FrameList::autoSelection(int s) {
   // gets the sharpest frames based on an fwhm on each
   // frame. 's' represents the percent of frame to keep
   unsigned char* datas;
   AVFrame* savedFrame;
   QProgressDialog* progress;
   int i,limit;
   double fwhmValue=0.0;

   // uncheck the list
   unSelectAll();

   // setting progress dialog
   progress = new QProgressDialog(QString("Computing sharpness..."),QString(),0,frameNumber);

   // feeds the fwhm list
   if(!fwhmFeeded) {
      // alloc datas if needed
      datas=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
      // loop
      for(i=0;i<frameNumber;i++) {
         // if we have a frame
         if(getFrame(i)) {
            savedFrame=frameRGB;
            // apply raw filter
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(datas,savedFrame->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(datas,savedFrame->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(datas,codecContext->width,codecContext->height);
            // compute fwhm
            fwhmValue=getFwhm(codecContext->width,codecContext->height,datas);
         }
         fwhmList->setNode(i,i,fwhmValue);

         // progress update
         progress->setValue(i);
      }
      fwhmFeeded=true;
      // free datas
      free(datas);
      // back to the beginning
      seekFrame(0);
      // select it
      item(0)->setSelected(true);
      // and display it
      displayFrame(0);
   }

   // closing progress dialog
   progress->close();
   delete(progress);

   // sort the list
   fwhmList->sort();

   // checks the frame
   limit=round((double)frameNumber/100.0*(double)s);
   for(i=0;i<limit;i++) {
      item(fwhmList->getFrame(i))->setCheckState(Qt::Checked);
   }
}

void FrameList::setFormatContext(AVFormatContext* fc) {
   formatContext=fc;
}

void FrameList::setCodecContext(AVCodecContext* cc) {
   int numBytes;

   codecContext=cc;
   // release buffer if needed
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
   int frameDecoded=0;
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
      if((avcodec_decode_video2(codecContext, frame, &frameDecoded, pkt)!=0)&&(frameDecoded!=0)) {
         i++;
         progress->setValue(i);
         // add item...
         current= new QListWidgetItem(itemName+QString::number(i),this);
         // and check it
         current->setFlags(current->flags()|Qt::ItemIsUserCheckable);
         current->setCheckState(Qt::Checked);
      }
      // read new frame
      av_free_packet(pkt);
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

   // create the fwhm list
   fwhmList=new FwhmTable(frameNumber);
   fwhmFeeded=false;
}

void FrameList::reset() {
   // remove all items
   if(count()!=0)
      clear();
   if(fwhmList) {
      delete(fwhmList);
      fwhmList=NULL;
   }
   fwhmFeeded=false;
}

bool FrameList::seekFrame(int number) {
   int frameDecoded;
   int res=0;

   // go to first frame
   if((number==0)||(number<framePosition)) {
      av_seek_frame(formatContext, -1, 0, AVSEEK_FLAG_ANY);
      framePosition=0;
   }
   while (framePosition<number) {
      av_free_packet(pkt);
      res=av_read_frame(formatContext, pkt);
      framePosition++;
   }
   // just read the frame
   av_free_packet(pkt);
   res=av_read_frame(formatContext, pkt);
   framePosition++;

   // decode frame
   res=avcodec_decode_video2(codecContext, frame, &frameDecoded, pkt);
   //cerr << frameDecoded << endl;
   frameOk=(res>0)&&(frameDecoded!=0);
   return(frameOk);
}

/*void getPlan(unsigned char* dest, unsigned char* source, int size, int colorPlan) {
   // red plan is RED=1
   colorPlan--;
   for(int i=0;i<size;i++) {
      dest[i]=source[(i*3)+colorPlan];
   }
}*/

void FrameList::dump(FileWriter* file) {
   unsigned char* datas;
   AVFrame* savedFrame;
   QProgressDialog* progress;
   QListWidgetItem* current;

   // setting progress dialog
   progress = new QProgressDialog(QString("File saving..."),QString(),0,frameNumber);
   // alloc datas if needed
   datas=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   // loop
   for(int i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            savedFrame=frameRGB;
            // apply raw filter
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(datas,savedFrame->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(datas,savedFrame->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(datas,codecContext->width,codecContext->height);
            // add the frame
            file->AddFrame(datas);
         }
      }
   }
   // free datas
   free(datas);
   // back to the beginning
   seekFrame(0);
   // closing progress window
   progress->close();
   delete progress;
}

// save darks and flats

void FrameList::sumGrey(FileWriter* file) {
int i,j;
   unsigned char* savedFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long));
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute frame sum..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // add frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j]+=*(frameRGB->data[0]+j*3+1);
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build sum frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=clip(datas[j]);
      savedFrame[j*3+1]=savedFrame[j*3];
      savedFrame[j*3+2]=savedFrame[j*3];
   }

   file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::sumRGB(FileWriter* file) {
   int i,j;
   unsigned char* savedFrame;
   unsigned char* newFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   newFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long)*3);
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute frame sum..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // add frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*3]+=newFrame[j*3];
               datas[j*3+1]+=newFrame[j*3+1];
               datas[j*3+2]+=newFrame[j*3+2];
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build sum frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=clip(datas[j*3]);
      savedFrame[j*3+1]=clip(datas[j*3+1]);
      savedFrame[j*3+2]=clip(datas[j*3+2]);
   }

   file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);
   free(newFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::maxGrey(FileWriter* file) {
int i,j;
   unsigned char* savedFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long));
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute max frame..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // max frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               if(datas[j]<=*(frameRGB->data[0]+j*3+1)) datas[j]=*(frameRGB->data[0]+j*3+1);
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build max frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=datas[j];
      savedFrame[j*3+1]=savedFrame[j*3];
      savedFrame[j*3+2]=savedFrame[j*3];
   }

   file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::maxRGB(FileWriter* file) {
   int i,j;
   unsigned char* savedFrame;
   unsigned char* newFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   newFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long)*3);
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute max frame..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // max frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               if(datas[j*3]<newFrame[j*3]) datas[j*3]=newFrame[j*3];
               if(datas[j*3+1]<newFrame[j*3+1]) datas[j*3+1]=newFrame[j*3+1];
               if(datas[j*3+2]<newFrame[j*3+2]) datas[j*3+2]<newFrame[j*3+2];
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build max frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=datas[j*3];
      savedFrame[j*3+1]=datas[j*3+1];
      savedFrame[j*3+2]=datas[j*3+2];
   }

   file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);
   free(newFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::darkFlatGreyMean(FileWriter* file) {
   int i,j,counter;
   unsigned char* savedFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   counter=0;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long));
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute mean frame..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            counter++;
            // add frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j]+=*(frameRGB->data[0]+j*3+1);
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build mean frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=datas[j]/counter;
      savedFrame[j*3+1]=savedFrame[j*3];
      savedFrame[j*3+2]=savedFrame[j*3];
   }

   if(counter<2)
      QMessageBox::information(this, tr("AstroAviBrowser"),"Less than 2 frames selected, aborted");
   else
      file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::darkFlatRgbMean(FileWriter* file) {
   int i,j,counter;
   unsigned char* savedFrame;
   unsigned char* newFrame;
   long* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   counter=0;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   newFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(long*)malloc(codecContext->width*codecContext->height*sizeof(long)*3);
   memset(datas,0,codecContext->width*codecContext->height*sizeof(long));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute mean frame..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            counter++;
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // add frames
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*3]+=newFrame[j*3];
               datas[j*3+1]+=newFrame[j*3+1];
               datas[j*3+2]+=newFrame[j*3+2];
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   //
   // build mean frame
   //
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      savedFrame[j*3]=datas[j*3]/counter;
      savedFrame[j*3+1]=datas[j*3+1]/counter;
      savedFrame[j*3+2]=datas[j*3+2]/counter;
   }

   if(counter<2)
      QMessageBox::information(this, tr("AstroAviBrowser"),"Less than 2 frames selected, aborted");
   else
      file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);
   free(newFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::darkFlatGreyMedian(FileWriter* file) {
   int i,j,counter;
   unsigned char* savedFrame;
   unsigned short* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   counter=0;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   datas=(unsigned short*)malloc(codecContext->width*codecContext->height*256*sizeof(unsigned short));
   if(datas==NULL) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Not enough memory, leaving"));
      exit(1);
   }
   memset(datas,0,codecContext->width*codecContext->height*256*sizeof(unsigned short));

   // setting progress dialog
   progress = new QProgressDialog(QString("Compute median frame..."),QString(),0,frameNumber);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            counter++;
            // build data table
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*256+(*(frameRGB->data[0]+j*3+1))]++;
            }
         }
      }
   }
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   // build median frame
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      int k=0;
      int sum=0;
      while((sum<(counter/2))&&(k<255)) {
         sum+=datas[j*256+k];
         k++;
      }
      savedFrame[j*3]=k;
      savedFrame[j*3+1]=k;
      savedFrame[j*3+2]=k;
   }

   if(counter<2)
      QMessageBox::information(this, tr("AstroAviBrowser"),"Less than 2 frames selected, aborted");
   else
      file->AddFrame(savedFrame);

   free(datas);
   free(savedFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

void FrameList::darkFlatRgbMedian(FileWriter* file) {
   int i,j,counter;
   unsigned char* savedFrame;
   unsigned char* newFrame;
   unsigned short* datas;
   QListWidgetItem* current;
   QProgressDialog* progress;

   counter=0;

   setCursor(Qt::BusyCursor);

   savedFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   newFrame=(unsigned char*)malloc(codecContext->width*codecContext->height*3);

   datas=(unsigned short*)malloc(codecContext->width*codecContext->height*256*sizeof(unsigned short));
   if(datas==NULL) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Not enough memory, leaving"));
      exit(1);
   }
   // setting progress dialog
   progress = new QProgressDialog(QString("Compute median frame..."),QString(),0,frameNumber*3);

   // Red loop
   memset(datas,0,codecContext->width*codecContext->height*256*sizeof(unsigned short));
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // build data table
            counter++;
            // build data table
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*256+newFrame[j*3]]++;
            }
         }
      }
   }
   // build median frame (Red)
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      int k=0;
      int sum=0;
      while((sum<(counter/2))&&(k<255)) {
         sum+=datas[j*256+k];
         k++;
      }
      savedFrame[j*3]=k;
   }
   // back to the beginning
   seekFrame(0);

// Green loop
   memset(datas,0,codecContext->width*codecContext->height*256*sizeof(unsigned short));
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i+frameNumber);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // build data table
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*256+newFrame[j*3+1]]++;
            }
         }
      }
   }
   // build median frame (Green)
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      int k=0;
      int sum=0;
      while((sum<(counter/2))&&(k<255)) {
         sum+=datas[j*256+k];
         k++;
      }
      savedFrame[j*3+1]=k;
   }
   // back to the beginning
   seekFrame(0);

// Blue loop
   memset(datas,0,codecContext->width*codecContext->height*256*sizeof(unsigned short));
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i+frameNumber+frameNumber);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            // raw to rgb if needed
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(newFrame,frameRGB->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(newFrame,frameRGB->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(newFrame,codecContext->width,codecContext->height);
            // build data table
            for(j=0;j<(codecContext->width*codecContext->height);j++) {
               datas[j*256+newFrame[j*3+2]]++;
            }
         }
      }
   }
   // build median frame (Blue)
   for(j=0;j<(codecContext->width*codecContext->height);j++) {
      int k=0;
      int sum=0;
      while((sum<(counter/2))&&(k<255)) {
         sum+=datas[j*256+k];
         k++;
      }
      savedFrame[j*3+2]=k;
   }

   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;

   if(counter<2)
      QMessageBox::information(this, tr("AstroAviBrowser"),"Less than 2 frames selected, aborted");
   else
      file->AddFrame(savedFrame);

   free(savedFrame);
   free(newFrame);

   setCursor(Qt::ArrowCursor);

   return;
}

int* FrameList::getAverage() {
   int i,j;
   int* values=NULL;
   int* tmpValues;
   long sum[256];
   unsigned char* datas;
   AVFrame* savedFrame;
   QListWidgetItem* current;
   QProgressDialog* progress;

   setCursor(Qt::BusyCursor);

   memset(sum,0,256*sizeof(long));
   values=(int*)malloc(256*sizeof(int));
   //
   // setting progress dialog
   progress = new QProgressDialog(QString("Compute average histogram..."),QString(),0,frameNumber);
   // alloc datas if needed
   datas=(unsigned char*)malloc(codecContext->width*codecContext->height*3);
   // loop
   for(i=0;i<frameNumber;i++) {
      // update progress bar
      progress->setValue(i);
      // if current frame checked...
      current=item(i);
      if(current->checkState()==Qt::Checked) {
         // if we have a frame
         if(getFrame(i)) {
            savedFrame=frameRGB;
            // apply raw filter
            if(frameDisplay->getRawmode()==RAW_NONE)
               memcpy(datas,savedFrame->data[0],codecContext->width*codecContext->height*3);
            else
               raw2rgb(datas,savedFrame->data[0],codecContext->width,codecContext->height,frameDisplay->getRawmode());
            bgr2rgb(datas,codecContext->width,codecContext->height);
            // add histogram
            tmpValues=getHistogram(codecContext->width,codecContext->height,datas);
            for(j=0;j<256;j++)
               sum[j]+=tmpValues[j];
            free(tmpValues);
         }
      }
   }
   // free datas
   free(datas);
   // back to the beginning
   seekFrame(0);
   // init the first item
   current=item(0);
   current->setSelected(true);
   // and display it
   displayFrame(0);
   // closing progress window
   progress->close();
   delete progress;
   //
   for(i=0;i<256;i++)
      values[i]=sum[i]/frameNumber;

   setCursor(Qt::ArrowCursor);

   return(values);
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
