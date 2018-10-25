LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE := crypto_verify.exe
LOCAL_SRC_FILES := \
		CryptoVerifyCa.c \
		CryptoVerifyCaDebug.c \
		CryptoVerifyCaTest.c


LOCAL_C_INCLUDES:= \
		$(LOCAL_PATH)/../inc \
		$(LOCAL_PATH)/

LOCAL_MODULE_TAGS:= eng
LOCAL_SHARED_LIBRARIES := libcutils libteec


include $(BUILD_EXECUTABLE)