LOCAL_PATH := $(call my-dir)

PRODUCT_COPY_FILES += \
	device/actions/gs702a/prebuilt/codec/media_profiles.xml:system/etc/media_profiles.xml \
	device/actions/gs702a/prebuilt/codec/media_codecs.xml:system/etc/media_codecs.xml \
	device/actions/gs702a/prebuilt/codec/omx_codec.xml:system/etc/omx_codec.xml
