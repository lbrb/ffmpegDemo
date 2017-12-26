package cn.migu.hasika.ffmpeg;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.view.Surface;
import android.view.SurfaceView;

/**
 * Created by hasika on 2017/12/9.
 */

public class PlayControl {

    private static AudioTrack audioTrack;

    public static void createAudioTrack(){
        int streamType = AudioManager.STREAM_MUSIC;
        int sampleRateInHz = 44100;
        int channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz,channelConfig,audioFormat);
        audioTrack = new AudioTrack(streamType,sampleRateInHz,channelConfig,audioFormat,bufferSizeInBytes,AudioTrack.MODE_STREAM);
    }

    public static void playPCM(){
        if (audioTrack == null){
            createAudioTrack();
        }
        audioTrack.play();
    }

    public static void writePCM(byte[] audioData, int offsetInBytes, int sizeInBytes){
        if (audioTrack == null){
            createAudioTrack();
        }
        audioTrack.write(audioData,offsetInBytes, sizeInBytes);
    }


    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("postproc-53");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("yuvutil");
        System.loadLibrary("yuv");
        System.loadLibrary("play-lib");

    }

    public native static void decodeYUV(String inputPath, String outputPath);

    public native static void decodePCM(String inputPath, String outputPath);

    public native static void playVedio(String inputPath, Surface surface);

    public native static void playAudio(String inputPath);

    //播放器
    public native static void init();
    public native static void destroy();
    public native static void play(String inputPath, Surface surface);
}
