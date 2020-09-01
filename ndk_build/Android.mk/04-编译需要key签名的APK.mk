# platform key 签名
LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# Build all java files in the java subdirectory
LOCAL_SRC_FILES:=$(call all-subdir-java-files)

# Name of the APK to build
LOCAL_PACKAGE_NAME:=localPackage

LOCAL_CERTIFICATE:=platform

# Tell it to build an APK
include $(BUILD_PACKAGE)

# vendor key 签名
LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# Build all java files in the java subdirectory
LOCAL_SRC_FILES:=$(call all-subdir-java-files)

# Name of the apk to build
LOCAL_PACKAGE_NAME:=localPackage
LOCAL_CERTIFICATE:=vendor/example/certs/app

# Tell it to build an APK
include $(BUILD_PACKAGE)
