//
// Created by jackzhous on 2020/10/30.
//

#include "CodecInputSurface.h"

CodecInputSurface::CodecInputSurface(ANativeWindow *surface) : surface(surface) {}

CodecInputSurface::~CodecInputSurface() {

}

void CodecInputSurface::setShareCtx(EGLContext *shareCtx) {
    CodecInputSurface::shareCtx = shareCtx;
}
