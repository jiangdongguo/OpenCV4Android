package com.jiangdg.opencv4android.functions;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.WindowManager;

import com.jiangdg.opencv4android.R;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

/**
 * 第一个OpenCV例子
 *
 * Created by jiangdongguo on 2017/12/29.
 */

public class HelloOpenCVActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener{
    private CameraBridgeViewBase mOpenCvCameraView;
    private LoaderCallbackInterface mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            super.onManagerConnected(status);
        }
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 全屏显示
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_hello);
        // 初始化OpenCV引擎
        mOpenCvCameraView = (CameraBridgeViewBase)findViewById(R.id.HelloOpenCvView);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }



    @Override
    protected void onResume() {
        super.onResume();
        // 初始化引擎
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_3_0,this,mLoaderCallback);
    }

    @Override
    public void onCameraViewStarted(int width, int height) {

    }

    @Override
    public void onCameraViewStopped() {

    }

    @Override
    public Mat onCameraFrame(Mat inputFrame) {
        return inputFrame;
    }

    @Override
    protected void onPause() {
        super.onPause();
        // 释放Camera资源
        if(mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 释放Camera资源
        if(mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }
}
