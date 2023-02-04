//
//  main.cpp
//  AppToGIF_Demo
//
//  Created by Jakub Nowak on 26/01/2023.
//
//extern "C" {
//    #include <libavcodec/avcodec.h>
//    #include <libavformat/avformat.h>
//    #include <libswscale/swscale.h>
//    #include <libavutil/avutil.h>
//    #include <libavutil/imgutils.h>
//}

#include "AppToGIF.h"
#include <iostream>
//#include "../src/ffmpegBackend/ffmpegBackend.hpp"
//#include <cinttypes>
//int main()
//{
////    int64_t nextPts = 0;
////    std::string fName = "TestMoj.gif";
////    AVFormatContext* formatContext = nullptr;
////    AVCodec* codec = nullptr;
////    AVCodecContext* codecContext =nullptr;
////    AVOutputFormat* outputFormat = nullptr;
////    AVStream* stream = nullptr;
////    AVPacket* packet = nullptr;
////
////    AVFrame* frame = nullptr;
////    AVFrame* tmpFrame = nullptr;
////    AVFrame* yuvFrame = nullptr;
////
////    struct AppToGIF::GIFSettings x;
////
////    struct SwsContext * swsCtx = nullptr;
////    struct SwsContext * swsGIFCtx = nullptr;
////
//    using namespace AppToGIF;
//
//    backend Obj;
////
//////    Obj.initFfmpeg(Obj.fName, Obj.formatContext, Obj.outputFormat);
//////    Obj.addStreams(Obj.codec, Obj.formatContext, Obj.codecContext, Obj.outputFormat, Obj.stream, Obj.packet);
//////    Obj.openFile(Obj.fName, Obj.codec, Obj.formatContext, Obj.codecContext, Obj.outputFormat, Obj.stream);
//////    Obj.writeHeader(Obj.codec, Obj.formatContext, Obj.codecContext, Obj.outputFormat, Obj.stream);
////
//////    std::cout<<initFfmpeg(fName, formatContext, outputFormat);
//////    std::cout<<addStreams(codec, formatContext, codecContext, outputFormat, stream, packet);
//////    std::cout<<openFile(fName, codec, formatContext, codecContext, outputFormat, stream);
//////    std::cout<<writeHeader(codec, formatContext, codecContext, outputFormat, stream);
//////    std::cout<<allocateThreeFrames(frame, tmpFrame, yuvFrame);
//////    std::cout<<createConversionContextes(swsCtx, swsCtx, codecContext, &x);
//    Obj.initFfmpeg();
//    Obj.addStreams();
//    Obj.openFile();
//    Obj.writeHeader();
//    Obj.allocateThreeFrames();
//    Obj.createConversionContextes();
////
//    Frame f={};
//    f.m_rgb = new uint8_t();
//    //GetFramedata
//    for (int y = 0; y < 1080; ++y)
//    {
//        uint8_t *row = &f.m_rgb[y * 1920];
//        for (int x = 0; x < 1920; ++x)
//        {
//            const int index = x * 3;
//            row[index + 0] = 0x00; // r
//            row[index + 1] = 0xFF; // g
//            row[index + 2] = 0x00;   // b
//        }
//    }
//    Obj.getFrameData(&f);
//
//
//    Obj.generateFrame();
//    Obj.writeFrame();
//    Obj.freeStreamData();
//    delete  f.m_rgb;
////    generateFrame(swsCtx, swsGIFCtx, codecContext, tmpFrame, tmpFrame, yuvFrame, nextPts);
////    writeFrame(frame, packet, codecContext, stream, formatContext);
////    freeStreamData(frame, tmpFrame, yuvFrame, packet, codecContext, stream, formatContext, swsCtx, swsGIFCtx, outputFormat, nextPts);
    
//    
//    return 0;
//}
