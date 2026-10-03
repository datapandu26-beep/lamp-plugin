LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := LightsControl64
LOCAL_SRC_FILES := main.cpp \
                   AML/mod/logger.cpp \
                   AML/mod/config.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/AML/include
LOCAL_CPPFLAGS  := -std=c++17 -fvisibility=hidden
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
