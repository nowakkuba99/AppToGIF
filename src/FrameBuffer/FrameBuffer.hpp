//
//  FrameBuffer.hpp
//  AppToGIF
//
//  Created by Jakub Nowak on 31/01/2023.
//
#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../Frame/Frame.hpp"
#include "../errorReporting/errorReporter.h"

#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp

namespace AppToGIF
{
class FrameBuffer
{
public:
    /* --- Constructor and Destructor --- */
    FrameBuffer(unsigned int size = 2)
    {
        if(size > m_MaxSize)
        {
            m_Size = m_MaxSize;
            std::cout<<"Max size exceeded. Setting size to:"<<m_MaxSize<<"\n";
        }
        else
            m_Size = size;
    };
    
    /* --- Functions --- */
    //Get frame that can be filled with data
    //Returns frame if possible and nullptr if frameBuffer is full
    std::shared_ptr<AppToGIF::Frame> getFrame();
    
    //Get frame filled with data to frame buffer
    //The app can no longer have access to data in order to work
    AppToGIF::ErrorReporter passFrame();
    
    //Commit frame from the top of the buffer
    //Sends frame to encoder to write to file
    AppToGIF::ErrorReporter commitFrame();
    
    //Function to wait for frame to be ready for encoding
    //Returns frame filled with data from user
    std::shared_ptr<AppToGIF::Frame> waitForFrame();
    
    //Function used to close the application
    inline void notifyThread() {m_ConditionVariable.notify_one();};
    
    //Function to set variable informing buffer that app has ended
    inline void setAppEnd() {m_AppEnd = true;};
    
    /* --- Encoder mode setter and getter --- */
    inline bool getIfModeAsynchonous() const {return m_AsynchronousMode;};
    inline void setAsynchronousMode() {m_AsynchronousMode = true;};
    
private:
    /* --- Buffer queue --- */
    std::queue<std::shared_ptr<AppToGIF::Frame>> m_Buffer;
    
    /* --- Buffer size related variables --- */
    unsigned int m_Size;
    const unsigned int m_MaxSize = 5;
    
    /* --- Frame owership variables --- */
    bool m_APIHasFrame = false;
    /* --- Multi threaded related variables --- */
    std::mutex m_Mutex;
    std::condition_variable m_ConditionVariable;
    /* --- Application settings variables --- */
    bool m_AppEnd = false;
    bool m_AsynchronousMode = false;
    
};
}

#endif /* FrameBuffer_hpp */
