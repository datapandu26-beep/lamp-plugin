LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := LightsControl
LOCAL_SRC_FILES := main.cpp
LOCAL_CPPFLAGS  := -std=c++17 -fvisibility=hidden
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
