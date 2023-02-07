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
#include "Encoder.hpp"

#ifndef EncoderApp_hpp
#define EncoderApp_hpp
namespace AppToGIF
{
class EncoderApp
{
public:
    /* --- Copy constructor and assign operator deletion --- */
    EncoderApp(const EncoderApp&) = delete;
    void operator=(const EncoderApp&) = delete;
    
    /* --- Functions to be called by user --- */
    //Initialize FrameBuffer and set video settings
    void init(std::string fileName = "AppToGIF.gif");
    
    //Start the encoderApp - start second thread
    void startEncoder();
    //Stop the encoderApp when file is ready
    void stopEncoder();
    
    /* --- Private functions --- */
    //Function executed by worker thread
    void worker();
    
protected:
    /* --- Constructor and destructor --- */
    EncoderApp() = default;
    ~EncoderApp()
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
public:
    /* --- Singelton class instance getter --- */
    static EncoderApp& getInstance()
    {
        static EncoderApp instance;
        return instance;
    }

private:
    
    /* --- Frame ownership variables --- */
    std::mutex m_Mutex;
    /* --- Thread responsible for encoding --- */
    std::unique_ptr<std::thread> p_WorkerThread;
    /* --- Frame bufffer --- */
    std::unique_ptr<FrameBuffer> p_FrameBuffer;
    /* --- Encoder object --- */
    std::unique_ptr<Encoder> p_Encoder;
    
};
}
    
#endif /* EncoderApp_hpp */
