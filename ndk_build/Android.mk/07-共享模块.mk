###############################################################
# 共享库 共享通用模块
###############################################################
LOCAL_PATH := $(call my-dir)

#
# 第三方AVI库
#
include $(CLEAR_VARS)

LOCAL_MODULE :=avilib
LOCAL_SRC_FILES := avilib.c platform_posix.c

include $(BUILD_SHARED_LIBRARY)

#
# 原生模块1
#
include $(CLEAR_VARS)

LOCAL_MODULE := module1
LOCAL_SRC_FILES := module1.c

LOCAL_STATIC_LIBRARIES := avilib

include $(BUILD_SHARED_LIBRARY)

#
# 原生模块2
#
include $(CLEAR_VARS)

LOCAL_MODULE := module2
LOCAL_SRC_FILES := module2.c

LOCAL_STATIC_LIBRARIES := avilib

include $(BUILD_SHARED_LIBRARY)

###############################################################
# 多个NDK项目间共享模块，从transcode/avilib 目录下导入库
###############################################################
#
# 原生模块
#
include $(CLEAR_VARS)

LOCAL_MODULE := module
LOCAL_SRC_FILES := module.c

LOCAL_STATIC_LIBRARIES := avilib

include $(BUILD_SHARED_LIBRARY)
$(call import-module, transcode/avilib)

###############################################################
# prebuild库，如下情形
#	不发布源码的情形下
#	使用共享库加速构建
###############################################################
#
# 原生模块
#
include $(CLEAR_VARS)

LOCAL_MODULE := avilib

LOCAL_STATIC_LIBRARIES := libavilib.so

include $(PREBUILT_SHARED_LIBRARY)