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

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "processing.hpp"

#include "bmpwriter.hpp"

using namespace std;

BmpWriter::BmpWriter(int codec, int plans, const char* name, int width, int height, int frameRate, bool count) : FileWriter (codec,plans,name,width,height,frameRate) {
   frameNumber=0;
   count_=count;
}

BmpWriter::~BmpWriter() {
}

void BmpWriter::AddFrame(unsigned char* datas) {
   int fd;
   int res;
   int extPos;
   char number[8];
   BMPHEADER bi;
   string fileName=name_;
   // removing extension
   extPos=fileName.rfind(".bmp");
   if(extPos!=-1)
      fileName.erase(extPos);
   extPos=fileName.rfind(".BMP");
   if(extPos!=-1)
      fileName.erase(extPos);

   // building file name
   if(count_) {
      fileName+="-";
      sprintf(number,"%d",frameNumber);
      fileName+=number;
   }
   fileName+=".bmp";

   fd=open(fileName.c_str(),O_WRONLY|O_CREAT,0644);

   // write header
   memset(&bi,0,sizeof(bi));
   bi.type[0]='B';
   bi.type[1]='M';
   bi.header_size=40;
   bi.width=w;
   bi.height=h;
   bi.compression=0;
   bi.important_colors=0;
   if(plans_==ALL_PLANS) {
      bi.offset=54;
      bi.plans=1;
      bi.depth=24;
      bi.picture_size=w*h*3;
   } else {
      bi.offset=54+(256*4);
      bi.plans=1;
      bi.depth=8;
      bi.picture_size=w*h;
   }
   bi.total_size=bi.offset+bi.picture_size;

   res=write(fd,&bi.type,2);
   res=write(fd,&bi.total_size,4);
   res=write(fd,&bi.reserved,4);
   res=write(fd,&bi.offset,4);
   res=write(fd,&bi.header_size,4);
   res=write(fd,&bi.width,4);
   res=write(fd,&bi.height,4);
   res=write(fd,&bi.plans,2);
   res=write(fd,&bi.depth,2);
   res=write(fd,&bi.compression,4);
   res=write(fd,&bi.picture_size,4);
   res=write(fd,&bi.hres,4);
   res=write(fd,&bi.vres,4);
   res=write(fd,&bi.palette_size,4);
   res=write(fd,&bi.important_colors,4);

   // write palette if needed
   if(plans_!=ALL_PLANS) {
      int color;
      for(unsigned int i=0;i<256;i++) {
         color=(i<<16)+(i<<8)+i;
         res=write(fd,&color,4);
      }
   }

   // write picture
   rgb24_vertical_swap(w,h,datas);
   if(plans_==ALL_PLANS) {
      res=write(fd,datas,bi.picture_size);
   } else {
      unsigned char* tmp;
      tmp=getPlan(w,h,datas,plans_);
      res=write(fd,tmp,bi.picture_size);
      free(tmp);
   }

   close(fd);
   frameNumber++;
}
