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

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/kernel:kernel

PRODUCT_COPY_FILES := \
    device/actions/gs702a/apns-conf.xml:system/etc/apns-conf.xml \
	device/actions/gs702a/init.gs702a.rc:root/init.gs702a.rc \
	device/actions/gs702a/init.eth0.rc:root/init.eth0.rc \
	device/actions/gs702a/init.gs702a.sdboot.rc:root/init.gs702a.sdboot.rc \
	device/actions/gs702a/fstab.gs702a:root/fstab.gs702a \
	device/actions/gs702a/fstab.sdboot.gs702a:root/fstab.sdboot.gs702a \
	device/actions/gs702a/init.quickboot.rc:root/init.quickboot.rc \
	device/actions/gs702a/init.gs702a.usb.rc:root/init.gs702a.usb.rc \
	device/actions/gs702a/ueventd.gs702a.rc:root/ueventd.gs702a.rc \
	device/actions/gs702a/ft5x0x_ts.idc:system/usr/idc/ft5x0x_ts.idc \
	device/actions/gs702a/GT813.idc:system/usr/idc/GT813.idc \
	device/actions/gs702a/mt395.idc:system/usr/idc/mt395.idc \
	device/actions/gs702a/atc260x-adckeypad.kl:system/usr/keylayout/atc260x-adckeypad.kl \
	device/actions/gs702a/vold.fstab:system/etc/vold.fstab \
	device/actions/gs702a/vold.sdboot.fstab:system/etc/vold.sdboot.fstab \
	device/actions/gs702a/excluded-input-devices.xml:system/etc/excluded-input-devices.xml \
	device/actions/gs702a/packages-compat-default.xml:system/etc/packages-compat-default.xml \
	device/actions/gs702a/builtinapk:system/etc/builtinapk \
	device/actions/gs702a/NOTICE.html:system/etc/NOTICE.html \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/extras/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/extras/android.hardware.location.gps.xml \
	frameworks/native/data/etc/android.hardware.sensor.compass.xml:/system/etc/permissions/extras/android.hardware.sensor.compass.xml \
	frameworks/native/data/etc/tablet_core_hardware.xml:/system/etc/permissions/tablet_core_hardware.xml \
	build/target/product/security/platform.pk8:/system/etc/security/platform.pk8 \
	build/target/product/security/platform.x509.pem:/system/etc/security/platform.x509.pem

# audio policy configuration
PRODUCT_COPY_FILES += \
	hardware/libhardware_legacy/audio/audio_policy.conf:/system/etc/audio_policy.conf
	
# FIXME init.rc doesn't run mkdir for system/vendor/app, so we use do this here using a dummy file
PRODUCT_COPY_FILES += \
	device/actions/gs702a/readme:system/vendor/app/readme

PRODUCT_PACKAGES := \
    make_ext4fs \
	com.android.future.usb.accessory

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	persist.sys.usb.config=adb,mass_storage
	
PRODUCT_PROPERTY_OVERRIDES := \
	hwui.render_dirty_regions=false

PRODUCT_PACKAGES += \
	com.android.future.usb.accessory

PRODUCT_PROPERTY_OVERRIDES += \
	ro.product.locale.language=en \
	ro.product.locale.region=US

PRODUCT_PROPERTY_OVERRIDES += \
	dalvik.vm.dexopt-flags=v=n,o=v \
	dalvik.vm.checkjni=false \
	dalvik.vm.heapgrowthlimit=80m

PRODUCT_PROPERTY_OVERRIDES += \
	ro.secure=1	\
	ro.debuggable=1 \
	ro.setupwizard.mode=DISABLE \
	persist.sys.timezone=America/Los_Angeles \
	persist.sys.strictmode.disable=true \
	wifi.supplicant_scan_interval=120 \
	wifi.interface=wlan0

DEVICE_PACKAGE_OVERLAYS := \
    device/actions/gs702a/overlay

PRODUCT_PACKAGES += \
	e2fsck

PRODUCT_PACKAGES += \
	recovery \
	updater

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
	ping \
	netperf \
	netserver \
	tcpdump \
	wpa_cli \
	strace

PRODUCT_PACKAGES += \
	dosfslabel	
	
PRODUCT_PACKAGES += \
	rild \
	libactions-ril \
	actions-ril \
	usb_modeswitch \
	libusb \
	libusb-compat

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
	ActSensorCalib \
	DLNA.apk 

PRODUCT_PACKAGES += \
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

#wifi
ifeq ($(strip $(BOARD_WIFI_VENDOR)), mt5931)
PRODUCT_PACKAGES += \
	WIFI_RAM_CODE \
	hostapd \
	hostapd_cli \
	libwapi.so \
	wpa_cli \
	wpa_supplicant \
	wpa_supplicant.conf \
	p2p_supplicant.conf \
	libwpa_client \
	init.wifi.rc
endif	

ifeq ($(strip $(BOARD_WIFI_VENDOR)), mt6622)
PRODUCT_PACKAGES += \
	init.mt6622.rc \
	libbluetooth_mtk \
	libbt-vendor \
	MTK_MT6622_E2_Patch.nb0 \
	audio.a2dp.default
endif

ifeq ($(strip $(BOARD_BLUETOOTH_VENDOR)), ap6210)
PRODUCT_PACKAGES += \
	libbt-vendor \
	bt_vendor.conf \
	bcm20710a1.hcd \
	audio.a2dp.default
endif

ifeq ($(strip $(BOARD_BLUETOOTH_VENDOR)), mt6620)
PRODUCT_PACKAGES += \
	init.mt6620.rc \
	6620_launcher \
	WMT.cfg \
	mt6620_patch_e3_hdr.bin \
	audio.a2dp.default
endif

ifeq ($(strip $(BOARD_WIFI_VENDOR)), mt6620) 
PRODUCT_PACKAGES += \
	WIFI_RAM_CODE \
	hostapd \
	libwapi.so \
	wpa_cli \
	wpa_supplicant \
	wpa_supplicant.conf \
	p2p_supplicant.conf \
	libwpa_client \
	init.wifi.rc
endif

ifeq ($(strip $(BOARD_BLUETOOTH_VENDOR)), rtl8723)
PRODUCT_PACKAGES += \
	rtk8723_bt_config \
	rlt8723a_chip_b_cut_bt40_fw \
	audio.a2dp.default  \
	libbt-vendor
endif

ifeq ($(strip $(BOARD_WIFI_VENDOR)), rtl8723as_vt)
PRODUCT_PACKAGES += \
	dhcpcd.conf \
	init.wifi.rc \
	lib_driver_cmd_rtl \
	hostapd \
	hostapd_cli \
	wpa_cli \
	libwpa_client \
	wpa_supplicant
endif	

ifeq ($(strip $(BOARD_WIFI_VENDOR)), broadcom)
PRODUCT_PACKAGES += \
	fw_bcmdhd.bin \
	fw_bcmdhd_p2p.bin \
	fw_bcmdhd_apsta.bin \
	nvram.txt \
	init.wifi.rc
endif

ifeq ($(strip $(BOARD_WIFI_VENDOR)), nanoradio)
PRODUCT_PACKAGES += \
	wpa_supplicant.conf \
	hostapd.conf \
	dhcpcd.conf \
	init.wifi.rc \
	nrx600-sta.fw \
	nrx600-softap.fw \
	wlan_loader \
	lib_nano_cmd_nrx
endif	

ifeq ($(BOARD_WIFI_VENDOR), realtek)

PRODUCT_PACKAGES += \
	dhcpcd.conf \
	init.wifi.rc \
	lib_driver_cmd_rtl \
	hostapd \
	hostapd_cli \
	wpa_cli \
	libwpa_client \
	wpa_supplicant
endif	

$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)
$(call inherit-product, build/target/product/full_base.mk)

PRODUCT_NAME := full_gs702a
PRODUCT_DEVICE := gs702a

TARGET_SCREEN_HEIGHT := 800
TARGET_SCREEN_WIDTH := 1280

PRODUCT_AAPT_CONFIG += large xlarge mdpi hdpi tvdpi
PRODUCT_AAPT_PREF_CONFIG := xlarge

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_TAGS += dalvik.gc.type-precise
