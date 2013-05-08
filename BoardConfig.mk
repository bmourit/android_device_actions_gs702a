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

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Use the non-open-source parts, if they're present
include vendor/actions/gs702a/BoardConfigVendor.mk

BOARD_USES_GENERIC_AUDIO := true
#BOARD_USES_AUDIO_LEGACY := true
USE_CAMERA_STUB := false
BOARD_USES_GPS_TYPE := simulator

TARGET_NO_RADIOIMAGE := true
TARGET_NO_BOOTLOADER := true
TARGET_BOOTLOADER_BOARD_NAME := gs702a
TARGET_HAVE_HDMI_OUT := true

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_ARCH_VARIANT_CPU := cortex-a5
TARGET_ARCH_VARIANT_FPU := neon
ARCH_ARM_HAVE_ARMV7A := true
ARCH_ARM_HAVE_VFP := true
ARCH_ARM_HAVE_NEON := true
ARCH_ARM_HAVE_THUMB_SUPPORT := true
ARCH_ARM_HAVE_TLS_REGISTER := true

#TARGET_TOOLS_PREFIX := /Volumes/MacintoshHD2/CM10.1/prebuilts/gcc/darwin-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
TARGET_GLOBAL_CFLAGS += -O3 -mtune=cortex-a5 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -O3 -mtune=cortex-a5 -mfpu=neon -mfloat-abi=softfp

BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_CMDLINE := console=ttyS5,115200 earlyprintk
BOARD_PAGE_SIZE := 2048

TARGET_BOARD_PLATFORM := ATM702X

USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/actions/gs702a/egl.cfg
ENABLE_WEBGL := true
TARGET_FORCE_CPU_UPLOAD := true
BOARD_USE_SKIA_LCDTEXT := true

TARGET_RECOVERY_INITRC := device/actions/gs702a/init.recovery.gs702a.rc
TARGET_PREBUILT_RECOVERY_KERNEL := device/actions/gs702a/rec/kernel
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_UMS_LUNFILE := "/sys/class/android_usb/android0/f_mass_storage/lun/file"
BOARD_UMS_2ND_LUNFILE := "/sys/class/android_usb/android0/f_mass_storage/lun1/file"

#BOARD_WLAN_DEVICE := rtl8192cu
#WPA_SUPPLICANT_VERSION := VER_0_8_X
#BOARD_WPA_SUPPLICANT_DRIVER := WEXT
#BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_rtl
#BOARD_HOSTAPD_DRIVER        := WEXT
#BOARD_HOSTAPD_PRIVATE_LIB   := lib_driver_cmd_rtl
WIFI_DRIVER_MODULE_PATH     := "/system/lib/modules/wlan.ko"
WIFI_DRIVER_FW_PATH_PARAM   := ""
WIFI_DRIVER_FW_PATH_STA     := ""
WIFI_DRIVER_FW_PATH_AP      := ""
WIFI_DRIVER_FW_PATH_P2P     := ""
WIFI_DRIVER_MODULE_NAME     := "wlan"
WIFI_DRIVER_MODULE_ARG      := ""

BOARD_HAS_LARGE_FILESYSTEM := true

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 50331648
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 50331648
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 377487360
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1073741824
BOARD_FLASH_BLOCK_SIZE := 4096

TARGET_PROVIDES_INIT_RC := true

TARGET_PREBUILT_KERNEL := device/actions/gs702a/kernel

TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true
TARGET_BOOTANIMATION_USE_RGB565 := true

BOARD_HAVE_BLUETOOTH := false
DEVICE_RESOLUTION := 1280x800

TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := device/actions/gs702a/releasetools/gs702a_ota_from_target_files
