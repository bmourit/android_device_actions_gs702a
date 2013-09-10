LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libdvm
#LOCAL_SRC_FILES := libdvm.so
#LOCAL_MODULE_STEM := libdvm.so
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := gpu_config
LOCAL_SRC_FILES := gpu_config
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := game_r3
LOCAL_SRC_FILES := game_r3
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := game_r2
LOCAL_SRC_FILES := game_r2
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
include $(BUILD_PREBUILT)

#omx/components/audio_decorder
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX.Action.Audio.Decoder
LOCAL_SRC_FILES := libOMX.Action.Audio.Decoder.so
LOCAL_MODULE_STEM := libOMX.Action.Audio.Decoder.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/components/camera
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX.Action.Video.Camera
LOCAL_SRC_FILES := libOMX.Action.Video.Camera.so
LOCAL_MODULE_STEM := libOMX.Action.Video.Camera.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libACT_V4L2HAL
LOCAL_SRC_FILES := libACT_V4L2HAL.so
LOCAL_MODULE_STEM := libACT_V4L2HAL.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/components/jpeg_decorder
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX.Action.Image.Decoder
LOCAL_SRC_FILES := libOMX.Action.Image.Decoder.so
LOCAL_MODULE_STEM := libOMX.Action.Image.Decoder.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
 include $(BUILD_PREBUILT)

#omx/components/omx_vce
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX.Action.Video.Encoder
LOCAL_SRC_FILES := libOMX.Action.Video.Encoder.so
LOCAL_MODULE_STEM := libOMX.Action.Video.Encoder.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libACT_EncAPI
LOCAL_SRC_FILES := libACT_EncAPI.so
LOCAL_MODULE_STEM := libACT_EncAPI.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/components/video_decorder
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX.Action.Video.Decoder
LOCAL_SRC_FILES := libOMX.Action.Video.Decoder.so
LOCAL_MODULE_STEM := libOMX.Action.Video.Decoder.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/libstagefreighthw
include $(CLEAR_VARS)
LOCAL_MODULE := libstagefrighthw
LOCAL_SRC_FILES := libstagefrighthw.so
LOCAL_MODULE_STEM := libstagefrighthw.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/omx_core
include $(CLEAR_VARS)
LOCAL_MODULE := libOMX_Core
LOCAL_SRC_FILES := libOMX_Core.so
LOCAL_MODULE_STEM:= libOMX_Core.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#gralloc_HW
include $(CLEAR_VARS)
LOCAL_MODULE := gralloc.$(TARGET_BOARD_PLATFORM)
LOCAL_SRC_FILES := gralloc.$(TARGET_BOARD_PLATFORM).so
LOCAL_MODULE_STEM := gralloc.$(TARGET_BOARD_PLATFORM).so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)

#omx/base
include $(CLEAR_VARS)
LOCAL_MODULE := libomxBellagio_base  
LOCAL_SRC_FILES := libomxBellagio_base.a
LOCAL_MODULE_STEM := libomxBellagio_base.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := pfmnceserver
LOCAL_SRC_FILES := pfmnceserver
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libjni_mosaic
#LOCAL_SRC_FILES := libjni_mosaic.so
#LOCAL_MODULE_STEM := libjni_mosaic.so
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#include $(BUILD_PREBUILT)

#utils_prebuilt_target :=

#if source code not available do prebuilt
#ifneq ($(words $(shell find device/actions/gs702a/packages/OTA/Android.mk)),1)
#include $(CLEAR_VARS)
#LOCAL_MODULE := update
#LOCAL_MODULE_STEM := update.apk
#LOCAL_SRC_FILES := update.apk
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS :=APPS
#LOCAL_CERTIFICATE := PRESIGNED
#include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/app/update.apk
#endif

#prebulit webcore for internal dev
#ifneq ($(words $(shell find external/webkitAct/Android.mk)),1)
#include $(CLEAR_VARS)
#LOCAL_MODULE := libwebcore
#LOCAL_SRC_FILES := libwebcore.so
#LOCAL_MODULE_STEM := libwebcore.so
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/lib/libwebcore.so
#endif

#prebuilt libhwc 
ifneq ($(words $(shell find device/actions/gs702a/hardware/libhwc/libhwc/Android.mk)),1)
include $(CLEAR_VARS)
LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)
LOCAL_SRC_FILES := $(LOCAL_MODULE).so
LOCAL_MODULE_STEM := hwcomposer.$(TARGET_BOARD_PLATFORM).so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/lib/hw/hwcomposer.ATM702X.so
endif

#prebuilt libperformance 
ifneq ($(words $(shell find device/actions/gs702a/performancemanager/performanceservice/Android.mk)),1)
include $(CLEAR_VARS)
LOCAL_MODULE := libperformance
LOCAL_SRC_FILES := $(LOCAL_MODULE).so
LOCAL_MODULE_STEM:= $(LOCAL_MODULE).so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/lib/libperformance.so
endif

#prebuilt performancepolicy 
ifneq ($(words $(shell find device/actions/gs702a/performancemanager/performancepolicy/Android.mk)),1)
include $(CLEAR_VARS)
LOCAL_MODULE := performancepolicy
LOCAL_MODULE_STEM := performancepolicy.apk
LOCAL_SRC_FILES := performancepolicy.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := PRESIGNED
include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/app/performancepolicy.apk
endif

#prebuilt performancepolicy 
ifneq ($(words $(shell find device/actions/gs702a/packages/ActSensorCalib/Android.mk)),1)
include $(CLEAR_VARS)
LOCAL_MODULE := ActSensorCalib
LOCAL_MODULE_STEM := ActSensorCalib.apk
LOCAL_SRC_FILES := ActSensorCalib.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS :=APPS
LOCAL_CERTIFICATE := PRESIGNED
include $(BUILD_PREBUILT)
#else
#utils_prebuilt_target += $(TARGET_OUT)/app/ActSensorCalib.apk
endif
