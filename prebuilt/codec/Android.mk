LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libsub.so \
	libbmp.a \
	id_jpg.so

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

###########################################################################
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	adAAC.so \
	adAC3.so \
	adACELP.so \
	adAMR.so \
	adAPE.so \
	adAWB.so \
	adCOOK.so \
	adDTS.so \
	adFLAC.so \
	adMP3.so \
	adOGG.so \
	adPCM.so \
	adWMALSL.so \
	adWMAPRO.so \
	adWMASTD.so \
	aeMP3.so \
	aeWAV.so \
	apAAC.so \
	apAC3.so \
	apAMR.so \
	apAPE.so \
	apDTS.so \
	apFLAC.so \
	apMP3.so \
	apOGG.so \
	apRMA.so \
	apWAV.so \
	apWMA.so

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

###########################################################################
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	avd_avi.so \
	avd_flv.so \
	avd_mkv.so \
	avd_mp4.so \
	avd_mpg.so \
	avd_ogm.so \
	avd_rm.so \
	avd_ts.so \
	avd_wmv.so \
	vd_flv1.so \
	vd_h263.so \
	vd_h264.so \
	vd_mjpg.so \
	vd_mpeg.so \
	vd_msm4.so \
	vd_rv34.so \
	vd_rvg2.so \
	vd_vc1.so \
	vd_vp6.so \
	vd_vp8.so \
	vd_xvid.so \
	libACT_ISP.so \
	libACT_FD.so \
	libvde_core.so \
	libbacklight.so \
	libimg_en.so

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

###########################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := ALTestPlatform.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_PATH := $(TARGET_OUT_APPS)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_CERTIFICATE := PRESIGNED

include $(BUILD_PREBUILT)
