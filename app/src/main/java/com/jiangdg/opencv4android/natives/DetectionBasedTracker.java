package com.jiangdg.opencv4android.natives;

import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;

public class DetectionBasedTracker {
    private long mNativeObj = 0;

    // 构造方法：初始化人脸检测引擎
    public DetectionBasedTracker(String cascadeName, String eyeCascadeName,int minFaceSize) {
        mNativeObj = nativeCreateObject(cascadeName, eyeCascadeName,minFaceSize);
    }
    // 开始人脸检测
    public void start() {
        nativeStart(mNativeObj);
    }

    // 停止人脸检测
    public void stop() {
        nativeStop(mNativeObj);
    }

    // 设置人脸最小尺寸
    public void setMinFaceSize(int size) {
        nativeSetFaceSize(mNativeObj, size);
    }

    // 检测人脸
    public void detect(Mat imageGray, MatOfRect faces) {
        nativeDetect(mNativeObj, imageGray.getNativeObjAddr(), faces.getNativeObjAddr());
    }

    // 检测眨眼
    public void detectEyeBlink(MatOfRect faceImage) {
        nativeDetectEyeBlink(faceImage.getNativeObjAddr());
    }

    // 释放资源
    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    // native方法
    private static native long nativeCreateObject(String cascadeName,String eyeCascadeName, int minFaceSize);
    private static native void nativeDestroyObject(long thiz);
    private static native void nativeStart(long thiz);
    private static native void nativeStop(long thiz);
    private static native void nativeSetFaceSize(long thiz, int size);
    private static native void nativeDetect(long thiz, long inputImage, long faces);

    public static native void nativeDetectEyeBlink(long faceImage);
    public static native void nativeRgba(long address,int width,int height);
}
