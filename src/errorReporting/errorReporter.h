//
//  errorReporter.h
//  AppToGIF
//
//  Created by Jakub Nowak on 26/01/2023.
//

#ifndef errorReporter_h
#define errorReporter_h
namespace AppToGIF
{
enum class ErrorReporter
{
    NoError,
    CouldNotAllocateFormatContext,
    CouldNotFindEncoder,
    CouldNotAllocateOutputStream,
    CouldNotAllocateCodecContext,
    InvalidMediaType,
    CouldNotOpenContexFormat,
    CouldNotGetParametersFromContext,
    CouldNotOpenNewFile,
    CouldNotWriteHeader,
    CouldNotAllocatePacket,
    FrameisNotWritable,
    CouldNotAllocateImageForFrame,
    CouldNotAllocateContextForScaler,
    CouldNotSendFrame,
    CouldNotReceivePacket,
    CouldNotWriteFrame,
    CouldNotWriteTrailer,
    CouldNotCloseFile,
    CouldNotGetFrame,
    CouldNotCommitFrameSinceBufferIsEmpty,
    CouldNotGenerateFrame
};
}
#endif /* errorReporter_h */
