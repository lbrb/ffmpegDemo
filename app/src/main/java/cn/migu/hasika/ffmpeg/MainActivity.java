package cn.migu.hasika.ffmpeg;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import java.io.File;

public class MainActivity extends Activity {

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            String[] perms = { "android.permission.RECORD_AUDIO", "android.permission.WRITE_EXTERNAL_STORAGE" };
            if (checkSelfPermission(perms[0]) == PackageManager.PERMISSION_DENIED ||
                    checkSelfPermission(perms[1]) == PackageManager.PERMISSION_DENIED)
            {
                requestPermissions(perms, 200);
            }
        }


        surfaceView = findViewById(R.id.playView);

        //初始化jni层参数
        PlayControl.init();
    }

    public void click(View btn) {
        int ids = btn.getId();

        String downloadPath = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+"Download";
        String inputPath = downloadPath+File.separator+"a.mp4";
        String outputYuvPath = downloadPath+File.separator+"a.yuv";
        String outputPcmPath = downloadPath+File.separator+"a.pcm";


//        String inputPath = "file:///android_asset/a.mp4";

        switch (ids){
            case R.id.decodeYuv:
                Log.d("AAA", "click: decodeYuv");
                PlayControl.decodeYUV(inputPath,outputYuvPath);
                break;
            case R.id.playVedio:
                Log.d("AAA", "click: playVedio");
                PlayControl.playVedio(inputPath, surfaceView.getHolder().getSurface());
                break;
            case R.id.decodePCM:
                Log.d("AAA", "click: decodePCM");
                PlayControl.decodePCM(inputPath, outputPcmPath);
                break;
            case R.id.playAudio:
                Log.d("AAA", "click: playAudio");
                PlayControl.playAudio(inputPath);
                break;

        }
    }


    @Override
    protected void onDestroy() {
        PlayControl.destroy();
        super.onDestroy();
    }
}
