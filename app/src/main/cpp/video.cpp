

extern "C" {
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "log.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
#include "libyuv/convert_argb.h"
#include <unistd.h>


JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_decodeYUV(JNIEnv *env, jclass jcls, jstring inputPathJstr,
                                              jstring outputPathJstr) {

    const char *inputPath = env->GetStringUTFChars(inputPathJstr, NULL);
    const char *outputPath = env->GetStringUTFChars(outputPathJstr, NULL);

    LOGI("%s\n", inputPath);
    LOGI("%s\n", outputPath);


    AVFormatContext *avFormatContext = avformat_alloc_context();
    AVStream *avStream;
    AVCodecContext *avCodecContext1 = NULL;
    AVCodec *avCodec;
    AVPacket *avPacket;
    AVFrame *avFrame;
    AVFrame *yuvFrame;
    int result;

    av_register_all();
    avcodec_register_all();
    LOGI("%s", "av_register_all success");
    result = avformat_open_input(&avFormatContext, inputPath, NULL, NULL);
    if (result != 0) {
        LOGI("%s%d", "avformat_open_input error,", result);
        return;
    };
    LOGI("%s", "avformat_open_input success");

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGI("%s", "avformat_find_stream_info error");
    };
    LOGI("%s", "avformat_find_stream_info success");

    result = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL,
                                 0);

    if (result < 0) {
        LOGI("%s", "av_find_best_stream error");
    } else {
        LOGI("%s%d", "av_find_best_stream success,", result);


        avStream = avFormatContext->streams[result];

        if (avStream == NULL) {
            LOGI("%s", "avStream; null");
            return;
        }
        LOGI("%s", "avStream; not null");
        avCodecContext1 = avStream->codec;
        if (avCodecContext1 == NULL) {
            LOGI("%s", "avCodecContext1; null");
            return;

        }
        LOGI("%s", "avCodecContext1; not null");
        avCodec = avcodec_find_decoder(avCodecContext1->codec_id);

        if (avCodec == NULL) {
            LOGI("%s", "avCodec; null");
            return;

        }
        LOGI("%s", "avCodec; not null");

        avcodec_open2(avCodecContext1, avCodec, NULL);

        avFrame = av_frame_alloc();
        yuvFrame = av_frame_alloc();
        LOGI("%s", "av_frame_alloc success");

//        av_init_packet(avPacket);
        avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
        avPacket->data = NULL;
        avPacket->size = 0;
        LOGI("%s", "av_init_packet success");

//只有指定了AVFrame的像素格式、画面大小才能真正分配内存
        //缓冲区分配内存
        int num_bytes = avpicture_get_size(AV_PIX_FMT_YUV420P, avCodecContext1->width,
                                           avCodecContext1->height);

        uint8_t *out_buffer = (uint8_t *) av_malloc(num_bytes * sizeof(uint8_t));
        //初始化缓冲区
        avpicture_fill((AVPicture *) yuvFrame, out_buffer, AV_PIX_FMT_YUV420P,
                       avCodecContext1->width, avCodecContext1->height);

        LOGI("%s", "avpicture_fill success");


        FILE *fp_yuv = fopen(outputPath, "wb");
        if (fp_yuv == NULL) {
            LOGI("%s", "fopen(outputPath); error");

        }
        LOGI("%s", "fopen(outputPath); success");

        struct SwsContext *sws_ctx = sws_getContext(
                avCodecContext1->width, avCodecContext1->height, avCodecContext1->pix_fmt,
                avCodecContext1->width, avCodecContext1->height, AV_PIX_FMT_YUV420P,
                SWS_BILINEAR, NULL, NULL, NULL);

        int len, got_frame, framecount, size = 0;

        while (av_read_frame(avFormatContext, avPacket) >= 0) {
            len = avcodec_decode_video2(avCodecContext1, avFrame, &got_frame, avPacket);

            if (got_frame) {
                framecount++;

                int h = sws_scale(sws_ctx,
                                  (const uint8_t *const *) avFrame->data, avFrame->linesize, 0,
                                  avFrame->height,
                                  yuvFrame->data, yuvFrame->linesize);

                int y_size = avCodecContext1->width * avCodecContext1->height;

                int y = fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
                int u = fwrite(yuvFrame->data[1], 1, y_size / 4, fp_yuv);
                int v = fwrite(yuvFrame->data[2], 1, y_size / 4, fp_yuv);
                size = y + u + v;

            }

            av_free_packet(avPacket);
        }
        LOGI("解码%d帧,最后大小：%dM", framecount++, size / 8 / 1000 / 1000);
        fclose(fp_yuv);
    }


    av_free_packet(avPacket);
    av_frame_free(&avFrame);
    av_frame_free(&yuvFrame);
    avcodec_close(avCodecContext1);
    avformat_free_context(avFormatContext);

    env->ReleaseStringUTFChars(inputPathJstr, inputPath);
    env->ReleaseStringUTFChars(outputPathJstr, outputPath);
}

JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_playVedio(JNIEnv *env, jclass jcls, jstring inputPathJstr, jobject jsurface) {

    const char *inputPath = env->GetStringUTFChars(inputPathJstr, NULL);

    LOGI("%s\n", inputPath);

    AVFormatContext *avFormatContext = avformat_alloc_context();
    AVStream *avStream;
    AVCodecContext *avCodecContext1 = NULL;
    AVCodec *avCodec;
    AVPacket *avPacket;
    AVFrame *avFrame;
    AVFrame *yuvFrame;
    AVFrame *argbFrame;
    int result;

    av_register_all();
    avcodec_register_all();
    LOGI("%s", "av_register_all success");
    result = avformat_open_input(&avFormatContext, inputPath, NULL, NULL);
    if (result != 0) {
        LOGI("%s%d", "avformat_open_input error,", result);
        return;
    };
    LOGI("%s", "avformat_open_input success");

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGI("%s", "avformat_find_stream_info error");
    };
    LOGI("%s", "avformat_find_stream_info success");

    result = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL,
                                 0);

    if (result < 0) {
        LOGI("%s", "av_find_best_stream error");
    } else {
        LOGI("%s%d", "av_find_best_stream success,", result);


        avStream = avFormatContext->streams[result];

        if (avStream == NULL) {
            LOGI("%s", "avStream; null");
            return;
        }
        LOGI("%s", "avStream; not null");
        avCodecContext1 = avStream->codec;
        if (avCodecContext1 == NULL) {
            LOGI("%s", "avCodecContext1; null");
            return;

        }
        LOGI("%s", "avCodecContext1; not null");
        avCodec = avcodec_find_decoder(avCodecContext1->codec_id);

        if (avCodec == NULL) {
            LOGI("%s", "avCodec; null");
            return;

        }
        LOGI("%s", "avCodec; not null");

        avcodec_open2(avCodecContext1, avCodec, NULL);

        avFrame = av_frame_alloc();
        yuvFrame = av_frame_alloc();
        argbFrame = av_frame_alloc();
        LOGI("%s", "av_frame_alloc success");

//        av_init_packet(avPacket);
        avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
        avPacket->data = NULL;
        avPacket->size = 0;
        LOGI("%s", "av_init_packet success");

        //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
        //缓冲区分配内存
        int num_bytes = avpicture_get_size(AV_PIX_FMT_YUV420P, avCodecContext1->width,
                                           avCodecContext1->height);

        uint8_t *out_buffer = (uint8_t *) av_malloc(num_bytes * sizeof(uint8_t));
        //初始化缓冲区
        avpicture_fill((AVPicture *) yuvFrame, out_buffer, AV_PIX_FMT_YUV420P,
                       avCodecContext1->width, avCodecContext1->height);

        int len, got_frame, framecount, size = 0;

        ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, jsurface);

        ANativeWindow_Buffer aNativeWindow_buffer;

//        ANativeWindow_setBuffersGeometry(aNativeWindow,avCodecContext1->width,avCodecContext1->height,WINDOW_FORMAT_RGBA_8888);

//        avpicture_fill((AVPicture *)argbFrame, (const uint8_t *) aNativeWindow_buffer.bits, PIX_FMT_RGBA, avCodecContext1->width, avCodecContext1->height);

        LOGI("width:%d height:%d",avCodecContext1->width,avCodecContext1->height);

        while (av_read_frame(avFormatContext, avPacket) >= 0) {
            len = avcodec_decode_video2(avCodecContext1, avFrame, &got_frame, avPacket);

            if (got_frame) {
                framecount++;

                ANativeWindow_setBuffersGeometry(aNativeWindow,avCodecContext1->width,avCodecContext1->height,WINDOW_FORMAT_RGBA_8888);

                result = ANativeWindow_lock(aNativeWindow, &aNativeWindow_buffer, NULL);

                result = libyuv::I420ToARGB(avFrame->data[0], avFrame->linesize[0],
                                            avFrame->data[2], avFrame->linesize[2],
                                            avFrame->data[1], avFrame->linesize[1],
                                            (uint8 *) aNativeWindow_buffer.bits, aNativeWindow_buffer.stride*4,
                                            avCodecContext1->width, avCodecContext1->height);

                ANativeWindow_unlockAndPost(aNativeWindow);
                usleep(1000 * 16);
            }

            av_free_packet(avPacket);

        }


        ANativeWindow_release(aNativeWindow);
    }


    av_free_packet(avPacket);
    av_frame_free(&avFrame);
    av_frame_free(&yuvFrame);
    avcodec_close(avCodecContext1);
    avformat_free_context(avFormatContext);


    env->ReleaseStringUTFChars(inputPathJstr, inputPath);

}

}
