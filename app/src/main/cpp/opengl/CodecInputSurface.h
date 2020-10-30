//
// Created by jackzhous on 2020/10/30.
//

#ifndef NDKMEDIACODEC_CODECINPUTSURFACE_H
#define NDKMEDIACODEC_CODECINPUTSURFACE_H


#include <android/native_window.h>
#include <EGL/egl.h>

class CodecInputSurface{
public:
    CodecInputSurface(ANativeWindow *surface);

    virtual ~CodecInputSurface();

    void setShareCtx(EGLContext *shareCtx);

private:
    ANativeWindow* surface;
    EGLContext *shareCtx;

};

#endif //NDKMEDIACODEC_CODECINPUTSURFACE_H
