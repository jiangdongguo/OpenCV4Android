#include "DetectionBasedTracker_jni.h"
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>

#include <android/log.h>
#include <opencv/cv.hpp>

#define LOG_TAG "FaceDetection/DetectionBasedTracker"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat)
{
    mat = Mat(v_rect, true);
}

/*
 * 级联分类器适配类，继承于IDetector
 */
class CascadeDetectorAdapter: public DetectionBasedTracker::IDetector
{
public:
    // 构造函数
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector):
            IDetector(),
            Detector(detector)
    {
        LOGD("CascadeDetectorAdapter::Detect::Detect");
        CV_Assert(detector);
    }
    // 多尺度检测人脸
    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects)
    {
        LOGD("CascadeDetectorAdapter::Detect: begin");
        LOGD("CascadeDetectorAdapter::Detect: scaleFactor=%.2f, minNeighbours=%d, minObjSize=(%dx%d), maxObjSize=(%dx%d)", scaleFactor, minNeighbours, minObjSize.width, minObjSize.height, maxObjSize.width, maxObjSize.height);
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
        LOGD("CascadeDetectorAdapter::Detect: end");
    }
    // 析构函数
    virtual ~CascadeDetectorAdapter()
    {
        LOGD("CascadeDetectorAdapter::Detect::~Detect");
    }

private:
    CascadeDetectorAdapter();
    cv::Ptr<cv::CascadeClassifier> Detector;
};

/*
 * 参数结构体
 */
struct DetectorAgregator
{
    cv::Ptr<CascadeDetectorAdapter> mainDetector;
    cv::Ptr<CascadeDetectorAdapter> trackingDetector;

    cv::Ptr<DetectionBasedTracker> tracker;
    DetectorAgregator(cv::Ptr<CascadeDetectorAdapter>& _mainDetector, cv::Ptr<CascadeDetectorAdapter>& _trackingDetector):
            mainDetector(_mainDetector),
            trackingDetector(_trackingDetector)
    {
        CV_Assert(_mainDetector);
        CV_Assert(_trackingDetector);

        DetectionBasedTracker::Parameters DetectorParams;
        tracker = makePtr<DetectionBasedTracker>(mainDetector, trackingDetector, DetectorParams);
    }
};

/*
 * 加载人脸检测分类器
 */
JNIEXPORT jlong JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeCreateObject
(JNIEnv * jenv, jclass, jstring jFileName, jstring eyeFile,jint faceSize)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject enter");
    const char* jnamestr = jenv->GetStringUTFChars(jFileName, NULL);
    string stdFileName(jnamestr);
    jlong result = 0;

    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject");

    try
    {
        cv::Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(stdFileName));
        cv::Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(stdFileName));
        result = (jlong)new DetectorAgregator(mainDetector, trackingDetector);
        if (faceSize > 0)
        {
            mainDetector->setMinObjectSize(Size(faceSize, faceSize));
            //trackingDetector->setMinObjectSize(Size(faceSize, faceSize));
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
        catch (...)
        {
        LOGD("nativeCreateObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeCreateObject()");
        return 0;
    }

    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject exit");
    return result;
}

JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeDestroyObject
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject");

    try
    {
        if(thiz != 0)
        {
            ((DetectorAgregator*)thiz)->tracker->stop();
            delete (DetectorAgregator*)thiz;
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDestroyObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeDestroyObject()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject exit");
}

JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeStart
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStart");

    try
    {
        ((DetectorAgregator*)thiz)->tracker->run();
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStart caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeStart caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeStart()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStart exit");
}

JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeStop
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStop");

    try
    {
        ((DetectorAgregator*)thiz)->tracker->stop();
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStop caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeStop caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeStop()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStop exit");
}

JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeSetFaceSize
(JNIEnv * jenv, jclass, jlong thiz, jint faceSize)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeSetFaceSize -- BEGIN");

    try
    {
        if (faceSize > 0)
        {
            ((DetectorAgregator*)thiz)->mainDetector->setMinObjectSize(Size(faceSize, faceSize));
            //((DetectorAgregator*)thiz)->trackingDetector->setMinObjectSize(Size(faceSize, faceSize));
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStop caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeSetFaceSize caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeSetFaceSize()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeSetFaceSize -- END");
}


JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeDetect
(JNIEnv * jenv, jclass, jlong thiz, jlong imageGray, jlong faces)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDetect");

    try
    {
        vector<Rect> RectFaces;
        ((DetectorAgregator*)thiz)->tracker->process(*((Mat*)imageGray));
        ((DetectorAgregator*)thiz)->tracker->getObjects(RectFaces);
        *((Mat*)faces) = Mat(RectFaces, true);
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDetect caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code DetectionBasedTracker.nativeDetect()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDetect END");
}

/*
 * 眨眼检测
 */
const jint calibrationDefault = 100;
jint calibrationFace = calibrationDefault;
Mat previousFace;
Mat currentFace;
jint blinkNumberLeft = 0;
jint blinkNumberRight = 0;
jboolean leftEyeOpen = true;
jboolean rightEyeOpen = true;

JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeDetectEyeBlink
(JNIEnv *env, jclass jcls){
//    Mat matCapturedGrayImage;
//    Rect face;
//    Rect detectedFaceRegion;
//    if(face.width == 0 && face.height) {
//        LOGD("no face found.");
//        return;
//    }
//    // 检测到人脸，缓存第一帧不作处理
//    if(detectedFaceRegion.height ==0 || calibrationFace<1) {
//        detectedFaceRegion = face;
//        previousFace = matCapturedGrayImage(face);
//        calibrationFace = calibrationDefault;
//    } else {
//        currentFace = matCapturedGrayImage(detectedFaceRegion);
//        Mat flow, cflow;
//        // 使用Gunnar Farneback算法计算密集光流
//        calcOpticalFlowFarneback(previousFace, currentFace, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
//        // 将灰度图像转换为BGR
//        cvtColor(previousFace, cflow, CV_GRAY2BGR);
//        // 计算光流，获取脸部移动偏移距离
//        int globalMovementX, globalMovementY;
//        calcFlow(flow, cflow, 1, globalMovementX, globalMovementY);
//        // 移动绘制矩形位置，重新计算光流
//        detectedFaceRegion.x = detectedFaceRegion.x + globalMovementX;
//        detectedFaceRegion.y = detectedFaceRegion.y + globalMovementY;
//        if (detectedFaceRegion.x < 0) {
//        	detectedFaceRegion.x = 0;
//        }
//        if (detectedFaceRegion.y < 0) {
//        	detectedFaceRegion.y = 0;
//        }
//        if (detectedFaceRegion.x + detectedFaceRegion.width > matCapturedImage.size().width - 1) {
//        	detectedFaceRegion.x = matCapturedImage.size().width - detectedFaceRegion.width - 1;
//        }
//        if (detectedFaceRegion.y + detectedFaceRegion.height > matCapturedImage.size().height - 1) {
//        	detectedFaceRegion.y = matCapturedImage.size().height - detectedFaceRegion.height - 1;
//        }
//        currentFace = matCapturedGrayImage(detectedFaceRegion);
//        eyeTracking(currentFace, previousFace);
//        swap(previousFace, currentFace);
//    }
}

/*
 * nativeRgba方法，处理预览帧
 */
JNIEXPORT void JNICALL Java_com_jiangdg_opencv4android_natives_DetectionBasedTracker_nativeRgba
(JNIEnv * jenv, jclass jcls,jlong address,jint width,jint height)
{
    Mat &img = *(Mat*)address;
    Mat imgT(height,width,CV_8UC4);
    Mat imgF(height,width,CV_8UC4);

    // 图片倒置
    transpose(img,imgT);
    // 重定义图片大小
    resize(imgT,imgF,Size(width,height));
    // 垂直翻转图片
    flip(imgF,img,1);
}

///*
// * 测量脸部移动
// */
//void calcFlow(const Mat& flow, Mat& cflowmap, int step, int &globalMovementX, int &globalMovementY)
//{
//	int localMovementX = 0;
//	int localMovementY = 0;
//
//	for (int y = 0; y < cflowmap.rows; y += step)
//	{
//		for (int x = 0; x < cflowmap.cols; x += step)
//		{
//			const Point2f& fxy = flow.at<Point2f>(y, x);
//
//			localMovementX = localMovementX + fxy.x;
//			localMovementY = localMovementY + fxy.y;
//		}
//	}
//
//	globalMovementX = (localMovementX / (cflowmap.cols * cflowmap.rows))*2;							//these are usable values for global movement of face (for example + 2 pixels to axis x)
//	globalMovementY = (localMovementY / (cflowmap.rows * cflowmap.cols))*2;
//}
//
///*
// * 眼睛跟踪
// */
//void eyeTracking(Mat &matCurrentFace, Mat &matPreviousFace) {
//
//	Mat matLeftEyePrevious;
//	Mat matRightEyePrevious;
//	Mat matLeftEyeCurrent;
//	Mat matRightEyeCurrent;
//	// 获取左、右眼位置
//	getEyesFromFace(matPreviousFace, matLeftEyePrevious, matRightEyePrevious);
//	getEyesFromFace(matCurrentFace, matLeftEyeCurrent, matRightEyeCurrent);
//	// 眨眼判断
//	detectBlink(matLeftEyePrevious, matLeftEyeCurrent, "left", leftEyeOpen, blinkNumberLeft, leftEyeCloseTime);				// each eye have its own blinking detection, timer and blink counter
//	detectBlink(matRightEyePrevious, matRightEyeCurrent, "right", rightEyeOpen, blinkNumberRight, rightEyeCloseTime);
//}
//
//void getEyesFromFace(Mat &matFace, Mat &matLeftEye, Mat &matRightEye) {
//	Size faceSize = matFace.size();
//	// 大概计算双眼位于脸部的位置
//	int eye_region_width = faceSize.width * (kEyePercentWidth / 100.0);
//	int eye_region_height = faceSize.width * (kEyePercentHeight / 100.0);
//	int eye_region_top = faceSize.height * (kEyePercentTop / 100.0);
//	Rect leftEyeRegion(faceSize.width*(kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);
//	Rect rightEyeRegion(faceSize.width - eye_region_width - faceSize.width*(kEyePercentSide / 100.0), eye_region_top, eye_region_width, eye_region_height);
//
//	matLeftEye = matFace(leftEyeRegion);
//	matRightEye = matFace(rightEyeRegion);
//}
//
//void detectBlink(Mat &matEyePrevious, Mat &matEyeCurrent, String eye, bool &eyeOpen, int &blinkNumber, clock_t &closeTime) {
//	Mat leftFlow, leftCflow;
//	calcOpticalFlowFarneback(matEyePrevious, matEyeCurrent, leftFlow, 0.5, 3, 15, 3, 5, 1.2, 0);
//	cvtColor(matEyePrevious, leftCflow, CV_GRAY2BGR);
//	int movementX, movementY;
//
//	calcFlowEyes(leftFlow, leftCflow, 1, movementX, movementY);
//
//	if (movementY == 0) {
//		return;
//	}
//
//	if (movementY > 0 && eyeOpen) {						//eye is now closed
//
//		closeTime = clock();
//
//		eyeOpen = false;
//		blinkNumber = blinkNumber + 1;					//increment blink count number for current eye
//	}
//	else if (movementY < 0 && !eyeOpen){				//eye is now open
//		eyeOpen = true;
//	}
//}
