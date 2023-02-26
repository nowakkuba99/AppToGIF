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
    //const int lineWidth = av_image_get_linesize(AV_PIX_FMT_BGRA, 255, 0);
    const int lineWidth = av_image_get_linesize(AV_PIX_FMT_RGB8, 255, 0);

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
    //set.inputAlpha = true;
    set.directEncoding = true;;
    set.bgraEncoding = false;
    set.doubleEncoding = false;
    app.init(set);
    app.startEncoder();
    std::shared_ptr<AppToGIF::Frame> appFrame;
    app.setAsynchronousMode();
    int counter = 0;
    for(int i = 0; i<255; i++)
    {
        int sin_i = std::lroundf(255 * (0.5 + 0.5 * std::sin(i * 0.01)));
        appFrame = app.getFrame();
        if(appFrame != nullptr)
        {
            {
                std::cout<<"Frame: "<<++counter<<"\n";
                for (int y = 0; y < set.outputHeight ; y++) {
                    uint8_t *row = appFrame->getRow(y);
                    for (int x = 0; x < set.outputWidth; x++) {
                        //const int index = x*4;  //4 for alpha channel
                        row[x] = (sin_i)&255;//((x+(i))&0xFF); //
//                        row[index + 0] = ((sin_i+(i))&0xFF);    //b
//                        row[index + 1] = ((128)&0xFF);    //g
//                        row[index + 2] = ((sin_i+(i))&0xFF);    //r
//                        row[index + 3] = 0xFF; // a
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
// Build in features like:
// - appFrame->getrow(),
// Fix issue with black line at the bottom
// Create build option with cmake
