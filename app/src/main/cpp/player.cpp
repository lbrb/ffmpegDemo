//
// Created by hasika on 2017/12/19.
//

#include <jni.h>
#include "log.h"
#include "inc/ffmpeg/libavformat/avformat.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>


struct player {
    AVFormatContext *avFormatContext;
    AVCodecContext *codecContext[5];
    const AVCodec *codec[5];
    int videoIndex;
    int audioIndex;
} *playP;

extern "C"
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("%s", "JNI_OnLoad");
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_init(JNIEnv *env, jclass type) {

    AVFormatContext *avFormatContext = avformat_alloc_context();
    av_register_all();

    playP->avFormatContext = avFormatContext;

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_destroy(JNIEnv *env, jclass type) {

}

void initCodec(const char *inputPath) {
    int ret;
    ret = avformat_open_input(&playP->avFormatContext, inputPath, NULL, NULL);
    if (ret != 0) {
        LOGI("%s", "avformat_open_input error");
        return;
    }

    ret = avformat_find_stream_info(playP->avFormatContext, NULL);
    if (ret != 0) {
        LOGI("%s", "avformat_find_stream_info error");
        return;
    }

    int i;
    for (i = 0; i < playP->avFormatContext->nb_streams; ++i) {
        AVStream *avStream = playP->avFormatContext->streams[i];
        AVCodecContext *avCodecContext = avStream->codec;
        const AVCodec *avCodec = avCodecContext->codec;
        playP->codecContext[avCodec->type] = avCodecContext;
        playP->codec[avCodec->type] = (AVCodec *) avCodec;
        if (avCodec->type == AVMEDIA_TYPE_VIDEO) {
            playP->videoIndex = i;
        } else if (avCodec->type == AVMEDIA_TYPE_AUDIO) {
            playP->audioIndex = i;
        }
    }
}

void decodeVideo(AVPacket inputPacket, jobject jsurface) {
    int ret, got;
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    JNI_CreateJavaVM(&javaVM, &jniEnv,NULL);
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(jniEnv,jsurface);

    AVFrame *inputFrame = av_frame_alloc();
    ANativeWindow_Buffer outBuffer;
    avpicture_fill((AVPicture *) inputFrame, (const uint8_t *) outBuffer.bits, AV_PIX_FMT_ARGB,
                   playP->codecContext[AVMEDIA_TYPE_VIDEO]->width,
                   playP->codecContext[AVMEDIA_TYPE_VIDEO]->height);


    ret = avcodec_decode_video2(playP->codecContext[AVMEDIA_TYPE_VIDEO], inputFrame, &got,
                                &inputPacket);
    if (ret < 0) {
        LOGI("%s", "avcodec_decode_video2 error");
        return;
    }

    if (got) {
        ANativeWindow_setBuffersGeometry(nativeWindow,playP->codecContext[AVMEDIA_TYPE_VIDEO]->width,
                                         playP->codecContext[AVMEDIA_TYPE_VIDEO]->height,)
    }


    av_frame_free(&inputFrame);

}

void decodeAudio() {

}

void decodePackage(AVMediaType type) {
    AVPacket inputPacket;
    av_init_packet(&inputPacket);

    int got;


    int ret;
    ret = avcodec_open2(playP->codecContext[type], playP->codec[type], NULL);
    if (ret != 0) {
        LOGI("%s", "avcodec_open2 error");
        return;
    }

    while (!av_read_frame(playP->avFormatContext, &inputPacket)) {
        if (inputPacket.stream_index == playP->videoIndex) {
            decodeVideo(inputPacket);
        } else if (inputPacket.stream_index == playP->audioIndex) {

        }

    }

    avcodec_close(playP->codecContext[type]);
    av_free_packet(&inputPacket);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_play(JNIEnv *env, jclass type, jstring inputPath_,
                                            jobject surface) {
    const char *inputPath = env->GetStringUTFChars(inputPath_, 0);

    initCodec(inputPath);


    avformat_close_input(&playP->avFormatContext);
    avformat_free_context(playP->avFormatContext);
    env->ReleaseStringUTFChars(inputPath_, inputPath);
}


