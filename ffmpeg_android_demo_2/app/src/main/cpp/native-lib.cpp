#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "FFNative"
#define ALOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define ALOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_12_MainActivity_avProtocolInfo(JNIEnv *env,
                                                                      jobject instance) {
    char info[40000] = {0};
    av_register_all();

    void *opaque = NULL;
    const char *name;

    //Input
    while((name = avio_enum_protocols(&opaque, 0)))
        sprintf(info, "%s[Input] : [%10s] \n", info, name);

    //Output
    opaque = NULL;
    while((name = avio_enum_protocols(&opaque, 1)))
        sprintf(info, "%s[Output] : [%10s] \n", info, name);

    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_12_MainActivity_avFormatInfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};

    av_register_all();

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    while (if_temp != NULL) {
        sprintf(info, "%s[Input]: %10s, %s \n", info, if_temp->name, if_temp->long_name ? if_temp->long_name : " ");
        if_temp = if_temp->next;
    }
    while (of_temp != NULL) {
        sprintf(info, "%s[Output]: %10s, %s \n", info, of_temp->name, of_temp->long_name ? of_temp->long_name : " ");
        of_temp = of_temp->next;
    }
    ALOGI("%s", info);
    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_12_MainActivity_avCodecInfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};

    av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%s[Dec]:", info);
        } else {
            sprintf(info, "%s[Enc]:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s (video) : ", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s (audio) : ", info);
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                sprintf(info, "%s (subtitle) : ", info);
                break;
            default:
                sprintf(info, "%s (other) : ", info);
                break;
        }
        sprintf(info, "%s[%10s] %s \n", info, c_temp->name, c_temp->long_name ? c_temp->long_name : " ");
        c_temp = c_temp->next;
    }
    ALOGI("%s", info);
    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_12_MainActivity_avFilterInfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};
    avfilter_register_all();

    AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
    while (f_temp != NULL) {
        sprintf(info, "%s%s\n", info, f_temp->name);
        f_temp = f_temp->next;
    }
    ALOGI("%s", info);
    return env->NewStringUTF(info);
}

}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_12_MainActivity_avConfigurationInfo(JNIEnv *env,
                                                                           jobject instance) {

    char info[10000] = {0};

    av_register_all();

    sprintf(info, "%s \n", avcodec_configuration());

    return env->NewStringUTF(info);
}

