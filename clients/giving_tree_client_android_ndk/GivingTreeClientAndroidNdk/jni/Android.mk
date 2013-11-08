LOCAL_PATH := $(call my-dir)
my_LOCAL_PATH := $(LOCAL_PATH)

# Include all libs (built and prebuilt)
include $(LOCAL_PATH)/libs/Android.mk

# Build my executable
LOCAL_PATH := $(my_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := giving_tree_client-android-ndk

LOCAL_SRC_FILES := \
  src/common/info.cc \
  src/common/uuid.cc \
  src/funapi/client_message_writer.cc \
  src/funapi/funapi_client.cc \
  src/gen/funapi/account/account_messages.pb.cc \
  src/gen/app_messages.pb.cc \
  src/gen/giving_tree_client_messages.pb.cc \
  src/gen/giving_tree_server_messages.pb.cc \
  src/client_app_message_writer.cc \
  src/giving_tree_client.cc \
  start.cc

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/src \
  $(LOCAL_PATH)/src/gen

LOCAL_CPPFLAGS := -frtti

LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := protobuf-prebuilt

include $(BUILD_SHARED_LIBRARY)
