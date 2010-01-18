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

#ifndef _QHISTOGRAM_HPP_
#define _QHISTOGRAM_HPP_

#include <iostream>

#include <Qt/qwidget.h>
#include <Qt/qpainter.h>
#include <Qt/qpen.h>

#include <QtGui/QPaintEvent>

#include "config.h"

using namespace std;

class Histogram : public QWidget
{
      Q_OBJECT
   public :
      Histogram(QWidget* parent=0);
      ~Histogram();
      // set the average histogram
      void setAverage(int* values);
      // get the frame
      void setValues(int* values);
   protected:
      void paintEvent(QPaintEvent * ev);
   private :
      int max;
      int* values_;
      int* average_;
      QPainter* painter_;
      QPen* redPen_;
      QPen* blackPen_;
      // functions
      int getMax();
};

#endif
