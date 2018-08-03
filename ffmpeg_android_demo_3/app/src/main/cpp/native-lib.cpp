#include <jni.h>
#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"
#include "libavutil/common.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"

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

JNIEXPORT jint JNICALL
Java_com_example_nxf31081_ffmpeg_1android_1demo_13_MainActivity_decode(JNIEnv *env,
                                                                       jobject instance,
                                                                       jstring inputurl_,
                                                                       jstring outputurl_) {
    const char *inputurl = env->GetStringUTFChars(inputurl_, NULL);
    const char *outputurl = env->GetStringUTFChars(outputurl_, NULL);

    AVFormatContext	*pFormatCtx;
    int				i, videoindex;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    AVCodecParameters *pCodecPar;
    AVFrame	*pFrame,*pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double  time_duration = 0.0;
    int err_code;
    char err_buf[128];
    char info[1000]={0};

    // TODO
    //FFmpeg av_log() callback
    av_log_set_callback(custom_log);

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if(err_code = avformat_open_input(&pFormatCtx, inputurl, NULL, NULL) != 0){
        av_strerror(err_code, err_buf, 1024);
        LOGE("Couldn't open input stream %s, error code: %d (%s). \n", inputurl, err_code, err_buf);
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx, NULL)<0){
        LOGE("Couldn't find stream information.\n");
        return -1;
    }

    videoindex = -1;
    for(i = 0; i < pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
            break;
        }
    if(videoindex == -1){
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }
    pCodecPar = pFormatCtx->streams[videoindex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    if(pCodec == NULL){
        LOGE("Couldn't find Codec.\n");
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);

    //Copy context
    if(avcodec_parameters_to_context(pCodecCtx, pCodecPar) != 0) {
        fprintf(stderr, "Couldn't copy codec context.");
        return -1;
    }

    LOGI("Stream frame rate ：%d fps\n", pFormatCtx->streams[videoindex]->r_frame_rate.num /
                           pFormatCtx->streams[videoindex]->r_frame_rate.den);

    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
                         AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


    sprintf(info,   "[Input     ]%s\n", inputurl);
    sprintf(info, "%s[Output    ]%s\n", info, outputurl);
    sprintf(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution]%dx%d\n", info, pCodecCtx->width,pCodecCtx->height);


    fp_yuv = fopen(outputurl,"wb+");
    if(fp_yuv==NULL){
        printf("Cannot open output file.\n");
        return -1;
    }

    frame_cnt = 0;
    time_start = clock();

    while(av_read_frame(pFormatCtx, packet) >= 0){
        if(packet->stream_index == videoindex){
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                LOGE("Decode Error.\n");
                return -1;
            }
            if(got_picture){
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                y_size=pCodecCtx->width*pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrameYUV->data[1], 1, y_size/4, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size/4, fp_yuv);  //V
                //Output info
                char pictype_str[10]={0};
                switch(pFrame->pict_type){
                    case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
                    case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
                    case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
                    default:sprintf(pictype_str,"Other");break;
                }
                LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size=pCodecCtx->width*pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, y_size/4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, y_size/4, fp_yuv);  //V
        //Output info
        char pictype_str[10]={0};
        switch(pFrame->pict_type){
            case AV_PICTURE_TYPE_I:sprintf(pictype_str, "I");break;
            case AV_PICTURE_TYPE_P:sprintf(pictype_str, "P");break;
            case AV_PICTURE_TYPE_B:sprintf(pictype_str, "B");break;
            default:sprintf(pictype_str,"Other");break;
        }
        LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double)(time_finish - time_start);

    sprintf(info, "%s[Time      ]%fms\n", info, time_duration);
    sprintf(info, "%s[Count     ]%d\n", info, frame_cnt);

    sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(inputurl_, inputurl);
    env->ReleaseStringUTFChars(outputurl_, outputurl);

    return 0;
}

}
