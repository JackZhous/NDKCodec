//
// Created by jackzhous on 2020/10/23.
//

#include <bits/struct_file.h>
#include "H264Encoder.h"

void H264Encoder::prepareVideoCodec(int32_t w, int32_t h, int32_t fpss) {
    width = w;
    height = h;
    fps = fpss;

    codec = AMediaCodec_createEncoderByType(VIDEO_ENCODER_NAME);
    AMediaFormat *format = AMediaFormat_new();
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, VIDEO_ENCODER_NAME);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT, SURFACE);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, fps);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 1);

    media_status_t s = AMediaCodec_configure(codec, format, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if(AMEDIA_OK != s){
        AMediaCodec_delete(codec);
        codec = nullptr;
        LOGE("configure video codec failed");
        return;
    }

//    AMediaFormat_delete(format);
//    FILE *file = fopen(filePath.c_str(), "wb");
//    if(file != nullptr){
//        mFd = fileno(file);
//    } else{
//        LOGE("create file %s failed", filePath.c_str());
//    }
//    if(muxer == nullptr){
//        muxer = AMediaMuxer_new(mFd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
//    }
//    isInit = true;
//    fclose(file);
//    prepareEncoderWithShareCtx(nullptr);
}

void H264Encoder::prepareEncoderWithShareCtx(EGLContext* context) {
    AMediaCodec_createInputSurface(codec, &inputSurface);
    if(AMediaCodec_start(codec) != AMEDIA_OK){
        LOGE("start meida codec failed");
        return;
    }
    codecInputSurface = new CodecInputSurface(inputSurface);
    codecInputSurface->setShareCtx(context);
}

void H264Encoder::startEncode() {

}

void H264Encoder::stopEncode() {

}

void H264Encoder::releaseCodec() {
    if(codec != nullptr){

    }
}


AMediaFormat* H264Encoder::getSupportFormat() {

}

H264Encoder::H264Encoder() : Codec(CodecVideo), width(0), height(0), mFd(-1), inputSurface(nullptr){
}

void H264Encoder::setFilePath(std::string file) {
    filePath = file;
}

H264Encoder::~H264Encoder() {
    codec = nullptr;
    if(inputSurface != nullptr){
        ANativeWindow_release(inputSurface);
        inputSurface = nullptr;
    }
}


