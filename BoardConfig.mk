#
# Copyright (C) 2012 The Android Open-Source Project
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
LOCAL_PATH := device/actions/gs702a

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Use the non-open-source parts, if they're present
-include vendor/actions/gs702a/BoardConfigVendor.mk

HAVE_HTC_AUDIO_DRIVER := true
BOARD_USES_GENERIC_AUDIO := true

TARGET_BOARD_PLATFORM := ATM702X
TARGET_BOARD_PLATFORM_GPU := VIVANTE
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a5
TARGET_ARCH_VARIANT_CPU := cortex-a5
TARGET_ARCH_VARIANT_FPU := neon-vfpv4
ARCH_ARM_HAVE_ARMV7A := true
ARCH_ARM_HAVE_VFP := true
ARCH_ARM_HAVE_VFP_D32 := true
ARCH_ARM_HAVE_NEON := true
ARCH_ARM_HAVE_THUMB_SUPPORT := true
ARCH_ARM_HAVE_TLS_REGISTER := true

#boot
BOARD_KERNEL_CMDLINE := console=ttyS5,115200 earlyprintk
BOARD_KERNEL_BASE := 0x00000000
BOARD_PAGE_SIZE := 2048

TARGET_PROVIDES_INIT_RC := true

TARGET_BOOTLOADER_BOARD_NAME := full_gs702a
TARGET_NO_RADIOIMAGE := true
TARGET_NO_BOOTLOADER := true
TARGET_HAVE_HDMI_OUT := true

#Vold
BOARD_VOLD_MAX_PARTITIONS := 20
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_DISC_HAS_MULTIPLE_MAJORS := true
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/platform/actions-dwc3.1/dwc3.0/gadget/lun%d/file"
TARGET_USE_CUSTOM_SECOND_LUN_NUM := 1

#Partitions
BOARD_BOOTIMAGE_PARTITION_SIZE := 50331648
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 50331648
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 536870912
BOARD_USERDATAIMAGE_PARTITION_SIZE := 2147483648
BOARD_FLASH_BLOCK_SIZE := 4096
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_HAS_LARGE_FILESYSTEM := true

#Indicate that the board has an Internal SD Card
BOARD_HAS_SDCARD_INTERNAL := true

BOARD_DATA_DEVICE := /dev/block/actd
BOARD_DATA_FILESYSTEM := ext4
BOARD_CACHE_DEVICE := /dev/block/acte
BOARD_CACHE_FILESYSTEM := ext4

TARGET_PREBUILT_KERNEL := device/actions/gs702a/kernel
DEVICE_RESOLUTION := 1280x800

#Releasetools
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := device/actions/gs702a/releasetools/gs702a_ota_from_target_files

#Graphics
BOARD_EGL_CFG := $(LOCAL_PATH)/egl.cfg
BOARD_USES_HGL := true
BOARD_USES_OVERLAY := true
USE_OPENGL_RENDERER := true
TARGET_USES_GL_VENDOR_EXTENSIONS := true
TARGET_DISABLE_TRIPLE_BUFFERING := false
ENABLE_WEBGL := true
BOARD_USE_SKIA_LCDTEXT := true
BOARD_USES_PROPRIETARY_OMX := true
BOARD_USES_HWCOMPOSER := true
USE_GPU_ALLOCATOR := true
PREBUILT_VIV_GPU := true

#Camera
USE_CAMERA_STUB := false

TARGET_GCC_VERSION := 4.7
TARGET_TOOLS_PREFIX :=
TARGET_GLOBAL_CFLAGS += -O3
TARGET_GLOBAL_CPPFLAGS += -O3

#Recovery
TARGET_RECOVERY_INITRC := device/actions/gs702a/init.recovery.gs702a.rc
TARGET_PREBUILT_RECOVERY_KERNEL := device/actions/gs702a/recovery-kernel
BOARD_HAS_NO_SELECT_BUTTON := true
TARGET_RECOVERY_FSTAB := device/actions/gs702a/ramdisk/recovery.fstab

# Wifi stuff
WIFI_DRIVER_MODULE_NAME := wlan
WIFI_DRIVER_MODULE_PATH := /misc/modules/wlan.ko

WIFI_DRIVER_FW_PATH_STA := none
WIFI_DRIVER_MODULE_ARG := "ifname=wlan0 if2name=p2p0"

WPA_SUPPLICANT_VERSION := VER_0_8_X_RTL
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_rtl
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_rtl

BOARD_WLAN_VENDOR := REALTEK
BOARD_WLAN_DEVICE := rtl8188eu

TARGET_CUSTOM_WIFI := device/actions/gs702a/wifi/wifi_realtek.c

TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true

BOARD_HAVE_BLUETOOTH := false
