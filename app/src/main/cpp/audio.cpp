//
// Created by hasika on 2017/12/11.
//



extern "C" {

#include "log.h"
#include <jni.h>
#include <libavformat/avformat.h>
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
#include "libyuv/convert_argb.h"
#include "libswresample/swresample.h"
#include "unistd.h"


#define MAX_AUDIO_FRME_SIZE 44100 * 4

JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_decodePCM(JNIEnv *env, jclass jcls, jstring inputPathJstr,
                                                 jstring outputPathJstr) {
    JavaVM *javaVM;
    env->GetJavaVM(&javaVM);
    JNIEnv *jniEnv;
    javaVM->AttachCurrentThread(&jniEnv, NULL);
    javaVM->DetachCurrentThread();

    const char *inputPath = env->GetStringUTFChars(inputPathJstr, NULL);
    const char *outputPath = env->GetStringUTFChars(outputPathJstr, NULL);

    LOGI("%s\n", inputPath);
    LOGI("%s\n", outputPath);

    int ret;


    AVFormatContext *avFormatContext = avformat_alloc_context();

    av_register_all();

    ret = avformat_open_input(&avFormatContext, inputPath, NULL, NULL);

    LOGI("avformat_open_input","");

    ret = avformat_find_stream_info(avFormatContext, NULL);

    LOGI("avformat_find_stream_info,nb_streams:%d",avFormatContext->nb_streams);

    AVCodec *avCodec;
    AVCodecContext *avCodecContext;
    int stream_idx = 0;
    for (; stream_idx < avFormatContext->nb_streams; stream_idx++) {
        if(avFormatContext->streams[stream_idx]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            avCodecContext = avFormatContext->streams[stream_idx]->codec;
            avCodec = avcodec_find_decoder(avCodecContext->codec_id);
            break;
        }
    }

    ret = avcodec_open2(avCodecContext, avCodec, NULL);

    LOGI("avcodec_open2, %d",avCodecContext->sample_fmt);


    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *avFrame = av_frame_alloc();
//    AVFrame *outFrame = av_frame_alloc();

    //frame->16bit 44100 PCM 统一音频采样格式与采样率
    SwrContext *swrContext = swr_alloc();

    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;

    //输入采样率
    int in_sample_rate = avCodecContext->sample_rate;
    //输出采样率
    int out_sample_rate = 44100;

    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    //av_get_default_channel_layout(codecCtx->channels);
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //输出的声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);

    ret = swr_init(swrContext);

    LOGI("swr_init","");

    //16bit 44100 PCM 数据
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);
    FILE *fp_pcm = fopen(outputPath, "wb");

    int nb_channels = av_get_channel_layout_nb_channels(out_ch_layout);
    int nb_samples = avFrame->nb_samples;

    int got_frame = 0, index = 0 , size;

    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if(avPacket->stream_index != stream_idx)
            return;

        ret = avcodec_decode_audio4(avCodecContext, avFrame, &got_frame, avPacket);
        if (got_frame>0) {
            LOGI("avcodec_decode_audio4,ret:%d,gotFrame:%d",ret,got_frame);

            swr_convert(swrContext, &out_buffer, MAX_AUDIO_FRME_SIZE,
                        (const uint8_t **) avFrame->data, avFrame->nb_samples);

            size = av_samples_get_buffer_size(NULL, nb_channels, nb_samples, out_sample_fmt, 0);
            fwrite(out_buffer, 1, size, fp_pcm);
        }
        av_free_packet(avPacket);

    }

    fclose(fp_pcm);
    av_frame_free(&avFrame);
    av_free(out_buffer);


    avformat_close_input(&avFormatContext);
    avformat_free_context(avFormatContext);


    env->ReleaseStringUTFChars(inputPathJstr, inputPath);
    env->ReleaseStringUTFChars(outputPathJstr, outputPath);
}

JNIEXPORT void JNICALL
Java_cn_migu_hasika_ffmpeg_PlayControl_playAudio(JNIEnv *env, jclass jcls, jstring inputPathJstr) {
    const char *inputPath = env->GetStringUTFChars(inputPathJstr, NULL);

    LOGI("%s\n", inputPath);

    int ret;

    AVFormatContext *avFormatContext = avformat_alloc_context();

    av_register_all();

    ret = avformat_open_input(&avFormatContext, inputPath, NULL, NULL);

    LOGI("avformat_open_input","");

    ret = avformat_find_stream_info(avFormatContext, NULL);

    LOGI("avformat_find_stream_info,nb_streams:%d",avFormatContext->nb_streams);

    AVCodec *avCodec;
    AVCodecContext *avCodecContext;
    int stream_idx = 0;
    for (; stream_idx < avFormatContext->nb_streams; stream_idx++) {
        if(avFormatContext->streams[stream_idx]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            avCodecContext = avFormatContext->streams[stream_idx]->codec;
            avCodec = avcodec_find_decoder(avCodecContext->codec_id);
            break;
        }
    }

    ret = avcodec_open2(avCodecContext, avCodec, NULL);

    LOGI("avcodec_open2, %d",avCodecContext->sample_fmt);


    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *avFrame = av_frame_alloc();
//    AVFrame *outFrame = av_frame_alloc();

    //frame->16bit 44100 PCM 统一音频采样格式与采样率
    SwrContext *swrContext = swr_alloc();

    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;

    //输入采样率
    int in_sample_rate = avCodecContext->sample_rate;
    //输出采样率
    int out_sample_rate = 44100;

    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    //av_get_default_channel_layout(codecCtx->channels);
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //输出的声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);

    ret = swr_init(swrContext);

    LOGI("swr_init","");

    //获取write方法
    jmethodID writeMid = env->GetStaticMethodID(jcls,"writePCM","([BII)V");

    //获取play方法
    jmethodID playMid = env->GetStaticMethodID(jcls, "playPCM", "()V");


    //16bit 44100 PCM 数据
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);

    int nb_channels = av_get_channel_layout_nb_channels(out_ch_layout);
    int nb_samples;

    LOGI("nb_channels:%d,nb_samples:%d,out_sample_fmt:%d",nb_channels,nb_samples,out_sample_fmt);

    int got_frame = 0, index = 0 , size;

    while (av_read_frame(avFormatContext, avPacket) >= 0) {

        ret = avcodec_decode_audio4(avCodecContext, avFrame, &got_frame, avPacket);
        if (got_frame>0) {


            swr_convert(swrContext, &out_buffer, MAX_AUDIO_FRME_SIZE,
                        (const uint8_t **) avFrame->data, avFrame->nb_samples);

            size = av_samples_get_buffer_size(NULL, nb_channels, avFrame->nb_samples, out_sample_fmt, 0);

            LOGI("avcodec_decode_audio4,ret:%d,gotFrame:%d,size:%d",ret,got_frame,size);

            //需要jbyteArray回传给java层
            jbyteArray bufferJbyteArray = env->NewByteArray(size);

            jbyte * bufferByteP = env->GetByteArrayElements(bufferJbyteArray, NULL);

            memcpy(bufferByteP, out_buffer, size);


            env->ReleaseByteArrayElements(bufferJbyteArray, bufferByteP, 0);

            //调用audioTrack.write方法
            env->CallStaticVoidMethod(jcls,writeMid,bufferJbyteArray,0, size);
            //调用audioTrack.play方法
            env->CallStaticVoidMethod(jcls, playMid);

            env->DeleteLocalRef(bufferJbyteArray);
            usleep(1000 * 16);


        }
        av_free_packet(avPacket);

    }

    av_frame_free(&avFrame);
    av_free(out_buffer);


    avformat_close_input(&avFormatContext);
    avformat_free_context(avFormatContext);


    env->ReleaseStringUTFChars(inputPathJstr, inputPath);


}
}

