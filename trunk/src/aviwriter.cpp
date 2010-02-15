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

AviWriter::AviWriter(int codec, int plans, const char* name, int width, int height, int frameRate) : FileWriter (codec,plans,name,width,height,frameRate) {
   aviChunkStream_=NULL;
   aviFrameStream_=NULL;
   temp_buffer=(unsigned char*)malloc(w*h*2);
   aviFile_=avm::CreateWriteFile(name);

   // use lossless codec
   if(codec_==CODEC_LOSSLESS) {
      BITMAPINFOHEADER bi;
      memset(&bi,0,sizeof(bi));
      bi.biSize=sizeof(bi);
      bi.biWidth=w;
      bi.biHeight=h;
      // RGB24
      if(plans_==ALL_PLANS) {
         bi.biSizeImage=w*h*3;
         bi.biPlanes=1;
         bi.biBitCount=24;
         bi.biCompression=BI_RGB;
      // 8 BITS (YUYV, U=V=zero)
      } else {
         bi.biSizeImage=w*h*2;
         bi.biPlanes=3;
         bi.biBitCount=16;
         bi.biCompression=RIFFINFO_YUY2;
      }
      aviFrameStream_=aviFile_->AddVideoStream(RIFFINFO_HFYU, &bi, frameRate);
      aviFrameStream_->SetQuality(10000);
      aviFrameStream_->Start();
   // use raw
   } else {
      if(plans_==ALL_PLANS) {
         BITMAPINFOHEADER bi;
         memset(&bi,0,sizeof(bi));
         bi.biSize=sizeof(bi);
         bi.biWidth=w;
         bi.biHeight=h;
         bi.biSizeImage=w*h*3;
         bi.biPlanes=1;
         bi.biBitCount=24;
         bi.biCompression=BI_RGB;
         aviChunkStream_=aviFile_->AddStream(AviStream::Video,&bi,sizeof(bi),BI_RGB,frameRate);
      } else {
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
         for(unsigned int i=0;i<256;i++)
            bi->bmiColors[i]=(i<<16)+(i<<8)+i;
         aviChunkStream_=aviFile_->AddStream(AviStream::Video,bi,sizeof(BITMAPINFOHEADER)+256*4,BI_RGB,frameRate);
         free(bi);
      }
   }
}

AviWriter::~AviWriter() {
   if(codec_==CODEC_LOSSLESS)
      aviFrameStream_->Stop();
   delete aviFile_;
   free(temp_buffer);
}

void AviWriter::AddFrame(unsigned char* datas) {
   BITMAPINFOHEADER bi;
   memset(&bi,0,sizeof(bi));
   bi.biSize=sizeof(bi);
   bi.biWidth=w;
   bi.biHeight=h;

   // lossless codec
   if(codec_==CODEC_LOSSLESS) {
      // RGB24
      if(plans_==ALL_PLANS) {
         bi.biSizeImage=w*h*3;
         bi.biPlanes=1;
         bi.biBitCount=24;
         bi.biCompression=BI_RGB;
         bi.biHeight=-bi.biHeight;
         BitmapInfo info(bi);
         CImage img(&info, datas, true);
         aviFrameStream_->AddFrame(&img);
      // 8 BITS (YUYV U=V=zero)
      } else {
         bi.biSizeImage=w*h*2;
         bi.biPlanes=3;
         bi.biBitCount=16;
         bi.biCompression=RIFFINFO_YUY2;
         unsigned char* tmp;
         tmp=getPlan(w,h,datas,plans_);
         if(tmp!=NULL) {
            grey_to_yuy2(w,h,tmp,temp_buffer);
            BitmapInfo info(bi);
            CImage img(&info, temp_buffer, true);
            aviFrameStream_->AddFrame(&img);
         } else
            cout << "Wrong plan : " << plans_ << endl;
         free(tmp);
      }
   // raw
   } else {
      rgb24_vertical_swap(w,h,datas);
      // RGB24
      if(plans_==ALL_PLANS) {
         bi.biSizeImage=w*h*3;
         bi.biPlanes=1;
         bi.biBitCount=24;
         bi.biCompression=BI_RGB;
         aviChunkStream_->AddChunk(datas,bi.biSizeImage,1);
      // 8 BITS RAW
      } else {
         bi.biSizeImage=w*h;
         bi.biPlanes=1;
         bi.biBitCount=8;
         bi.biCompression=BI_RGB;
         unsigned char* tmp;
         tmp=getPlan(w,h,datas,plans_);
         if(tmp!=NULL)
            aviChunkStream_->AddChunk(tmp,bi.biSizeImage,1);
         else
            cout << "Wrong plan : " << plans_ << endl;
         free(tmp);
      }
   }
}
