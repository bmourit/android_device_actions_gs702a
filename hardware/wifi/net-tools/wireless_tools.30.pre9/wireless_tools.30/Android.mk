LOCAL_PATH:= $(call my-dir)

BUILD_WIRELESS_TOOLS  := false

ifeq ($(BUILD_WIRELESS_TOOLS), true)

################## build iwlib ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwlib.c
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= libiw
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libcutils libc libm
#include $(BUILD_STATIC_LIBRARY)

################## build iwconfig ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwconfig.c
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= iwconfig
LOCAL_STATIC_LIBRARIES := libcutils libc libm libiw
#LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES) # install to system/xbin
#LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_UNSTRIPPED)
LOCAL_MODULE_TAGS := optional
#include $(BUILD_EXECUTABLE)

################## build iwlist ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwlist.c iwlib.h
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= iwlist
LOCAL_STATIC_LIBRARIES := libcutils libc libm libiw
#LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES) # install to system/xbin
#LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_UNSTRIPPED)
LOCAL_MODULE_TAGS := optional
#include $(BUILD_EXECUTABLE)
#####################################################

endif
