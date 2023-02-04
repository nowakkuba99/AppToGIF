//
//  API.hpp
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

/* --- Common includes --- */
#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono> //TO BE DELETED

/* --- User includes --- */
#include "Frame.hpp"
#include "FrameBuffer.hpp"

#ifndef EncoderApp_hpp
#define EncoderApp_hpp
namespace AppToGIF
{
class EncoderApp
{
public:
    void start(FrameBuffer* bufferPointer)
    {
        m_Buffer = bufferPointer;
        m_TestThread = std::make_unique<std::thread>(&AppToGIF::EncoderApp::test,this);
    }
    
    void test()
    {
        std::cout<<"Start thread 2\n";
        std::unique_lock<std::mutex> lock(m_Mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        m_Buffer->passFrame();
        lock.unlock();
        m_Buffer->testCVNotify();
        //m_ConditionVariable.notify_all();
        std::cout<<"Notify poszedl!\n";
    }
    
    void stop()
    {
        m_TestThread->join();
        m_TestThread.reset();
    }
    
private:
    /* --- Frame ownership variables --- */
    std::mutex m_Mutex;
//    std::condition_variable m_ConditionVariable;
    std::unique_ptr<std::thread> m_TestThread;
    
    FrameBuffer* m_Buffer;
    
};
}
    
#endif /* EncoderApp_hpp */
