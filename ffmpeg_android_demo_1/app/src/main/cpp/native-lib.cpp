#include <jni.h>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_11_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_11_MainActivity_string2FromJNI(JNIEnv *env,
                                                                               jobject instance) {

    // TODO
    char info[10000] = {0};
    sprintf(info, "%s \n", avcodec_configuration());


    return env->NewStringUTF(info);
}

}
