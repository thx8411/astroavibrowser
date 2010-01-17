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

Histogram::Histogram(QWidget* parent) : QWidget(parent) {
   painter_ = new QPainter();
   values_=NULL;
   average_=NULL;
   setFixedSize(128,96);
}

Histogram::~Histogram() {
   free(average_);
   free(average_);
   delete painter_;
}

void Histogram::setAverage(int* values) {
   free(average_);
   average_=(int*)malloc(256*sizeof(int));
   memcpy(average_,values,256*sizeof(int));
}

void Histogram::setValues(int* values) {
   free(values_);
   values_=(int*)malloc(256*sizeof(int));
   memcpy(values_,values,256*sizeof(int));
}

void Histogram::paintEvent(QPaintEvent * ev) {
   painter_->begin(this);
   painter_->setClipRegion(ev->region());
   //
   if(average_!=NULL) {
      //
   }
   if(values_!=NULL) {
      //
   }
   painter_->end();
}
