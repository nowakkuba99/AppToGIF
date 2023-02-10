//
//  frame.hpp
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

#ifndef frame_hpp
#define frame_hpp
#include <cinttypes>
namespace AppToGIF {
//Define FrameBuffer class so that it can be set as friend class
class FrameBuffer;

class Frame
{
    friend class FrameBuffer;
public:
    Frame() = default;
    ~Frame()
    {
        delete [] m_rgb;
    }

    uint8_t *m_rgb;
    int m_width, m_height;
    int m_maxWidth, m_maxHeight;
    int m_lineWidth;
private:
    inline void setReady() { m_ReadyToEncode = true; }
    inline bool ready() const { return m_ReadyToEncode; }
    bool m_ReadyToEncode = false;
};
}

#endif /* frame_hpp */
