/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/native_handle.h>

#include <hardware/overlay.h>
#include <hardware/gralloc_priv.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <EGL/egl.h>
#define ALIGN(x, align) (((x) + ((align)-1)) & ~((align)-1))
#define MAX_VIDEO_BUFF_NUM 8
/*****************************************************************************/
static int overlay_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t overlay_module_methods = {
    open: overlay_device_open
};

overlay_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: OVERLAY_HARDWARE_MODULE_ID,
        name: "Actions overlay module",
        author: "The Android Open Source Project",
        methods: &overlay_module_methods,
    }
};

static int overlay_device_close(struct hw_device_t *dev)
{
    struct overlay_context_t * ctx = (struct overlay_context_t*)dev;
    if (ctx) {
        free(ctx);
    }
    return 0;
}
/*****************************************************************************/
// overlay fouctions
/*****************************************************************************/
static int overlay_setFormat(struct overlay_device *dev, uint32_t w,uint32_t h, uint32_t fmt) {
	struct overlay_context_t* ctx = (struct overlay_context_t *) dev;
	struct v4l2_format format;
	int rc = 0;
  ALOGI("overlay_setFormat:: w=%d h=%d format=%d\n", w,h, fmt) ;
	
  if(ctx->imageW == w
  	&& ctx->imageH == h
  	&& ctx->format == fmt){
  	ALOGI("Format not change , nothing to do \n");
		return rc;
  }
  switch(fmt){
	  case HAL_PIXEL_FORMAT_YV12:
			fmt = V4L2_PIX_FMT_YVU420;
			break;
		case HAL_PIXEL_FORMAT_ACT_YU12:
			fmt = V4L2_PIX_FMT_YUV420;
			break;
		case HAL_PIXEL_FORMAT_YCrCb_420_SP:
			fmt = V4L2_PIX_FMT_NV21;
			break;
	 case HAL_PIXEL_FORMAT_ACT_NV12:
			fmt = V4L2_PIX_FMT_NV12;
			break;
		case HAL_PIXEL_FORMAT_RGBA_8888:
			fmt = V4L2_PIX_FMT_RGB32;
			break;
		case HAL_PIXEL_FORMAT_BGRA_8888:
			fmt = V4L2_PIX_FMT_BGR32;
			break;
		case HAL_PIXEL_FORMAT_RGB_565:
			fmt = V4L2_PIX_FMT_RGB565;
			break;
  }
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;		
	rc = ioctl(ctx->fd, VIDIOC_G_FMT, &format);	
	if (rc < 0) {
		ALOGE("Get Format Failed!/%d\n", rc);
		return rc;
	}	
	
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;	
	format.fmt.pix.pixelformat = fmt;
	format.fmt.pix.width = w;
	format.fmt.pix.height = h;	
	
	rc = ioctl(ctx->fd, VIDIOC_S_FMT, &format);	
	if (rc < 0) {
		ALOGE("Set Format Failed!/%d\n", rc);
		return rc;
	}		
	
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	rc = ioctl(ctx->fd, VIDIOC_G_FMT, &format);	
	if (rc < 0) {
		ALOGE("Get Format Failed!/%d\n", rc);
		return rc;
	}	
	ALOGI("v4l2_overlay_init:: w=%d h=%d format=%d\n", format.fmt.pix.width, format.fmt.pix.height, format.fmt.pix.pixelformat) ;
	
	/*updata cache data */
	ctx->imageW = w;
	ctx->imageH = h;
	ctx->format = fmt;
	return rc;
}
static int overlay_setCrop(struct overlay_device *dev, uint32_t x,uint32_t y, uint32_t w, uint32_t h) {
	int rc = 0;
	struct v4l2_crop crop;
	struct overlay_context_t* ctx = (struct overlay_context_t *) dev;

	if (ctx->cropX == x 
		  && ctx->cropY == y 
		  && ctx->cropW == w
			&& ctx->cropH == h) {
		ALOGI("Nothing to do!\n");
		return rc;
	}
	
	ALOGI("Crop /X%d/Y%d/W%d/H%d\n", x, y, w, h);	
	
	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;	
	rc = ioctl(ctx->fd, VIDIOC_G_CROP, &crop);	
	if (rc < 0) {
		ALOGE("Get Crop Failed!/%d\n", rc);
		return rc;
	}		
	
	crop.c.left = x;
	crop.c.top = y;
	crop.c.width = w;
	crop.c.height = h;		
	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;	
	rc = ioctl(ctx->fd, VIDIOC_S_CROP, &crop);		
	if (rc) {
		ALOGE("Set Crop Failed!/%d\n", rc);
		return rc;
	}	
	/*updata cache data */
	ctx->cropX = x;
	ctx->cropY = y;
	ctx->cropW = w;
	ctx->cropH = h;
	return rc;
}

static int overlay_setPosition(struct overlay_device *dev, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rotation) {
	int rc = 0;
	struct overlay_context_t* ctx = (struct overlay_context_t *) dev;
	struct v4l2_control ctrl;
	struct v4l2_format format;

  ALOGI("v4l2_overlay_set_position:: x=%d y=%d  w=%d h=%d",x,y,w,h);
		
	if (ctx->posX == x 
		  && ctx->posY == y 
		  && ctx->posW == w
			&& ctx->posH == h
			&& ctx->rotation == rotation) {
		ALOGI("positon informations not changed!\n");
		return rc;
	}
	ALOGI("v4l2_overlay_set_rotation::ctx->rotation =%d rotation =%d",ctx->rotation ,rotation);
	if(ctx->rotation != rotation){
		ctrl.id = V4L2_CID_ROTATE;
		ctrl.value = rotation;
		rc = ioctl(ctx->fd, VIDIOC_S_CTRL, &ctrl);		
		if (rc) {
			ALOGE("Set rotation Failed!/%d\n", rc);
			return rc;
		}
	}	
	ALOGI("v4l2_overlay_set_position old positons :: w=%d h=%d", format.fmt.win.w.width, format.fmt.win.w.height);
		
	if(ctx->posX != x 
		  || ctx->posY != y 
		  || ctx->posW != w
		  || ctx->posH != h){
				
		format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		rc = ioctl(ctx->fd, VIDIOC_G_FMT, &format);		
		if (rc) {
			ALOGE("get v4l2_overlay format Failed!/%d\n", rc);
			return rc;
		}	
		format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		format.fmt.win.w.left = x;
		format.fmt.win.w.top = y;
		format.fmt.win.w.width = w;
		format.fmt.win.w.height = h;
		rc = ioctl(ctx->fd, VIDIOC_S_FMT, &format);	
			
		ALOGI("v4l2_overlay_set_position new positons :: w=%d h=%d", format.fmt.win.w.width, format.fmt.win.w.height);
	
		if (rc) {
			ALOGE("set v4l2_overlay format Failed!/%d\n", rc);
			return rc;
		}
	}
	/*updata cache data */
	ctx->posX = x;
  ctx->posY = y;
	ctx->posW = w;
	ctx->posH = h;
	ctx->rotation = rotation;
	return 0;
}

static int overlay_request_buffer(struct overlay_device *dev,int buf_count){
	int ret=0;
	struct overlay_context_t* ctx = (struct overlay_context_t *) dev;
	struct v4l2_requestbuffers reqbuf;
	// request buffers
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_USERPTR;
	reqbuf.count = buf_count;
	
	ret = ioctl(ctx->fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
	  ALOGE("reqbuf ioctl ERROR ,Error Code = %d\n",ret);
	  return ret;
	}
	else
	{
		ALOGI("reqbuf success!!\n");
	}
	return 0;
}

static int overlay_init(struct overlay_device *dev){
	
	ALOGI("overlay_init called ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	int fd = 0;
	struct overlay_context_t * ctx = (struct overlay_context_t*)dev;
	
	fd = open("/dev/video2", O_RDWR);
	if (fd < 0) {
		ALOGE("Failed to open video device ");
		return -1;
	}	
	ctx->fd = fd;	
	ctx->mInit = 1;
	return 0;
}

static int overlay_prepare(struct overlay_device *dev,hwc_layer_t *layer){
	//ALOGI("overlay_prepare called ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	struct overlay_context_t * ctx = (struct overlay_context_t*)dev;
	int rc = 0;
	
	if (!ctx || !layer){
		ALOGE("ctx or layer is null");
		return -1;
	}			
	private_handle_t* hnd = (private_handle_t *)layer->handle;
	
	if (!hnd){
		ALOGE("prepare_overlay hnd is null");
		return -1;
	}	
	
	if(ctx->mInit == 0){
		rc = overlay_init(dev);
		if(rc){
			ALOGE("overlay_init error !/%d\n", rc);
			return rc ;
		}
		// ALOGI("overlay_init ok \n");
		 rc = overlay_setFormat(dev,hnd->width,hnd->height,hnd->format);
		 if(rc){
		  	ALOGE("overlay_setFormatFailed!/%d\n", rc);
		  	return rc;
		 }
		 rc = overlay_request_buffer(dev,MAX_VIDEO_BUFF_NUM);
	   if(rc){
		  	ALOGE("overlay_request_buffer !/%d\n", rc);
		  	return rc;
	   }
  	//ALOGI("overlay_request_buffer ok \n");
		ctx->mInit = 1;
	}
	/*set the overlay format */
  rc = overlay_setFormat(dev,hnd->width,hnd->height,hnd->format);
  if(rc){
  	ALOGE("overlay_setFormatFailed!/%d\n", rc);
  	return rc;
  }
  //ALOGI("overlay_setFormat ok \n");
  
  /*set the overlay crop */
  hwc_rect_t sourceCrop = layer->sourceCrop;
	rc = overlay_setCrop(dev,sourceCrop.left, sourceCrop.top, sourceCrop.right - sourceCrop.left, sourceCrop.bottom - sourceCrop.top);
	if(rc){
  	ALOGE("overlay_setCrop!/%d\n", rc);
  	return rc;
  }
  //ALOGI("overlay_setCrop ok \n");
	/*set the overlay position */
	hwc_rect_t displayFrame = layer->displayFrame;
	rc = overlay_setPosition(dev, displayFrame.left, displayFrame.top, displayFrame.right - displayFrame.left, displayFrame.bottom - displayFrame.top,0);
	if(rc){
  	ALOGE("overlay_setPosition!/%d\n", rc);
  	return rc;
  }
  //ALOGI("overlay_setPosition ok \n");
  if(ctx->isReady == 0){
  	/*set stream on , can queue buffer after this */
	  uint32_t type = V4L2_BUF_TYPE_VIDEO_OUTPUT;	
		rc = ioctl(ctx->fd, VIDIOC_STREAMON, &type);		
		if (rc) {
			ALOGE("stream on Failed!/%d\n", rc);
			return rc;
		}
		ctx->isReady = 1;
		ctx->isFirst = 1;
  }  
	return rc;
} 
static void* sys_get_phyaddr_coda(void *cpuphy)
{
  if(!((unsigned long)cpuphy&0x80000000)){
          return cpuphy;
  }else{
          return (void*)(((unsigned long)cpuphy & 0x0FFFFFFF) | 0x10000000);
  }
}
static void YUV422toYUV420(uint8_t *ptr, int width, int height)
{
	 ptr += width*height;	 
	 for(int i=0; i < height; i++){
        for(int j=0; j < width/2; j++){
            ptr[i*width/2+j] = ptr[i*width+j];// ptr[i*2*width/2+j];
        }
    }
}
static int overlay_set(struct overlay_device *dev,hwc_layer_t *layer){
	
	struct overlay_context_t* ctx = (struct overlay_context_t*)dev;
	private_handle_t *hnd = (private_handle_t *)layer->handle;
	void* phy_addr = NULL;
	int rc = 0;
	
	static int index = 0;
	
	if(ctx->isReady == 0){
		ALOGE("layer layer informations not set");
		return -1;
	}
	if (hnd == NULL){
		ALOGW("layer composed by overlay but hnd is null");
		return 0;
  }  
	if (hnd->flags & private_handle_t::PRIV_FLAGS_USES_ION){
		phy_addr = (void*)hnd->phys_addr;
	}
	/*if (hnd->format == HAL_PIXEL_FORMAT_YCbCr_422_P){
        YUV422toYUV420((uint8_t *)hnd->base, ALIGN(hnd->width, 32), ALIGN(hnd->height, 32));
  }
	if (hnd->flags & private_handle_t::PRIV_FLAGS_USES_PMEM_ADSP){
		phy_addr = (void*)hnd->pbase;
	}*/
	/*queue buffer ,set the phy addr to video buffer */	
	struct v4l2_buffer buf;	
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.field = V4L2_FIELD_NONE;
	buf.timestamp.tv_sec = 0;
	buf.timestamp.tv_usec = 0;
	buf.flags = 0;	
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.m.userptr = (unsigned long)sys_get_phyaddr_coda(phy_addr);
	buf.index = index ;
	
	buf.length = 2048*2048* 2;
	//ALOGE(" ioctl(ctx->fd, VIDIOC_QBUF, &buf) buf.length = %d hnd->size =%d \n", buf.length,hnd->size);
	rc = ioctl(ctx->fd, VIDIOC_QBUF, &buf);	
		
	if (rc) {
		ALOGE("queue buffer  Failed!/%d\n", rc);
		return rc;
	} 
	//ALOGI("index = %d queue buffer ok ! \n",index);
	index ++;
	if(index >= MAX_VIDEO_BUFF_NUM){
		index = 0;
	}
	if(!ctx->isFirst){
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_USERPTR;
		
		//ALOGI("de queue buffer called  ! \n");
		rc = ioctl(ctx->fd, VIDIOC_DQBUF, &buf);		
		if (rc) {
			ALOGE("dequeue buffer Failed!/%d\n", rc);
			return rc;
		} 
		//ALOGI("DQ index = %d \n",buf.index);
	} 
	ctx->isFirst = 0;

	return 0;
} 
static int overlay_disable(struct overlay_device *dev){
	/*set stream off, can't queue buffer after this, video layer is closed*/
	int rc = 0;
	struct overlay_context_t* ctx = (struct overlay_context_t*)dev;
  uint32_t type = V4L2_BUF_TYPE_VIDEO_OUTPUT;	 
  if(ctx->mInit && ctx->isReady){ 
		rc = ioctl(ctx->fd, VIDIOC_STREAMOFF, &type);		
		if (rc) {
			ALOGE("stream on Failed!/%d\n", rc);
			return rc;
		}
	}
	close(ctx->fd);
	 
	ctx->fd = -1;	
	ctx->mInit = 0;
	ctx->isReady = 0;
  ctx->posX = 0;
	ctx->posY = 0;
	ctx->posW = 0;
	ctx->posH = 0;
	ctx->rotation= 0;				
	/*image informations */
	ctx->imageW = 0;
	ctx->imageH = 0;
	ctx->format = 0;		
	/*crop informations */
	ctx->cropX = 0;
	ctx->cropY = 0;
	ctx->cropW = 0;
	ctx->cropH = 0;
	return 0;
}
/*****************************************************************************/

static int overlay_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;
    if (!strcmp(name, OVERLAY_HARDWARE)) {
        struct overlay_context_t *dev;
        dev = (overlay_context_t*)malloc(sizeof(*dev));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = overlay_device_close;
        dev->device.prepare = overlay_prepare;
        dev->device.set =  overlay_set;
        dev->device.close = overlay_disable;
        *device = &dev->device.common;
         status = 0;
    }
    return status;
}
