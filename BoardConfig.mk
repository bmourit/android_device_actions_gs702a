#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Arch vars
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_ARCH_VARIANT_CPU := cortex-a5
TARGET_CPU_VARIANT := cortex-a5

TARGET_GLOBAL_CFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp

# Depreciated vars
TARGET_FPU_VARIANT := neon
TARGET_ARCH_VARIANT_FPU := neon
ARCH_ARM_HAVE_TLS_REGISTER := true
ARCH_ARM_HAVE_32_BYTE_CACHE_LINES := true

TARGET_NO_BOOTLOADER := true
TARGET_NO_RECOVERY := false

TARGET_NO_RADIOIMAGE := true
TARGET_BOARD_PLATFORM := ATM702X
TARGET_BOOTLOADER_BOARD_NAME := full_gs702a

BOARD_EGL_CFG := device/actions/gs702a/configs/egl.cfg

# Workaround for missing EGL_KHL_gl_colorspace extention
BOARD_EGL_WORKAROUND_BUG_10194508 := true

#OVERRIDE_RS_DRIVER := libRSDriverArm.so
#BOARD_PROVIDES_LIBRIL := true

#BOARD_USES_HGL := true
#BOARD_USES_OVERLAY := true
USE_OPENGL_RENDERER := true
#NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
BOARD_HAVE_PIXEL_FORMAT_INFO := true

TARGET_RECOVERY_LCD_BACKLIGHT_PATH := /sys/class/backlight/act_pwm_backlight/brightness
TARGET_USERIMAGES_USE_EXT4 := true
#BOARD_BOOTIMAGE_PARTITION_SIZE := 50331648
#BOARD_RECOVERYIMAGE_PARTITION_SIZE := 50331648
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 536870912
BOARD_USERDATAIMAGE_PARTITION_SIZE := 536870912
BOARD_FLASH_BLOCK_SIZE := 4096

#TARGET_PROVIDES_INIT_RC := true

#BOARD_HAL_STATIC_LIBRARIES :=
TARGET_USES_CM_POWERHAL := true

#TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888
TARGET_RECOVERY_FSTAB := device/actions/gs702a/rootdir/recovery.fstab
#TARGET_RECOVERY_UI_LIB := librecovery_ui_gs702a
#TARGET_RECOVERY_UPDATER_LIBS += librecovery_updater_gs702a
#TARGET_RELEASETOOLS_EXTENSIONS := device/actions/gs702a
SKIP_SET_METADATA := true

DEVICE_PACKAGE_OVERLAYS += device/actions/gs702a/overlay

# Use the non-open-source parts, if they're present
-include vendor/actions/gs702a/BoardConfigVendor.mk

BOARD_USES_GENERIC_AUDIO := true
USE_CAMERA_STUB := true
BOARD_HARDWARE_CLASS := device/actions/gs702a/cmhw

# Kernel
#BOARD_KERNEL_IMAGE_NAME := Image
TARGET_PREBUILT_KERNEL := device/actions/gs702a/configs/kernel
#TARGET_KERNEL_SOURCE := kernel/actions/gs702a
#TARGET_KERNEL_CONFIG := leopard_evb_defconfig
BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_CMDLINE := console=ttyS5,115200 earlyprintk androidboot.selinux=0
BOARD_PAGE_SIZE := 2048

#TARGET_KERNEL_MODULES += ACT_MODULES

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR=device/generic/common/bluetooth

# Board
TARGET_BOARD_INFO_FILE := device/actions/gs702a/board-info.txt

# Graphics
ENABLE_WEBGL := true
BOARD_USE_SKIA_LCDTEXT := true
BOARD_USES_HWCOMPOSER := true
TARGET_USES_ION := true

BOARD_CHARGER_DISABLE_INIT_BLANK := true
TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := true
SENSORS_NEED_SETRATE_ON_ENABLE := true
#BOARD_USE_MHEAP_SCREENSHOT := true

# TWRP
DEVICE_RESOLUTION := 1280x800
TW_INTERNAL_STORAGE_PATH := "/dev/block/actj"
TW_INTERNAL_STORAGE_MOUNT_POINT := "/sdcard"
TW_ENTERNAL_STORAGE_PATH := "/dev/block/mmcblk0"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "/sdcardext"
#RECOVERY_TOUCHSCREEN_SWAP :=
#RECOVERY_TOUCHSCREEN_FLIP_Y :=
#RECOVERY_TOUCHSCREEN_FLIP_X :=
TW_NO_REBOOT_BOOTLOADER := true
TW_DEFAULT_EXTERNAL_STORAGE := true
TW_FLASH_FROM_STORAGE := true

# Toolchain
TARGET_GCC_VERSION := 4.8
TARGET_TOOLS_PREFIX :=
TARGET_GLOBAL_CFLAGS += -O3
TARGET_GLOBAL_CPPFLAGS += -O3

# Recovery
#TARGET_RECOVERY_INITRC := device/actions/gs702a/init.recovery.gs702a.rc
TARGET_PREBUILT_RECOVERY_KERNEL := device/actions/gs702a/recovery/recovery-kernel
BOARD_HAS_NO_SELECT_BUTTON := true

RECOVERY_FSTAB_VERSION := 2

# Bootanimation
TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true
# Enable workaround for slow rom flash
#BOARD_SUPPRESS_SECURE_ERASE := true

# Releasetool
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := build/tools/releasetools/ota_from_target_files

# Wifi stuff
BOARD_WIFI_VENDOR := realtek
BOARD_WLAN_VENDOR := realtek
TARGET_CUSTOM_WIFI := device/actions/gs702a/hardware/wlan/rtl8188eu/libhardware_legacy/wifi/wifi_realtek.c

-include device/actions/gs702a/wifi.mk

# Android 4.4 defs
COMMON_GLOBAL_CFLAGS += -DACT_CODECS -DACT_HARDWARE -DCHARGER_DISABLE_INIT_BLANK -DACT_GRLC -DACT_AUDIO -DACT_CODEC
# For Camera
COMMON_GLOBAL_CFLAGS += -DNEEDS_VECTORIMPL_SYMBOLS
COMMON_LOCAL_CFLAGS += -DHAVE_PRE_KITKAT_AUDIO_BLOB

# Device select
TARGET_DEVICE := gs702a
#TARGET_DEVICE := venus2
#TARGET_DEVICE := crystal2

# Selinux policies
#BOARD_SEPOLICY_DIRS := \
#	device/actions/gs702a/selinux

#BOARD_SEPOLICY_UNION := \
#	app.te \
#	installd.te \
#	pfmnceserver.te \
#	hostapd.te \
#	mediaserver.te \
#	wpa_supplicant.te
