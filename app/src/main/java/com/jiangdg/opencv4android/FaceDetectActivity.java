package com.jiangdg.opencv4android;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.WindowManager;
import android.widget.TextView;

import com.jiangdg.opencv4android.natives.DetectionBasedTracker;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.InstallCallbackInterface;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * 人脸检测
 * <p>
 * Created by jiangdongguo on 2018/1/4.
 */

public class FaceDetectActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2 {
    private static final int JAVA_DETECTOR = 0;
    private static final int NATIVE_DETECTOR = 1;
    private static final String TAG = "FaceDetectActivity";
    @BindView(R.id.cameraView_face)
    CameraBridgeViewBase mCameraView;

    private Mat mGray;
    private Mat mRgba;
    private int mDetectorType = NATIVE_DETECTOR;
    private int mAbsoluteFaceSize = 0;
    private float mRelativeFaceSize = 0.2f;
    private DetectionBasedTracker mNativeDetector;
    private CascadeClassifier mJavaDetector;
    private static final Scalar FACE_RECT_COLOR = new Scalar(0, 255, 0, 255);

    private File mCascadeFile;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                    // OpenCV初始化加载成功，再加载本地so库
                    System.loadLibrary("detection_based_tracker");

                    try {
                        // 加载人脸检测模式文件
                        InputStream is = getResources().openRawResource(R.raw.lbpcascade_frontalface);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "lbpcascade_frontalface.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);
                        byte[] buffer = new byte[4096];
                        int byteesRead;
                        while ((byteesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, byteesRead);
                        }
                        is.close();
                        os.close();
                        // 使用模型文件初始化人脸检测引擎
                        mJavaDetector = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (mJavaDetector.empty()) {
                            Log.e(TAG, "加载cascade classifier失败");
                            mJavaDetector = null;
                        } else {
                            Log.d(TAG, "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        }
                        mNativeDetector = new DetectionBasedTracker(mCascadeFile.getAbsolutePath(), 0);
                        cascadeDir.delete();
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    // 开启渲染Camera
                    mCameraView.enableView();
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
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_facedetect);
        // 绑定View
        ButterKnife.bind(this);
        mCameraView.setVisibility(CameraBridgeViewBase.VISIBLE);
        // 注册Camera渲染事件监听器
        mCameraView.setCvCameraViewListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        // 静态初始化OpenCV
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "无法加载OpenCV本地库，将使用OpenCV Manager初始化");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_3_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "成功加载OpenCV本地库");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // 停止渲染Camera
        if (mCameraView != null) {
            mCameraView.disableView();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 停止渲染Camera
        if (mCameraView != null) {
            mCameraView.disableView();
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        // 灰度图像
        mGray = new Mat();
        // R、G、B彩色图像
        mRgba = new Mat();
    }

    @Override
    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();
        // 设置脸部大小
        if (mAbsoluteFaceSize == 0) {
            int height = mGray.rows();
            if (Math.round(height * mRelativeFaceSize) > 0) {
                mAbsoluteFaceSize = Math.round(height * mRelativeFaceSize);
            }
            mNativeDetector.setMinFaceSize(mAbsoluteFaceSize);
        }
        // 获取检测到的脸部数据
        MatOfRect faces = new MatOfRect();
        if (mDetectorType == JAVA_DETECTOR) {
            if (mJavaDetector != null) {
                mJavaDetector.detectMultiScale(mGray, faces, 1.1, 2, 2,
                        new Size(mAbsoluteFaceSize, mAbsoluteFaceSize), new Size());
            }
        } else if (mDetectorType == NATIVE_DETECTOR) {
            if (mNativeDetector != null) {
                mNativeDetector.detect(mGray, faces);
            }
        } else {
            Log.e(TAG, "Detection method is not selected!");
        }
        // 绘制检测框
        Rect[] facesArray = faces.toArray();
        for (int i = 0; i < facesArray.length; i++) {
            Imgproc.rectangle(mRgba, facesArray[i].tl(), facesArray[i].br(), FACE_RECT_COLOR, 3);
        }

        Log.i(TAG, "共检测到 " + faces.toArray().length + " 张脸");
        return mRgba;
    }
}
