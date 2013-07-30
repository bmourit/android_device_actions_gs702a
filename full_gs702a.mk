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

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/kernel:kernel

# Ramdisk
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/charger:root/charger \
    $(LOCAL_PATH)/ramdisk/cp_vendor_app.sh:root/cp_vendor_app.sh \
    $(LOCAL_PATH)/ramdisk/default.prop:root/default.prop \
    $(LOCAL_PATH)/ramdisk/fstab.gs702a:root/fstab.gs702a \
    $(LOCAL_PATH)/ramdisk/fstab.sdboot.gs702a:root/fstab.sdboot.gs702a \
    $(LOCAL_PATH)/ramdisk/init.eth0.rc:root/init.eth0.rc \
    $(LOCAL_PATH)/ramdisk/init.extra_modules.rc:root/init.extra_modules.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.rc:root/init.gs702a.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.sdboot.rc:root/init.gs702a.sdboot.rc \
    $(LOCAL_PATH)/ramdisk/init.gs702a.usb.rc:root/init.gs702a.usb.rc \
    $(LOCAL_PATH)/ramdisk/init.modules.rc:root/init.modules.rc \
    $(LOCAL_PATH)/ramdisk/init.modules.sdboot.rc:root/init.modules.sdboot.rc \
    $(LOCAL_PATH)/ramdisk/init.quickboot.rc:root/init.quickboot.rc \
    $(LOCAL_PATH)/ramdisk/init.rc:root/init.rc \
    $(LOCAL_PATH)/ramdisk/init.trace.rc:root/init.trace.rc \
    $(LOCAL_PATH)/ramdisk/init.usb.rc:root/init.usb.rc \
    $(LOCAL_PATH)/ramdisk/init.wifi.rc:root/init.wifi.rc \
    $(LOCAL_PATH)/ramdisk/quickboot.sh:root/quickboot.sh \
    $(LOCAL_PATH)/ramdisk/recovery.fstab:root/recovery.fstab \
    $(LOCAL_PATH)/ramdisk/ueventd.gs702a.rc:root/ueventd.gs702a.rc \
    $(LOCAL_PATH)/ramdisk/ueventd.rc:root/ueventd.rc \
    $(LOCAL_PATH)/ramdisk/usbmond.sh:root/usbmond.sh

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/dev,root/dev)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/data,root/data)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/lib,root/lib)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/proc,root/proc)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/cache,root/cache)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/res,root/res)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/sys,root/sys)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/init.d,system/etc/init.d)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/ramdisk/system,root/system)

# Prebuilt configs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml \
    $(LOCAL_PATH)/configs/gpu_config:system/etc/gpu_config \
    $(LOCAL_PATH)/configs/game_r2:system/etc/game_r2 \
    $(LOCAL_PATH)/configs/game_r3:system/etc/game_r3 \
    $(LOCAL_PATH)/configs/builtinapk:system/etc/builtinapk \
    $(LOCAL_PATH)/configs/audio_effects.conf:system/etc/audio_effects.conf \
    $(LOCAL_PATH)/configs/audio_policy.conf:system/etc/audio_policy.conf \
    $(LOCAL_PATH)/configs/media_codecs.xml:system/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media_profiles.xml:system/etc/media_profiles.xml \
    $(LOCAL_PATH)/configs/platform.xml:system/etc/permissions/platform.xml \
    $(LOCAL_PATH)/configs/features.xml:system/etc/permissions/features.xml \
    $(LOCAL_PATH)/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    $(LOCAL_PATH)/configs/packages-compat-default.xml:system/etc/packages-compat-default.xml \
    $(LOCAL_PATH)/configs/omx_codec.xml:system/etc/omx_codec.xml \
    $(LOCAL_PATH)/configs/ppp/ip-up-vpn:system/etc/ppp/ip-up-vpn \
    $(LOCAL_PATH)/configs/ft5x0x_ts.idc:system/usr/idc/ft5x0x_ts.idc \
    $(LOCAL_PATH)/configs/GT813.idc:system/usr/idc/GT813.idc \
    $(LOCAL_PATH)/configs/mt395.idc:system/usr/idc/mt395.idc \
    $(LOCAL_PATH)/configs/atc260x-adckeypad.kl:system/usr/keylayout/atc260x-adckeypad.kl \
    $(LOCAL_PATH)/configs/dhcpcd/dhcpcd-run-hooks:system/etc/dhcpcd/dhcpcd-run-hooks \
    $(LOCAL_PATH)/configs/dhcpcd/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    $(LOCAL_PATH)/configs/dhcpcd/dhcpcd-hooks/20-dns.conf:system/etc/dhcpcd/dhcpcd-hooks/20-dns.conf \
    $(LOCAL_PATH)/configs/dhcpcd/dhcpcd-hooks/95-configured:system/etc/dhcpcd/dhcpcd-hooks/95-configured \
    $(LOCAL_PATH)/configs/apns-conf.xml:system/etc/apns-conf.xml

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(LOCAL_PATH)/configs/usb_modeswitch.d,system/etc/usb_modeswitch.d)

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml

#HAL
PRODUCT_COPY_FILES += \
    device/actions/gs702a/prebuilt/audio_policy.default.so:system/lib/hw/audio_policy.default.so \
    device/actions/gs702a/prebuilt/audio.primary.default.so:system/lib/hw/audio.primary.default.so \
    device/actions/gs702a/prebuilt/audio.r_submix.default.so:system/lib/hw/audio.r_submix.default.so \
    device/actions/gs702a/prebuilt/camera.ATM702X.so:system/lib/hw/camera.ATM702X.so \
    device/actions/gs702a/prebuilt/camera.goldfish.so:system/lib/hw/camera.goldfish.so \
    device/actions/gs702a/prebuilt/cvbs.ATM702X.so:system/lib/hw/cvbs.ATM702X.so \
    device/actions/gs702a/prebuilt/displayengine.ATM702X.so:system/lib/hw/displayengine.ATM702X.so \
    device/actions/gs702a/prebuilt/gps.goldfish.so:system/lib/hw/gps.goldfish.so \
    device/actions/gs702a/prebuilt/gpuhwcomposer.ATM702X.so:system/lib/hw/gpuhwcomposer.ATM702X.so \
    device/actions/gs702a/prebuilt/gralloc.ATM702X.so:system/lib/hw/gralloc.ATM702X.so \
    device/actions/gs702a/prebuilt/gralloc.default.so:system/lib/hw/gralloc.default.so \
    device/actions/gs702a/prebuilt/hdmi.ATM702X.so:system/lib/hw/hdmi.ATM702X.so \
    device/actions/gs702a/prebuilt/hwcomposer.ATM702X.so:system/lib/hw/hwcomposer.ATM702X.so \
    device/actions/gs702a/prebuilt/lights.ATM702X.so:system/lib/hw/lights.ATM702X.so \
    device/actions/gs702a/prebuilt/lights.goldfish.so:system/lib/hw/lights.goldfish.so \
    device/actions/gs702a/prebuilt/local_time.default.so:system/lib/hw/local_time.default.so \
    device/actions/gs702a/prebuilt/power.default.so:system/lib/hw/power.default.so \
    device/actions/gs702a/prebuilt/sensors.ATM702X.so:system/lib/hw/sensors.ATM702X.so \
    device/actions/gs702a/prebuilt/sensors.goldfish.so:system/lib/hw/sensors.goldfish.so \
    device/actions/gs702a/prebuilt/udptest:system/bin/udptest \
    device/actions/gs702a/prebuilt/wfd:system/bin/wfd \
    device/actions/gs702a/prebuilt/updater:system/bin/updater \
    device/actions/gs702a/prebuilt/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    device/actions/gs702a/prebuilt/updater:system/bin/updater \
    device/actions/gs702a/prebuilt/qemud:system/bin/qemud \
    device/actions/gs702a/prebuilt/dbus-daemon:system/bin/dbus-daemon \
    device/actions/gs702a/prebuilt/qemu-props:system/bin/qemu-props \
    device/actions/gs702a/prebuilt/usbmond:system/bin/usbmond 

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/actions/gs702a/prebuilt/lib,system/lib)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/actions/gs702a/prebuilt/egl,system/lib/egl)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/actions/gs702a/prebuilt/plugins,system/lib/plugins)

PRODUCT_COPY_FILES += \
  device/actions/gs702a/prebuilt/bin/dosfslabel:system/bin/dosfslabel \
  device/actions/gs702a/prebuilt/bin/pfmnceserver:system/bin/pfmnceserver \
  device/actions/gs702a/prebuilt/bin/stagefright:system/bin/stagefright \
  device/actions/gs702a/prebuilt/bin/rild:system/bin/rild \
  device/actions/gs702a/prebuilt/xbin/iostat:system/xbin/iostat \
  device/actions/gs702a/prebuilt/xbin/tcpdump:system/xbin/tcpdump \
  device/actions/gs702a/prebuilt/xbin/mke2fs:system/xbin/mke2fs \
  device/actions/gs702a/prebuilt/xbin/e2fsck:system/xbin/e2fsck \
  device/actions/gs702a/prebuilt/xbin/usb_modeswitch:system/xbin/usb_modeswitch \
  device/actions/gs702a/prebuilt/app/ActSensorCalib.apk:system/app/ActSensorCalib.apk \
  device/actions/gs702a/prebuilt/app/AdobeFlashPlayer.apk:system/app/AdobeFlashPlayer.apk \
  device/actions/gs702a/prebuilt/app/DLNA.apk:system/app/DLNA.apk \
  device/actions/gs702a/prebuilt/framework/actions.jar:system/framework/actions.jar \
  device/actions/gs702a/prebuilt/lib/libalc.so:obj/lib/libalc.so \
  device/actions/gs702a/prebuilt/lib/libGAL.so:obj/lib/libGAL.so \

# HACK: Creat some intermediate files to link with
    $(shell mkdir -p out/target/product/gs702a/obj/SHARED_LIBRARIES/libalc_intermediates) \
    $(shell mkdir -p out/target/product/gs702a/obj/SHARED_LIBRARIES/libGAL_intermediates) \
    $(shell mkdir -p out/target/product/gs702a/obj/SHARED_LIBRARIES/libsync_intermediates) \
    $(shell touch out/target/product/gs702a/obj/SHARED_LIBRARIES/libalc_intermediates/export_includes) \
    $(shell touch out/target/product/gs702a/obj/SHARED_LIBRARIES/libGAL_intermediates/export_includes)

PRODUCT_PACKAGES += \
    libalc \
    libGAL \
    init.superuser.rc \
    kallsymsprint

PRODUCT_PACKAGES += \
    audio.r_submix.default \
    libaudioutils

PRODUCT_PACKAGES += \
    librs_jni \
    com.android.future.usb.accessory

PRODUCT_PACKAGES += \
    make_ext4fs

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072 \
    ro.config.softopengles=0 \
    ro.config.used_hw_vsync=0 \
    service.adb.root=1 \
    ro.setupwizard.enable_bypass=1 \
    persist.sys.usb.config=mtp \
    ro.product.usbdevice.VID=10d6 \
    ro.product.usbdevice.PID=fffe \
    ro.product.mtpdevice.PID=4e41 \
    ro.product.ptpdevice.PID=4e43 \
    ro.usb.descriptor=ainol,Novo10,Hero,3.00 \
    ro.usbdevice.volumelabel=Novo10,Hero \
    ro.serialno=4512482adf0feeee \
    ro.config.quickboot=0 \
    ro.im.keysounddefenable=true \
    ro.support.gpswithwifi=1 \
    ro.product.pribrand=actions \
    ro.product.primodel=owlx1 \
    debug.egl.hw=1 \
    debug.sf.hw=1 \
    debug.performance.tuning=1 \
    debug.composition.type=gpu \
    persist.sys.ui.hw=1 \
    ro.device.model=unknown \
    hwui.render_dirty_regions=false \
    dalvik.vm.dexopt-flags=v=n,o=v \
    dalvik.vm.checkjni=false \
    dalvik.vm.heapgrowthlimit=80m \
    dalvik.vm.heapminfree=512k \
    ro.sf.hwrotation=270 \
    ro.sf.hdmi_rotation=0 \
    ro.sf.default_rotation=1 \
    ro.camerahal.configorientation=90 \
    ro.camerahal.prevres0=SVGA,HD \
    ro.camerahal.imageres0=SVGA,2M \
    ro.camerahal.prevresdft0=SVGA \
    ro.camerahal.imageresdft0=2M \
    ro.camerahal.fpsdft0=30 \
    ro.camerahal.prevres1=QVGA,VGA \
    ro.camerahal.imageres1=QVGA,VGA \
    ro.camerahal.prevresdft1=VGA \
    ro.camerahal.imageresdft1=VGA \
    ro.camerahal.fpsdft1=30

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

PRODUCT_AAPT_CONFIG := large xlarge mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_TAGS += dalvik.gc.type-precise

$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
