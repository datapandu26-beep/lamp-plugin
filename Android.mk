LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := LightsControl
LOCAL_SRC_FILES := main.cpp \
                   AML/include/mod/config.cpp \
                   AML/include/mod/logger.cpp

LOCAL_CPPFLAGS  := -std=c++17 -fvisibility=hidden
LOCAL_C_INCLUDES := $(LOCAL_PATH)/AML/include
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
