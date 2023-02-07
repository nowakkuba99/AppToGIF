//
//  FrameBuffer.cpp
//  AppToGIF
//
//  Created by Jakub Nowak on 31/01/2023.
//

#include "FrameBuffer.hpp"

namespace AppToGIF
{

//Get frame that can be filled with data
//Returns frame if possible and nullptr if frameBuffer is full
std::shared_ptr<AppToGIF::Frame> FrameBuffer::getFrame()
{
    if(m_Buffer.size() < m_Size && !m_APIHasFrame)
    {
        m_Buffer.push(std::make_shared<AppToGIF::Frame>());
        m_APIHasFrame = true;
        return m_Buffer.back();
    }
    return nullptr; //Buffer full or frame not passed to buffer!
}

//Get frame filled with data to frame buffer
//The app can no longer have access to data in order to work
AppToGIF::ErrorReporter FrameBuffer::passFrame()
{
    if(m_Buffer.back().unique() && m_APIHasFrame)
    {
        std::cout<<"Success!"<<std::endl;
        std::cout<<m_Buffer.front()->m_width<<std::endl; //Debug
        m_APIHasFrame = false;
        return ErrorReporter::NoError;
    }
    return ErrorReporter::CouldNotGetFrame;
}

//Commit frame from the top of the buffer
//Sends frame to encoder to write to file
AppToGIF::ErrorReporter FrameBuffer::commitFrame()
{
    if(m_Buffer.size()>0 && !m_Buffer.front()->ready())     //If some frame has not been passed to encoder
    {
        m_Buffer.front()->setReady();
        m_ConditionVariable.notify_one();
        return ErrorReporter::NoError;
    }
    return ErrorReporter::CouldNotCommitFrameSinceBufferIsEmpty;
}


//Function to wait for frame to be ready for encoding
//Returns frame filled with data from user
std::shared_ptr<AppToGIF::Frame> FrameBuffer::waitForFrame()
{
    //Lock frame access
    std::unique_lock<std::mutex> lock(m_Mutex);
    //Prepare return value
    std::shared_ptr<AppToGIF::Frame> retValue;
    retValue = m_Buffer.front();
    //Check condition
    m_ConditionVariable.wait(lock,[this]()
                                        {
                                            return m_Buffer.front()->ready();
                                        });
    //Delete pointer and delete Frame object from queue
    m_Buffer.front().reset();
    m_Buffer.pop();
    //Return pointer to ready frame
    return retValue;
}
}
