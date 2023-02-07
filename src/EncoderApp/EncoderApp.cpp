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
void EncoderApp::init(std::string fileName)
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    /* Creation of member variables */
    p_Encoder = std::make_unique<Encoder>(fileName);    //Create encoder object
    p_FrameBuffer = std::make_unique<FrameBuffer>();    //Create FrameBuffer
    
    /* Initialization of member variables */
    p_Encoder->init();
}

//Start the encoderApp - start second thread
void EncoderApp::startEncoder()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
}

//Stop the encoderApp when file is ready
void EncoderApp::stopEncoder()
{
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
    while (true) {
        std::shared_ptr<Frame> frame = p_FrameBuffer->waitForFrame();
        if(frame != nullptr)
        {
            p_Encoder->generateFrame(frame);
        }
    }
}
}
