//
//  ffmpeg.h
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

#ifndef ffmpeg_h
#define ffmpeg_h

extern "C" {
    #include <libavutil/avassert.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/opt.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/timestamp.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
}

#endif /* ffmpeg_h */
