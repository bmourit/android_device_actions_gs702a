#
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		core/java/com/actions/hardware/DisplayManager.java \
		core/java/com/actions/hardware/PerformanceManager.java \
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := actions

# AIDL
LOCAL_AIDL_INCLUDES += $(LOCAL_PATH)/core/java
LOCAL_SRC_FILES += \
		core/java/com/actions/hardware/ICableStatusListener.aidl \
		core/java/com/actions/hardware/IDisplayService.aidl \
		core/java/com/actions/hardware/IPerformanceService.aidl


built_framework := $(call java-lib-deps,framework)
LOCAL_PRELINK_MODULE := false
include $(BUILD_JAVA_LIBRARY)

# The JNI component
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))
