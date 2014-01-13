# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

PRODUCT_PROPERTY_OVERRIDES := \
        net.dns1=8.8.8.8 \
        net.dns2=8.8.4.4

# Inherit from those products. Most specific first.
$(call inherit-product, build/target/product/aosp_base.mk)
$(call inherit-product, device/actions/gs702a/device.mk)
$(call inherit-product-if-exists, device/actions/gs702a/prebuilt/codec/actcodec.mk)

TARGET_SCREEN_HEIGHT := 800
TARGET_SCREEN_WIDTH := 1280

PRODUCT_AAPT_CONFIG += large xlarge mdpi hdpi tvdpi
PRODUCT_AAPT_PREF_CONFIG := xlarge

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_TAGS += dalvik.gc.type-precise	

# Set variables here to overwrite the inherited values.
PRODUCT_NAME := full_gs702a
PRODUCT_DEVICE := gs702a
PRODUCT_BRAND := Android
PRODUCT_MODEL := Novo 10 Hero QuadCore
PRODUCT_MANUFACTURER := ainol
