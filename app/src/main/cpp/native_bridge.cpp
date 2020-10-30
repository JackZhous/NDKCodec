#include <jni.h>
#include <string>
#include <H264Encoder.h>
#include "ALog.h"
#include "ALog.h"
#include "CameraEngine.h"

static JavaVM* JVM = NULL;
const char* JAVA_CLASS = "com/jz/codec/MainActivity";
class BridgPointer{

public:
    CameraEngine* engine;
    H264Encoder* encoder;

    BridgPointer(): engine(nullptr), encoder(nullptr) {}
};

static BridgPointer bridge;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_jz_codec_MainActivity_createNDKCamera(JNIEnv *env, jobject thiz, jint w, jint h) {
    CameraEngine* engine = new CameraEngine(env, thiz, w, h);
    LOGI("create ndk camera");
    return reinterpret_cast<jlong>(engine);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_jz_codec_MainActivity_getPreviewSize(JNIEnv *env, jobject thiz, jlong pointer) {
    if(!pointer){
        return nullptr;
    }
    CameraEngine* engine = reinterpret_cast<CameraEngine *>(pointer);
    jclass size = env->FindClass("android/util/Size");
    jmethodID constructor = env->GetMethodID(size, "<init>", "(II)V");
    return env->NewObject(size, constructor, engine->getImageFormat()->width, engine->getImageFormat()->height);
}


extern "C" JNIEXPORT void JNICALL
Java_com_jz_codec_MainActivity_onPreviewSurfaceCreated(JNIEnv *env, jobject thiz, jlong pointer,
                                                       jobject surface) {
    if(!pointer){
        return;
    }
    CameraEngine* engine = reinterpret_cast<CameraEngine *>(pointer);
    engine->createCameraSession(surface);
    engine->startPreview(true);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jz_codec_MainActivity_onPreviewSurfaceDestroyed(JNIEnv *env, jobject thiz, jlong pointer,
                                                         jobject surface) {
    if(!pointer){
        return;
    }
    CameraEngine* engine = reinterpret_cast<CameraEngine *>(pointer);
    jclass cls = env->FindClass("android/view/Surface");
    jmethodID toString = env->GetMethodID(cls, "toString", "()Ljava/lang/String;");
    jstring destroyObjStr = reinterpret_cast<jstring>(env->CallObjectMethod(surface, toString));
    const char *destroyObjName = env->GetStringUTFChars(destroyObjStr, nullptr);

    jstring appobj = reinterpret_cast<jstring>(env->CallObjectMethod(engine->getSurface(), toString));
    const char *appObjName = env->GetStringUTFChars(appobj, nullptr);
    env->ReleaseStringUTFChars(destroyObjStr, destroyObjName);
    env->ReleaseStringUTFChars(appobj, appObjName);

    engine->startPreview(false);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jz_codec_MainActivity_deleteCamera(JNIEnv *env, jobject thiz, jlong pointer) {
    if(!pointer){
        return;
    }
    CameraEngine* engine = reinterpret_cast<CameraEngine *>(pointer);
    delete engine;
}



extern "C"
JNIEXPORT jlong JNICALL
Java_com_jz_codec_MainActivity_createEncoder(JNIEnv *env, jobject thiz, jint w, jint h) {
    if(!bridge.encoder){
        bridge.encoder = new H264Encoder();
    }
}