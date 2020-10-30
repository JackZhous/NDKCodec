//
// Created by jackzhous on 2020/10/23.
//

#ifndef NDKMEDIACODEC_CODEC_H
#define NDKMEDIACODEC_CODEC_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaMuxer.h>

enum CodecType {CodecVideo, CodecAudio};


class Codec{

protected:
    AMediaCodec *codec;
    bool isInit;

private:
    CodecType codecType;


public:
    Codec(CodecType type) : codec(nullptr){
        codecType = type;
        isInit = false;
    }
    virtual void prepareCodec() = 0;

    virtual void startEncode() = 0;

    virtual void stopEncode() = 0;

    virtual void releaseCodec() = 0;

    CodecType getCodecType() const{ return codecType; }
};

#endif //NDKMEDIACODEC_CODEC_H
