LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := com.actions.hardware.xml
LOCAL_SRC_FILES := com.actions.hardware.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/permissions
include $(BUILD_PREBUILT)
