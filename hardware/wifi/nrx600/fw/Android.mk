ifeq ($(strip $(BOARD_WIFI_VENDOR)), nanoradio)

LOCAL_PATH := $(call my-dir)

# if you change the wifi_dir you must change and in 
# hardware/libhardware_legacy/wifi/wifi.c the :
# DRIVER_MODULE_IF_PATH and DRIVER_MODULE_TRANS_PATH
ifndef WIFI_DRIVER_FIRMWARE_PATH
wifi_dir :=  $(TARGET_OUT)/vendor/nanoradio/wifi
else
wifi_dir :=  $(TARGET_OUT)$(WIFI_DRIVER_FIRMWARE_PATH)
endif

##################################################
# NRX600 firmware files
ifeq ($(WIFI_SUPPORT_NRX600),true)

include $(CLEAR_VARS)
LOCAL_MODULE := nrx600-sta.fw
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(wifi_dir)/fw
LOCAL_SRC_FILES := $(LOCAL_MODULE)
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := nrx600-softap.fw
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(wifi_dir)/fw
LOCAL_SRC_FILES := $(LOCAL_MODULE)
#include $(BUILD_PREBUILT)

endif

endif
