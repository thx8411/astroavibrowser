/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include "bayer.hpp"

#include "Qframedisplay.moc"

FrameDisplay::FrameDisplay(QWidget* parent) : QWidget(parent) {
   painter_ = new QPainter();
   frameData=NULL;
   rawMode=RAW_NONE;
}

FrameDisplay::~FrameDisplay() {
}

void FrameDisplay::setFrame(int width, int height, AVFrame* f) {
   frameWidth=width;
   frameHeight=height;
   frameData=f;
   //cerr << "set frame" << endl;
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
      applyRawMode();
      painter_->begin(this);
      painter_->setClipRegion(ev->region());
      painter_->drawImage(0,0,QImage((unsigned char*)frameData->data[0],frameWidth,frameHeight,QImage::Format_RGB888));
      painter_->end();
   } else {
      painter_->begin(this);
      painter_->setClipRegion(ev->region());
      painter_->fillRect(0,0,frameWidth,frameHeight,Qt::black);
      painter_->end();
   }
}

void FrameDisplay::applyRawMode() {
   unsigned char* buffer;
   if ((frameData!=NULL)&&(rawMode!=RAW_NONE)) {
         buffer=(unsigned char*)malloc(frameWidth*frameHeight*3);
         memcpy(buffer,frameData->data[0],frameWidth*frameHeight*3);
         //cerr << "raw " << rawMode << endl;
         raw2rgb(frameData->data[0],buffer,frameWidth,frameHeight,rawMode);
         free(buffer);
   }
}
