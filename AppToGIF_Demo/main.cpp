/*
    AppToGIF demo application
    Created by Jakub Nowak 2023.
 */

/* --- Common includes --- */
#include <iostream>
#include <random>
/* --- AppToGIF library include --- */
#include "AppToGIF.h"

/* --- Main function --- */
int main()
{
    const int lineWidth = av_image_get_linesize(AV_PIX_FMT_BGRA, 255, 0);

    AppToGIF::EncoderApp& app = AppToGIF::EncoderApp::getInstance();
    AppToGIF::GIFSettings set;
    set.fileName = "AppToGIF.gif";
    set.inputHeight = 255;
    set.inputWidth = 255;
    set.outputWidth = 255;
    set.outputHeight = 255;
    set.bitRate = 100000;
    set.frameRate = 50;
    set.bitRate = 10000;
    app.init(set);
    app.startEncoder();
    std::shared_ptr<AppToGIF::Frame> appFrame;
    app.setAsynchronousMode();
    for(int i = 0; i<255; i+=2)
    {
        int sin_i = std::lroundf(255 * (0.5 + 0.5 * std::sin(i * 0.01)));
        appFrame = app.getFrame();
        if(appFrame != nullptr)
        {
            {
                std::cout<<"Frame: "<<i<<"\n";
                appFrame->m_width=255;
                appFrame->m_height=255;
                appFrame->m_rgb = new uint8_t[static_cast<size_t>(255)*lineWidth];
                for (int y = 0; y < 255; ++y) {
                    uint8_t *row = &appFrame->m_rgb[y * lineWidth];
                    for (int x = 0; x < 255; x++) {
                        const int index = x*4;
                        row[index + 0] = ((x+(i))&0xFF); // b
                        row[index + 1] = ((sin_i)&0xFF);//((y+(i))&0xFF); // g
                        row[index + 2] = 0; // r
                        row[index + 3] = 0xFF; // a
                    }
                }
            }
            appFrame.reset();
            app.passFrame();
            app.commitFrame();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(500));  //Simulate asynchronous mode -> Some work beign done my main thread
    }
    app.stopEncoder();
    app.destroyEncoder();
    
    std::cout<<"GIF file has been created!\n";
    return 0;
}



/* Notes */
// No conversion option - straight to output format
// Create build option with cmake
