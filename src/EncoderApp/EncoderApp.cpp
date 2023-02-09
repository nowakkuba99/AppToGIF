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
    std::unique_lock<std::mutex> lock(m_Mutex);
    /* Creation of member variables */
    p_Encoder = std::make_unique<Encoder>(settings);    //Create encoder object
    p_FrameBuffer = std::make_unique<FrameBuffer>();    //Create FrameBuffer
    
    /* Initialization of member variables */
    p_Encoder->init();
}

//Start the encoderApp - start second thread
void EncoderApp::startEncoder()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_running = true;
    p_WorkerThread = std::make_unique<std::thread>(&AppToGIF::EncoderApp::worker,this);
}

//Stop the encoderApp when file is ready
void EncoderApp::stopEncoder()
{
    m_running = false;
}
//Destroy encoder thread
void EncoderApp::destroyEncoder()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
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

/* --- Private functions --- */                 //PRZEMYŚL ROLĘ FRAME BUFFER!
//Function executed by worker thread
void EncoderApp::worker()
{
    ErrorReporter err = ErrorReporter::NoError;
    while (true) {
        std::shared_ptr<Frame> frame = p_FrameBuffer->waitForFrame();
        if(frame != nullptr)
        {
            err = p_Encoder->generateFrame(frame);
            if(err != AppToGIF::ErrorReporter::NoError) goto end;
            err = p_Encoder->addFrame();
            if(err != AppToGIF::ErrorReporter::NoError) goto end;
        }
        else
        {
            if(!m_running)
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
    destroyEncoder();
    
}
}
