LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
# 编译打包OpenCV动态库
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
# 指定OpenCV SDK中OpenCV.mk文件所在路径
include E:\Environment\opencv-3.3.0-android-sdk\OpenCV-android-sdk\sdk\native\jni\OpenCV.mk
# 指定jni目录中src文件
LOCAL_SRC_FILES  := DetectionBasedTracker_jni.cpp
# 指定本地链接库
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl
# 本地so名称
LOCAL_MODULE     := detection_based_tracker

include $(BUILD_SHARED_LIBRARY)
