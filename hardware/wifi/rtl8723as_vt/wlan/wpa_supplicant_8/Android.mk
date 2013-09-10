ifeq ($(strip $(BOARD_WIFI_VENDOR)), rtl8723as_vt)

ifeq ($(WPA_SUPPLICANT_VERSION),VER_0_8_X)
    include $(call all-subdir-makefiles)
endif

endif
