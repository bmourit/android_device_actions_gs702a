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

ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := $(KERNEL_OUT)/arch/arm/boot/Image
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES := \
	device/actions/gs702a/rootdir/init.gs702a.rc:root/init.gs702a.rc \
	#device/actions/gs702a/rootdir/init.gs702a.sdboot.rc:root/init.gs702a.sdboot.rc \
	device/actions/gs702a/rootdir/fstab.gs702a:root/fstab.gs702a \
	#device/actions/gs702a/rootdir/fstab.sdboot.gs702a:root/fstab.sdboot.gs702a \
	device/actions/gs702a/rootdir/init.quickboot.rc:root/init.quickboot.rc \
	device/actions/gs702a/rootdir/init.gs702a.usb.rc:root/init.gs702a.usb.rc \
	device/actions/gs702a/rootdir/ueventd.gs702a.rc:root/ueventd.gs702a.rc \
	device/actions/gs702a/rootdir/fstab.gs702a:system/etc/fstab.gs702a \

PRODUCT_COPY_FILES += \
    device/actions/gs702a/configs/apns-conf.xml:system/etc/apns-conf.xml \
	device/actions/gs702a/configs/ft5x0x_ts.idc:system/usr/idc/ft5x0x_ts.idc \
	device/actions/gs702a/configs/GT813.idc:system/usr/idc/GT813.idc \
	device/actions/gs702a/configs/mt395.idc:system/usr/idc/mt395.idc \
	device/actions/gs702a/configs/atc260x-adckeypad.kl:system/usr/keylayout/atc260x-adckeypad.kl \

PRODUCT_COPY_FILES += \
	device/actions/gs702a/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml \
	device/actions/gs702a/configs/packages-compat-default.xml:system/etc/packages-compat-default.xml \
	device/actions/gs702a/configs/usbmond:system/bin/usbmond \
	device/actions/gs702a/configs/usb_modeswitch:system/xbin/usb_modeswitch \
	device/actions/gs702a/configs/boost.sh:system/bin/boost.sh \
	device/actions/gs702a/configs/init.d/80booster:system/etc/init.d/80booster \
	device/actions/gs702a/configs/init.d/90userinit:system/etc/init.d/90userinit \
	device/actions/gs702a/configs/builtinapk:system/etc/builtinapk \
	device/actions/gs702a/configs/NOTICE.html:system/etc/NOTICE.html \
	device/actions/gs702a/rootdir/lib/modules/blk1drv.ko:recovery/root/lib/modules/blk1drv.ko \
	device/actions/gs702a/rootdir/lib/modules/blk2drv.ko:recovery/root/lib/modules/blk2drv.ko \
	device/actions/gs702a/rootdir/lib/modules/ctp_ft5x06.ko:recovery/root/lib/modules/ctp_ft5x06.ko \

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:system/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:system/etc/permissions/android.hardware.audio.low_latency.xml \
	hardware/libhardware_legacy/audio/audio_policy.conf:/system/etc/audio_policy.conf \
	device/actions/gs702a/readme:system/vendor/app/readme

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES := \
	persist.sys.usb.config=adb,mass_storage
	
PRODUCT_PROPERTY_OVERRIDES := \
	wifi.interface=wlan0 \
    ro.sf.lcd_density=160 \
	hwui.render_dirty_regions=false \
    ro.hwui.texture_cache_size=72 \
    ro.hwui.layer_cache_size=48 \
    ro.hwui.r_buffer_cache_size=8 \
    ro.hwui.path_cache_size=32 \
    ro.hwui.gradient_cache_size=1 \
    ro.hwui.drop_shadow_cache_size=6 \
    ro.hwui.texture_cache_flushrate=0.4 \
    ro.hwui.text_small_cache_width=1024 \
    ro.hwui.text_small_cache_height=1024 \
    ro.hwui.text_large_cache_width=2048 \
    ro.hwui.text_large_cache_height=1024 \
    ro.hwui.disable_scissor_opt=true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.product.locale.language=en \
	ro.product.locale.region=US

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dexopt-data-only=1 \
	dalvik.vm.dexopt-flags=v=n,o=v \
	dalvik.vm.checkjni=false \
	dalvik.vm.heapgrowthlimit=80m

PRODUCT_PROPERTY_OVERRIDES += \
	ro.secure=0	\
	ro.debuggable=1 \
	ro.build.selinux=0 \
	ro.setupwizard.mode=DISABLE \
	persist.sys.timezone=America/Los_Angeles \
	persist.sys.strictmode.disable=true \
	wifi.supplicant_scan_interval=120 \
	ro.bq.gpu_to_cpu_unsupported=1 \

PRODUCT_PROPERTY_OVERRIDES += \
	ro.support.gpswithwifi=1

DEVICE_PACKAGE_OVERLAYS := \
    device/actions/gs702a/overlay

PRODUCT_PACKAGES := \
    make_ext4fs \
    librs_jni \
	com.android.future.usb.accessory

# Ramdisk
PRODUCT_PACKAGES += \
	fstab.gs702a \
	init.gs702a.rc \
	init.rc \
	init.wifi.rc \
	init.usb.rc \
	init.gs702a.usb.rc \
	init.modules.rc \
	init.extra_modules.rc \
	init.quickboot.rc \
	ueventd.gs702a.rc

PRODUCT_PACKAGES += \
	actions \
	pfmnceserver \
	libperformance \
	libactions_runtime \
	usbmond \
	charger \
	charger_res_images \
	SpeechRecorder \
	libsrec_jni

PRODUCT_PACKAGES += \
	updater \
	e2fsck \
	ping \
	netperf \
	netserver \
	tcpdump \
	wpa_cli \
	strace \
	powerdebug

PRODUCT_PACKAGES += \
	dosfslabel \
	iostat

# New Power HAL
PRODUCT_PACKAGES += \
	power.ATM702X

PRODUCT_PACKAGES += \
	rild \
	libril \
	libactions-ril \
	usb_modeswitch \
	libusb \
	libusb-compat \
	usbmond

PRODUCT_PACKAGES += \
	AdobeFlashPlayer.apk \
	libflashplayer.so \
	libstagefright_froyo.so \
	libstagefright_honeycomb.so \
	libysshared.so \
	libffmpeg_wrapper \
	libPopupVideo \
	libvinit \
	performancepolicy \
	ActSensorCalib

PRODUCT_PACKAGES += \
    com.actions.hardware.xml \
	init.superuser.rc

PRODUCT_PACKAGES += \
	libGAL \
	libGLSLC \
	gpuhwcomposer.ATM702X.so \
	libGLESv1_CM_VIVANTE.so \
	libEGL_VIVANTE.so \
	gpu_config \
	game_r2 \
	game_r3 \
	libGLESv2_VIVANTE.so
	
PRODUCT_PACKAGES += \
	libsub \
	libbmp \
	id_jpg\
	adAAC \
	adAC3 \
	adACELP \
	adAMR \
	adAPE \
	adAWB \
	adCOOK \
	adDTS \
	adFLAC \
	adMP3 \
	adOGG \
	adPCM \
	adWMALSL \
	adWMAPRO \
	adWMASTD \
	aeMP3 \
	aeWAV \
	apAAC \
	apAC3 \
	apAMR \
	apAPE \
	apDTS \
	apFLAC \
	apMP3 \
	apOGG \
	apRMA \
	apWAV \
	apWMA \
	avd_avi \
	avd_flv \
	avd_mkv \
	avd_mp4 \
	avd_mpg \
	avd_ogm \
	avd_rm \
	avd_ts \
	avd_wmv \
	vd_flv1 \
	vd_h263 \
	vd_h264 \
	vd_mjpg \
	vd_mpeg \
	vd_msm4 \
	vd_rv34 \
	vd_rvg2 \
	vd_vc1 \
	vd_vp6 \
	vd_vp8 \
	vd_xvid \
	libACT_ISP \
	libACT_FD \
	libvde_core \
	libbacklight \
	libimg_en

PRODUCT_PACKAGES += \
	libOMX_Core \
	gralloc.ATM702X \
	cvbs.ATM702X \
	displayengine.ATM702X \
	hdmi.ATM702X \
	hwcomposer.ATM702X \
	overlay.ATM702X \
	lights.ATM702X \
	sensors.ATM702X \
	libomxBellagio_base \
	libOMX.Action.Audio.Decoder \
	libOMX.Action.Video.Camera \
	libACT_V4L2HAL \
	libOMX.Action.Image.Decoder \
	libOMX.Action.Video.Encoder \
	libACT_EncAPI \
	libOMX.Action.Video.Decoder \
	libstagefrighthw \
	camera.ATM702X \
    audio.r_submix.default

PRODUCT_PACKAGES += \
    libvideoeditor_jni \
    libvideoeditor_core \
    libvideoeditor_osal \
    libvideoeditor_videofilters \
    libvideoeditorplayer

PRODUCT_PACKAGES += \
	dhcpcd.conf \
	init.wifi.rc \
	lib_driver_cmd_rtl \
	hostapd \
	hostapd_cli \
	wpa_cli \
	libwpa_client \
	wpa_supplicant \
	wpa_supplicant.conf

PRODUCT_CHARACTERISTICS := tablet

$(call inherit-product, device/actions/gs702a/usb_modeswitch.d/usb_modeswitch.mk)
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)
