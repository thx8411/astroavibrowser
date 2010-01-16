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

#include "bayer.hpp"

#include "Qframedisplay.moc"

FrameDisplay::FrameDisplay(QWidget* parent) : QWidget(parent) {
   painter_ = new QPainter();
   frameData=NULL;
   rawMode=RAW_NONE;
   resize(640,480);
}

FrameDisplay::~FrameDisplay() {
   delete painter_;
}

void FrameDisplay::setFrame(int width, int height, AVFrame* f) {
   frameWidth=width;
   frameHeight=height;
   frameData=f;
   resize(width,height);
   repaint();
}

AVFrame* FrameDisplay::getFrame() {
   return(frameData);
}

void FrameDisplay::setRawmode(int mode) {
   rawMode=mode;
   applyRawMode();
   repaint();
}

int FrameDisplay::getRawmode() {
   return(rawMode);
}

void FrameDisplay::paintEvent(QPaintEvent * ev) {
   if (frameData!=NULL) {
      // if we have a frame...
      applyRawMode();
      painter_->begin(this);
      painter_->setClipRegion(ev->region());
      // load frame in widget
      QImage* tmpImage=new QImage((unsigned char*)frameData->data[0],frameWidth,frameHeight,QImage::Format_RGB888);
      painter_->drawImage(0,0,*tmpImage);
      delete tmpImage;
      painter_->end();
   } else {
      // no frame...
      painter_->begin(this);
      painter_->setClipRegion(ev->region());
      // black rect
      painter_->fillRect(0,0,width(),height(),Qt::black);
      painter_->end();
   }
}

void FrameDisplay::applyRawMode() {
   unsigned char* buffer;
   if ((frameData!=NULL)&&(rawMode!=RAW_NONE)) {
         buffer=(unsigned char*)malloc(frameWidth*frameHeight*3);
         memcpy(buffer,frameData->data[0],frameWidth*frameHeight*3);
         raw2rgb(frameData->data[0],buffer,frameWidth,frameHeight,rawMode);
         free(buffer);
   }
}
