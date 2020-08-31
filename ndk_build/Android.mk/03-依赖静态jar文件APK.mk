LOCAL_PATH：=$(call my-dir)

include $(CLEAR_VARS)

# List of static libraries to include in the package
LOCAL_STATIC_JAVA_LIBRARIES:= static-library

# Build all java files in the java subdirectory
LOCAL_SRC_FILES:=$(call all-subdir-java-files)

# Name of the APK to build
LOCAL_PACKAGE_NAME:= localPackage

# Tell it to build an APK

include $(BUILD_PACKAGE)
