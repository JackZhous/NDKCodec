//
// Created by jackzhous on 2020/10/28.
//

#ifndef NDKMEDIACODEC_MANAGERCAMERA_H
#define NDKMEDIACODEC_MANAGERCAMERA_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCameraError.h>
#include <string>
#include <vector>
#include <map>
#include "ALog.h"

struct ImageFormat{
    int32_t width;
    int32_t height;

    int32_t format;
};

enum class CaptureSessionState : int32_t {
    READY = 0,
    ACTIVE,
    CLOSED,
    MAX_STATE
};

struct CaptureRequestInfo {
    ANativeWindow* outputWindow;
    ACaptureSessionOutput* sessionOutput;
    ACameraOutputTarget* target;
    ACaptureRequest* request;
    ACameraDevice_request_template atemplate;
    int sessionSequenceId;
};

enum PREVIEW_INCLUDES {
    PREVIEW_REQUEST_IDX = 0,
    JPG_CAPTURE_REQUEST_IDX,
    CAPTURE_REQUEST_COUNT
};


template <typename T>
class RangeValue{
public:
    T min, max;
    T value(int percent){
        return static_cast<T>(min + (max - min) * percent / 100);
    }

    RangeValue(){
        min = max = static_cast<T>(0);
    }

    bool support(void) const {
        return min != max;
    }
};

class CameraId;
class NDKCamera{
    public:
        NDKCamera();


    ~NDKCamera();

    bool matchCaptureSizeRequest(int32_t width, int32_t height, ImageFormat* resView);

    bool matchCaptureSizeRequest(int32_t width, int32_t height, ImageFormat* resView, ImageFormat* resCap);

    void enumerateCamera();

    void onCameraStateChanged(const char* id, bool avaliable);

    void createSession(ANativeWindow* previewWindow);
    void createSession(ANativeWindow* previewWindow, ANativeWindow* jpgWindow, bool manualPreview, int32_t rotation);
    void startPreview(bool start);

    void OnSessionState(ACameraCaptureSession* ses, CaptureSessionState state);

    void onDeviceState(ACameraDevice* dev);
    void onDeviceError(ACameraDevice* dev, int err);


private:
    uint32_t cameraOrientation;
    ACameraManager* cameraMr;
    std::map<std::string, CameraId> cameras;
    std::string activeCameraId;
    //相机正面
    uint32_t cameraFacing;

    std::vector<CaptureRequestInfo> request;

    ACaptureSessionOutputContainer* outputContainer;
    ACameraCaptureSession* captureSession;
    CaptureSessionState captureSessionState;
    //曝光时间
    int64_t exposureTime;
    volatile bool valid;
    RangeValue<int64_t > exposureRange;
    int32_t sensitivity;
    RangeValue<int32_t > sensitivityRange;

    ACameraDevice_stateCallbacks* GetDeviceListener();
    ACameraManager_AvailabilityCallbacks* GetManagerListener();
    ACameraCaptureSession_stateCallbacks* GetSessionListener();


};

class CameraId{
public:
    ACameraDevice* device;
    std::string id_;
    acamera_metadata_enum_android_lens_facing_t facing;
    bool avaliable;  // free to use ( no other apps are using
    bool owner;  // we are the owner of the camera
    explicit CameraId(const char* id) : device(nullptr),facing(ACAMERA_LENS_FACING_FRONT),
                                        avaliable(false), owner(false){
        id_ = id;
    }


    explicit CameraId(void){ CameraId(""); }

};

#endif //NDKMEDIACODEC_MANAGERCAMERA_H
