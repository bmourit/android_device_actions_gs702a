LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libGAL.so libGLSLC.so
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

###########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := gpuhwcomposer.ATM702X.so
LOCAL_SRC_FILES := hw/gpuhwcomposer.ATM702X.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/hw
include $(BUILD_PREBUILT)

###########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv1_CM_VIVANTE.so
LOCAL_SRC_FILES := egl/libGLESv1_CM_VIVANTE.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/egl
include $(BUILD_PREBUILT)

###########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libEGL_VIVANTE.so
LOCAL_SRC_FILES := egl/libEGL_VIVANTE.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/egl
include $(BUILD_PREBUILT)

###########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv2_VIVANTE.so
LOCAL_SRC_FILES := egl/libGLESv2_VIVANTE.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/egl
include $(BUILD_PREBUILT)
