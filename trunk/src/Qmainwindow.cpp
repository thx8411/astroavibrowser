/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include <iostream>

#include <Qt/qmenu.h>
#include <Qt/qmenubar.h>
#include <Qt/qmessagebox.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>

#include "Qmainwindow.moc"

using namespace std;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
   //
   //   GUI
   //

   //   MENU
   // actions
   open = new QAction("&Open...",this);
   save = new QAction("&Save...",this);
   quit = new QAction("&Quit", this);
   properties = new QAction("&Properties...",this);
   format = new QAction("Output F&ormat...",this);
   codec = new QAction("Output &Codec...",this);
   // file menu
   QMenu* file;
   file = menuBar()->addMenu("&File");
   file->addAction(open);
   file->addAction(save);
   file->addSeparator();
   file->addAction(quit);
   // video menu
   QMenu* video;
   video = menuBar()->addMenu("&Video");
   video->addAction(properties);
   video->addAction(format);
   video->addAction(codec);
   // signal connections
   connect(open, SIGNAL(triggered()), this, SLOT(MenuOpen()));
   connect(save, SIGNAL(triggered()), this, SLOT(MenuSave()));
   connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
   connect(properties, SIGNAL(triggered()), this, SLOT(MenuProperties()));
   connect(format, SIGNAL(triggered()), this, SLOT(MenuFormat()));
   connect(codec, SIGNAL(triggered()), this, SLOT(MenuCodec()));

   // CENTRAL ZONE
   // widgets and layouts
   QWidget* centralZone = new QWidget(this);
   QHBoxLayout* central = new QHBoxLayout;
   QVBoxLayout* left = new QVBoxLayout;
   QVBoxLayout* buttons = new QVBoxLayout;
   // buttons
   selectAll = new QPushButton("Select All");
   unSelectAll = new QPushButton("Unselect All");
   invertSelection = new QPushButton("Invert Selection");
   // set buttons size (should be ok for 1024*600 with PAL format)
   selectAll->setMaximumWidth(128);
   selectAll->setMinimumWidth(128);
   unSelectAll->setMaximumWidth(128);
   unSelectAll->setMinimumWidth(128);
   invertSelection->setMaximumWidth(128);
   invertSelection->setMinimumWidth(128);
   // disabling widgets (no file loaded at this time)
   save->setEnabled(false);
   properties->setEnabled(false);
   format->setEnabled(false);
   codec->setEnabled(false);
   selectAll->setEnabled(false);
   unSelectAll->setEnabled(false);
   invertSelection->setEnabled(false);
   // frame display and list
   frameDisplay= new FrameDisplay(centralZone);
   frameDisplay->setMinimumWidth(640);
   frameDisplay->setMinimumHeight(480);
   frameDisplay->setAutoFillBackground( true );
   frameList= new FrameList(centralZone);
   frameList->setFrameDisplay(frameDisplay);
   // set frame display background to black
   QPalette pal = frameDisplay->palette();
   pal.setColor(frameDisplay->backgroundRole(), Qt::black);
   frameDisplay->setPalette(pal);
   // connections
   connect(selectAll, SIGNAL(released()), this, SLOT(ButtonSelectAll()));
   connect(unSelectAll, SIGNAL(released()), this, SLOT(ButtonUnSelectAll()));
   connect(invertSelection, SIGNAL(released()), this, SLOT(ButtonInvert()));
   // add widgets and layouts
   buttons->addWidget(selectAll);
   buttons->addWidget(unSelectAll);
   buttons->addWidget(invertSelection);
   left->addWidget(frameList);
   left->addLayout(buttons);
   central->addLayout(left);
   central->addWidget(frameDisplay);
   centralZone->setLayout(central);
   // set and display
   setCentralWidget(centralZone);
   QPixmap* pixmap = new QPixmap("../icons/main-icon.png");
   setWindowIcon(*pixmap);
   frameDisplay->show();
   frameList->show();
   centralZone->show();
   setFixedSize(sizeHint());

   //
   //   FFMPEG
   //

   // register all formats and codecs
   av_register_all();
   // ffmpeg datas init
   inputFileFormatContext=NULL;
   inputFileCodec=NULL;
}

MainWindow::~MainWindow() {
   // release opened file
   freeFile();
}

void MainWindow::freeFile() {
   // close codec
   if(inputFileCodec!=NULL)
      avcodec_close(inputFileCodecContext);
   // close file
   if(inputFileFormatContext!=NULL)
      av_close_input_file(inputFileFormatContext);
   // disabling widgets (no more file opened)
   save->setEnabled(false);
   properties->setEnabled(false);
   format->setEnabled(false);
   codec->setEnabled(false);
   selectAll->setEnabled(false);
   unSelectAll->setEnabled(false);
   invertSelection->setEnabled(false);
}

//
// MENU CALLBACKS
//

// OPEN
void MainWindow::MenuOpen() {
   // release opened file
   freeFile();
   // open new file
   // getting file name
   inputFileName = QFileDialog::getOpenFileName(this,tr("Open Video"),"/home", tr("Avi Files (*.avi)"));
   if(inputFileName=="")
      return;

   // opening file
   if(av_open_input_file(&inputFileFormatContext, inputFileName.toStdString().c_str(), NULL, 0, NULL)!=0) {
      // Couldn't open file
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to open the file"));
      return;
   }
   cerr << "Opening file " << inputFileName.toStdString().c_str() << endl;

   // get streams
   if(av_find_stream_info(inputFileFormatContext)<0) {
      // Couldn't find stream information
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to get file streams"));
      freeFile();
      return;
   }
   // file infos on cerr
   //dump_format(inputFileFormatContext, 0, inputFileName.toStdString().c_str(), false);
   //cerr << endl;

   // open stream
   inputStreamNumber=-1;
   for(int i=0;i<inputFileFormatContext->nb_streams;i++) {
      if(inputFileFormatContext->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
      inputStreamNumber=i;
      break;
      }
   }
   if(inputStreamNumber==-1) {
      // no video stream
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No video stream found"));
      freeFile();
      return;
   }
   //cerr << "Using stream " << inputStreamNumber << endl;

   // load codec
   inputFileCodecContext=inputFileFormatContext->streams[inputStreamNumber]->codec;
   inputFileCodec=avcodec_find_decoder(inputFileCodecContext->codec_id);
   if(inputFileCodec==NULL) {
      // no codec found
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No codec found for this stream"));
      freeFile();
      return;
   }
   // enable truncated codec
   if(inputFileCodec->capabilities & CODEC_CAP_TRUNCATED)
        inputFileCodecContext->flags|=CODEC_FLAG_TRUNCATED;
   if(avcodec_open(inputFileCodecContext, inputFileCodec)<0) {
      // could not open codec
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to load the codec"));
      freeFile();
      return;
   }
   //cerr << "Using codec :" << inputFileCodecContext->codec_name << endl;

   // frame list update
   frameList->setFormatContext(inputFileFormatContext);
   frameList->setCodecContext(inputFileCodecContext);
   // it could be long...
   setCursor(Qt::BusyCursor);
   frameList->fill();
   setCursor(Qt::ArrowCursor);

   // enabling widgets
   save->setEnabled(true);
   properties->setEnabled(true);
   //format->setEnabled(true);
   //codec->setEnabled(true);
   selectAll->setEnabled(true);
   unSelectAll->setEnabled(true);
   invertSelection->setEnabled(true);

   // fixing new size (to times to get refresh...)
   setFixedSize(sizeHint());
   setFixedSize(sizeHint());
}

// SAVE
void MainWindow::MenuSave() {
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",tr("Avi Files (*.avi)"));
   if(outputFileName=="")
      return;
}

// GET INPUT STREAM PROPERTIES
void MainWindow::MenuProperties() {
   //cout << "properties" << endl;
}

// SET OUTPUT STREAM FORMAT (YUV, RGB, etc...)
void MainWindow::MenuFormat() {
   //cout << "format" << endl;
}

// SET OUTPUT STREAM FORMAT (RAW, LOSSLESS)
void MainWindow::MenuCodec() {
   //cout << "codec" << endl;
}

//
// BUTTONS CALLBACKS
//

void MainWindow::ButtonSelectAll() {
   // select all frames
   frameList->selectAll();
}

void MainWindow::ButtonUnSelectAll() {
   // unselect all frames
   frameList->unSelectAll();
}

void MainWindow::ButtonInvert() {
   // invert frame selection
   frameList->invertSelection();
}
