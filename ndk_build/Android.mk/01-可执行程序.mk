LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
          test.c

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= sil_test
LOCAL_MODULE_STEM_32 := $(LOCAL_MODULE)32
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE)64
LOCAL_MULTILIB := both

# 将会生成32位和64位的可执行程序
include $(BUILD_EXECUTABLE)
