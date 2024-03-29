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

#define ICON_FILE "/usr/share/astroavibrowser/icons/astroavibrowser-icon.png"

#define HELP_FILE "/usr/share/astroavibrowser/astroavibrowser_help.html"

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>

#include <Qt/qmenu.h>
#include <Qt/qmenubar.h>
#include <Qt/qmessagebox.h>
#include <Qt/qscrollarea.h>
#include <Qt/qtextedit.h>
#include <Qt/qdialog.h>
#include <Qt/qtextstream.h>
#include <Qt/qpushbutton.h>
#include <Qt/qlabel.h>
#include <QIntValidator>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>

#include "version.hpp"
#include "processing.hpp"
#include "aviwriter.hpp"
#include "bmpwriter.hpp"

#include "Qmainwindow.hpp"

using namespace std;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
   // basic init
   outputCodec=CODEC_RAW;
   outputFormat=AVI_FILE;
   RGB24forced=false;

   // ffmpeg datas init
   inputFileFormatContext=NULL;
   inputFileCodec=NULL;

   //
   //   GUI
   //

   //   MENU
   // actions
   openfile = new QAction("&Open...",this);
   save = new QAction("&Save...",this);
   quit = new QAction("&Quit", this);
   properties = new QAction("Get &Properties...",this);
   // dark/flat
   sumGrey=new QAction("Build sum frame (mono)",this);
   sumRGB=new QAction("Build sum frame (rgb)",this);
   maxGrey=new QAction("Build max frame (mono)",this);
   maxRGB=new QAction("Build max frame (rgb)",this);
   darkFlatGreyMean=new QAction("Build mean frame (mono)...",this);
   darkFlatRGBMean=new QAction("Build mean frame (rgb)...",this);
   darkFlatGreyMedian=new QAction("Build median frame (mono)...",this);
   darkFlatRGBMedian=new QAction("Build median frame (rgb)...",this);
   // save plans
   lPlan=new QAction("Save &Luminance...",this);
   rPlan=new QAction("Save &R plan...",this);
   gPlan=new QAction("Save &G plan...",this);
   bPlan=new QAction("Save &B plan...",this);
   useBmp=new QAction("BMP sequence",this);
   useBmp->setCheckable(true);
   forceRGB24=new QAction("Force RGB24 output",this);
   forceRGB24->setCheckable(true);
   helpfile=new QAction("&Help...",this);
   about=new QAction("&About...",this);
   // file menu
   QMenu* file;
   file = menuBar()->addMenu("&File");
   file->addAction(openfile);
   file->addAction(save);
   file->addSeparator();
   file->addAction(lPlan);
   file->addAction(rPlan);
   file->addAction(gPlan);
   file->addAction(bPlan);
   file->addSeparator();
   file->addAction(sumGrey);
   file->addAction(sumRGB);
   file->addAction(maxGrey);
   file->addAction(maxRGB);
   file->addSeparator();
   file->addAction(darkFlatGreyMean);
   file->addAction(darkFlatRGBMean);
   file->addAction(darkFlatGreyMedian);
   file->addAction(darkFlatRGBMedian);
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
   output->addAction(useBmp);
   codec= output->addMenu("Avi movie");
   // settings menu
   QMenu* settings;
   settings = menuBar()->addMenu("Settings");
   settings->addAction(forceRGB24);
   // help menu
   QMenu* help;
   help = menuBar()->addMenu("&Help");
   help->addAction(helpfile);
   help->addAction(about);
   // signal connections
   connect(openfile, SIGNAL(triggered()), this, SLOT(MenuOpen()));
   connect(save, SIGNAL(triggered()), this, SLOT(MenuSaveAll()));
   // dark / flat
   connect(sumGrey, SIGNAL(triggered()), this, SLOT(MenuGreySum()));
   connect(sumRGB, SIGNAL(triggered()), this, SLOT(MenuRGBSum()));
   connect(maxGrey, SIGNAL(triggered()), this, SLOT(MenuGreyMax()));
   connect(maxRGB, SIGNAL(triggered()), this, SLOT(MenuRGBMax()));
   connect(darkFlatGreyMean, SIGNAL(triggered()), this, SLOT(MenuDarkFlatGreyMean()));
   connect(darkFlatRGBMean, SIGNAL(triggered()), this, SLOT(MenuDarkFlatRGBMean()));
   connect(darkFlatGreyMedian, SIGNAL(triggered()), this, SLOT(MenuDarkFlatGreyMedian()));
   connect(darkFlatRGBMedian, SIGNAL(triggered()), this, SLOT(MenuDarkFlatRGBMedian()));
   // plans
   connect(lPlan, SIGNAL(triggered()), this, SLOT(MenuSaveL()));
   connect(rPlan, SIGNAL(triggered()), this, SLOT(MenuSaveR()));
   connect(gPlan, SIGNAL(triggered()), this, SLOT(MenuSaveG()));
   connect(bPlan, SIGNAL(triggered()), this, SLOT(MenuSaveB()));
   connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
   // menus
   connect(properties, SIGNAL(triggered()), this, SLOT(MenuProperties()));
   connect(useBmp,SIGNAL(toggled(bool)),this,SLOT(MenuBmp(bool)));
   connect(forceRGB24,SIGNAL(toggled(bool)),this,SLOT(MenuForceRGB24(bool)));
   connect(helpfile, SIGNAL(triggered()), this, SLOT(MenuHelp()));
   connect(about, SIGNAL(triggered()), this, SLOT(MenuAbout()));

   // CENTRAL ZONE
   // widgets and layouts
   QWidget* centralZone = new QWidget(this);
   QHBoxLayout* central = new QHBoxLayout;
   QVBoxLayout* left = new QVBoxLayout;
   QVBoxLayout* buttons = new QVBoxLayout;
   QHBoxLayout* autolevel = new QHBoxLayout;
   // buttons
   selectAll = new QPushButton("Select All");
   unSelectAll = new QPushButton("Unselect All");
   invertSelection = new QPushButton("Invert Selection");
   autoSelection = new QPushButton("Auto Selection");
   // auto selection level entry
   QLabel* autoLegend= new QLabel("Keep :");
   autoValue= new QLineEdit("80");
   autoValue->setAlignment(Qt::AlignRight);
   QIntValidator* autoValid= new QIntValidator();
   autoValid->setRange(1,99);
   autoValue->setValidator(autoValid);
   QLabel* autoLegend2= new QLabel("%");
   autolevel->addWidget(autoLegend);
   autolevel->addWidget(autoValue);
   autolevel->addWidget(autoLegend2);
   // set buttons size (should be ok for 1024*600 with PAL format)
   selectAll->setMaximumWidth(135);
   selectAll->setMinimumWidth(135);
   unSelectAll->setMaximumWidth(135);
   unSelectAll->setMinimumWidth(135);
   invertSelection->setMaximumWidth(135);
   invertSelection->setMinimumWidth(135);
   autoSelection->setMaximumWidth(135);
   autoSelection->setMinimumWidth(135);
   autoValue->setMaximumWidth(32);
   autoValue->setMinimumWidth(32);
   // disabling widgets (no file loaded at this time)
   save->setEnabled(false);
   properties->setEnabled(false);
   bayer->setEnabled(false);
   // dark/flat
   sumGrey->setEnabled(false);
   sumRGB->setEnabled(false);
   maxGrey->setEnabled(false);
   maxRGB->setEnabled(false);
   darkFlatGreyMean->setEnabled(false);
   darkFlatRGBMean->setEnabled(false);
   darkFlatGreyMedian->setEnabled(false);
   darkFlatRGBMedian->setEnabled(false);
   // plan
   lPlan->setEnabled(false);
   rPlan->setEnabled(false);
   gPlan->setEnabled(false);
   bPlan->setEnabled(false);
   useBmp->setEnabled(false);
   codec->setEnabled(false);
   selectAll->setEnabled(false);
   unSelectAll->setEnabled(false);
   invertSelection->setEnabled(false);
   autoSelection->setEnabled(false);
   autoValue->setEnabled(false);
   // scroll area
   QScrollArea* picture= new QScrollArea();
   // frame display and list
   frameDisplay= new FrameDisplay(centralZone);
   frameList= new FrameList(centralZone);
   histogram=new Histogram(centralZone);
   histogram->setEnabled(false);
   frameDisplay->setHistogram(histogram);
   frameList->setFrameDisplay(frameDisplay);
   picture->setBackgroundRole(QPalette::Dark);
   picture->setWidget(frameDisplay);
   picture->setAlignment(Qt::AlignCenter);
   // connections
   connect(selectAll, SIGNAL(released()), this, SLOT(ButtonSelectAll()));
   connect(unSelectAll, SIGNAL(released()), this, SLOT(ButtonUnSelectAll()));
   connect(invertSelection, SIGNAL(released()), this, SLOT(ButtonInvert()));
   connect(autoSelection, SIGNAL(released()), this, SLOT(ButtonAuto()));
   // add widgets and layouts
   buttons->addWidget(selectAll);
   buttons->addWidget(unSelectAll);
   buttons->addWidget(invertSelection);
   buttons->addWidget(autoSelection);
   left->addWidget(frameList,10);
   left->addLayout(buttons,0);
   left->addLayout(autolevel,0);
   left->addWidget(histogram,0);
   central->addLayout(left,0);
   central->addWidget(picture,10);
   centralZone->setLayout(central);
   // update menu
   createBayerMenu();
   createCodecMenu();
   // set and display
   setCentralWidget(centralZone);
   QPixmap* pixmap = new QPixmap(ICON_FILE);
   setWindowIcon(*pixmap);
   delete pixmap;
   frameDisplay->show();
   frameList->show();
   histogram->show();
   centralZone->show();
   // fix size
   sizeHint();

   //
   //   FFMPEG
   //

   // register all formats and codecs
   av_register_all();
}

MainWindow::~MainWindow() {
   // release opened file
   freeFile();
   delete frameList;
   delete frameDisplay;
   delete histogram;
}

void MainWindow::freeFile() {
   // close codec
   if(inputFileCodec!=NULL) {
      avcodec_close(inputFileCodecContext);
      inputFileCodecContext=NULL;
   }
   // close file
   if(inputFileFormatContext!=NULL) {
      avformat_close_input(&inputFileFormatContext);
      inputFileFormatContext=NULL;
   }

   // disabling widgets (no more file opened)
   save->setEnabled(false);
   properties->setEnabled(false);
   bayer->setEnabled(false);
   // dark/flat
   sumGrey->setEnabled(false);
   sumRGB->setEnabled(false);
   maxGrey->setEnabled(false);
   maxRGB->setEnabled(false);
   darkFlatGreyMean->setEnabled(false);
   darkFlatRGBMean->setEnabled(false);
   darkFlatGreyMedian->setEnabled(false);
   darkFlatRGBMedian->setEnabled(false);
   // plan
   lPlan->setEnabled(false);
   rPlan->setEnabled(false);
   gPlan->setEnabled(false);
   bPlan->setEnabled(false);
   useBmp->setEnabled(false);
   codec->setEnabled(false);
   selectAll->setEnabled(false);
   unSelectAll->setEnabled(false);
   invertSelection->setEnabled(false);
   autoSelection->setEnabled(false);
   autoValue->setEnabled(false);
   histogram->setEnabled(false);
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
   if(avformat_open_input(&inputFileFormatContext, inputFileName.toStdString().c_str(), NULL, NULL)!=0) {
      // Couldn't open file
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to open the file"));
      return;
   }

   // get streams
   if(avformat_find_stream_info(inputFileFormatContext,NULL)<0) {
      // Couldn't find stream information
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to get file streams"));
      freeFile();
      return;
   }
   // open stream
   inputStreamNumber=-1;
   for(int i=0;i<inputFileFormatContext->nb_streams;i++) {
      if(inputFileFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
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
   if(avcodec_open2(inputFileCodecContext, inputFileCodec, NULL)<0) {
      // could not open codec
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to load the codec"));
      freeFile();
      return;
   }

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
   // dark/flat
   sumGrey->setEnabled(true);
   sumRGB->setEnabled(true);
   maxGrey->setEnabled(true);
   maxRGB->setEnabled(true);
   darkFlatGreyMean->setEnabled(true);
   darkFlatRGBMean->setEnabled(true);
   darkFlatGreyMedian->setEnabled(true);
   darkFlatRGBMedian->setEnabled(true);
   // plan
   lPlan->setEnabled(true);
   rPlan->setEnabled(true);
   gPlan->setEnabled(true);
   bPlan->setEnabled(true);
   useBmp->setEnabled(true);
   codec->setEnabled(true);
   selectAll->setEnabled(true);
   unSelectAll->setEnabled(true);
   invertSelection->setEnabled(true);
   autoSelection->setEnabled(true);
   autoValue->setEnabled(true);
   histogram->setEnabled(true);
   // fixing new size
   sizeHint();

   // set bayer
   setNone();

   repaint();
}

// PRE-PROCESS DARK/FLAT

void MainWindow::MenuGreySum() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,LUM_PLAN,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->sumGrey(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuRGBSum() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,ALL_PLANS,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->sumRGB(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuGreyMax() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,LUM_PLAN,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->maxGrey(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuRGBMax() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,ALL_PLANS,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->maxRGB(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuDarkFlatGreyMean() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,LUM_PLAN,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->darkFlatGreyMean(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuDarkFlatRGBMean() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,ALL_PLANS,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->darkFlatRgbMean(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuDarkFlatGreyMedian() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,LUM_PLAN,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->darkFlatGreyMedian(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

void MainWindow::MenuDarkFlatRGBMedian() {
   string extension;
   FileWriter*  file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()<=1) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No enough frames selected"));
      return;
   }

   extension="Bmp Files (*.bmp *.BMP)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // saving the new frame
   file=new BmpWriter(outputCodec,ALL_PLANS,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,0,false);
   frameList->darkFlatRgbMedian(file);
   delete file;

   // set cursor back
   setCursor(Qt::ArrowCursor);
}

// SAVE

void MainWindow::MenuSaveAll() {
   MenuSaveImpl(ALL_PLANS);
}

void MainWindow::MenuSaveL() {
   MenuSaveImpl(LUM_PLAN);
}

void MainWindow::MenuSaveR() {
   MenuSaveImpl(RED_PLAN);
}

void MainWindow::MenuSaveG() {
   MenuSaveImpl(GREEN_PLAN);
}

void MainWindow::MenuSaveB() {
   MenuSaveImpl(BLUE_PLAN);
}

void MainWindow::MenuSaveImpl(int p) {
   string extension;
   FileWriter*	file;
   // is there selected frames ?
   if(frameList->getSelectedFrames()==0) {
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No frame selected"));
      return;
   }

   if(outputFormat==BMP_FILE)
      extension="Bmp Files (*.bmp *.BMP)";
   else
      extension="Avi Files (*.avi *.AVI)";
   // query file name
   outputFileName = QFileDialog::getSaveFileName(this, tr("Save Video"),"/home",extension.c_str());

   // is the name valid ?
   if(outputFileName=="")
      return;

   // testing file access
   int fd;
   if(fd=open(outputFileName.toStdString().c_str(),O_WRONLY|O_CREAT,S_IWUSR)<0){
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Write access denied"));
      return;
   }
   unlink(outputFileName.toStdString().c_str());

   // it could be long...
   setCursor(Qt::BusyCursor);

   // compute frame rate
   int frameRate;
   frameRate=(float)inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.num/(float)inputFileFormatContext->streams[inputStreamNumber]->r_frame_rate.den;

   // saving the new avi
   if(outputFormat==BMP_FILE) {
      file=new BmpWriter(outputCodec,p,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,frameRate);
   } else {
      file=new AviWriter(outputCodec,p,outputFileName.toStdString().c_str(),inputFileCodecContext->width,inputFileCodecContext->height,frameRate);
   }
   frameList->dump(file);
   delete file;

   // set cursor back
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
   tmp=av_get_pix_fmt_name(inputFileCodecContext->pix_fmt);
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

// bmp output
void MainWindow::MenuBmp(bool v) {
   if(v) {
      codec->setEnabled(false);
      outputFormat=BMP_FILE;
   } else {
      codec->setEnabled(true);
      outputFormat=AVI_FILE;
   }
}

// force RGB24
void MainWindow::MenuForceRGB24(bool v) {
   RGB24forced=v;
   if(v&&outputCodec==CODEC_RAW)
         outputCodec=CODEC_RAW_FORCE_RGB24;
   if(!v&&outputCodec==CODEC_RAW_FORCE_RGB24)
         outputCodec=CODEC_RAW;
}

// help window
void MainWindow::MenuHelp() {
   QDialog helpwindow;
   QTextEdit* helptext;
   QTextStream* helpstream;
   FILE* help_file=NULL;

   // opening help file
   help_file=fopen(HELP_FILE,"r");
   if(help_file)
      helpstream=new QTextStream(help_file);
   else
      helpstream=new QTextStream("Help file not found");

   // html display widget
   helptext=new QTextEdit(&helpwindow);
   helptext->setReadOnly(true);
   helptext->setHtml(helpstream->readAll());
   helptext->resize(500,600);

   // close button
   QPushButton* close=new QPushButton("Close",&helpwindow);
   connect(close,SIGNAL(released()),&helpwindow,SLOT(close()));

   // dialog box
   helpwindow.setWindowTitle(tr("astroavibrowser help"));

   // layout and size
   QVBoxLayout* vbox = new QVBoxLayout(&helpwindow);
   vbox->addWidget(helptext);
   vbox->addWidget(close);
   helpwindow.resize(500,600);

   // dialog display
   helpwindow.exec();

   // cleaning
   delete helptext;
   delete helpstream;
   delete close;
   delete vbox;
   if(help_file)
      fclose(help_file);
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

void MainWindow::ButtonAuto() {
   // auto selects frames
   QString val=autoValue->text();

   setCursor(Qt::BusyCursor);
   frameList->autoSelection(val.toInt());
   setCursor(Qt::ArrowCursor);
}

//
// bayer modes callbacks
//

void MainWindow::setNone() {
   int* average;
   bayerNone->setChecked(true);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_NONE);
   average=frameList->getAverage();
   histogram->setAverage(average);
   free(average);
   frameList->refreshFrame();
}

void MainWindow::setBg() {
   int* average;
   bayerNone->setChecked(false);
   bayerBg->setChecked(true);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_BG);
   average=frameList->getAverage();
   histogram->setAverage(average);
   free(average);
   frameList->refreshFrame();
}

void MainWindow::setGb() {
   int* average;
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(true);
   bayerRg->setChecked(false);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_GB);
   average=frameList->getAverage();
   histogram->setAverage(average);
   free(average);
   frameList->refreshFrame();
}

void MainWindow::setRg() {
   int* average;
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(true);
   bayerGr->setChecked(false);
   frameDisplay->setRawmode(RAW_RG);
   average=frameList->getAverage();
   histogram->setAverage(average);
   free(average);
   frameList->refreshFrame();
}

void MainWindow::setGr() {
   int* average;
   bayerNone->setChecked(false);
   bayerBg->setChecked(false);
   bayerGb->setChecked(false);
   bayerRg->setChecked(false);
   bayerGr->setChecked(true);
   frameDisplay->setRawmode(RAW_GR);
   average=frameList->getAverage();
   histogram->setAverage(average);
   free(average);
   frameList->refreshFrame();
}

//
// output codec callbacks
//

void MainWindow::setRaw() {
   codecRaw->setChecked(true);
   codecLossless->setChecked(false);

   // update codec
   if(RGB24forced)
      outputCodec=CODEC_RAW_FORCE_RGB24;
   else
      outputCodec=CODEC_RAW;
}

void MainWindow::setLossless() {
   codecRaw->setChecked(false);
   codecLossless->setChecked(true);

   // update codec
   outputCodec=CODEC_LOSSLESS;
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

   frameDisplay->setRawmode(RAW_NONE);
   frameList->refreshFrame();

   connect(bayerNone,SIGNAL(triggered()),this,SLOT(setNone()));
   connect(bayerBg,SIGNAL(triggered()),this,SLOT(setBg()));
   connect(bayerGb,SIGNAL(triggered()),this,SLOT(setGb()));
   connect(bayerRg,SIGNAL(triggered()),this,SLOT(setRg()));
   connect(bayerGr,SIGNAL(triggered()),this,SLOT(setGr()));
}

void MainWindow::createCodecMenu() {
   codecRaw=new QAction("Raw",codec);
   codecLossless=new QAction("Lossless Huffyuv",codec);
   codecRaw->setCheckable(true);
   codecLossless->setCheckable(true);
   codec->addAction(codecRaw);
   codec->addAction(codecLossless);

   setRaw();

   connect(codecRaw,SIGNAL(triggered()),this,SLOT(setRaw()));
   connect(codecLossless,SIGNAL(triggered()),this,SLOT(setLossless()));
}


bool MainWindow::openFile(QString fileName) {
   inputFileName=fileName;
   // open new file
   if(inputFileName=="")
      return(false);

   // release opened file
   freeFile();

   // opening file
   if(avformat_open_input(&inputFileFormatContext, inputFileName.toStdString().c_str(), NULL, NULL)!=0) {
      // Couldn't open file
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to open the file"));
      return(false);
   }

   // get streams
   if(avformat_find_stream_info(inputFileFormatContext,NULL)<0) {
      // Couldn't find stream information
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to get file streams"));
      freeFile();
      return(false);
   }
   // open stream
   inputStreamNumber=-1;
   for(int i=0;i<inputFileFormatContext->nb_streams;i++) {
      if(inputFileFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      inputStreamNumber=i;
      break;
      }
   }
   if(inputStreamNumber==-1) {
      // no video stream
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No video stream found"));
      freeFile();
      return(false);
   }

   // load codec
   inputFileCodecContext=inputFileFormatContext->streams[inputStreamNumber]->codec;
   inputFileCodec=avcodec_find_decoder(inputFileCodecContext->codec_id);
   if(inputFileCodec==NULL) {
      // no codec found
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("No codec found for this stream"));
      freeFile();
      return(false);
   }
   // enable truncated codec
   if(inputFileCodec->capabilities & CODEC_CAP_TRUNCATED)
        inputFileCodecContext->flags|=CODEC_FLAG_TRUNCATED;
   if(avcodec_open2(inputFileCodecContext, inputFileCodec, NULL)<0) {
      // could not open codec
      QMessageBox::critical(this, tr("AstroAviBrowser"),tr("Unable to load the codec"));
      freeFile();
      return(false);
   }

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
   // dark/flat
   sumGrey->setEnabled(true);
   sumRGB->setEnabled(true);
   maxGrey->setEnabled(true);
   maxRGB->setEnabled(true);
   darkFlatGreyMean->setEnabled(true);
   darkFlatRGBMean->setEnabled(true);
   darkFlatGreyMedian->setEnabled(true);
   darkFlatRGBMedian->setEnabled(true);
   // plan
   lPlan->setEnabled(true);
   rPlan->setEnabled(true);
   gPlan->setEnabled(true);
   bPlan->setEnabled(true);
   useBmp->setEnabled(true);
   codec->setEnabled(true);
   selectAll->setEnabled(true);
   unSelectAll->setEnabled(true);
   invertSelection->setEnabled(true);
   autoSelection->setEnabled(true);
   autoValue->setEnabled(true);
   histogram->setEnabled(true);
   // fixing new size
   sizeHint();

   // set bayer
   setNone();

   repaint();
}
