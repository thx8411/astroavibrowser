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

#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "processing.hpp"

#include "aviwriter.hpp"

using namespace std;

AviWriter::AviWriter(int codec, int plans, const char* name, int width, int height, int frameRate) {
   w=width;
   h=height;
   codec_=codec;
   if(codec==CODEC_RAWGREY)
      plans_=LUM_PLAN;
   else
      plans_=plans;
   aviChunkStream_=NULL;
   aviFrameStream_=NULL;
   aviFile_=avm::CreateWriteFile(name);
   if(plans_==ALL_PLANS) {
      // 3 plans
      BITMAPINFOHEADER bi;
      memset(&bi,0,sizeof(bi));
      bi.biSize=sizeof(bi);
      bi.biWidth=w;
      bi.biHeight=h;
      bi.biSizeImage=w*h*3;
      bi.biPlanes=1;
      bi.biBitCount=24;
      bi.biCompression=BI_RGB;
      if(codec==CODEC_LOSSLESSRGB) {
         aviFrameStream_=aviFile_->AddVideoStream(RIFFINFO_ZLIB, &bi, frameRate);
         aviFrameStream_->SetQuality(10000);
         aviFrameStream_->Start();
      } else {
         aviChunkStream_=aviFile_->AddStream(AviStream::Video,&bi,sizeof(bi),BI_RGB,frameRate);
      }
   } else {
      // 1 plan
      BITMAPINFO* bi;
      bi=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+256*4);
      memset(bi,0,sizeof(BITMAPINFOHEADER)+256*4);
      bi->bmiHeader.biSize=sizeof(BITMAPINFOHEADER)+256*4;
      bi->bmiHeader.biWidth=w;
      bi->bmiHeader.biHeight=h;
      bi->bmiHeader.biSizeImage=w*h;
      bi->bmiHeader.biPlanes=1;
      bi->bmiHeader.biBitCount=8;
      bi->bmiHeader.biCompression=BI_RGB;
      // setting palette
      for(unsigned char i=0;i<256;i++)
         bi->bmiColors[i]=(i<<16)+(i<<8)+i;
      aviChunkStream_=aviFile_->AddStream(AviStream::Video,bi,sizeof(BITMAPINFOHEADER)+256*4,BI_RGB,frameRate);
      free(bi);
   }
}

AviWriter::~AviWriter() {
   if(codec_==CODEC_LOSSLESSRGB)
      aviFrameStream_->Stop();
   delete aviFile_;
}

void AviWriter::AddFrame(unsigned char* datas) {
   BITMAPINFOHEADER bi;
   memset(&bi,0,sizeof(bi));
   bi.biSize=sizeof(bi);
   bi.biWidth=w;
   bi.biHeight=h;
   bi.biCompression=BI_RGB;
   if(plans_==ALL_PLANS) {
      // 3 plans
      bi.biSizeImage=w*h*3;
      bi.biPlanes=1;
      bi.biBitCount=24;
   } else {
      // 1 plan
      bi.biSizeImage=w*h;
      bi.biPlanes=1;
      bi.biBitCount=8;
   }
   if(codec_==CODEC_LOSSLESSRGB) {
      bi.biHeight=-bi.biHeight;
      BitmapInfo info(bi);
      CImage img(&info, datas, true);
      aviFrameStream_->AddFrame(&img);
   } else {
      rgb24_vertical_swap(w,h,datas);
      if(plans_!=ALL_PLANS) {
         // 1 plan
         unsigned char* tmp;
         tmp=getPlan(w,h,datas,plans_);
         if(tmp!=NULL)
            aviChunkStream_->AddChunk(tmp,bi.biSizeImage,1);
         else
            cout << "Wrong plan : " << plans_ << endl;
         free(tmp);
      } else
      aviChunkStream_->AddChunk(datas,bi.biSizeImage,1);
   }
}
