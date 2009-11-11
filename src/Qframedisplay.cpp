/* part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009 */

#include "Qframedisplay.moc"

FrameDisplay::FrameDisplay(QWidget* parent) : QWidget(parent) {
   painter_ = new QPainter();
   frameData=NULL;
}

void FrameDisplay::setFrame(int width, int height, AVFrame* f) {
   frameWidth=width;
   frameHeight=height;
   frameData=f;
}

void FrameDisplay::setRawMode(int mode) {
   rawMode=mode;
   applyRawMode();
   repaint();
}

void FrameDisplay::paintEvent(QPaintEvent * ev) {
   if (frameData!=NULL) {
      applyRawMode();
      painter_->begin(this);
      painter_->setClipRegion(ev->region());
      painter_->drawImage(0,0,QImage((unsigned char*)frameData->data[0],frameWidth,frameHeight,QImage::Format_RGB888));
      painter_->end();
   }
}

void FrameDisplay::applyRawMode() {
   // add frame conversion
}
