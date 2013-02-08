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

#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "processing.hpp"

#include "aviwriter.hpp"

using namespace std;

AviWriter::AviWriter(int codec, int plans, const char* name, int width, int height, int frameRate) : FileWriter (codec,plans,name,width,height,frameRate) {
   uint8_t* picture_buffer;

   // get output format
   output_format=av_guess_format("avi", NULL, NULL );
   if(!output_format) {
      fprintf(stderr,"Can't get AVI format, leaving...\n");
      exit(1);
   }

   // allocate context
   output_format_cx=avformat_alloc_context();
   if(!output_format_cx) {
      fprintf(stderr,"Can't allocate format context, leaving...\n");
      exit(1);
   }

   // format context setting
   output_format_cx->oformat=output_format;
   snprintf(output_format_cx->filename,sizeof(output_format_cx->filename),"%s",name);

   // getting the codec
   if(codec_==CODEC_LOSSLESS) {
      output_codec=avcodec_find_encoder(CODEC_ID_HUFFYUV);
   } else {
      output_codec=avcodec_find_encoder(CODEC_ID_RAWVIDEO);
   }
   if(!output_codec) {
      fprintf(stderr,"Can't get codec, leaving...\n");
      exit(1);
   }

   // add the video stream
   output_video_stream=NULL;
   output_video_stream=avformat_new_stream(output_format_cx,output_codec);
   if(!output_video_stream) {
      fprintf(stderr,"Can't add video stream, leaving...\n");
      exit(1);
   }

   // setting codec context
   output_codec_cx=output_video_stream->codec;
   output_codec_cx->width=width;
   output_codec_cx->height=height;
   output_codec_cx->time_base.den=(int)frameRate;
   output_codec_cx->time_base.num=1;

   // setting codec pix format
   if(codec_==CODEC_LOSSLESS)
      if(plans_==ALL_PLANS)
         output_codec_cx->pix_fmt=PIX_FMT_RGB32;
      else
         output_codec_cx->pix_fmt=PIX_FMT_YUV422P;
   else if(plans_==ALL_PLANS)
      output_codec_cx->pix_fmt=PIX_FMT_RGB24;
   else
      output_codec_cx->pix_fmt=PIX_FMT_GRAY8;

   // opening codec
   if(avcodec_open2(output_codec_cx,output_codec,NULL)<0) {
      fprintf(stderr,"Can't open codec, leaving...\n");
      exit(1);
   }

   // buffer allocations
   if(codec_==CODEC_LOSSLESS)
      /* worst case, RGB32 */
      video_outbuf_size=height*width*4;
   else if (plans_==ALL_PLANS)
      video_outbuf_size=height*width*3;
   else
      video_outbuf_size=height*width;
   video_outbuf=(uint8_t*)av_malloc(video_outbuf_size);

   // picture allocation
   picture=avcodec_alloc_frame();
   if(!picture) {
      fprintf(stderr,"Can't allocate picture, leaving...\n");
      exit(1);
   }
   picture_buffer=(uint8_t*)av_malloc(avpicture_get_size(output_codec_cx->pix_fmt,width,height));
   if(!picture_buffer) {
      fprintf(stderr,"Can't allocate picture buffer, leaving...\n");
      exit(1);
   }
   avpicture_fill((AVPicture*)picture,picture_buffer,output_codec_cx->pix_fmt,width,height);

   // dump format
   av_dump_format(output_format_cx, 0, name, 1);

   // opening file
   if(avio_open(&output_format_cx->pb,name,AVIO_FLAG_WRITE)<0) {
      fprintf(stderr,"Can't open file, leaving...\n");
      exit(1);
   }

   // write header
   avformat_write_header(output_format_cx,NULL);
}

AviWriter::~AviWriter() {
   // write trailer
   av_write_trailer(output_format_cx);
   // close codec
   avcodec_close(output_codec_cx);
   // free picture and buffer
   av_free(picture->data[0]);
   av_free(picture);
   av_free(video_outbuf);
   // free code and stream
   av_freep(&output_format_cx->streams[0]->codec);
   av_freep(&output_format_cx->streams[0]);
   // close file
   avio_close(output_format_cx->pb);
   // free format context
   av_free(output_format_cx);
}

void AviWriter::AddFrame(unsigned char* datas) {
   int out_size,i;
   unsigned char* plan_buf;

   // lossless codec
   if(codec_==CODEC_LOSSLESS) {
      // RGB32
      if(plans_==ALL_PLANS) {
      // setting alpha
      memset(picture->data[0],0,h*w*4);
      for(i=0;i<(w*h);i++) {
         picture->data[0][i*4]=datas[i*3];
         picture->data[0][i*4+1]=datas[i*3+1];
         picture->data[0][i*4+2]=datas[i*3+2];
      }
      // 8 BITS (YUYV U=V=zero)
      } else {
         plan_buf=getPlan(w,h,datas,plans_);
         memcpy(picture->data[0],plan_buf,h*w);
         memset(picture->data[1],128,h*w/2);
         memset(picture->data[2],128,h*w/2);
         free(plan_buf);
      }
   // RAW
   } else {
      // RGB24
      if(plans_==ALL_PLANS) {
         memcpy(picture->data[0],datas,w*h*3);
      // 8 BITS GRAY
      } else {
         plan_buf=getPlan(w,h,datas,plans_);
         memcpy(picture->data[0],plan_buf,h*w);
         free(plan_buf);
      }
   }

   out_size=avcodec_encode_video(output_codec_cx,video_outbuf,video_outbuf_size,picture);
   if(out_size>0) {
      AVPacket pkt;
      av_init_packet(&pkt);
      pkt.flags|=AV_PKT_FLAG_KEY;
      pkt.stream_index=output_video_stream->index;
      pkt.data=video_outbuf;
      pkt.size=out_size;
      av_write_frame(output_format_cx,&pkt);
   }
}
