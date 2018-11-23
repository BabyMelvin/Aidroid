LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_PROPRIETARY_MOUDLE :=true
LOCAL_MODULE := melvin_test
LOCAL_SRC_FILES := \
	client.cpp
LOCAL_SHARED_LIBARARIES :=\
	liblog \
	libhidlbase \
	libutils \
	android.hardware.melvin@1.0
include $(BUILD_EXECUTABLE)
