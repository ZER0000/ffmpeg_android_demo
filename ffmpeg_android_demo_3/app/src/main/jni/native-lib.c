#include <jni.h>

extern "C"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif


//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char* fmt, va_list vl){
    FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
    if(fp){
        vfprintf(fp,fmt,vl);
        fflush(fp);
        fclose(fp);
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_13_MainActivity_decode(JNIEnv *env,
                                                                       jobject instance,
                                                                       jstring inputurl_,
                                                                       jstring outputurl_) {
    const char *inputurl = (*env)->GetStringUTFChars(env, inputurl_, 0);
    const char *outputurl = (*env)->GetStringUTFChars(env, outputurl_, 0);

    // TODO

    (*env)->ReleaseStringUTFChars(env, inputurl_, inputurl);
    (*env)->ReleaseStringUTFChars(env, outputurl_, outputurl);

    return (*env)->NewStringUTF(env, returnValue);
}