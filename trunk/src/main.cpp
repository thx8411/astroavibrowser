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

#include <sys/fcntl.h>

#include <iostream>

#include <Qt/qapplication.h>
#include <Qt/qmainwindow.h>

#include "config.h"

#include "version.hpp"

#include "Qmainwindow.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   // init messages
   //cout << endl;
   //cout << "<init>" << Name << " " << Version << endl;
   //cout << "<init>" << "Build "<< Build << endl;
   //cout << "<init>" << Web << endl;
   //cout << "<init>" << Mail << endl << endl;
   // qapp creation
   QApplication* app=new QApplication(argc, argv);
   MainWindow* w=new MainWindow();
   // display and loop
   w->show();
   app->exec();

   delete w;
   delete app;
   return(0);
}
