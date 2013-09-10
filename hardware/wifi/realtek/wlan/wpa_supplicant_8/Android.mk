ifeq ($(strip $(BOARD_WIFI_VENDOR)), realtek)

ifeq ($(WPA_SUPPLICANT_VERSION),VER_0_8_X)
    include $(call all-subdir-makefiles)
endif

endif
