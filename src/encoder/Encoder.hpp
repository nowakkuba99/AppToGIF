//
//  encoder.hpp
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

#ifndef encoder_hpp
#define encoder_hpp
/* --- Includes ---*/
#include <string>
/* --- User incldues --- */
#include "ffmpeg.h"
#include "errorReporter.h"


namespace AppToGIF {

/* --- GIF setting --- */
struct GIFSettings {
    std::string fileName = "";
    int inputWidth = 400;
    int inputHeight = 400;
    int outputWidth = 400;
    int outputHeight = 400;
    int frameRate = 11;
    int bitRate = 10;
    bool doubleEncoding = false;
    bool inputAlpha = false;
};

/* --- ffmpeg member variables --- */
struct OutputStream {
    //Contextes
    AVFormatContext* formatContext  = nullptr;
    AVCodecContext* codecContext    = nullptr;
    //OutputFormat and codec
    AVOutputFormat* outputFormat    = nullptr;
    AVCodec* codec                  = nullptr;
    //OutputStream
    AVStream* stream                = nullptr;
    //Frames
    AVFrame* frame                  = nullptr;
    AVFrame* tmpFrame               = nullptr;
    AVFrame* yuvFrame               = nullptr;
    //Packet
    AVPacket* packet                = nullptr;
    //ScaleContexetes
    struct SwsContext* swsCtx       = nullptr;
    struct SwsContext* swsGIFCtx    = nullptr;
    struct SwsContext* swsFast      = nullptr;
    //Frame counter
    int64_t nextPts = 0;
};

class Encoder
{
public:
    /* Constructor and Destructor */
    Encoder() = delete;
    Encoder(std::string fName)
    {
        m_Settings.fileName = fName;
    }
    ~Encoder() = default;
    /* Functions */
    //Function used to initialize ffpmeg pipeline
    ErrorReporter init();
    //Function used to convert data from API to frame
    ErrorReporter generateFrame(int sin_i,int i);
    //Function used to write current frame data to file
    ErrorReporter addFrame();
    //Function used to close file and write tail
    ErrorReporter commitFile();
    
    
    
private:
    /* --- File parameters --- */
    GIFSettings m_Settings;
    /* --- ffmpeg member variables --- */
    OutputStream m_OutputStream;
    
    /* --- Initialize variables --- */
    bool m_initialized = false;
    bool m_fileOpen = false;
    
    /* --- File ready variables --- */
    bool m_fileReady = false;
    
    /* --- Error reporter --- */
    ErrorReporter m_error = ErrorReporter::NoError;
};


}

#endif /* encoder_hpp */
