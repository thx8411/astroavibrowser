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

#include "Qhistogram.moc"

#define W_SIZE	134
#define H_SIZE	120

Histogram::Histogram(QWidget* parent) : QWidget(parent) {
   painter_ = new QPainter();
   redPen_= new QPen(Qt::red);
   blackPen_=new QPen(Qt::black);
   max=0;
   values_=NULL;
   average_=NULL;
   setFixedSize(W_SIZE,H_SIZE);
}

Histogram::~Histogram() {
   free(average_);
   free(average_);
   delete blackPen_;
   delete redPen_;
   delete painter_;
}

void Histogram::setAverage(int* values) {
   free(average_);
   average_=(int*)malloc(256*sizeof(int));
   memcpy(average_,values,256*sizeof(int));
   max=getMax();
   max*=2;
}

void Histogram::setValues(int* values) {
   free(values_);
   values_=(int*)malloc(256*sizeof(int));
   memcpy(values_,values,256*sizeof(int));
   max=getMax();
   max*=2;
}

void Histogram::paintEvent(QPaintEvent * ev) {
   int i;
   int h;
   painter_->begin(this);
   painter_->setPen(*blackPen_);
   painter_->eraseRect(0,0,W_SIZE,H_SIZE);
   painter_->drawRect(1,1,W_SIZE-2,H_SIZE-20);
   painter_->drawText(8,H_SIZE-4,QString("Frame"));
   painter_->setPen(*redPen_);
   painter_->drawText(W_SIZE/2+4,H_SIZE-4,QString("Average"));
   painter_->setPen(*blackPen_);
   if(values_!=NULL) {
      for(i=0;i<128;i++) {
         if(max!=0) {
               h=(values_[i*2]+values_[i*2+1])*(H_SIZE-24)/max;
               if(h!=0)
                  painter_->drawLine(i+3,H_SIZE-22,i+3,H_SIZE-22-h);
         }
      }
   }
   if(average_!=NULL) {
      painter_->setPen(*redPen_);
      for(i=0;i<128;i++) {
         if(max!=0)
            h=(average_[i*2]+average_[i*2+1])*(H_SIZE-24)/max;
            if(h!=0)
               painter_->drawPoint(i+3,H_SIZE-22-h);
      }
   }
   painter_->end();
}

// returns the biggest value from the two arrays
int Histogram::getMax() {
   int maxA=0;
   int maxV=0;
   int i;
   if(average_!=NULL)
   for(i=0;i<256;i++) {
      if(average_[i]>maxA)
         maxA=average_[i];
   }
   if(values_!=NULL)
   for(i=0;i<256;i++) {
      if(values_[i]>maxV)
         maxV=values_[i];
   }
   if(maxA>maxV)
      return(maxA);
   return(maxV);
}
