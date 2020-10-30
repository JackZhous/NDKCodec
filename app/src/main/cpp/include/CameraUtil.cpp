//
// Created by jackzhous on 2020/10/28.
//

#include "CameraUtil.h"
#include <typeinfo>
#include <vector>
#include "ALog.h"

#define MAKE_PAIR(value) std::make_pair(value, #value)


template <typename T>
const char * GetPairStr(T key, std::vector<std::pair<T, const char*>> &store){
    typedef typename std::vector<std::pair<T, const char*>>::iterator iterator;
    for (iterator it = store.begin(); it != store.end(); ++it) {
        if(it->first == key){
            return it->second;
        }
    }
}

using ERROR_PAIR = std::pair<camera_status_t, const char *>;
static std::vector<ERROR_PAIR> errorInfo {
        MAKE_PAIR(ACAMERA_OK),
        MAKE_PAIR(ACAMERA_ERROR_UNKNOWN),
        MAKE_PAIR(ACAMERA_ERROR_INVALID_PARAMETER),
        MAKE_PAIR(ACAMERA_ERROR_CAMERA_DISCONNECTED),
        MAKE_PAIR(ACAMERA_ERROR_NOT_ENOUGH_MEMORY),
        MAKE_PAIR(ACAMERA_ERROR_METADATA_NOT_FOUND),
        MAKE_PAIR(ACAMERA_ERROR_CAMERA_DEVICE),
        MAKE_PAIR(ACAMERA_ERROR_CAMERA_SERVICE),
        MAKE_PAIR(ACAMERA_ERROR_SESSION_CLOSED),
        MAKE_PAIR(ACAMERA_ERROR_INVALID_OPERATION),
        MAKE_PAIR(ACAMERA_ERROR_STREAM_CONFIGURE_FAIL),
        MAKE_PAIR(ACAMERA_ERROR_CAMERA_IN_USE),
        MAKE_PAIR(ACAMERA_ERROR_MAX_CAMERA_IN_USE),
        MAKE_PAIR(ACAMERA_ERROR_CAMERA_DISABLED),
        MAKE_PAIR(ACAMERA_ERROR_PERMISSION_DENIED),
};

const char* GetErrorStr(camera_status_t err){
    return GetPairStr(err, errorInfo);
}


using DEV_ERR_PAIT = std::pair<int , const char *>;
static std::vector<DEV_ERR_PAIT > devError{
        MAKE_PAIR(ERROR_CAMERA_IN_USE),   MAKE_PAIR(ERROR_MAX_CAMERAS_IN_USE),
        MAKE_PAIR(ERROR_CAMERA_DISABLED), MAKE_PAIR(ERROR_CAMERA_DEVICE),
        MAKE_PAIR(ERROR_CAMERA_SERVICE),
};

const char* GetCameraDeviceErrorStr(int err) {
    return GetPairStr<int>(err, devError);
}

void PrintCameraDeviceError(int err){
    LOGE("print camera error code %#x and error desc %s", err, GetCameraDeviceErrorStr(err));
}