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

#include <fcntl.h>

#include <iostream>

#include <Qt/qapplication.h>
#include <Qt/qmainwindow.h>

#include "config.h"

#include "version.hpp"

#include "Qmainwindow.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   string logFileName(".astroavibrowser.log");

   // cout redirection
   FILE* logFile=freopen(logFileName.c_str(),"w",stdout);

   // qapp creation
   QApplication* app=new QApplication(argc, argv);

   MainWindow* w=new MainWindow();
   // display and loop
   w->show();

   // testing parameters
   if(argc>1) {
      // if a file is passed, we open it
      w->openFile(QString(argv[1]));
   }

   app->exec();

   // finish
   delete w;
   delete app;
   return(0);
}
