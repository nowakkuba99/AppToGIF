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
#include "../Frame/Frame.hpp"
#include "../FrameBuffer/FrameBuffer.hpp"
#include "../encoder/Encoder.hpp"
#include "../errorReporting/errorReporter.h"

#ifndef EncoderApp_hpp
#define EncoderApp_hpp
namespace AppToGIF
{
struct FrameData
{
    int width = 0;
    int height = 0;
    int lineWidth = 0;
};
class EncoderApp
{
public:
    /* --- Copy constructor and assign operator deletion --- */
    EncoderApp(const EncoderApp&) = delete;
    void operator=(const EncoderApp&) = delete;
    
    /* --- Functions to be called by user --- */
    /**
     * Initialize FrameBuffer and set video settings.
     *
     * After specifying settings for the encoder, the application will be initialized with given settings.
     * @param settings Settings structure that specifies how you want to create your GIF file
     */
    void init(AppToGIF::GIFSettings settings);
    
    /**
     * Starting the flow related to the encoder application.
     *
     * Function starts the second thread and related with it worker function that encodes GIF files in
     * the background. Function uses condition variables and mutexes to avoid confilts with your main app.
     */
    void startEncoder();
    
    /**
     * Function stops endocer when all frames that you wanted to encode where written to the file.
     *
     * Function call results in ending worker thread and writing tail to a GIF file that is necessary to correct encoding.
     */
    void stopEncoder();
    
    /**
     * Free allocated data related with encoder app.
     *
     * Calling this function free's all resources related to the encoder app. This step is not necessary however when you want to use your application
     * after creating GIF file.
     */
    void destroyEncoder();
    
    /* --- Frames related functions --- */
    /**
     * Get frame that can be filled with data from your application.
     *
     * This function call results in different results based on a selected mode:
     *
     *  - Synchronous mode: Function always returns a pointer to a Frame object that can be filled with data. This can take some time based on GIF file size.
     *  
     *  - Asynchronous mode: Function can return a pointer to a Frame object or a nullptr if encoding has not yet finished on last frame.
     *
     * @return Frame object shared pointer/nullptr - see full description.
     *
     */
    inline std::shared_ptr<AppToGIF::Frame> getFrame()
    {
        auto temp = p_FrameBuffer->getFrame();
        if(temp != nullptr)
        {
            temp->m_width = m_FrameData.width;
            temp->m_height = m_FrameData.height;
            temp->m_lineWidth = m_FrameData.lineWidth;
            temp->m_rgb =  new uint8_t[static_cast<size_t>(m_FrameData.height)*m_FrameData.lineWidth];
        }
        return temp;
    }

    /**
     * Pass frame filled with data to frame buffer.
     *
     * Function passes a Frame object to encoder.
     *
     * Warning: Before passing frame to encoder your application has to reset the ownership of an shared pointer with Frame.
     *
     *@return Error Error reporter object: Check if equal to noError.
     */
    inline AppToGIF::ErrorReporter passFrame()
    {
        return p_FrameBuffer->passFrame();
    }

    /**
     * Commit frame from the top of the buffer
     *
     * Function sends the frame from the top of the buffer for encoding in GIF file through ffmpeg interface.
     *
     *@return Error Error reporter object: Check if equal to noError.
     */
    inline AppToGIF::ErrorReporter commitFrame()
    {
        return p_FrameBuffer->commitFrame();
    }
    
    /* --- Encoder mode setter and getter --- */
    inline bool getIfModeAsynchonous() const {return p_FrameBuffer != nullptr ? p_FrameBuffer->getIfModeAsynchonous() : false;};
    inline void setAsynchronousMode() {if(p_FrameBuffer != nullptr) p_FrameBuffer->setAsynchronousMode();};
    
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
    /* --- Frame data --- */
    FrameData m_FrameData;
};
}
    
#endif /* EncoderApp_hpp */
