LOCAL_PATH:=$(call my-dir)

###############SERVER###################
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES:=\
	libcutils \
	libutils \
	libbinder

LOCAL_MODULE:=ServiceDemo
LOCAL_SRC_FILES:= \
	IMyService.cpp \
	ServiceDemo.cpp
LOCAL_MODULE_TAGS:=optional
include $(BUILD_EXECUTABLE)

################CLIENT###################
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES:= \
	libcutils \
	libutils \
	libbinder
LOCAL_MODULE:=ServiceDemo
LOCAL_SRC_FILES:= \
	IMyService.cpp \
	ClientDemo.cpp
LOCAL_MODULE_TAGS:=optional
include $(BUILD_EXECUTABLE)
########################################
