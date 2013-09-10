#include <cutils/log.h>
#include <linux/fb.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <cutils/properties.h>
#include <assert.h>

#include "cvbs.h"
#include "tvout_hal_private.h"

/** State information for each device instance */
struct cvbs_context_t {
	struct cvbs_device_t device;
	int mFD;
};

static int open_cvbs(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static struct hw_module_methods_t cvbs_module_methods =
{
	open : open_cvbs
};

/*
 * The COPYBIT Module
 */
struct cvbs_module_t HAL_MODULE_INFO_SYM =
{
	common :
	{
		tag : HARDWARE_MODULE_TAG,
		version_major : 1,
		version_minor : 0,
		id : CVBS_HARDWARE_MODULE_ID,
		name : "Action tvout Module",
		author : "Action-Semi co.ltd",
		methods : &cvbs_module_methods,
		dso: 0,
		reserved:
		{	0,}
	}
};

/**
 * cvbs ≤Â∞ŒœﬂºÏ≤‚
 * @return 1: œﬂ¿¬≤Â»Î£¨0,Œ¥≤Â»Î, <0£¨ ß∞‹
 *
 */
static int cvbs_is_connected(struct cvbs_device_t *dev) {
	int cmd;
	fd_set fds;
	unsigned int val;
	int status = -EINVAL;

	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;

	/****   todo:  hdmi drvier not ready , so just failed ***/
	return -1;

	if (!ctx)
		return status;

	/* check whether any cvbs  plugged */
	status = ioctl(ctx->mFD, CVBS_GET_STATUS, &val);
	if (status < 0) {
		ALOGE("ioctl   GET_STATUS error!\n");
		return -1;
	}
	return (int) val;
}

static int cvbs_enable(struct cvbs_device_t *dev) {
	int status = -EINVAL;
	unsigned int val = 1; // enable
	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;
	if (!ctx)
		return status;
	// enable output
	status = ioctl(ctx->mFD, CVBS_SET_ENABLE, &val);
	return status;

}

static int cvbs_disable(struct cvbs_device_t *dev) {
	int status = -EINVAL;
	unsigned int val = 0; // enable
	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;
	if (!ctx)
		return status;
	// enable output
	status = ioctl(ctx->mFD, CVBS_SET_ENABLE, &val);
	return status;
}

static int cvbs_setmode(struct cvbs_device_t *dev, int mode) {
	ALOGD("enter cvbs_setmode!mode:%d\n ", mode);
	int ret_val = 0;
	struct tv_settings tv_set;
	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;

	if (mode == HAL_TVOUT_PAL) {
		tv_set.tv_mode = TV_MODE_PAL;
	} else {
		tv_set.tv_mode = TV_MODE_NTSC;
	}

	ret_val = ioctl(ctx->mFD, CVBS_SET_CONFIGURE, &tv_set);
	if (ret_val != 0) {
		ALOGE("configure TVOUT device error!\n ");
	}

	return ret_val;

}

static int cvbs_getmode(struct cvbs_device_t *dev) {
	ALOGD("enter cvbs_getmode!\n ");
	int ret_val = 0;
	struct tv_settings tv_set;
	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;

	ret_val = ioctl(ctx->mFD, CVBS_GET_CONFIGURE, &tv_set);
	if (ret_val != 0) {
		ALOGE("configure TVOUT device error!\n ");
	}

	return tv_set.tv_mode;

}

static int close_cvbs(struct hw_device_t* dev) {
	struct cvbs_context_t* ctx = (struct cvbs_context_t*) dev;
	if (ctx) {
		close(ctx->mFD);
		free(ctx);
	}
	return 0;
}

static int open_cvbs(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device) {
	int status = -EINVAL;
	char value[1024];

	struct cvbs_context_t *ctx = (struct cvbs_context_t *) malloc(
			sizeof(struct cvbs_context_t));
	memset(ctx, 0, sizeof(*ctx));
	ALOGD("[enter open_cvbs]began open tvout");

	ctx->device.common.tag = HARDWARE_DEVICE_TAG;
	ctx->device.common.version = 0;
	ctx->device.common.module = (struct hw_module_t*) module;
	ctx->device.common.close = close_cvbs;

	ctx->device.enable = cvbs_enable;
	ctx->device.disable = cvbs_disable;
	ctx->device.is_connected = cvbs_is_connected;
	ctx->device.set_mode = cvbs_setmode;
	ctx->device.get_mode = cvbs_getmode;

	ctx->mFD = open(CVBS_HARDWARE_DEVICE, O_RDWR);
	ALOGD("[enter open_cvbs]  ctx->mFD:%d\n", ctx->mFD);

	if (ctx->mFD < 0) {
		status = errno;
		ALOGE("Error opening tvout fd=%d errno=%d (%s)", ctx->mFD, status,
				strerror(status));
		status = -status;
		*device = NULL;
	} else {
		status = 0;
	}

	ALOGD("  status=%d ", status);

	if (status == 0) {
		*device = &ctx->device.common;
		ALOGI("open tvout successfully! fd: %d", ctx->mFD);
	} else {
		close_cvbs(&ctx->device.common);
		*device = NULL;
		ALOGE("open tvout failed! %d %s", status, strerror(-status));
	}
	return status;
}

