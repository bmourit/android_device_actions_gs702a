LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	iostat.c

LOCAL_CFLAGS += -static -g -O3 -Wall -fomit-frame-pointer -Wno-error=sequence-point
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= iostat

include $(BUILD_EXECUTABLE)

