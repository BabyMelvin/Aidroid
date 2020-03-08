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
