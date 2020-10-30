//
// Created by jackzhous on 2020/10/27.
//

#ifndef NDKMEDIACODEC_CAMERAENGINE_H
#define NDKMEDIACODEC_CAMERAENGINE_H

#include <jni.h>
#include "ManagerCamera.h"
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

class CameraEngine{
    public:
        CameraEngine(JNIEnv* env, jobject instance, jint w, jint h);
        ~CameraEngine();

        ImageFormat* getImageFormat();

        void createCameraSession(jobject surface);

        void startPreview(bool preview);

        jobject getSurface() const;


private:
        JNIEnv* env;
        jobject jInstance;
        int32_t width;
        int32_t height;
        jobject surface;
        NDKCamera* camera;
        ImageFormat cameraFormat;



};

#endif //NDKMEDIACODEC_CAMERAENGINE_H
