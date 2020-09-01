#####################################################
# JAVA 静态库
#####################################################
LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# Build all java files in the java subdirectory
LOCAL_SRC_FILES:=$(call all-subdir-java-files)

# any libraries that this library depends on
LOCAL_JAVA_LIBRRIES:=android.test.runner

# the name of the jar files to creates
LOCAL_MODULE:=sample

# build a static jar file
include $(BUILD_STATIC_JAVA_LIBRARY)


#####################################################
# 构建静态库，然后将静态库包含在动态库中
#	第三方AVI库	 （静态库）
#	原生模块 	 （动态库，依赖第三方静态库AVI）
#####################################################
LOCAL_PATH := $(call my-dir)

#
# 第三方AVI库
#
include $(CLEAR_VARS)

LOCAL_MODULE :=avilib
LOCAL_SRC_FILES := avilib.c platform_posix.c

include $(BUILD_STATIC_LIBRARY)

#
# 原生模块
#
include $(CLEAR_VARS)

LOCAL_MODULE := module
LOCAL_SRC_FILES := module.c

LOCAL_STATIC_LIBRARIES := avilib

include $(BUILD_SHARED_LIBRARY)