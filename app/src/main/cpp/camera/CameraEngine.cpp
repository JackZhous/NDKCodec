//
// Created by jackzhous on 2020/10/27.
//

#include "CameraEngine.h"
#include <cstdio>
#include <cstring>

CameraEngine::CameraEngine(JNIEnv *env, jobject instance, jint w, jint h)
    : env(env), jInstance(instance), width(w), height(h), camera(nullptr),surface(nullptr){
    memset(&cameraFormat, 0, sizeof(cameraFormat));
    camera = new NDKCamera();
    camera->matchCaptureSizeRequest(width, height, &cameraFormat);
}

CameraEngine::~CameraEngine() {
    if(camera != nullptr){
        delete camera;
        camera = nullptr;
    }
    if(surface != nullptr){
        env->DeleteGlobalRef(surface);
        surface = nullptr;
    }
}


ImageFormat* CameraEngine::getImageFormat() {
    return &cameraFormat;
}

void CameraEngine::createCameraSession(jobject surf) {
    surface = env->NewGlobalRef(surf);
    camera->createSession(ANativeWindow_fromSurface(env, surf));
}


void CameraEngine::startPreview(bool preview) {
    camera->startPreview(preview);
}

jobject CameraEngine::getSurface() const {
    return surface;
}
