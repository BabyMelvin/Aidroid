############################
#Android optee-hello-world makefile
############################
LOCAL_PATH:=$(call my-dir)

CFG_TEEC_PUBLIC_INCLUDE = $(LOCAL_PATH)/../optee_client/public

###############################
# build hello world
# ############################

include $(CLEAR_VARS)
LOCAL_CFLAGS+=-DANDROID_BUILD
LOCAL_CFLAGS+=-Wall

LOCAL_SRC_FILES += host/main.c

LOCAL_C_INCLUDE :=$(LOCAL_PATH)/ta/include \
	$(CFG_TEEC_PUBLIC_INCLUDE) \

LOCAL_SHARED_LIBRARIES :=libteec
LOCAL_MODULE :=tee_helloworld

include $(BUILD_EXECUTABLE)

#include $(LOCAL_PATH)/ta/Android.mk
