#####################
#装载一个普通的apk
#####################
LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# module name should match apk name to be installed
LOCAL_MODULE:=localMoudleName

LOCAL_SRC_FILES:=$(LOCAL_MODULE).apk
LOCAL_MODULE_CLASS:=APPS

LOCAL_MOUDLE_SUFFIX:=$(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_CRETIFICATE:=platform
include $(BUILD_PREBUILT)


#############################################
# 装载需要.so 第三方apk
#############################################

LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:=baiduinput_android_v1.1_1000e
LOCAL_SRC_FILES:=$(LOCAL_MODULE).apk
LOCAL_MODULE_CLASS:=APPS
LOCAL_MODULE_SUFFIX:=$(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_CRETIFICATE:=platform
include $(BUILD_PREBUILT)

######copy the library to /system/lib #####
include $(CLEAR_VARS)

LOCAL_MODULE:=libinputcore.so
LOCAL_MODULE_CLASS:=SHARED_LIBRARIES
LOCAL_MODULE_PATH:=$(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SRC_FILES:=lib/$(LOCAL_MODULE)
OVERRIED_BUILD_MOUDLE_PATH:=$(TARGET_OUT_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)



