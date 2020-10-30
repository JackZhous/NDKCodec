//
// Created by jackzhous on 2020/10/23.
//

#ifndef NDKMEDIACODEC_G711ENCODER_H
#define NDKMEDIACODEC_G711ENCODER_H

#include "Codec.h"


class G711Encoder : Codec {

public:
    G711Encoder();

    virtual void prepareCodec();

    virtual void startEncode();

    virtual void stopEncode();

    virtual void releaseCodec();


private:

};

#endif //NDKMEDIACODEC_G711ENCODER_H
