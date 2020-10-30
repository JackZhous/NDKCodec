//
// Created by jackzhous on 2020/10/28.
//

#include <CameraUtil.h>
#include "ManagerCamera.h"
#include <camera/NdkCameraManager.h>
#include <media/NdkImage.h>
#include <cstring>

/**
 * Range of Camera Exposure Time:
 *     Camera's capability range have a very long range which may be disturbing
 *     on camera. For this sample purpose, clamp to a range showing visible
 *     video on preview: 100000ns ~ 250000000ns
 */
static const uint64_t kMinExposureTime = static_cast<uint64_t>(1000000);
static const uint64_t kMaxExposureTime = static_cast<uint64_t>(250000000);

/**
 * 图像尺寸对比辅助类，不在乎横竖屏模式
 */
class DisplayDimension{
public:
    DisplayDimension(int32_t w, int32_t h) : w_(w), h_(h), portrait(false){
        if(h > w){
            w_ = h;
            h_ = w;
            portrait = true;
        }
    }

    DisplayDimension(const DisplayDimension& other){
        w_ = other.w_;
        h_ = other.h_;
        portrait = other.portrait;
    }

    DisplayDimension(){
        w_ = 0;
        h_ = 0;
        portrait = false;
    }

    DisplayDimension& operator=(const DisplayDimension other){
        w_ = other.w_;
        h_ = other.h_;
        portrait = other.portrait;
        return (*this);
    }

    /**
     * 是否是相同的比例
     * @param other
     * @return
     */
    bool isSameRatio(DisplayDimension& other){
        return (w_ * other.h_ == h_ * other.w_);
    }

    bool operator>(DisplayDimension& other){
        return (w_ >= other.w_ & h_ >= other.h_);
    }

    bool operator==(DisplayDimension& other){
        return (w_ == other.w_ & h_ == other.h_);
    }

    DisplayDimension* operator-(DisplayDimension& other){
        DisplayDimension* delta = new DisplayDimension(w_ - other.w_, h_ - other.h_);
        return delta;
    }
    void flip(){portrait = !portrait;}

    int32_t with() const {
        return w_;
    }

    int32_t height() const {
        return h_;
    }

    bool isPortrait() const {
        return portrait;
    }

    int32_t org_width(){ return portrait ? h_ : w_;}
    int32_t org_height(){ return portrait ? w_ : h_;}

private:
    int32_t w_, h_;
    bool portrait;
};

/**
 * 选择相机，获取相机属性，灵敏度、曝光时间
 */
NDKCamera::NDKCamera()
                : cameraMr(nullptr), activeCameraId(""), cameraFacing(ACAMERA_LENS_FACING_BACK),
                cameraOrientation(0), outputContainer(nullptr), captureSessionState(CaptureSessionState::MAX_STATE),
                exposureTime(0){
    valid = false;
    request.resize(CAPTURE_REQUEST_COUNT);
    memset(request.data(), 0, request.size() * sizeof(request[0]));
    cameras.clear();
    cameraMr = ACameraManager_create();
    if(cameraMr == NULL){
        LOGE("camera manager create failed");
        return ;
    }
    //选择摄像头，后置优先
    enumerateCamera();
    //第三个参数不是很懂  打开相机，并把相机device保存
    CALL_MGR(openCamera(cameraMr, activeCameraId.c_str(), GetDeviceListener(), &cameras[activeCameraId].device));
    CALL_MGR(registerAvailabilityCallback(cameraMr, GetManagerListener()));
    ACameraMetadata* metadata;
    CALL_MGR(getCameraCharacteristics(cameraMr, activeCameraId.c_str(), &metadata));
    ACameraMetadata_const_entry val = {0};
    camera_status_t status = ACameraMetadata_getConstEntry(metadata, ACAMERA_SENSOR_INFO_EXPOSURE_TIME_RANGE, &val);
    if(ACAMERA_OK == status){
        exposureRange.min = val.data.i64[0] < kMinExposureTime ? kMinExposureTime : val.data.i64[0];
        exposureRange.max = val.data.i64[1] > kMaxExposureTime ? kMaxExposureTime : val.data.i64[1];
        exposureTime = exposureRange.value(2);
    } else{
        LOGW("此相机传感器不支持设定的曝光时间");
        exposureRange.max = exposureRange.min = 0l;
        exposureTime = 0l;
    }
    //应该是快门灵敏度，就是从按下快门到开始曝光的这段时间
    status = ACameraMetadata_getConstEntry(metadata, ACAMERA_SENSOR_INFO_SENSITIVITY_RANGE, &val);
    if(ACAMERA_OK == status){
        sensitivityRange.min = val.data.i32[0];
        sensitivityRange.max = val.data.i32[1];
        sensitivity = sensitivityRange.value(2);
    } else{
        LOGW("此相机没有获取灵敏度的api");
        sensitivityRange.max = sensitivityRange.min = 0;
        sensitivity = 0;
    }
    valid = true;
}

NDKCamera::~NDKCamera() {
    valid = false;
    if(captureSessionState == CaptureSessionState::ACTIVE){
        ACameraCaptureSession_stopRepeating(captureSession);
    }
    ACameraCaptureSession_close(captureSession);
    for(auto& req : request){
        if(!req.outputWindow) continue;

        CALL_REQUEST(removeTarget(req.request, req.target));
        ACaptureRequest_free(req.request);
        ACameraOutputTarget_free(req.target);

        CALL_CONTAINER(remove(outputContainer, req.sessionOutput));
        ACaptureSessionOutput_free(req.sessionOutput);
        ANativeWindow_release(req.outputWindow);
    }

    request.resize(0);
    ACaptureSessionOutputContainer_free(outputContainer);

    for(auto& cam : cameras){
        if(cam.second.device){
            CALL_DEV(close(cam.second.device));
        }
    }
    cameras.clear();
    if(cameraMr){
        CALL_MGR(unregisterAvailabilityCallback(cameraMr, GetManagerListener()));
        ACameraManager_delete(cameraMr);
        cameraMr = nullptr;
    }
}

bool NDKCamera::matchCaptureSizeRequest(int32_t width, int32_t height, ImageFormat *format) {
    return matchCaptureSizeRequest(width, height, format, nullptr);
}

/**
 * 找寻窗口尺寸与相机预览尺寸比例相同的
 * @param width
 * @param height
 * @param resView
 * @param resCap
 * @return
 */
bool NDKCamera::matchCaptureSizeRequest(int32_t width, int32_t height, ImageFormat *resView,
                                        ImageFormat *resCap) {
    DisplayDimension dis(width, height);
    if(cameraOrientation == 90 || cameraOrientation == 270){
        dis.flip();
    }
    ACameraMetadata *metadata;
    CALL_MGR(getCameraCharacteristics(cameraMr, activeCameraId.c_str(), &metadata));
    ACameraMetadata_const_entry entry;
    CALL_METADATA(getConstEntry(metadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry));
    // format of the data: format, width, height, input?, type int32
    bool foundId = false;
    DisplayDimension foundRes(4000, 4000);
    DisplayDimension maxJPG(0, 0);
    for (int i = 0; i < entry.count; i += 4) {
        int32_t input = entry.data.i32[i+3];
        int32_t format = entry.data.i32[i+0];
        if(input) continue;
        //寻找YUV和JPEG格式，并且比例要和屏幕比例的相机
        if(format == AIMAGE_FORMAT_YUV_420_888 || format == AIMAGE_FORMAT_JPEG){
            DisplayDimension res(entry.data.i32[i+1], entry.data.i32[i+2]);
            if(!dis.isSameRatio(res)) continue;
            if(format == AIMAGE_FORMAT_YUV_420_888 && foundRes > res){
                foundId = true;
                foundRes = res;
            } else if(format == AIMAGE_FORMAT_JPEG && res > maxJPG){
                maxJPG = res;
            }
        }
    }
    //找到合适的尺寸
    if(foundId){
        resView->width = foundRes.org_width();
        resView->height = foundRes.org_height();
        if(resCap){
            resCap->width = maxJPG.org_width();
            resCap->height = maxJPG.org_height();
        }
    } else{
        LOGW("没有找到合适的尺寸，就自定义一个尺寸640x480");
        if(dis.isPortrait()){
            resView->width = 480;
            resView->height = 640;
        } else{
            resView->width = 640;
            resView->height = 480;
        }
        if(resCap){
            *resCap = *resView;
        }
    }
    resView->format = AIMAGE_FORMAT_YUV_420_888;
    if(resCap) resCap->format = AIMAGE_FORMAT_JPEG;
    return foundId;
}

/**
 * 如果可用的话，选择后置摄像；否则就前置摄像头
 */
void NDKCamera::enumerateCamera() {
    ACameraIdList* cameraIds = nullptr;
    camera_status_t status1 = ACameraManager_getCameraIdList(cameraMr , &cameraIds);
//    CALL_MGR(getCameraIdList(cameraMr, &cameraIds));
    for(int i = 0; i < cameraIds->numCameras; i++){
        const char* id = cameraIds->cameraIds[i];

        ACameraMetadata* metadata;
        CALL_MGR(getCameraCharacteristics(cameraMr, id, &metadata));
        int32_t count = 0;
        const uint32_t *tags = nullptr;
        ACameraMetadata_getAllTags(metadata, &count, &tags);
        for(int j=0; j< count; j++){
            //如果这个tag是包含有相机朝向属性的
            if(ACAMERA_LENS_FACING == tags[j]){
                ACameraMetadata_const_entry lensInfo = {0};
                CALL_METADATA(getConstEntry(metadata,tags[j], &lensInfo));
                CameraId cam(id);
                cam.facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(lensInfo.data.u8[0]);
                cameras[cam.id_] = cam;
                if(cam.facing == ACAMERA_LENS_FACING_BACK){
                    activeCameraId = cam.id_;
                }
                break;
            }
        }
        ACameraMetadata_free(metadata);
    }
    if(activeCameraId.length() == 0){
        activeCameraId = cameras.begin()->second.id_;
    }
    ACameraManager_deleteCameraIdList(cameraIds);
}


void NDKCamera::createSession(ANativeWindow *previewWindow) {
    createSession(previewWindow, nullptr, false, 0);
}

void NDKCamera::createSession(ANativeWindow *previewWindow, ANativeWindow *jpgWindow,
                              bool manualPreview, int32_t rotation) {
    request[PREVIEW_REQUEST_IDX].outputWindow = previewWindow;
    //定义此次请求是一个预览的request
    request[PREVIEW_REQUEST_IDX].atemplate = TEMPLATE_PREVIEW;
    request[JPG_CAPTURE_REQUEST_IDX].outputWindow = jpgWindow;
    request[JPG_CAPTURE_REQUEST_IDX].atemplate = TEMPLATE_STILL_CAPTURE;

    CALL_CONTAINER(create(&outputContainer));
    for(auto& req : request){
        if(!req.outputWindow) continue;

        ANativeWindow_acquire(req.outputWindow);
        CALL_OUTPUT(create(req.outputWindow, &req.sessionOutput));
        CALL_CONTAINER(add(outputContainer, req.sessionOutput));
        CALL_TARGET(create(req.outputWindow, &req.target));
        CALL_DEV(createCaptureRequest(cameras[activeCameraId].device, req.atemplate, &req.request));
        //为他请求的目标相机target配置请求request参数
        CALL_REQUEST(addTarget(req.request, req.target));
    }
    captureSessionState = CaptureSessionState ::READY;
    CALL_DEV(createCaptureSession(cameras[activeCameraId].device, outputContainer,GetSessionListener(), &captureSession));

    if(jpgWindow){
        ACaptureRequest_setEntry_i32(request[JPG_CAPTURE_REQUEST_IDX].request, ACAMERA_JPEG_ORIENTATION, 1, &rotation);
    }

    if(!manualPreview){
        return;
    }
}

void NDKCamera::startPreview(bool start) {
    if(start){
        CALL_SESSION(setRepeatingRequest(captureSession, nullptr, 1, &request[PREVIEW_REQUEST_IDX].request,
                                         nullptr));
    } else if(captureSessionState == CaptureSessionState::ACTIVE){
        CALL_SESSION(stopRepeating(captureSession));
    }
}

