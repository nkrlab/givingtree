LOCAL_PATH := $(call my-dir)

# Prebuilt Lib
include $(CLEAR_VARS)
LOCAL_MODULE := protobuf-prebuilt
LOCAL_SRC_FILES := libprotobuf-android-ndk.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CPPFLAGS := -frtti
include $(PREBUILT_SHARED_LIBRARY)
