#include <cutils/log.h>

#include <linux/fb.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>
#include "hdmi.h"
#include "hdmi_tx_drv.h"

#define ASPECT_RATIO_4VS3  0
#define ASPECT_RATIO_16VS9  1

typedef struct video_mode_info_table {
	int width;
	int height;
	float refreshrate;
	int isprogressive;
	int aspect_ratio;
	char para[30];
	int vid_index;
} video_mode_info_table_t;

/** keep sync with VIDEO_ID_TABLE in hdmi_tx_drv.h */
#if 0
static video_mode_info_table_t all_video_modes[] = {
    { 640, 480, 60, 1, ASPECT_RATIO_4VS3, "640x480p_60hz_4:3", 1 },
    { 720, 480, 60, 1, ASPECT_RATIO_4VS3, "720x480p_60hz_4:3", 2 },
    { 720, 480, 60, 1, ASPECT_RATIO_16VS9, "720x480p_60hz_16:9", 3 },
    { 1280, 720, 60, 1, ASPECT_RATIO_16VS9, "1280x720p_60hz_16:9", 4 },
    { 1920, 1080, 60, 0, ASPECT_RATIO_16VS9, "1920x1080i_60hz_16:9",5 },
    { 720, 480, 60, 0, ASPECT_RATIO_4VS3, "720x480i_60hz_4:3", 6 },
    { 720, 480, 60, 0, ASPECT_RATIO_16VS9, "720x480i_60_16:9", 7 },
    { 1440, 480, 60, 1, ASPECT_RATIO_4VS3, "1440x480p_60hz_4:3", 14 },
    { 1440, 480, 60, 1, ASPECT_RATIO_16VS9, "1440x480p_60hz_16:9",15 },
    { 1920, 1080, 60, 1, ASPECT_RATIO_16VS9, "1920x1080p_60hz_16:9",16 },
    { 720, 576, 50, 1, ASPECT_RATIO_4VS3, "720x576p_50hz_4:3", 17 },
    { 720, 576, 50, 1, ASPECT_RATIO_16VS9, "720x576p_50hz_16:9", 18 },
    { 1280, 720, 50, 1, ASPECT_RATIO_16VS9, "1280x720p_50hz_16:9",19 },
    { 1920, 1080, 60, 0, ASPECT_RATIO_16VS9, "1920x1080i_50hz_16:9",20 },
    { 720, 576, 50, 0, ASPECT_RATIO_4VS3, "720x576i_50hz_4:3", 21 },
    { 720, 576, 50, 0, ASPECT_RATIO_16VS9, "720x576i_50hz_16:9", 22 },
    { 1440, 576, 50, 1, ASPECT_RATIO_4VS3, "1440x576p_50hz_4:3", 29 },
    { 1440, 576, 50, 1, ASPECT_RATIO_16VS9, "1440x576p_50hz_16:9",30 }, 
    { 1920, 1080, 50, 1, ASPECT_RATIO_16VS9,"1920x1080p_50hz_16:9", 31 }, 
    { 1920, 1080, 24, 1,ASPECT_RATIO_16VS9, "1920x1080p_24hz_16:9", 32 }, 
    {1920, 1080, 25, 1, ASPECT_RATIO_16VS9,"1920x1080p_25hz_16:9", 33 }, 
    { 1920, 1080, 30, 1,ASPECT_RATIO_16VS9, "1920x1080p_30hz_16:9", 34 },
//{ 720, 480, 59.94, 1, ASPECT_RATIO_4VS3, "720x480p_59.94hz_4:3", 72 },
//{ 720, 480, 59.94, 1, ASPECT_RATIO_16VS9, "720x480p_59.94hz_16:9", 73 },
    { 1280, 720, 59.94, 1, ASPECT_RATIO_16VS9,"1280X720p_59.94hz_16:9", 74 },
//{ 1920, 1080, 59.94, 0, ASPECT_RATIO_16VS9, "1920x1080i_59.94hz_16:9", 75 },
//{ 720, 480, 59.94, 0, ASPECT_RATIO_4VS3, "720x480i_59.54hz_4:3", 76 },
//{ 720, 480, 59.94, 0, ASPECT_RATIO_16VS9, "720x480i_59.54hz_16:9", 77 },
    { 1920, 1080, 59.94, 1, ASPECT_RATIO_16VS9,"1920x1080p_59.94hz_16:9", 86 },
//{ 1920, 1080, 29.97, 1, ASPECT_RATIO_16VS9, "1920x1080p_29.97hz_16:9", 104 },
};
#else
/** keep sync with VIDEO_ID_TABLE in hdmi_tx_drv.h */
static video_mode_info_table_t all_video_modes[] = {
 
    { 1920, 1080, 60, 1, ASPECT_RATIO_16VS9, "1920x1080p_60hz_16:9",16 },
    { 1920, 1080, 50, 1, ASPECT_RATIO_16VS9,"1920x1080p_50hz_16:9", 31 }, 
    { 1280, 720, 60, 1, ASPECT_RATIO_16VS9, "1280x720p_60hz_16:9", 4 },
    { 1280, 720, 50, 1, ASPECT_RATIO_16VS9, "1280x720p_50hz_16:9",19 },
    
    { 1920, 1080, 59.94, 1, ASPECT_RATIO_16VS9,"1920x1080p_59.94hz_16:9", 86 },
    { 1920, 1080, 30, 1,ASPECT_RATIO_16VS9, "1920x1080p_30hz_16:9", 34 },
    {1920, 1080, 25, 1, ASPECT_RATIO_16VS9,"1920x1080p_25hz_16:9", 33 }, 
    { 1920, 1080, 24, 1,ASPECT_RATIO_16VS9, "1920x1080p_24hz_16:9", 32 }, 
    { 1920, 1080, 60, 0, ASPECT_RATIO_16VS9, "1920x1080i_60hz_16:9",5 },
    { 1920, 1080, 60, 0, ASPECT_RATIO_16VS9, "1920x1080i_50hz_16:9",20 },
    { 1440, 576, 50, 1, ASPECT_RATIO_16VS9, "1440x576p_50hz_16:9",30 }, 
    { 1440, 576, 50, 1, ASPECT_RATIO_4VS3, "1440x576p_50hz_4:3", 29 },
    { 1440, 480, 60, 1, ASPECT_RATIO_16VS9, "1440x480p_60hz_16:9",15 },
    { 1440, 480, 60, 1, ASPECT_RATIO_4VS3, "1440x480p_60hz_4:3", 14 },
    { 1440, 480, 60, 0, ASPECT_RATIO_4VS3, "1440x480i_60hz_4:3", 6 },//same as 720x480i_60hz_4:3
    { 1440, 576, 50, 0, ASPECT_RATIO_4VS3, "1440x576i_50hz_4:3", 21 },//same as 720x576i_50hz_4:3
    { 1280, 720, 59.94, 1, ASPECT_RATIO_16VS9,"1280X720p_59.94hz_16:9", 74 },
    { 720, 576, 50, 1, ASPECT_RATIO_16VS9, "720x576p_50hz_16:9", 18 },
    { 720, 576, 50, 1, ASPECT_RATIO_4VS3, "720x576p_50hz_4:3", 17 },
    { 720, 576, 50, 0, ASPECT_RATIO_16VS9, "720x576i_50hz_16:9", 22 },
    { 720, 480, 60, 1, ASPECT_RATIO_16VS9, "720x480p_60hz_16:9", 3 },
    { 720, 480, 60, 1, ASPECT_RATIO_4VS3, "720x480p_60hz_4:3", 2 },
    { 640, 480, 60, 1, ASPECT_RATIO_4VS3, "640x480p_60hz_4:3", 1 },
};
#endif

/** State information for each device instance */
struct hdmi_context_t {
	struct hdmi_device_t device;
	int mFD;
};

static int open_hdmi(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static struct hw_module_methods_t hdmi_module_methods =
{
	open: open_hdmi,
};

/*
 * The hdmi Module
 */
struct hdmi_module_t HAL_MODULE_INFO_SYM =
{
	common :
	{
		tag : HARDWARE_MODULE_TAG,
		version_major : 1,
		version_minor : 0,
		id : HDMI_HARDWARE_MODULE_ID,
		name : "actions  HDMI Module",
		author : "Action-Semi, Co.Ltd",
		methods : &hdmi_module_methods,
		dso:0,
		reserved:
		{	0,}
	}
};

static int find_video_index(int id) {
	int index;
	int size = sizeof(all_video_modes) / sizeof(video_mode_info_table_t);

	for (index = 0; index <= size; index++) {
		if (id == all_video_modes[index].vid_index)
			return index;
		else
			continue;
	}

	return -1;
}

/**
 *
 * convert hdmi output parameter to video_ids of hdmi driver
 *
 */
static int _map_hdmi_param_to_drv_id(int width, int height, float refreshrate,
		int isprogressive, int aspect_ratio) {

	for (unsigned int i = 0;
			i < sizeof(all_video_modes) / sizeof(video_mode_info_table_t);
			i++) {

		if (width != all_video_modes[i].width
				|| height != all_video_modes[i].height) {
			continue;
		}
		if (refreshrate >= 0 && refreshrate != all_video_modes[i].refreshrate) {
			continue;
		}
		if (isprogressive >= 0
				&& isprogressive != all_video_modes[i].isprogressive) {
			continue;
		}
		if (aspect_ratio >= 0
				&& aspect_ratio != all_video_modes[i].aspect_ratio) {
			continue;
		}

		return all_video_modes[i].vid_index;

	}

	return -1;
}

static int hdmi_is_connected(struct hdmi_device_t *dev) {
	unsigned int val = 0;
	int status = -EINVAL;

	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;

	if (!ctx)
		return status;
	/* check whether any HDMIdevice plugged */
	status = ioctl(ctx->mFD, HDMI_GET_LINK_STATUS, &val);
	if (status < 0) {
		ALOGE("HDMI_GET_LINK_STATUS  ioctl   GET_STATUS error!\n");
		return -1;
	}
	return val;
}

static int hdmi_enable(struct hdmi_device_t *dev) {
	int status = -EINVAL;
	int val = 1; // enable
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (ctx)
		status = ioctl(ctx->mFD, HDMI_SET_ENABLE, &val);
	return status;
}

static int hdmi_disable(struct hdmi_device_t *dev) {
	int status = -EINVAL;
	int val = 0;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (ctx)
		status = ioctl(ctx->mFD, HDMI_SET_ENABLE, &val);
	return status;
}

static int hdmi_set_mode(struct hdmi_device_t *dev, int width, int height,
		float freshrate, int isprogressive, int aspect_ratio) {
	ALOGD(" enter hdmi_set_mode\n");
	int vid;
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	vid = _map_hdmi_param_to_drv_id(width, height, freshrate, isprogressive,
			aspect_ratio);
	if (vid < 0) {
		return status;
	}
	status = ioctl(ctx->mFD, HDMI_SET_VIDEO_CONFIG, &vid);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_VIDEO_CONFIG error! err=%s", strerror(errno));
	}

	return status;
}

static int hdmi_switch_set_status(struct hdmi_device_t *dev, int state) {
	ALOGD(" enter hdmi_switch_set_state\n");
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	if (state < 0) {
		return status;
	}
	status = ioctl(ctx->mFD, HDMI_SWITCH_SET_STATUS, &state);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SWITCH_SET_STATUS error! err=%s", strerror(errno));
	}

	return status;
}

static bool hdmi_get_disconnect_flag(struct hdmi_device_t *dev) {
	ALOGD(" enter hdmi_set_disconnect_flag\n");
	int status = -EINVAL;
	bool flag;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_GET_DICONNECT_FLAG, &flag);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_DICONNECT_FLAG error! err=%s", strerror(errno));
	}

	return flag;
}

static int hdmi_set_disconnect_flag(struct hdmi_device_t *dev,bool flag) {
	ALOGD(" enter hdmi_set_disconnect_flag\n");
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_SET_DICONNECT_FLAG, &flag);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_DICONNECT_FLAG error! err=%s", strerror(errno));
	}

	return status;
}

static int hdmi_set_manual_flag(struct hdmi_device_t *dev,bool flag) {
	ALOGD(" enter hdmi_set_manual_flag\n");
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_SET_MUNUAL_FLAG, &flag);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_MANUAL_FLAG error! err=%s", strerror(errno));
	}

	return status;
}

static int hdmi_set_hdcp_switch(struct hdmi_device_t *dev,bool flag) {
	ALOGD(" enter hdmi_set_hdcp_switch\n");
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_SET_HDCP_AUTHENTICATION, &flag);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_HDCP_AUTHENTICATION error! err=%s", strerror(errno));
	}

	return status;
}

static int hdmi_set_vid(struct hdmi_device_t *dev, int vid) {
	ALOGD(" enter hdmi_set_mode\n");
	int status = -EINVAL;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	if (vid < 0) {
		return status;
	}
	status = ioctl(ctx->mFD, HDMI_SET_VIDEO_CONFIG, &vid);
	if (status < 0) {
		ALOGE(" ioctl HDMI_SET_VIDEO_CONFIG error! err=%s", strerror(errno));
	}

	return status;
}

static int hdmi_get_vid(struct hdmi_device_t *dev) {
	ALOGD(" enter hdmi_set_mode\n");
	int status = -EINVAL;
	int vid;
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_GET_VIDEO_CONFIG, &vid);
	if (status < 0) {
		ALOGE(" ioctl HDMI_GET_VIDEO_CONFIG error! err=%s", strerror(errno));
	}

	return vid;
}
#if 0
static int hdmi_get_capbility(struct hdmi_device_t *dev, char *buf) {
	ALOGD(" enter hdmi_get_capbility\n");
	int status = -EINVAL;
	int i, j;
	int num;
	int offset = 0;
	int index;

	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
	if (!ctx) {
		return -EINVAL;
	}

	status = ioctl(ctx->mFD, HDMI_GET_SINK_CAPABILITY, &ctx->device.sink_cap);
	if (status < 0) {
		ALOGE(" ioctl HDMI_GET_SINK_CAPABILITY error! err=%s", strerror(errno));
		return status;
	}

	num = sizeof(ctx->device.sink_cap.video_formats) * 8;
	for (i = 0, j = 0; i < num; i++) {
		if ((i % 32 == 0) && i)
			j++;

		if (ctx->device.sink_cap.video_formats[j]
				& (1 << (i % (sizeof(int) * 8)))) {
			index = find_video_index(i + 1);
			if (index >= 0) {
				offset += sprintf(buf + offset, "%s,%d;",
						all_video_modes[index].para,
						all_video_modes[index].vid_index);
			} else
				continue;
		}
	}

	return status;
}
#else
static int hdmi_get_capbility(struct hdmi_device_t *dev, char *buf) {
    ALOGD(" enter hdmi_get_capbility\n");
    int status = -EINVAL;
    int i, j, k;
    int num;
    
    int offset = 0;
    int index;

    struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;
    if (!ctx) 
        return -EINVAL;
	
    status = ioctl(ctx->mFD, HDMI_GET_SINK_CAPABILITY, &ctx->device.sink_cap);
    if (status < 0) {
        ALOGE(" ioctl HDMI_GET_SINK_CAPABILITY error! err=%s", strerror(errno));
        return status;
    }
    
    num = sizeof(all_video_modes) / sizeof( video_mode_info_table_t);
    for (i = 0; i < num; i++) {
        j = (all_video_modes[i].vid_index - 1)  / 32;
        k = (all_video_modes[i].vid_index - 1)  % 32;
        if (ctx->device.sink_cap.video_formats[j]  & (1 << k))
        	offset += sprintf(buf + offset, "%s,%d;",all_video_modes[i].para,
        	    all_video_modes[i].vid_index);
        	
    }
    
    return status;
}

#endif

static int close_hdmi(struct hw_device_t* dev) {
	struct hdmi_context_t* ctx = (struct hdmi_context_t*) dev;

	if (ctx) {
		close(ctx->mFD);
		free(ctx);
	}
	return 0;
}

static int hdmi_resolve_config(struct hdmi_device_t *dev, /* write back to Hdmi */
video_setting_t *vid_setting) {
	return 0;

}
static int open_hdmi(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device) {
	int status = -EINVAL;
	struct hdmi_context_t *ctx = (struct hdmi_context_t *) malloc(
			sizeof(struct hdmi_context_t));
	memset(ctx, 0, sizeof(*ctx));

	ctx->device.common.tag = HARDWARE_DEVICE_TAG;
	ctx->device.common.version = 0;
	ctx->device.common.module = (struct hw_module_t*) module;
	ctx->device.common.close = close_hdmi;

	ctx->device.enable = hdmi_enable;
	ctx->device.disable = hdmi_disable;

	ctx->device.is_connected = hdmi_is_connected;
	ctx->device.resolve_config = hdmi_resolve_config;
	ctx->device.set_mode = hdmi_set_mode;
	ctx->device.switch_set_status = hdmi_switch_set_status;
	ctx->device.get_disconnect_flag = hdmi_get_disconnect_flag;
	ctx->device.set_disconnect_flag = hdmi_set_disconnect_flag;
	ctx->device.set_manual_flag = hdmi_set_manual_flag;
	ctx->device.set_hdcp_switch = hdmi_set_hdcp_switch;
	ctx->device.set_vid = hdmi_set_vid;
	ctx->device.get_vid = hdmi_get_vid;

	ctx->device.get_capability = hdmi_get_capbility;

	ctx->mFD = open(HDMI_HARDWARE_DEVICE, O_RDWR);

	if (ctx->mFD < 0) {
		status = errno;
		ALOGE("Error opening hdmi fd=%d errno=%d (%s)", ctx->mFD, status,
				strerror(status));
		status = -status;
		*device = NULL;
	} else {
		status = 0;
	}

	if (status == 0) {
		*device = &ctx->device.common;
		ALOGI("open HDMI successfully! fd: %d", ctx->mFD);
	} else {
		close_hdmi(&ctx->device.common);
		*device = NULL;
		ALOGE("open HDMI failed! %d %s", status, strerror(-status));
	}
	return status;
}

