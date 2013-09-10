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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, device/actions/gs702a/gs702a.mk)
$(call inherit-product, device/actions/gs702a/wifi.mk)
$(call inherit-product, device/actions/gs702a/usb_modeswitch.d/usb_modeswitch.mk)
$(call inherit-product-if-exists, device/actions/gs702a/prebuilt/codec/actcodec.mk)	
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)

#$(call inherit-product, device/actions/gs702a/prebuilt/gpu/gpu.mk)

# Set variables here to overwrite the inherited values.
PRODUCT_NAME := full_gs702a
PRODUCT_DEVICE := gs702a
PRODUCT_BRAND := Android
PRODUCT_MODEL := Novo 10 Hero QuadCore
PRODUCT_MANUFACTURER := ainol
