LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	boot.c \
	check.c \
	common.c \
	fat.c \
	file.c \
	io.c \
	lfn.c \
	dosfslabel.c

	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= dosfslabel

include $(BUILD_EXECUTABLE)
