//
//  Encoder.cpp
//  AppToGIF
//
//  Created by Jakub Nowak on 30/01/2023.
//

#include "Encoder.hpp"

namespace AppToGIF
{
//Function used to initialize ffpmeg pipeline
ErrorReporter Encoder::init()
{
    int error;
    /* -------------- Allocate formatContext --------------*/
    error = avformat_alloc_output_context2(&m_OutputStream.formatContext, NULL, NULL, m_Settings.fileName.data());
    if (error < 0) {
        return ErrorReporter::CouldNotAllocateFormatContext;
    }

    /* -------------- Check outputFormat from allocated formatContext --------------*/
    m_OutputStream.outputFormat = const_cast<AVOutputFormat*>(m_OutputStream.formatContext->oformat);
    if (m_OutputStream.outputFormat->video_codec != AV_CODEC_ID_NONE) {
        // Finding a registered encoder with a matching codec ID...
         m_OutputStream.codec = const_cast<AVCodec*>(avcodec_find_encoder(m_OutputStream.outputFormat->video_codec));
        if (m_OutputStream.codec == NULL) {
            return ErrorReporter::CouldNotFindEncoder;
        }

       /* -------------- Allocate new output stream --------------*/
        m_OutputStream.stream= avformat_new_stream(m_OutputStream.formatContext, m_OutputStream.codec);
        if (m_OutputStream.stream== NULL) {
            return ErrorReporter::CouldNotAllocateOutputStream;
        }
        m_OutputStream.stream->id = m_OutputStream.formatContext->nb_streams - 1;

        /* -------------- Allocate codecContext --------------*/
        m_OutputStream.codecContext = avcodec_alloc_context3(m_OutputStream.codec);
        if (m_OutputStream.codecContext == NULL) {
            return ErrorReporter::CouldNotAllocateCodecContext;
        }

        /* -------------- Check codecType --------------*/
        switch (m_OutputStream.codec->type)
        {
            case AVMEDIA_TYPE_VIDEO:
            {
                m_OutputStream.codecContext->codec_id   = m_OutputStream.outputFormat->video_codec; // should be AV_CODEC_ID_GIF
                m_OutputStream.codecContext->bit_rate   = m_Settings.bitRate;
                
                m_OutputStream.codecContext->width      = m_Settings.outputWidth;
                m_OutputStream.codecContext->height     = m_Settings.outputHeight;
                
                m_OutputStream.codecContext->pix_fmt    = AV_PIX_FMT_RGB8;
                
                // Timebase: this is the fundamental unit of time (in seconds) in terms of which frame
                // timestamps are represented. For fixed-fps content, timebase should be 1/framerate
                // and timestamp increments should be identical to 1.
                
                // I needed to multiply this by 3 because for some reason, the GIF was 3x slower than normal.
                // GIF frame rates have a hard limit of 100FPS. If FRAME_RATE=30 i.e. 30FPS then this workaround actually encodes it at 90FPS
                // If however the GIF moves too fast because of this then remove the *3 from the next line.
                m_OutputStream.stream->time_base       = (AVRational){1, m_Settings.frameRate*3};
                m_OutputStream.codecContext->time_base = m_OutputStream.stream->time_base;
                
                // Intra frames only, no groups of frames. Bad for compression but I don't want to handle receiving extra frames here,
                // And I don't think GIF codec can even emit groups of frames.
                m_OutputStream.codecContext->gop_size  = 0;
                break;
            }
            case AVMEDIA_TYPE_UNKNOWN:
            case AVMEDIA_TYPE_AUDIO:
            case AVMEDIA_TYPE_DATA:
            case AVMEDIA_TYPE_SUBTITLE:
            case AVMEDIA_TYPE_ATTACHMENT:
            case AVMEDIA_TYPE_NB:
            {
                return ErrorReporter::InvalidMediaType;
                break;
            }
        }

        if (m_OutputStream.formatContext->oformat->flags & AVFMT_GLOBALHEADER) {
            m_OutputStream.codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    /* -------------- Open avCodecContext --------------*/
    error = avcodec_open2(m_OutputStream.codecContext, m_OutputStream.codec, NULL);
    if (error < 0) {
        return ErrorReporter::CouldNotOpenContexFormat;
    }
    /* -------------- Get parameters from avCodecContext --------------*/
    error = avcodec_parameters_from_context(m_OutputStream.stream->codecpar, m_OutputStream.codecContext);
    if (error < 0) {
        return ErrorReporter::CouldNotGetParametersFromContext;
    }
    
    /* -------------- Print information about file about to be created --------------*/
    av_dump_format(m_OutputStream.formatContext, 0, m_Settings.fileName.data(), 1);

    
    /* -------------- Open file towrite to --------------*/
    if (!(m_OutputStream.outputFormat->flags & AVFMT_NOFILE)) {
        // it should not have AVFMT_NOFILE
        error = avio_open(&m_OutputStream.formatContext->pb, m_Settings.fileName.data(), AVIO_FLAG_WRITE);
        if (error < 0) {
            return ErrorReporter::CouldNotOpenNewFile;
        }
    }

    /* -------------- Write file header --------------*/
    error = avformat_write_header(m_OutputStream.formatContext, NULL);
    if (error < 0) {
        return ErrorReporter::CouldNotWriteHeader;
    }
//    else if (error == AVSTREAM_INIT_IN_WRITE_HEADER) {
//        std::cerr << "AVSTREAM_INIT_IN_WRITE_HEADER" << std::endl;  // informational message, not an error
//    }
//    else if (error == AVSTREAM_INIT_IN_INIT_OUTPUT) {
//        std::cerr << "AVSTREAM_INIT_IN_INIT_OUTPUT" << std::endl;  // informational message, not an error
//    }
    
    /* -------------- Allocate packet --------------*/
    m_OutputStream.packet = av_packet_alloc();
    if (m_OutputStream.packet == NULL) {
        return ErrorReporter::CouldNotAllocatePacket;
    }
    /* -------------- Initiliaze packet --------------*/
    m_OutputStream.packet->data = NULL;
    m_OutputStream.packet->size = 0;

    
    /* -------------- Prepare AVFrames --------------*/
    // Here we need 3 frames for encoding. Basically, the QImage is firstly extracted in AV_PIX_FMT_BGRA.
    // We need then to convert it to AV_PIX_FMT_RGB8 which is required by the .gif format.
    // If we do that directly, there will be some artefacts and bad effects... to prevent that
    // we convert FIRST AV_PIX_FMT_BGRA into AV_PIX_FMT_YUV420P THEN into AV_PIX_FMT_RGB8.
    
    /* -------------- Allocate frames --------------*/
    m_OutputStream.frame = av_frame_alloc();
    m_OutputStream.tmpFrame = av_frame_alloc();
    m_OutputStream.yuvFrame = av_frame_alloc();
    
    /* -------------- Set parameters - frame --------------*/
    m_OutputStream.frame->width = m_OutputStream.codecContext->width;
    m_OutputStream.frame->height = m_OutputStream.codecContext->height;
    m_OutputStream.frame->format = m_OutputStream.codecContext->pix_fmt;

    /* -------------- Set parameters - tmpFrame --------------*/
    m_OutputStream.tmpFrame->width = m_OutputStream.codecContext->width;
    m_OutputStream.tmpFrame->height = m_OutputStream.codecContext->height;
    m_OutputStream.tmpFrame->format = AV_PIX_FMT_BGRA;
    
    /* -------------- Set parameters - yuvFrame --------------*/
    m_OutputStream.yuvFrame->width = m_OutputStream.codecContext->width;
    m_OutputStream.yuvFrame->height = m_OutputStream.codecContext->height;
    m_OutputStream.yuvFrame->format = AV_PIX_FMT_YUV420P;

    
    /* -------------- Check if tmpframe is writable --------------*/
    error = av_frame_is_writable(m_OutputStream.tmpFrame);
    if (error < 0) {
        return ErrorReporter::FrameisNotWritable;
    }

    /* -------------- Check if frame is writable --------------*/
    error = av_frame_is_writable(m_OutputStream.frame);
    if (error < 0) {
        return ErrorReporter::FrameisNotWritable;
    }
    
    /* -------------- Check if yuvFrame is writable --------------*/
    error = av_frame_is_writable(m_OutputStream.yuvFrame);
    if (error < 0) {
        return ErrorReporter::FrameisNotWritable;
    }
    
    /* -------------- Allocate image data for tmpFrame --------------*/
    error = av_image_alloc(m_OutputStream.tmpFrame->data, m_OutputStream.tmpFrame->linesize, m_Settings.inputWidth, m_Settings.inputHeight, AV_PIX_FMT_BGRA, 32);
    if (error < 0) {
        return ErrorReporter::CouldNotAllocateImageForFrame;
    }
    
    /* -------------- Allocate image data for yuvFrame --------------*/
    error = av_image_alloc(m_OutputStream.yuvFrame->data, m_OutputStream.yuvFrame->linesize, m_Settings.inputWidth, m_Settings.inputHeight, AV_PIX_FMT_YUV420P, 32);
    if (error < 0) {
        return ErrorReporter::CouldNotAllocateImageForFrame;
    }

    /* -------------- Allocate image data for frame --------------*/
    error = av_image_alloc(m_OutputStream.frame->data, m_OutputStream.frame->linesize, m_Settings.inputWidth, m_Settings.inputHeight, m_OutputStream.codecContext->pix_fmt, 32);
    if (error < 0) {
        return ErrorReporter::CouldNotAllocateImageForFrame;
    }

    if(m_Settings.doubleEncoding == true)
    {
        /* -------------- Allocate scale context for BGRA -> YUV420P --------------*/
        if (!m_OutputStream.swsCtx) {
            m_OutputStream.swsCtx = sws_getContext(m_Settings.inputWidth, m_Settings.inputHeight,
                                    AV_PIX_FMT_BGRA,
                                    m_Settings.inputWidth, m_Settings.inputHeight,
                                    AV_PIX_FMT_YUV420P,
                                    SWS_BICUBIC, NULL, NULL, NULL);
            if (m_OutputStream.swsCtx == nullptr) {
                return ErrorReporter::CouldNotAllocateContextForScaler;
            }
        }
        
        /* -------------- Allocate scale context for YUV420P -> RGB8 --------------*/
        if (!m_OutputStream.swsGIFCtx) {
            m_OutputStream.swsGIFCtx = sws_getContext(m_Settings.inputWidth, m_Settings.inputHeight,
                                       AV_PIX_FMT_YUV420P,
                                       m_OutputStream.codecContext->width, m_OutputStream.codecContext->height,
                                       m_OutputStream.codecContext->pix_fmt,
                                       SWS_BICUBIC, NULL, NULL, NULL);
            if (m_OutputStream.swsGIFCtx == nullptr) {
                return ErrorReporter::CouldNotAllocateContextForScaler;
            }
        }
    }
    else
    {
        /* -------------- Allocate scale context for YUV420P -> RGB8 --------------*/
        if(!m_OutputStream.swsFast)
        {
            m_OutputStream.swsFast = sws_getContext(m_Settings.inputWidth, m_Settings.inputHeight,
                                     AV_PIX_FMT_BGRA,
                                     m_OutputStream.codecContext->width,m_OutputStream.codecContext->height,
                                     m_OutputStream.codecContext->pix_fmt,
                                     SWS_BICUBIC, NULL, NULL, NULL);
            if(m_OutputStream.swsFast == nullptr) {
                return ErrorReporter::CouldNotAllocateContextForScaler;
            }
        }
    }
    
    return ErrorReporter::NoError;
}//init()


//Function used to convert data from API to frame
ErrorReporter Encoder::generateFrame(int sin_i,int i)
{
//    for (int y = 0; y < m_Settings.inputHeight; y++)
//    {
//        for (int x = 0; x < m_Settings.inputWidth * 4; x+=4) {
//            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+0] = 0;  //B
//            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+1] = 0xff;  //G
//            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+2] = 0;  //R
//            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+3] = 0xFF;    //A
//        }
//    }
    for (int y = 0; y < m_Settings.inputHeight; y++)
    {
        for (int x = 0; x < m_Settings.inputWidth * 4; x+=4) {
            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+0] = (x + i)&0xFF;
            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+1] = (y + i)&0xff;
            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+2] = sin_i&0xFF;  //R
            m_OutputStream.tmpFrame->data[0][y * m_OutputStream.tmpFrame->linesize[0] + x+3] = 0xFF;    //A
        }
    }
    
//    for (int y = 0; y <1080; ++y) {
//        uint8_t *row = &frame->m_rgb[y * lineWidth];
//        for (int x = 0; x < 1920; ++x) {
//            const int index = x * 3;
//            row[index + 0] = (x + i) & 0xFF; // r
//            row[index + 1] = (y + i) & 0xFF; // g
//            row[index + 2] = sin_i & 0xFF;   // b
//        }
    return ErrorReporter::NoError;
}//generateFrame


//Function used to write current frame data to file
ErrorReporter Encoder::addFrame()
{
    int error;

    /* If double encoding perform BRGA -> YUV420P -> RGB8*/
    if(m_Settings.doubleEncoding)
    {
        // This double scaling prevent some artifacts on the GIF and improve
        // significantly the display quality
        if (m_OutputStream.swsCtx != nullptr)
        {
            sws_scale(m_OutputStream.swsCtx,
                      (const uint8_t * const *)m_OutputStream.tmpFrame->data,
                      m_OutputStream.tmpFrame->linesize,
                      0,
                      m_OutputStream.codecContext->height,
                      m_OutputStream.yuvFrame->data,
                      m_OutputStream.yuvFrame->linesize);
        }
        if (m_OutputStream.swsGIFCtx != nullptr)
        {
            sws_scale(m_OutputStream.swsGIFCtx,
                      (const uint8_t * const *)m_OutputStream.yuvFrame->data,
                      m_OutputStream.yuvFrame->linesize,
                      0,
                      m_OutputStream.codecContext->height,
                      m_OutputStream.frame->data,
                      m_OutputStream.frame->linesize);
        }
    }
    else    //Single BGRA -> RGB8
    {
        if(m_OutputStream.swsFast != nullptr)
        {
            sws_scale(m_OutputStream.swsFast,
                      (const uint8_t * const *)m_OutputStream.tmpFrame->data,
                      m_OutputStream.tmpFrame->linesize,
                      0,
                      m_OutputStream.codecContext->height,
                      m_OutputStream.frame->data,
                      m_OutputStream.frame->linesize);
        }
    }

    /* -------------- Increase frame->pts --------------*/
    m_OutputStream.frame->pts = m_OutputStream.nextPts++;

    /* -------------- Send frame --------------*/
    error = avcodec_send_frame(m_OutputStream.codecContext, m_OutputStream.frame);
    if (error < 0) {
        return ErrorReporter::CouldNotSendFrame;
    }
    
    /* -------------- Receive packet --------------*/
    error = avcodec_receive_packet(m_OutputStream.codecContext, m_OutputStream.packet);
    if (error < 0) {
        return ErrorReporter::CouldNotReceivePacket;
    }
    
    /* -------------- Rescale output packet timestamp values from codec to stream timebase --------------*/
    av_packet_rescale_ts(m_OutputStream.packet, m_OutputStream.codecContext->time_base, m_OutputStream.stream->time_base);
    m_OutputStream.packet->stream_index = m_OutputStream.stream->index;
    
    /* -------------- Write the compressed frame to the media file --------------*/
    error = av_interleaved_write_frame(m_OutputStream.formatContext, m_OutputStream.packet);
    if (error < 0) {
        return ErrorReporter::CouldNotWriteFrame;
    }
    return ErrorReporter::NoError;
}//addFrame()

ErrorReporter Encoder::commitFile()
{
    int error;
    m_OutputStream.nextPts = 0;
    
    /* -------------- Free packets allocated for images  --------------*/
    if(& m_OutputStream.tmpFrame->data[0] != nullptr)
        av_freep(& m_OutputStream.tmpFrame->data[0]);
    if(& m_OutputStream.yuvFrame->data[0] != nullptr)
        av_freep(& m_OutputStream.yuvFrame->data[0]);
    if(& m_OutputStream.frame->data[0] != nullptr)
        av_freep(& m_OutputStream.frame->data[0]);

    /* -------------- Free allocated frames  --------------*/
    if( m_OutputStream.frame != nullptr)
    {
        av_frame_free(& m_OutputStream.frame);
        m_OutputStream.frame = nullptr;
    }
    if( m_OutputStream.tmpFrame != nullptr)
    {
        av_frame_free(& m_OutputStream.tmpFrame);
        m_OutputStream.tmpFrame = nullptr;
    }
    if( m_OutputStream.yuvFrame != nullptr)
    {
        av_frame_free(& m_OutputStream.yuvFrame);
        m_OutputStream.yuvFrame = nullptr;
    }
    
    /* -------------- Free allocated packet  --------------*/
    if( m_OutputStream.packet != nullptr)
    {
        av_packet_free(& m_OutputStream.packet);
        m_OutputStream.packet = nullptr;
    }
    
    /* -------------- Free allocated switchContextes  --------------*/
    if(m_Settings.doubleEncoding)
    {
        if( m_OutputStream.swsCtx != nullptr)
        {
            sws_freeContext( m_OutputStream.swsCtx);
            m_OutputStream.swsCtx = nullptr;
        }
        if( m_OutputStream.swsGIFCtx != nullptr)
        {
            sws_freeContext( m_OutputStream.swsGIFCtx);
            m_OutputStream.swsGIFCtx = nullptr;
        }
    }
    else
    {
        if( m_OutputStream.swsFast != nullptr)
        {
            sws_freeContext( m_OutputStream.swsFast);
            m_OutputStream.swsFast = nullptr;
        }
    }
        
    /* -------------- Write trailer to the file  --------------*/
    error = av_write_trailer( m_OutputStream.formatContext);
    if (error < 0) {
        return ErrorReporter::CouldNotWriteTrailer;
    }

    /* -------------- Free allocated codecContext  --------------*/
    if( m_OutputStream.codecContext != nullptr)
    {
        avcodec_free_context(& m_OutputStream.codecContext);
        m_OutputStream.codecContext = nullptr;
    }
    

    /* -------------- Close file  --------------*/
    if (!( m_OutputStream.outputFormat->flags & AVFMT_NOFILE)) {
        error = avio_closep(& m_OutputStream.formatContext->pb);
        if (error < 0) {
            return ErrorReporter::CouldNotCloseFile;
        }
    }
    /* -------------- Free alocated formatContext  --------------*/
    if( m_OutputStream.formatContext != nullptr)
    {
        avformat_free_context( m_OutputStream.formatContext);
        m_OutputStream.formatContext = nullptr;
    }
    
    return ErrorReporter::NoError;
}



}//End of AppToGIF namespace
