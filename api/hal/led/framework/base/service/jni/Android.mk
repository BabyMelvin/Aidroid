LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)

#[optional ,usr,eng]
# eng=required
# optional=no install on tartget
LOCAL_MODULE_TAGS:=eng
LOCAL_MOUDLE:=libmelvin_runtime
LOCAL_MOUDLE_PATH:=$(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SRC_FILES:= \
	com_melvin_server_LedService.cpp
LOCAL_SHARED_LIBRARIES:= \
	libandroid_runtime \
	libnativehelper \
	libcutils \
	libutils \
	libhardware

LOCAL_STATIC_LIBRARIES:=

LOCAL_C_INCLUDE += \
				   $(JNI_H_INCLUDE) \
				  hardware/modules/include

LOCAL_CFALGS +=

# 更有效，添加库到prelink映射
LOCAL_PRELINK_MODULE :=false
include $(BUILD_SHARED_LIBRARY)


