
# Configuration
AP_BT_FIRMWARE_DIR := bluetooth
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := bt_vendor.conf
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := bcm20710a1.hcd
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)











