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

    uint8_t* m_rgb;
    int m_width, m_height;
    int m_maxWidth, m_maxHeight;
    int m_lineWidth;
    /**
     * Returns pointer to the begining of asked row.
     *
     *  After specifing the row numer function returns the pointer to the first element of the row which
     *  can be filled with data accoring to picked format used for encoding.
     *
     *  @param rowNum Number of row that you want to fill with data
     *  @return rowPointer Pointer to the begining of allocated data connected to asked row
     */
    inline uint8_t* getRow(int rowNum) {return &m_rgb[rowNum * m_lineWidth];};
private:
    inline void setReady() { m_ReadyToEncode = true; }
    inline bool ready() const { return m_ReadyToEncode; }
    bool m_ReadyToEncode = false;
};
}

#endif /* frame_hpp */
