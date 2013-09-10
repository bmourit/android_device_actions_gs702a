LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/tvout_display_test.cpp \
	src/DisplayParameters.cpp \
	main.cpp 
	
base := $(LOCAL_PATH)/../../frameworks
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc \
	$(base)/include
	
LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libnativehelper \
	libcutils \
	libutils \
	libbinder \
	libhardware

#-----     ����������һ������� AVOID_COMPILE_UNSTABLE_CODE 	
#LOCAL_CFLAGS:=-DAVOID_COMPILE_UNSTABLE_CODE

#---  ���Զ���һЩ����c++��ص�option�����LOCAL_CPPFLAGS��������LOCAL_CFLAGS�ظ���option
# LOCAL_CPPFLAGS�е����ûḲ��LOCAL_CFLAGS
#LOCAL_CPPFLAGS:= 
#-- ����c++�ļ��ĺ�׺������.cpp
LOCAL_CPP_EXTENSION :=.cpp

# ָ��������ld�Ĳ���
LOCAL_LDFLAGS :=
#ָ����������ӿ⣬��LOCAL_LDLIBS += -lcurses -lpthread	
LOCAL_LDLIBS := 

#ָ��c++������������,arm-linux-android-androideabi-gcc��Ĭ�ϵı�����
#LOCAL_CXX :=$(TOP)/prebuilt/linux-x86/toolchain/arm-linux-androideabi-4.4.x/bin/arm-linux-androideabi-gcc
#LOCAL_CXX :=$(TOP)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc

	
LOCAL_MODULE:= test_tvout_hal
LOCAL_MODULE_TAGS := tests
#---------����rootfs�е�һЩ��ִ���ļ�����Ҫʱ��̬���ӵģ�������������ɾ�̬����
#��Ҫͬʱ�� LOCAL_FORCE_STATIC_EXECUTABLE, ��LOCAL_STATIC_LIBRARIES
#LOCAL_FORCE_STATIC_EXECUTABLE:=true
#LOCAL_STATIC_LIBRARIES :=  libc libutils


#Ĭ������£�test_tvout_display��װ��system/binĿ¼�£�����ϣ��system��Ŀ¼��
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data
LOCAL_PRELINK_MODULE := false
include $(BUILD_EXECUTABLE)
