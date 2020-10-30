//
// Created by jackzhous on 2020/10/23.
//

#ifndef NDKMEDIACODEC_ALOG_H
#define NDKMEDIACODEC_ALOG_H

#include <android/log.h>

#define TAG_I "jni_tag_i"
#define TAG_E "jni_tag_e"
#define TAG_W "jni_tag_w"

#ifdef __ANDROID__
    #define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, TAG_I, format, ##__VA_ARGS__)
    #define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR, TAG_E, format, ##__VA_ARGS__)
    #define LOGW(format, ...) __android_log_print(ANDROID_LOG_WARN, TAG_W, format, ##__VA_ARGS__)
#else
    #define LOGI(format, ...) {}
    #define LOGE(format, ...) {}
#endif

#endif //NDKMEDIACODEC_ALOG_H
