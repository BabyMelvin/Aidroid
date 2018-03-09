LOCAL_PATH:=$(call my-dir)

#######################AIDL########################
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(call all-subdir-java-files)

LOCAL_MODULE_TAG:=eng

LOCAL_SRC_FILES += \
				   core/java/melvin/hardware/ILedService.aidl

include $(BUILD_JAVA_LIBRARY)
####################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

