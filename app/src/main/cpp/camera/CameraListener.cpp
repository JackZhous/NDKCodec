//
// Created by jackzhous on 2020/10/29.
//

#include "ManagerCamera.h"
#include "CameraUtil.h"

void onCameraAvailable(void* context,const char* cameraId){
    reinterpret_cast<NDKCamera*>(context)->onCameraStateChanged(cameraId, true);
}

void onCameraUnavailable(void* context,const char* cameraId){
    reinterpret_cast<NDKCamera*>(context)->onCameraStateChanged(cameraId, false);
}

// CaptureSession state callbacks
void OnSessionClosed(void* ctx, ACameraCaptureSession* ses) {
    LOGW("session %p closed", ses);
    reinterpret_cast<NDKCamera*>(ctx)
            ->OnSessionState(ses, CaptureSessionState::CLOSED);
}
void OnSessionReady(void* ctx, ACameraCaptureSession* ses) {
    LOGW("session %p ready", ses);
    reinterpret_cast<NDKCamera*>(ctx)
            ->OnSessionState(ses, CaptureSessionState::READY);
}
void OnSessionActive(void* ctx, ACameraCaptureSession* ses) {
    LOGW("session %p active", ses);
    reinterpret_cast<NDKCamera*>(ctx)
            ->OnSessionState(ses, CaptureSessionState::ACTIVE);
}


void OnDeviceStateChanges(void* ctx, ACameraDevice* dev) {
    reinterpret_cast<NDKCamera*>(ctx)->onDeviceState(dev);
}

void OnDeviceErrorChanges(void* ctx, ACameraDevice* dev, int err) {
    reinterpret_cast<NDKCamera*>(ctx)->onDeviceError(dev, err);
}

ACameraDevice_stateCallbacks* NDKCamera::GetDeviceListener(){
    static ACameraDevice_stateCallbacks lis = {
            .context = this,
            .onDisconnected = ::OnDeviceStateChanges,
            .onError = OnDeviceErrorChanges
    };
    return &lis;
}


ACameraManager_AvailabilityCallbacks* NDKCamera::GetManagerListener(){
    static ACameraManager_AvailabilityCallbacks listener = {
            .context = this,
            .onCameraAvailable = ::onCameraAvailable,
            .onCameraUnavailable = ::onCameraUnavailable
    };
    return &listener;
}

ACameraCaptureSession_stateCallbacks* NDKCamera::GetSessionListener() {
    static ACameraCaptureSession_stateCallbacks lis = {
            .context = this,
            .onActive = ::OnSessionActive,
            .onClosed = ::OnSessionClosed,
            .onReady = ::OnSessionReady
    };
    return &lis;
}

void NDKCamera::OnSessionState(ACameraCaptureSession *ses, CaptureSessionState state) {
    if(!ses || ses != captureSession){
        LOGW("CaptureSession is NULL or is other session");
        return;
    }
    captureSessionState = state;
}

void NDKCamera::onCameraStateChanged(const char *id, bool avaliable) {
    if(valid){
        cameras[id].avaliable = avaliable;
    }
}

void NDKCamera::onDeviceState(ACameraDevice *dev) {
    std::string id(ACameraDevice_getId(dev));
    LOGW("camera id %s is disconnected", id.c_str());
    cameras[id].avaliable = false;
    ACameraDevice_close(cameras[id].device);
    cameras.erase(id);
}

void NDKCamera::onDeviceError(ACameraDevice *dev, int err) {
    std::string id(ACameraDevice_getId(dev));
    LOGW("camera id %s is error", id.data());
    PrintCameraDeviceError(err);

    CameraId& cam = cameras[id];
    switch (err) {
        case ERROR_CAMERA_IN_USE:
            cam.avaliable = false;
            cam.owner = false;
            break;
        case ERROR_CAMERA_SERVICE:
        case ERROR_CAMERA_DEVICE:
        case ERROR_CAMERA_DISABLED:
        case ERROR_MAX_CAMERAS_IN_USE:
            cam.avaliable = false;
            cam.owner = false;
            break;
        default:
            LOGI("Unknown Camera Device Error: %#x", err);
    }
}