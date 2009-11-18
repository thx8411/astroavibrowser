/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include <iostream>

#include <Qt/qmenu.h>
#include <Qt/qmenubar.h>
#include <Qt/qmessagebox.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>

#include "version.hpp"

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
   properties = new QAction("Get &Properties...",this);
   separateRgb= new QAction("RGB Separation",this);
   separateRgb->setCheckable(true);
   about = new QAction("&About...",this);
   // file menu
   QMenu* file;
   file = menuBar()->addMenu("&File");
   file->addAction(open);
   file->addAction(save);
   file->addSeparator();
   file->addAction(quit);
   // input menu
   QMenu* input;
   input = menuBar()->addMenu("&Input");
   input->addAction(properties);
   input->addSeparator();
   bayer= input->addMenu("Bayer Mode");
   // output menu
   QMenu* output;
   output = menuBar()->addMenu("O&utput");
   output->addAction(separateRgb);
   output->addSeparator();
   codec= output->addMenu("Output Codec");
   // help menu
   QMenu* help;
   help = menuBar()->addMenu("&Help");
   help->addAction(about);
   // signal connections
   connect(open, SIGNAL(triggered()), this, SLOT(MenuOpen()));
   connect(save, SIGNAL(triggered()), this, SLOT(MenuSave()));
   connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
   connect(properties, SIGNAL(triggered()), this, SLOT(MenuProperties()));
   connect(separateRgb, SIGNAL(triggered()), this, SLOT(setSeparate()));
   connect(about, SIGNAL(triggered()), this, SLOT(MenuAbout()));

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
   bayer->setEnabled(false);
   separateRgb->setEnabled(false);
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
   // update menu
   createBayerMenu();
   createCodecMenu();
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
   bayer->setEnabled(false);
   separateRgb->setEnabled(false);
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
   // open new file
   // getting file name
   inputFileName = QFileDialog::getOpenFileName(this,tr("Open Video"),"/home", tr("Video Files (*.avi *.mpg *.mpeg *.divx *.mkv *.mov *.wmv *.AVI *.MPG *.MPEG *.DIVX *.MKV *.MOV *.WMV)"));
   sleep(1);
   if(inputFileName=="")
      return;

   // release opened file
   freeFile();

   // opening file
   if(av_open_input_file(&inputFileFormatContext, inputFileName.toStdString().c_str(), NULL, 0, NULL)!=0) {
      // Couldn't open file
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to open the file"));
      return;
   }
   //cerr << "Opening file " << inputFileName.toStdString().c_str() << endl;

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
   frameList->setStreamNumber(inputStreamNumber);
   // it could be long...
   setCursor(Qt::BusyCursor);
   frameList->fill();
   setCursor(Qt::ArrowCursor);

   // enabling widgets
   save->setEnabled(true);
   properties->setEnabled(true);
   bayer->setEnabled(true);
   separateRgb->setEnabled(true);
   codec->setEnabled(true);
   selectAll->setEnabled(true);
   unSelectAll->setEnabled(true);
   invertSelection->setEnabled(true);

   // fixing new size (to times to get refresh...)
   setFixedSize(sizeHint());
   setFixedSize(sizeHint());

   // set bayer and codec
   setNone();
   //setRawrgb();
}

// SAVE
void MainWindow::MenuSave() {
   // is there selected frames ?
   if(frameList->getSelectedFrames()==0) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No frame selected"));
      return;
   }

   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",tr("Avi Files (*.avi *.AVI)"));
   // removes ".avi" postfix if needed
   if(outputFileName.contains(".avi",Qt::CaseInsensitive))
      outputFileName.remove(".avi",Qt::CaseInsensitive);

   // is the name valid ?
   if(outputFileName=="")
      return;

   // it could be long...
   setCursor(Qt::BusyCursor);
   // vars
   int frameRate;
   IAviWriteFile* outputFile;
   IAviVideoWriteStream *outputStream;
   BITMAPINFOHEADER bi;
   // init
   memset(&bi, 0, sizeof(bi));
   bi.biSize = sizeof(bi);
   bi.biWidth = inputFileCodecContext->width;
   bi.biHeight = inputFileCodecContext->height;
   frameRate=(float)inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.den/(float)inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.num*1000000;
   // separate or not
   if(separateRgb->isChecked()) {
      // init
      bi.biSizeImage = (bi.biWidth * bi.biHeight);
      bi.biPlanes = 1;
      bi.biBitCount = 8;
      bi.biCompression = IMG_FMT_Y800;

      // R plan
      outputFile=avm::CreateWriteFile((outputFileName.toStdString()+"_R.avi").c_str());
      outputStream=outputFile->AddVideoStream(RIFFINFO_Y800, &bi, frameRate);
      outputStream->SetQuality(10000);
      outputStream->Start();
      // frame list dump
      frameList->dump(outputStream, &bi, RED);
      // closing
      outputStream->Stop();
      delete outputFile;

      // G plan
      outputFile=avm::CreateWriteFile((outputFileName.toStdString()+"_G.avi").c_str());
      outputStream=outputFile->AddVideoStream(RIFFINFO_Y800, &bi, frameRate);
      outputStream->SetQuality(10000);
      outputStream->Start();
      // frame list dump
      frameList->dump(outputStream, &bi, GREEN);
      // closing
      outputStream->Stop();
      delete outputFile;

      // B plan
      outputFile=avm::CreateWriteFile((outputFileName.toStdString()+"_B.avi").c_str());
      outputStream=outputFile->AddVideoStream(RIFFINFO_Y800, &bi, frameRate);
      outputStream->SetQuality(10000);
      outputStream->Start();
      // frame list dump
      frameList->dump(outputStream, &bi, BLUE);
   } else {
      bi.biSizeImage = (bi.biWidth * bi.biHeight * 3);
      bi.biPlanes = 1;
      bi.biBitCount = 24;
      bi.biCompression = BI_RGB;
      //bi.biPlanes = 3;
      //bi.biBitCount = 12;
      //bi.biCompression=RIFFINFO_I420;

      outputFile=avm::CreateWriteFile((outputFileName.toStdString()+".avi").c_str());
      outputStream=outputFile->AddVideoStream(outputCodec, &bi, frameRate);
      outputStream->SetQuality(10000);
      outputStream->Start();

      // frame list dump
      frameList->dump(outputStream, &bi, ALL);
   }
   // closing
   outputStream->Stop();
   delete outputFile;
   // finished
   setCursor(Qt::ArrowCursor);
}

// GET INPUT STREAM PROPERTIES
void MainWindow::MenuProperties() {
   QString message;
   QString tmp;
   // display stream properties
   // frame size
   message+="Size : ";
   tmp.setNum(inputFileCodecContext->width,10);
   message+=tmp;
   message+="x";
   tmp.setNum(inputFileCodecContext->height,10);
   message+=tmp;
   message+="\n";
   // sequence duration
   message+="Duration : ";
   tmp.setNum(inputFileFormatContext->duration/AV_TIME_BASE,10);
   message+=tmp;
   message+=" s \n";
   // stream bitrate
   message+="Bitrate : ";
   tmp.setNum(inputFileFormatContext->bit_rate/1024,10);
   message+=tmp;
   message+=" Kbps \n";
   // stream codec
   message+="Codec : ";
   tmp=inputFileCodec->name;
   message+=tmp;
   message+="\n";
   // picture format
   message+="Format : ";
   tmp=avcodec_get_pix_fmt_name(inputFileCodecContext->pix_fmt);
   message+=tmp;
   message+="\n";
   // stream framerate
   message+="Framerate : ";
   tmp.setNum(inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.num/inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.den,10);
   message+=tmp;
   message+=" fps\n";
   // frame numbers
   message+="Frames : ";
   tmp.setNum(frameList->getFrameNumber(),10);
   message+=tmp;
   message+="\n";
   QMessageBox::information(this, tr("AstroAviBrowser"),tr(message.toStdString().c_str()));
}

// ABOUT...
void MainWindow::MenuAbout() {
   QString message;
   QString tmp;

   message+=Name;
   message+="\n";
   message+=Version;
   message+="\n";
   message+="Build : ";
   tmp.setNum(Build,10);
   message+=tmp;
   message+="\n\n";
   message+=Web;
   message+="\n";
   message+=Author;
   message+="\n";
   message+="(c) ";
   message+=Year;
   message+="\n";
   message+=Mail;
   message+="\n";
   QMessageBox::information(this, tr("AstroAviBrowser"),tr(message.toStdString().c_str()));
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

//
// bayer modes callbacks
//

void MainWindow::setNone() {
   bayerNone->setChecked(true);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_NONE);
   frameList->refreshFrame();
}

void MainWindow::setBg() {
   bayerNone->setChecked(false);
   bayerBg->setChecked(true);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_BG);
   frameList->refreshFrame();
}

void MainWindow::setGb() {
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(true);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_GB);
   frameList->refreshFrame();
}

void MainWindow::setRg() {
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(true);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_RG);
   frameList->refreshFrame();
}

void MainWindow::setGr() {
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(true);
   frameDisplay->setRawmode(RAW_GR);
   frameList->refreshFrame();
}

//
// rgb separation callback
//

void MainWindow::setSeparate() {
   if(separateRgb->isChecked()) {
      //setRawgrey();
      //codecSame->setEnabled(false);
      //codecRawrgb->setEnabled(false);
      codecLossless->setEnabled(false);
   } else {
      setRawrgb();
      //codecSame->setEnabled(true);
      //codecRawrgb->setEnabled(true);
      codecLossless->setEnabled(true);
   }
}

//
// output codec callbacks
//

//void MainWindow::setSame() {
//  codecSame->setChecked(true);
//   codecRawgrey->setChecked(false);
//   codecRawrgb->setChecked(false);
//   codecLossless->setChecked(false);

   // update codec
//   if(inputFileCodecContext!=NULL) {
//      outputCodecId=inputFileCodecContext->codec_id;
      // set pix format
//      outputFmt=inputFileCodecContext->pix_fmt;
//   }
//}

void MainWindow::setRawgrey() {
   //codecSame->setChecked(false);
   //codecRawgrey->setChecked(true);
   //codecRawrgb->setChecked(false);
   codecLossless->setChecked(false);

   // update codec
   outputCodec=RIFFINFO_Y800;
}

void MainWindow::setRawrgb() {
   //codecSame->setChecked(false);
   //codecRawgrey->setChecked(false);
   //codecRawrgb->setChecked(true);
   codecLossless->setChecked(false);

   // update codec
   outputCodec=BI_RGB;
}

void MainWindow::setLossless() {
   //codecSame->setChecked(false);
   //codecRawgrey->setChecked(false);
   //codecRawrgb->setChecked(false);
   codecLossless->setChecked(true);

   // update codec
   outputCodec=RIFFINFO_ZLIB;
}

//
// MENU CREATION TOOLS
//

void MainWindow::createBayerMenu() {
   bayerNone=new QAction("None",bayer);
   bayerBg=new QAction("Raw BG",bayer);
   bayerGb=new QAction("Raw GB",bayer);
   bayerRg=new QAction("Raw RG",bayer);
   bayerGr=new QAction("Raw GR",bayer);
   bayerNone->setCheckable(true);
   bayerBg->setCheckable(true);
   bayerGb->setCheckable(true);
   bayerRg->setCheckable(true);
   bayerGr->setCheckable(true);
   bayer->addAction(bayerNone);
   bayer->addAction(bayerBg);
   bayer->addAction(bayerGb);
   bayer->addAction(bayerRg);
   bayer->addAction(bayerGr);

   setNone();

   connect(bayerNone,SIGNAL(triggered()),this,SLOT(setNone()));
   connect(bayerBg,SIGNAL(triggered()),this,SLOT(setBg()));
   connect(bayerGb,SIGNAL(triggered()),this,SLOT(setGb()));
   connect(bayerRg,SIGNAL(triggered()),this,SLOT(setRg()));
   connect(bayerGr,SIGNAL(triggered()),this,SLOT(setGr()));
}

void MainWindow::createCodecMenu() {
   //codecSame=new QAction("Unchanged",codec);
   //codecRawrgb=new QAction("Raw RGB",codec);
   //codecRawgrey=new QAction("Raw Grey",codec);
   codecLossless=new QAction("ZLIB Lossless",codec);
   //codecSame->setCheckable(true);
   //codecRawrgb->setCheckable(true);
   //codecRawgrey->setCheckable(true);
   codecLossless->setCheckable(true);
   //codec->addAction(codecSame);
   //codec->addAction(codecRawrgb);
   //codec->addAction(codecRawgrey);
   codec->addAction(codecLossless);

   setLossless();

   //connect(codecSame,SIGNAL(triggered()),this,SLOT(setSame()));
   //connect(codecRawrgb,SIGNAL(triggered()),this,SLOT(setRawrgb()));
   //connect(codecRawgrey,SIGNAL(triggered()),this,SLOT(setRawgrey()));
   connect(codecLossless,SIGNAL(triggered()),this,SLOT(setLossless()));
}
