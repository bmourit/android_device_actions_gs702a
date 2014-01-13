#
# Copyright (C) 2013 The Android Open Source Project
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

#ifneq ($(TARGET_BUILD_JAVA_SUPPORT_LEVEL),)

# This makefile shows how to build your own shared library that can be
# shipped on the system of a phone, and included additional examples of
# including JNI code with the library and writing client applications against it.

LOCAL_PATH := $(call my-dir)

# the library
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
			$(call all-java-files-under, core) \
			$(call all-java-files-under, services)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := actions

LOCAL_MODULE_CLASS := JAVA_LIBRARIES

# The AIDL component as part of java lib
# ============================================================
# Also build all of the sub-targets under this one: the library's
# associated AIDL code, and a client of the library.
LOCAL_AIDL_INCLUDES += $(LOCAL_PATH)/core/java

LOCAL_SRC_FILES += \
	core/java/com/actions/hardware/ICableStatusListener.aidl \
	core/java/com/actions/hardware/IDisplayService.aidl \
	core/java/com/actions/hardware/IPerformanceService.aidl

framework_built := $(call java-lib-deps,framework)
LOCAL_PRELINK_MODULE := false

include $(BUILD_JAVA_LIBRARY)

# the documentation
# ============================================================
#include $(CLEAR_VARS)

#LOCAL_SRC_FILES := $(call all-subdir-java-files) $(call all-subdir-html-files)

#LOCAL_MODULE:= hardware
#LOCAL_DROIDDOC_OPTIONS := actions
#LOCAL_MODULE_CLASS := JAVA_LIBRARIES
#LOCAL_DROIDDOC_USE_STANDARD_DOCLET := true

#include $(BUILD_DROIDDOC)

# The JNI component
# ============================================================
# Also build all of the sub-targets under this one: the library's
# associated JNI code, and a sample client of the library.
include $(CLEAR_VARS)

include $(call all-makefiles-under,$(LOCAL_PATH))

#endif # JAVA_SUPPORT
