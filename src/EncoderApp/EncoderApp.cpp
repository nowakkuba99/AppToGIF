//
//  EncoderApp.cpp
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

#include "EncoderApp.hpp"
#include <iostream>

namespace AppToGIF
{
/* --- Functions to be called by user --- */
//Initialize FrameBuffer and set video settings
void EncoderApp::init(AppToGIF::GIFSettings settings)
{
    AppToGIF::ErrorReporter err;
    std::unique_lock<std::mutex> lock(m_Mutex);
    /* Creation of member variables */
    if(settings.frameRate>100)
    {
        settings.frameRate=100;
        std::cerr<<"Max frame rate for gif files is 100.\n";
    }
    /* Check the encoding mode selection */
    if(!settings.directEncoding && !settings.bgraEncoding && !settings.doubleEncoding)
    {
        settings.bgraEncoding = true;
        std::cerr<<"No encoding mode selected! Defualt bgraEncding selection done.\n";
    }
    p_Encoder = std::make_unique<Encoder>(settings);    //Create encoder object
    p_FrameBuffer = std::make_unique<FrameBuffer>();    //Create FrameBuffer
    
    /* Initialization of member variables */
    err = p_Encoder->init();
    if(err == ErrorReporter::NoError)
    {
        m_initialized = true;
    }
    else
    {
        m_initialized = false;
        std::cerr<<"Error while initializing encoder! \n";
    }
    m_FrameData.width = settings.inputWidth;
    m_FrameData.height = settings.inputHeight;
    if(settings.directEncoding == true)
        m_FrameData.lineWidth = av_image_get_linesize(AV_PIX_FMT_RGB8, settings.inputWidth, 0);
    else if(settings.bgraEncoding == true || settings.doubleEncoding == true)
        m_FrameData.lineWidth = av_image_get_linesize(AV_PIX_FMT_BGRA, settings.inputWidth, 0);
}

//Start the encoderApp - start second thread
void EncoderApp::startEncoder()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    if(m_running)
        std::cerr<<"Encoder already running!\n";
    else if(m_initialized)
    {
        m_running = true;
        p_WorkerThread = std::make_unique<std::thread>(&AppToGIF::EncoderApp::worker,this);
    }
    else if(!m_initialized)
    {
        std::cerr<<"Encoder not initialized! \n";
    }
    std::cout<<R"(
    _____              ___________      ________.______________
   /  _  \ ______ _____\__    ___/___  /  _____/|   \_   _____/
  /  /_\  \\____ \\____ \|    | /  _ \/   \  ___|   ||    __)  
 /    |    \  |_> >  |_> >    |(  <_> )    \_\  \   ||     \   
 \____|__  /   __/|   __/|____| \____/ \______  /___|\___  /   
         \/|__|   |__|                        \/         \/ 
    )" << "\n";
}

//Stop the encoderApp when file is ready
void EncoderApp::stopEncoder()
{
    m_running = false;
    p_FrameBuffer->setAppEnd();
}
//Destroy encoder thread
void EncoderApp::destroyEncoder()
{
    if(p_FrameBuffer != nullptr)
        p_FrameBuffer->notifyThread();
    if(p_WorkerThread != nullptr)
    {
        p_WorkerThread->join();
        p_WorkerThread.reset();
    }
    if(p_Encoder != nullptr)
    {
        p_Encoder->freeAllocatedData();
        p_Encoder.reset();
    }
}

/* --- Private functions --- */
//Function executed by worker thread
void EncoderApp::worker()
{
    ErrorReporter err = ErrorReporter::NoError;
    while (true)
    {
        std::shared_ptr<Frame> frame = p_FrameBuffer->waitForFrame();   //Get frame
        if(frame != nullptr)
        {
            err = p_Encoder->generateFrame(frame);                      //Generate AVFrame object
            if(err != AppToGIF::ErrorReporter::NoError) goto end;
            err = p_Encoder->addFrame();                                //Add frame to GIF file
            if(err != AppToGIF::ErrorReporter::NoError) goto end;
        }
        else
        {
            if(!m_running)                                              //If nullptr is returned check if app has been closed or asynchronus mode is set
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                break;
            }
        }
    }
    p_Encoder->commitFile();
end:
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_error = err;
    m_running = false;
    p_Encoder->freeAllocatedData();
}
}
