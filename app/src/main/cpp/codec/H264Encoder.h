//
// Created by jackzhous on 2020/10/23.
//

#ifndef NDKMEDIACODEC_H264ENCODER_H
#define NDKMEDIACODEC_H264ENCODER_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <CodecInputSurface.h>
#include <string>
#include "Codec.h"
#include "ALog.h"


#define VIDEO_ENCODER_NAME "video/avc"

class H264Encoder : Codec{

public:
    H264Encoder();

    void prepareCodec() override{};

    void startEncode() override;

    void stopEncode() override;

    void releaseCodec() override;

    void prepareVideoCodec(int32_t w, int32_t h, int32_t fps);

    void setFilePath(std::string file);

    virtual ~H264Encoder();

private:
    AMediaFormat* getSupportFormat();
    int32_t width, height;
    int32_t fps;
    std::string filePath;
    int mFd;
    ANativeWindow* inputSurface;
    AMediaMuxer* muxer;
    CodecInputSurface* codecInputSurface;


    void prepareEncoderWithShareCtx(EGLContext* context);
};


#endif //NDKMEDIACODEC_H264ENCODER_H
