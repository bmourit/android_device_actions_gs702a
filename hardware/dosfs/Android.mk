LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	utils/dosfstools-3.0.12/boot.c \
	utils/dosfstools-3.0.12/check.c \
	utils/dosfstools-3.0.12/common.c \
	utils/dosfstools-3.0.12/fat.c \
	utils/dosfstools-3.0.12/file.c \
	utils/dosfstools-3.0.12/io.c \
	utils/dosfstools-3.0.12/lfn.c \
	utils/dosfstools-3.0.12/dosfslabel.c

	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= dosfslabel

include $(BUILD_EXECUTABLE)
