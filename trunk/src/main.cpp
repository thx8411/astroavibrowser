/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include <iostream>

#include <Qt/qapplication.h>
#include <Qt/qmainwindow.h>

#include "version.hpp"

#include "Qmainwindow.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   // init messages
   cout << "<init>" << Name << " " << Version << endl;
   cout << "<init>" << "Build "<< Build << endl;
   cout << "<init>" << Web << endl;
   cout << "<init>" << Mail << endl << endl;
   // qapp creation
   QApplication app(argc, argv);
   MainWindow w;
   // display and loop
   w.show();
   return app.exec();
}
