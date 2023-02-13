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
#include "errorReporter.h"

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
    void init(AppToGIF::GIFSettings settings);
    //Start the encoderApp - start second thread
    void startEncoder();
    //Stop the encoderApp when file is ready
    void stopEncoder();
    //Destroy encoder thread
    void destroyEncoder();
    
    /* --- Frames related functions --- */
    //Get frame that can be filled with data
    //Returns frame if possible and nullptr if frameBuffer is full
    inline std::shared_ptr<AppToGIF::Frame> getFrame()
    {
        return p_FrameBuffer->getFrame();
    }
    //Get frame filled with data to frame buffer
    //The app can no longer have access to data in order to work
    inline AppToGIF::ErrorReporter passFrame()
    {
        return p_FrameBuffer->passFrame();
    }
    //Commit frame from the top of the buffer
    //Sends frame to encoder to write to file
    inline AppToGIF::ErrorReporter commitFrame()
    {
        return p_FrameBuffer->commitFrame();
    }
    
    /* --- Encoder mode setter and getter --- */
    inline bool getIfModeAsynchonous() const {return m_asynchronousMode;};
    inline void setAsynchronousMode() {m_asynchronousMode = true;};
    
private:
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
    /* --- App run variable --- */
    bool m_running = false;
    bool m_initialized = false;
    bool m_asynchronousMode = false;
    /* --- Error reporting variable --- */
    AppToGIF::ErrorReporter m_error = AppToGIF::ErrorReporter::NoError;
    
};
}
    
#endif /* EncoderApp_hpp */
