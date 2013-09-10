ifeq ($(strip $(BOARD_WIFI_VENDOR)), nanoradio)

LOCAL_PATH := $(call my-dir)


WIFI_RELATIVE_PATH := ../
LIBNRX_RELATIVE_PATH := $(WIFI_RELATIVE_PATH)

WIFI_DIR_OBJ := $(LOCAL_PATH)/$(WIFI_RELATIVE_PATH)/obj
WIFI_DIR_LIBNRX := $(LOCAL_PATH)/$(LIBNRX_RELATIVE_PATH)
WIFI_DIR_IC := $(LOCAL_PATH)/$(WIFI_RELATIVE_PATH)/kernel/ic


ifeq ($(TARGET_SIMULATOR),true)
  $(error This makefile must not be included when building the simulator)
endif

ifeq ($(WPA_SUPPLICANT_VERSION),VER_0_6_X)
    WPA_SUPPL_DIR = external/wpa_supplicant_6/wpa_supplicant
endif

ifeq ($(WPA_SUPPLICANT_VERSION),VER_0_8_X)
    WPA_SUPPL_DIR = external/wpa_supplicant_8/wpa_supplicant
endif

include $(WPA_SUPPL_DIR)/android.config

ifneq ($(BOARD_WPA_SUPPLICANT_DRIVER),)
  CONFIG_DRIVER_$(BOARD_WPA_SUPPLICANT_DRIVER) := y
endif

L_CFLAGS = -DCONFIG_DRIVER_WEXT -DWPA_SUPPLICANT_$(WPA_SUPPLICANT_VERSION)
L_SRC :=

ifdef CONFIG_NO_STDOUT_DEBUG
L_CFLAGS += -DCONFIG_NO_STDOUT_DEBUG
endif

ifdef CONFIG_DEBUG_FILE
L_CFLAGS += -DCONFIG_DEBUG_FILE
endif

ifdef CONFIG_ANDROID_LOG
L_CFLAGS += -DCONFIG_ANDROID_LOG
endif

ifdef CONFIG_IEEE8021X_EAPOL
L_CFLAGS += -DIEEE8021X_EAPOL
endif

ifdef CONFIG_WPS
L_CFLAGS += -DCONFIG_WPS
endif

ifdef CONFIG_DRIVER_WEXT
L_SRC += driver_nano_cmd_wext.c nano_android.c libWifiApi.c
endif

ifdef CONFIG_DRIVER_NL80211
L_SRC += driver_nano_cmd_nano.c nano_android.c libWifiApi.c
endif

L_SRC += $(LIBNRX_RELATIVE_PATH)/counters.c  \
        $(LIBNRX_RELATIVE_PATH)/cvt.c       \
        $(LIBNRX_RELATIVE_PATH)/event.c     \
        $(LIBNRX_RELATIVE_PATH)/ibss.c      \
        $(LIBNRX_RELATIVE_PATH)/ifname.c    \
        $(LIBNRX_RELATIVE_PATH)/init.c      \
        $(LIBNRX_RELATIVE_PATH)/mib.c       \
        $(LIBNRX_RELATIVE_PATH)/param.c     \
        $(LIBNRX_RELATIVE_PATH)/powersave.c \
        $(LIBNRX_RELATIVE_PATH)/printbuf.c  \
        $(LIBNRX_RELATIVE_PATH)/priomap.c   \
        $(LIBNRX_RELATIVE_PATH)/radio.c     \
        $(LIBNRX_RELATIVE_PATH)/roam.c		\
        $(LIBNRX_RELATIVE_PATH)/scan.c		\
        $(LIBNRX_RELATIVE_PATH)/wext.c		\
        $(LIBNRX_RELATIVE_PATH)/android.c

INCLUDES = $(WPA_SUPPL_DIR) \
    $(WPA_SUPPL_DIR)/src \
    $(WPA_SUPPL_DIR)/src/common \
    $(WPA_SUPPL_DIR)/src/drivers \
    $(WPA_SUPPL_DIR)/src/l2_packet \
    $(WPA_SUPPL_DIR)/src/utils \
    $(WPA_SUPPL_DIR)/src/wps \
    external/libnl-headers \
    $(WIFI_DIR_OBJ) \
    $(WIFI_DIR_LIBNRX) \
    $(WIFI_DIR_IC) \
    ../../../../../../../leopard/kernel/linux-3.4/include/linux \
    ../../../../../../../leopard/kernel/linux-3.4/include/

include $(CLEAR_VARS)
LOCAL_MODULE := lib_nano_cmd_nrx
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libc libcutils
LOCAL_CFLAGS := $(L_CFLAGS)
LOCAL_SRC_FILES := $(L_SRC)
LOCAL_C_INCLUDES := $(INCLUDES)
#include $(BUILD_STATIC_LIBRARY)

########################
endif
