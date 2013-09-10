# Inherit device configuration
$(call inherit-product, device/actions/gs702a/full_gs702a.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit GSM common stuff
$(call inherit-product, vendor/cm/config/gsm.mk)


#Device identifier. This must come after all inclusions
PRODUCT_NAME := cm_gs702a
PRODUCT_BRAND := Android
PRODUCT_DEVICE := gs702a
PRODUCT_MODEL := Novo 10 Hero QuadCore
PRODUCT_MANUFACTURER := ainol

TARGET_SCREEN_HEIGHT := 800
TARGET_SCREEN_WIDTH := 1280
