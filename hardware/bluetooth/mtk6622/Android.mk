# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


# Copyright (C) 2008 The Android Open Source Project
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

# Configuration

LOCAL_PATH := $(call my-dir)


# applied to MT6622

BUILD_LAUNCHER  := true
BUILD_WMT_CFG := false
BUILD_PATCH := true
BUILD_INIT_CFG := true

ifeq ($(BUILD_LAUNCHER), true)
include $(CLEAR_VARS)
LOCAL_MODULE := libbluetooth_mtk
LOCAL_MODULE_TAGS := optional eng
LOCAL_SRC_FILES := \
  mtk.c \
  radiomgr.c \
  radiomod.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../external/bluetooth/bluedroid/hci/include

LOCAL_CFLAGS := \
  -DMTK_MT6622 -DBD_ADDR_AUTOGEN
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
endif



ifeq ($(BUILD_WMT_CFG), true)
include $(CLEAR_VARS)
LOCAL_MODULE := WMT.cfg
LOCAL_MODULE_TAGS := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif

ifeq ($(BUILD_INIT_CFG), true)
include $(CLEAR_VARS)
LOCAL_MODULE := init.mt6622.rc
LOCAL_MODULE_TAGS := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif


ifeq ($(BUILD_PATCH), true)
include $(CLEAR_VARS)
LOCAL_MODULE := MTK_MT6622_E2_Patch.nb0
LOCAL_MODULE_TAGS := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif

###########################################################################
# MTK BT DRIVER SOLUTION
###########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  bt_drv.c

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../../../../../external/bluetooth/bluedroid/hci/include

LOCAL_CFLAGS := 

LOCAL_MODULE_TAGS := optional eng
LOCAL_MODULE := libbt-vendor
LOCAL_SHARED_LIBRARIES := liblog libbluetooth_mtk
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)

