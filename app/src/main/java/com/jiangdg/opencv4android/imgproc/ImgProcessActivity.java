package com.jiangdg.opencv4android.imgproc;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;

import com.jiangdg.opencv4android.R;
import com.jiangdg.opencv4android.natives.DetectionBasedTracker;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * 第一个OpenCV例子
 * Created by jiangdongguo on 2017/12/29.
 */

public class ImgProcessActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2{
    private static final String TAG = "ImgProcessActivity";
    @BindView(R.id.HelloOpenCvView)
    CameraBridgeViewBase mOpenCvCameraView;
    private Mat mRgba;
    private int mWidth;
    private int mHeight;

    static {
        System.loadLibrary("opencv341");
    }

    private LoaderCallbackInterface mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                // OpenCV引擎加载成功，渲染Camera数据
                case LoaderCallbackInterface.SUCCESS:
                    Log.i(TAG, "OpenCV loaded successfully.");
                    mOpenCvCameraView.enableView();
                    break;
                default:
                    super.onManagerConnected(status);
                    break;
            }
        }
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 全屏显示
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_hello);
        // 绑定View
        ButterKnife.bind(this);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }


    @Override
    protected void onResume() {
        super.onResume();
        // 静态加载
        if(!OpenCVLoader.initDebug()) {
            Log.w(TAG,"static loading library fail,Using Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_3_0, this, mLoaderCallback);
        } else {
            Log.w(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // 释放Camera资源
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 释放Camera资源
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mWidth = width;
        mHeight = height;
        // 创建一个Mat数据对象
        mRgba = new Mat(width,height, CvType.CV_8UC4);

    }

    @Override
    public void onCameraViewStopped() {
        mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();
        // 获取当前一帧图像的内存地址
        long address = mRgba.getNativeObjAddr();
        // 对图像进行处理
        DetectionBasedTracker.nativeRgba(address,mWidth,mHeight);
        // 处理后，屏幕回显
        return mRgba;
    }
}
