# Copyright (C) 2012 The Android Open Source Project
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

$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)

DEVICE_PACKAGE_OVERLAYS := device/actions/gs702a/overlay

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/kernel:kernel

# Ramdisk
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/charger:root/charger \
    $(LOCAL_PATH)/ramdisk/init:root/init \
    $(LOCAL_PATH)/ramdisk/init.rc:root/init.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.rc:root/init.gs702a.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.usb.rc:root/init.gs702a.usb.rc \
    $(LOCAL_PATH)/ramdisk/fstab.gs702a:root/fstab.gs702a \
    $(LOCAL_PATH)/ramdisk/fstab.sdboot.gs702a:root/fstab.sdboot.gs702a \
    $(LOCAL_PATH)/ramdisk/init.eth0.rc:root/init.eth0.rc \
    $(LOCAL_PATH)/ramdisk/init.extra_modules.rc:root/init.extra_modules.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.sdboot.rc:root/init.gs702a.sdboot.rc \
    $(LOCAL_PATH)/ramdisk/init.modules.rc:root/init.modules.rc \
    $(LOCAL_PATH)/ramdisk/init.modules.sdboot.rc:root/init.modules.sdboot.rc \
    $(LOCAL_PATH)/ramdisk/init.quickboot.rc:root/init.quickboot.rc \
    $(LOCAL_PATH)/ramdisk/init.trace.rc:root/init.trace.rc \
    $(LOCAL_PATH)/ramdisk/init.wifi.rc:root/init.wifi.rc \
    $(LOCAL_PATH)/ramdisk/quickboot.sh:root/quickboot.sh \
    $(LOCAL_PATH)/ramdisk/ueventd.gs702a.rc:root/ueventd.gs702a.rc \
    $(LOCAL_PATH)/ramdisk/usbmond.sh:root/usbmond.sh

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/dev,root/dev)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/lib,root/lib)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/res,root/res)

# init.d scripts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.d/01preload:system/etc/init.d/01preload \
    $(LOCAL_PATH)/init.d/03kernel:system/etc/init.d/03kernel \
    $(LOCAL_PATH)/init.d/04mount:system/etc/init.d/04mount \
    $(LOCAL_PATH)/init.d/98tweaks:system/etc/init.d/98tweaks

# Prebuilt configs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml \
    $(LOCAL_PATH)/configs/gpu_config:system/etc/gpu_config \
    $(LOCAL_PATH)/configs/audio_effects.conf:system/etc/audio_effects.conf \
    $(LOCAL_PATH)/configs/audio_policy.conf:system/etc/audio_policy.conf \
    $(LOCAL_PATH)/configs/media_codecs.xml:system/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media_profiles.xml:system/etc/media_profiles.xml \
    $(LOCAL_PATH)/configs/omx_codec.xml:system/etc/omx_codec.xml \
    $(LOCAL_PATH)/configs/vold.fstab:system/etc/vold.fstab \
    $(LOCAL_PATH)/configs/vold.sdboot.fstab:system/etc/vold.sdboot.fstab \
    $(LOCAL_PATH)/configs/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    $(LOCAL_PATH)/configs/ft5x0x_ts.idc:system/usr/idc/ft5x0x_ts.idc \
    $(LOCAL_PATH)/configs/GT813.idc:system/usr/idc/GT813.idc \
    $(LOCAL_PATH)/configs/mt395.idc:system/usr/idc/mt395.idc \
    $(LOCAL_PATH)/configs/atc260x-adckeypad.kl:system/usr/keylayout/atc260x-adckeypad.kl 


PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/configs/usb_modeswitch.d,system/etc/usb_modeswitch.d)

# Prebuilt kernel modules
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/modules,system/lib/modules)

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml
    
PRODUCT_PACKAGES := \
    HoloSpiralWallpaper \
    LiveWallpapersPicker \
    VisualizationWallpapers 

PRODUCT_PACKAGES += \
    Camera

PRODUCT_PACKAGES += \
    audio.primary.default \
    audio_policy.default

PRODUCT_PACKAGES += \
    librs_jni \
    com.android.future.usb.accessory
    
PRODUCT_PACKAGES += \
    make_ext4fs \
    setup_fs

PRODUCT_PROPERTY_OVERRIDES := \
    service.adb.root=1 \
    ro.secure=0 \
    ro.allow.mock.location=1 \
    ro.debuggable=1

PRODUCT_PROPERTY_OVERRIDES += \
    ro.com.google.locationfeatures=1 \
    ro.setupwizard.enable_bypass=1 \
    dalvik.vm.lockprof.threshold=500 \
    dalvik.vm.dexopt-flags=v=n,o=v \
    ro.sf.lcd_density=160 \
    ro.opengles.version=131072 \
    ro.config.softopengles=0 \
    ro.config.used_hw_vsync=0 \
    ro.launcher.swipe=disable \
    ro.settings.config.hdmi=off \
    ro.systemui.volumekey=disable \
    ro.systemui.capture=disable \
    ro.launcher.config.cling=enable \
    ro.launcher.hideAPP=disable \
    ro.launcher.hideactivity=disable \
    ro.launcher.allapp.landX=10 \
    ro.launcher.allapp.landY=6 \
    ro.launcher.allapp.portX=5 \
    ro.launcher.allapp.portY=8 \
    ro.launcher.workspace.landX=8 \
    ro.launcher.workspace.landY=7 \
    ro.launcher.workspace.portX=8 \
    ro.launcher.workspace.portY=7 \
    launcher.force_enable_rotation=true \
    ro.product.usbdevice.VID=10d6 \
    ro.product.usbdevice.PID=fffe \
    ro.product.mtpdevice.PID=4e41 \
    ro.product.ptpdevice.PID=4e43 \
    ro.shutmenu.recovery=enable \
    ro.shutmenu.planemode=enable \
    ro.shutmenu.restart=enable \
    ro.usb.descriptor=ainol,Novo10 Hero,3.00 \
    ro.usbdevice.volumelabel=Novo10 Hero \
    ro.serialno=4512482adf0feeee \
    ro.config.quickboot=1 \
    ro.im.keysounddefenable=true \
    ro.support.gpswithwifi=1 \
    ro.wifi.signal.level.1=-70 \
    ro.wifi.signal.level.2=-65 \
    ro.wifi.signal.level.3=-60 \
    ro.wifi.signal.level.4=-55 \
    ro.product.pribrand=actions \  
    ro.product.primodel=owlx1 \
    ro.ota.autorecovery=enable \
    ro.device.model=ainol_LG 

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=USA/Pacific \
    persist.sys.language=en \
    persist.sys.country=US \
    persist.sys.use_dithering=1 \
    persist.sys.purgeable_assets=0 \
    windowsmgr.max_events_per_sec=300 \
    view.touch_slop=2 \
    view.minimum_fling_velocity=25 

DEVICE_PACKAGE_OVERLAYS += \
    $(LOCAL_PATH)/overlay
   
PRODUCT_AAPT_CONFIG := large mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0

$(call inherit-product, frameworks/native/build/tablet-dalvik-heap.mk)
$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product-if-exists, vendor/actions/gs702a/gs702a-vendor.mk)