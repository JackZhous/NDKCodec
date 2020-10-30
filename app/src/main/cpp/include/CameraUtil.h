//
// Created by jackzhous on 2020/10/28.
//

#ifndef NDKMEDIACODEC_CAMERAUTIL_H
#define NDKMEDIACODEC_CAMERAUTIL_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraError.h>
#include "ALog.h"

#define CALL_CAMERA(func)  \
    {                           \
        camera_status_t status = func;  \
        if(status != ACAMERA_OK){        \
            LOGE("%s call failed with code: %d, %s", __FUNCTION__, status, GetErrorStr(status));    \
        }                                                                       \
    }

#define CALL_MGR(func) CALL_CAMERA(ACameraManager_##func)
#define CALL_METADATA(func) CALL_CAMERA(ACameraMetadata_##func)
#define CALL_CONTAINER(func) CALL_CAMERA(ACaptureSessionOutputContainer_##func)
#define CALL_OUTPUT(func) CALL_CAMERA(ACaptureSessionOutput_##func)
#define CALL_TARGET(func) CALL_CAMERA(ACameraOutputTarget_##func)
#define CALL_DEV(func) CALL_CAMERA(ACameraDevice_##func)
#define CALL_REQUEST(func) CALL_CAMERA(ACaptureRequest_##func)
#define CALL_SESSION(func) CALL_CAMERA(ACameraCaptureSession_##func)


const char* GetErrorStr(camera_status_t err);
const char* GetCameraDeviceErrorStr(int err);
void PrintCameraDeviceError(int err);

#endif //NDKMEDIACODEC_CAMERAUTIL_H
