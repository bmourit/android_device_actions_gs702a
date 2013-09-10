/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/hardware.h>

#include <fcntl.h>
#include <errno.h>

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <linux/fb.h>

#include "de.h"
#include "de_drv.h"

#define FLAG_TVOUT_SIMULTANEOUS 0x1
#define FLAG_LCD_HAS_VIDEO 0x2
#define FLAG_MIN_BANDWIDTH 0x4

/*this macro should keep the same with DisplayManager.java*/
#define DISPLAY_MODE_LOW_BANDWIDTH (FLAG_MIN_BANDWIDTH)
#define DISPLAY_MODE_MEDIUM_BANDWIDTH (FLAG_MIN_BANDWIDTH | FLAG_LCD_HAS_VIDEO)
#define DISPLAY_MODE_HIGH_BANDWIDTH \
	(FLAG_TVOUT_SIMULTANEOUS | FLAG_LCD_HAS_VIDEO | FLAG_MIN_BANDWIDTH)

struct de_context_t {
	struct de_control_device_t device;

	int mFD; /* the framebuffer file descriptor */
	int mode; /* current playing mode */
	int width;
	int height;
	int scale_x;
	int scale_y;

	int vtv_ui_width;
	int vtv_ui_height;
	int tvout_width;
	int tvout_height;

};

/**
 * Common hardware methods
 */
static int open_display_engine(const struct hw_module_t* module,
		const char* name, struct hw_device_t** device);

static struct hw_module_methods_t de_module_methods = {
	open: open_display_engine,
};

/* The hal module for de */
/*****************************************************************************/

struct de_module_t HAL_MODULE_INFO_SYM =
{
	common:
	{
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0, id: DE_HARDWARE_MODULE_ID,
		name: "actions display manger",
		author: "ywwang",
		methods: &de_module_methods,
		dso:0,
		reserved:
		{	0,}
	},
};

/*set follows:
 hw.tvout.virtual_tv_xscale;
 hw.tvout.virtual_tv_yscale
 */
static void update_virtual_tv_scale_info(int xscale, int yscale) {
	int ret;
	char value[PROPERTY_VALUE_MAX];
	if (xscale <= 0 || yscale <= 0 || xscale > 100 || yscale > 100) {
		ALOGW("tvout param xscale/yscale fault: xscale=%d yscale=%d\n", xscale,
				yscale);
		return;
	}
	sprintf(value, "%d", xscale);
	ret = property_set("hw.tvout.virtual_tv_xscale", value);
	sprintf(value, "%d", yscale);
	ret = property_set("hw.tvout.virtual_tv_yscale", value);
	usleep(20000);

}

static void update_tvout_gap_info(int gap_top, int gap_bottom) {

	char value[PROPERTY_VALUE_MAX];
	sprintf(value, "%d", gap_top);
	property_set("hw.tvout.top_gap", value);

	sprintf(value, "%d", gap_bottom);
	property_set("hw.tvout.bottom_gap", value);

}

/**
 *
 * determine the ui scale mode: 2x-scale use de graphic layer or arbitary scale use
 *  video layer
 *
 */
#if 0
static void update_tvout_scale_mode(struct de_context_t *ctx) {

	char value[PROPERTY_VALUE_MAX];
	int use2x_scale;
	int retval;
	int de_support_2x;
	//normally, we should read from de
	// ---------------------------------------
	if(!ctx) {
		return;
	}

	if(ctx->vtv_ui_height*2<=ctx->tvout_height &&
			ctx->vtv_ui_width*2 <=ctx->tvout_width) {
		use2x_scale=1;
	} else {
		use2x_scale=0;
	}

	//ywwang todo: to be improved
	if(ctx->tvout_width ==1920 && ctx->tvout_height==1080) {
		use2x_scale=1;
		//overriden default
		if(property_get("ro.hw.tvout.use2x_scale", value, NULL)>0) {
			use2x_scale=atoi(value);
		}
	} else if(ctx->tvout_width ==1280 && ctx->tvout_height==720) {
		use2x_scale=0;
		//overriden default
		if(property_get("ro.hw.tvout.use2x_scale2", value, NULL)>0) {
			use2x_scale=atoi(value);
		}
	}

	// notes: currently
	retval=ioctl(ctx->mFD, DEIO_SET_2X_MODE, &use2x_scale);
	if(retval<0) {
		ALOGW("failed to set enable 2x scale=%d \n", use2x_scale);
		retval=ioctl(ctx->mFD, DEIO_GET_2X_MODE, &de_support_2x);
		if(retval==0) {
			use2x_scale=de_support_2x;
		}
	}
	sprintf(value, "%d", use2x_scale);
	property_set("hw.tvout.use2x_scale", value);

}

/*
 set follows:
 hw.tvout.virtual_tv_width;
 hw.tvout.virtual_tv_height;
 hw.tvout.tv_width;
 hw.tvout.tv_height
 */
static void update_virtual_tv_config(struct de_context_t *ctx, int *vtv_width, int *vtv_height, int *tv_width,
		int *tv_height)
{
	int w;
	int h;
	char value[PROPERTY_VALUE_MAX];
	property_get("hw.tvout.virtual_tv_width", value, "1");
	w = atoi(value);
	property_get("hw.tvout.virtual_tv_height", value, "1");
	h = atoi(value);

	//try put virtual ui resolution into sysetm.prop
	// ui resolution is 1024*576  default
	if (w <= 1 || h <= 1) {
		ALOGW("forget to set hw.tvout.* in .prop ? ");
		w = HAL_DEFAULT_TVOUT_VIRTUAL_WIDTH;
		sprintf(value, "%d", w);
		property_set("hw.tvout.virtual_tv_width", value);
		h = HAL_DEFAULT_TVOUT_VIRTUAL_HEIGHT;
		sprintf(value, "%d", h);
		property_set("hw.tvout.virtual_tv_height", value);
	}

	if (vtv_width) {
		*vtv_width = w;
	}

	if (vtv_height) {
		*vtv_height = h;
	}
	//then tell de to

	//tvout : use 1920*1080 in Gl5201

	mdsb_info info;
	if (ioctl(ctx->mFD, DEIO_GET_MDSB_INFO, &info) < 0) {
		ALOGE("failed to get mdsb info");
		w = -1;
		h = -1;
	} else {
		w = info.xres;
		h = info.yres;

		if (info.drop_line_front >= 0 && info.drop_line_front < h && info.drop_line_back >= 0 && info.drop_line_back
				< h) {
			update_tvout_gap_info(info.drop_line_front, info.drop_line_back);

		} else {
			update_tvout_gap_info(HAL_TVOUT_TOP_GAP_DEFAULT, HAL_TVOUT_BOTTOM_GAP_DEFAULT);
		}
	}

	if (w <= 1) {
		w = HAL_DEFAULT_TVOUT_TV_WIDTH;
	}

	if (h <= 1) {
		h = HAL_DEFAULT_TVOUT_TV_HEIGHT;
	}

	sprintf(value, "%d", w);
	property_set("hw.tvout.tv_width", value);
	sprintf(value, "%d", h);
	property_set("hw.tvout.tv_height", value);
	if (tv_width) {
		*tv_width = w;
	}
	if (tv_height) {
		*tv_height = h;
	}

}
#endif

static void write_virtual_tv_ui_config(int width, int height) {
	char value[PROPERTY_VALUE_MAX];
	sprintf(value, "%d", width);
	property_set("hw.tvout.virtual_tv_width", value);
	sprintf(value, "%d", height);
	property_set("hw.tvout.virtual_tv_height", value);

}

/**
 *  get dimension of virtual resolution seen by app when tvout,
 *  currently 1024x576
 *
 */
static int de_get_virtual_tv_ui_dimension(int *pwidth, int *pheight) {

	if (pwidth) {
		*pwidth = HAL_DEFAULT_TVOUT_VIRTUAL_WIDTH;
	}

	if (pheight) {
		*pheight = HAL_DEFAULT_TVOUT_VIRTUAL_HEIGHT;
	}
	return 0;

}

static int de_set_displayer(struct de_control_device_t *dev,
		displayer_config *displayer_config) {
	int status = -EINVAL;
	unsigned int mask;
	struct de_context_t* ctx = (struct de_context_t*) dev;

	if (!ctx) {
		return -EINVAL;
	}
	ALOGD("FBIOSET_DISPLAYER: id-%d", displayer_config->displayer_id);
	status = ioctl(ctx->mFD, DEIOSET_DISPLAYER,
			&displayer_config->displayer_id);

#if 0
	mask = LCD_DISPLAYER;
	mask = ~mask;
	if (mask & displayer_config->displayer_id) {
		//after switch to tv display, we now get should update mdsb info
		update_virtual_tv_config(ctx, &ctx->vtv_ui_width, &ctx->vtv_ui_height, &ctx->tvout_width, &ctx->tvout_height);
		//notes: muse called after update_virtual_tv_config
		update_tvout_scale_mode(ctx);
	}
#endif

	return status;
}

static int de_get_displayer(struct de_control_device_t *dev) {
	int status = -EINVAL;
	struct de_context_t* ctx = (struct de_context_t*) dev;
	int current_displayer;
	status = ioctl(ctx->mFD, DEIOGET_DISPLAYER, &current_displayer);
	if (status) {
		ALOGE("DEIOGET_DISPLAYER error!");
		return status;
	}

	return current_displayer;
}

/**
 * here we assume: when boot, tvout is disabled, so ,if we find tvout is opened
 * then closed tvout
 * consider this scene: when in tvout, system_server is restart.....
 *
 */
static int de_force_switch_to_lcd(struct de_control_device_t *dev) {
	int status = -EINVAL;
	int displayer = 0;
	struct de_context_t* ctx = (struct de_context_t*) dev;
	if (!ctx) {
		return status;
	}

	status = ioctl(ctx->mFD, DEIOGET_DISPLAYER, &displayer);
	ALOGD("get displayer=%d \n", displayer);
	if ((status == 0) && (displayer & ~LCD_DISPLAYER)) {
		displayer = LCD_DISPLAYER;
		ioctl(ctx->mFD, DEIOSET_DISPLAYER, &displayer);
		ALOGI("force switch to lcd display");
		return 0;
	} else {
		return status;
	}

}

/**
 * todo: currently, always select content of fb0 to tv, even more fbs exist
 */
static int de_set_content_output_tv(struct de_control_device_t *dev, int set) {
	struct de_context_t* ctx = (struct de_context_t*) dev;
	int status = -1;
	if (ctx) {

		status = ioctl(ctx->mFD, DEIO_SELECT_AS_MAIN_FB, NULL);
	}
	return status;
}

/**
 *  调试电视输出时，画面的缩放比例，共76级
 *
 * @param dev   dev设备结构
 * @param xscale  x方向缩放比例
 * @param yscale  y方向缩放比例
 * @return 0成功, <0失败
 */
int de_set_tv_display_scale(struct de_control_device_t *dev, int xscale,
		int yscale) {
	int de_param;
	int xscale_drv;
	int yscale_drv;

	struct de_context_t* ctx = (struct de_context_t*) dev;
	if (ctx == 0) {
		return -EINVAL;
	}
	if (xscale < DEIO_SET_SCALE_RATE_FULL_SCREEN_MIN_X
			|| xscale > DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_X) {
		return -EINVAL;
	}
	if (yscale < DEIO_SET_SCALE_RATE_FULL_SCREEN_MIN_Y
			|| yscale > DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_Y) {
		return -EINVAL;
	}
	xscale_drv = xscale + 100 - DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_X;
	yscale_drv = yscale + 100 - DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_Y;
	de_param = (xscale_drv << 8) | yscale_drv;
	if (ioctl(ctx->mFD, DEIO_SET_SCALE_RATE_FULL_SCREEN, &de_param) < 0) {
		return -ENOSYS;
	}
	ctx->scale_x = xscale;
	ctx->scale_y = yscale;
	update_virtual_tv_scale_info(xscale_drv, yscale_drv);
	return 0;
}

int de_get_tv_display_scale(struct de_control_device_t *dev, int *pxscale,
		int *pyscale) {
	int de_param;
	struct de_context_t* ctx = (struct de_context_t*) dev;
	if (ctx == 0 || pxscale == 0 || pyscale == 0) {
		return -EINVAL;
	}
	*pxscale = ctx->scale_x;
	*pyscale = ctx->scale_y;
	return 0;
}

/**
 *  获取连接电缆的状态
 *
 * @param dev   dev设备结构
 * @param status   用于返回电缆的连接状态
 * @return 0成功, <0失败
 */
int de_get_tv_cable_status(struct de_control_device_t *dev, int *status) {

	struct de_context_t* ctx = (struct de_context_t*) dev;
	if (ctx == 0 || status == 0) {
		ALOGW("call tv_cable_status  failed: invalidate argumeent");
		return -EINVAL;
	}
	if (ioctl(ctx->mFD, DEIO_GET_DISP_CHANGED, status) < 0) {
		ALOGW("call DEIO_GET_DISP_CHANGED failed");
		return -ENOSYS;
	}
	return 0;
}

void de_set_display_mode_single(struct de_control_device_t *dev, int mode) {
	int retval;
	ALOGW("call de_set_display_mode,mode:%d\n", mode);
	struct de_context_t* ctx = (struct de_context_t*) dev;
	switch (mode) {
	/*(1)src: 1; des:3; mode:1*/
	case MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

		/*(2)src: 2; des:2; mode:3*/
	case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

	case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_G:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

	case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_V_LCD_G:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

		/*(3)src: 2; des:2 or more; mode:3. des2 :displayers same with (2),more than 2
		 displayers same with (1)*/

		/*(4)src:2; des:2 or more;mode:3.des2:same with (2)*/
	case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_GV:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

	case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_G:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

	case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_V_LCD_G:
		retval = ioctl(ctx->mFD, DEIO_SET_DISP_MODE, &mode);
		if (retval < 0) {
			ALOGW("failed to set display mode:%d\n", mode);
		}
		break;

	default:
		ALOGW("don't support single mode :%d\n", mode);
		break;
	}

}

static void de_set_display_mode(struct de_control_device_t *dev, int mode) {
	switch (mode) {
	case DISPLAY_MODE_LOW_BANDWIDTH:
		de_set_display_mode_single(dev, MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV);
		break;

	case DISPLAY_MODE_MEDIUM_BANDWIDTH:
		de_set_display_mode_single(dev,
				MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_G);
		break;

	case DISPLAY_MODE_HIGH_BANDWIDTH:
		de_set_display_mode_single(dev,
				MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_GV);
		break;

	default:
		ALOGW("don't support mode :%d\n", mode);
		break;
	}
}

/** Close the display device */
static int close_display_engine(struct hw_device_t *dev) {
	struct de_context_t* ctx = (struct de_context_t*) dev;
	if (ctx) {
		close(ctx->mFD);
		free(ctx);
	}

	return 0;
}

static int open_display_engine(const struct hw_module_t* module,
		const char* name, struct hw_device_t** device) {
	int status = -EINVAL;
	struct de_context_t *ctx = static_cast<struct de_context_t *>(malloc(
			sizeof(struct de_context_t)));memset
	(ctx, 0, sizeof(*ctx));
	ALOGD("enter open_display_engine\n");
	ctx->device.common.tag = HARDWARE_DEVICE_TAG;
	ctx->device.common.version = 0;
	ctx->device.common.module = const_cast<struct hw_module_t *>(module);
	ctx->device.common.close = close_display_engine;
	ctx->device.set_displayer = de_set_displayer;
	ctx->device.get_displayer = de_get_displayer;
	ctx->device.de_set_tv_display_scale = de_set_tv_display_scale;
	ctx->device.de_get_tv_display_scale = de_get_tv_display_scale;
	ctx->device.de_get_tv_cable_status = de_get_tv_cable_status;
	ctx->device.de_set_display_mode_single = de_set_display_mode_single;
	ctx->device.de_set_display_mode = de_set_display_mode;

	ctx->mFD = open(DE_HARDWARE_DEVICE, O_RDWR, 0);
	ctx->scale_x = DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_X;
	ctx->scale_y = DEIO_SET_SCALE_RATE_FULL_SCREEN_MAX_Y;
#if 0
	update_virtual_tv_config(ctx, &ctx->vtv_ui_width, &ctx->vtv_ui_height, &ctx->tvout_width, &ctx->tvout_height);
	//notes: muse called after update_virtual_tv_config
	update_tvout_scale_mode(ctx);

	if(de_set_virtual_tv_ui_dimension(&ctx->device, ctx->vtv_ui_width, ctx->vtv_ui_height)==0) {

	} else {
		//failed: we use value provider by de
		de_get_virtual_tv_ui_dimension(&ctx->vtv_ui_width,&ctx->vtv_ui_height);
		write_virtual_tv_ui_config(ctx->vtv_ui_width, ctx->vtv_ui_height);
	}
#endif

	//todo: config tvout resolution, currently de only support
	//when boot: scale is alwalys 100
	de_set_tv_display_scale(&ctx->device, ctx->scale_x, ctx->scale_y);

	if (ctx->mFD >= 0) {
		ALOGI("open de successfully! fd: %d", ctx->mFD);
		*device = &ctx->device.common;
		status = 0;
		//de_force_switch_to_lcd(&ctx->device);
	} else {
		status = errno;
		close_display_engine(&ctx->device.common);
		ALOGE("ctx->mFD:%d,Error open de failed: %d %s", ctx->mFD,status, strerror(status));
		status = -status;
	}
	return status;
}

