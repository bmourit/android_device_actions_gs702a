/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
*/
/******************************************************************************/

/******************************************************************************/
#ifndef __ASOC_IOCTL_H__
#define __ASOC_IOCTL_H__

#ifdef __cplusplus
extern "C" {
#endif
/*每个驱动模块各自取一个magic number，然后再按顺序生产各个CMD*/

/******************************************************************************/
/*dma mem driver magic number*/
#define DMA_MAGIC_NUMBER                'm'
/******************************************************************************/
/*cmd invaild cache of writeback type*/
#define DMA_CACHE_WBACK_INV             _IOWR(DMA_MAGIC_NUMBER, 0, struct cache_info_t)
/*cmd invaild cache*/
#define DMA_CACHE_INV                   _IOWR(DMA_MAGIC_NUMBER, 1, struct cache_info_t)
/*cmd show the memory node informations*/
#define DMA_MEM_DUMP                    _IOWR(DMA_MAGIC_NUMBER, 2, u32)
/*cmd show the memory node bitmaps*/
#define DMA_MEM_SHOW                    _IOWR(DMA_MAGIC_NUMBER, 3, u32)
/*cmd alloc the memory form reserved zone*/
#define DMA_MEM_ALLOC                   _IOWR(DMA_MAGIC_NUMBER, 4, struct mem_info_t)
/*cmd free the memory to reserved zone*/
#define DMA_MEM_FREE                    _IOWR(DMA_MAGIC_NUMBER, 5, struct mem_info_t)
/*cmd free the memory to reserved zone after lib unload*/
#define DMA_MEM_EXIT                    _IOWR(DMA_MAGIC_NUMBER, 6, u32)
/******************************************************************************/


/******************************************************************************/
#define FRAMEBUFFER_MAGIC_NUMBER      'f'
/******************************************************************************/
#define FBIOSET_START_VIDEO             _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 0, u32)   /*进入视频输出状态*/
#define FBIOSET_CLEAR_SCREEN		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 1, u32)  
#define FBIOSET_RESET_REGION            _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 2, layer_region_config)
#define FBIOSET_SET_REGION              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 3, layer_region_config)
#define FBIOGET_REGION_CONFIG           _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 4, layer_region_config)
#define FBIOSET_FLIP_REGION             _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 5, layer_region_config)
#define FBIOSET_PAN_REGION              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 6, layer_region_config)
#define FBIOSET_COLORKEY                _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 7, ColorKey)
#define FBIOGET_CUR_FLUSH_ADDR		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 8, u32)
#define FBIOGET_READY_FLUSH_ADDR        _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 9, u32)
#define FBIOSET_DISPLAYER               _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 10, Displayer_config)
#define FBIOSET_OPEN_RGB_LCD            _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 11, u32)
#define FBIOSET_CLOSE_RGB_LCD           _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 12, u32)
#define FBIOSET_FRAME_INFO		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 13, disp_frame_info_t)
#define FBIOSET_UPDATE_VIDEO		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 14, disp_frame_addr_t)
#define FBIOSET_END_VIDEO		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 15, u32)
#define FBIOGET_DISPLAYER		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 16, Displayer_config)
#define FBIOSET_SCALE_REGION		_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 17, disp_scale_t)
#define FBIOGET_MAX_SCALE_RATE	_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 18, unsigned int)
#define FBIOSET_PRINT_INFO	_IOWR(FRAMEBUFFER_MAGIC_NUMBER, 19, u32)
#define FBIOSET_MOVE_REGION              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 20, layer_region_config)
#define FBIOSET_BACKGROUND_COLOR              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 21, struct rgb_color)
#define FBIOSET_REGION_OPACITY              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 22, layer_region_config)
#define FBIOGET_FRAMEBUFFER_DATA              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 23, struct framebuffer_data_info)
#define FBIOSET_CLOSE_DISP_DEV              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 24, u32)
#define FBIOSET_OPEN_DISP_DEV              _IOWR(FRAMEBUFFER_MAGIC_NUMBER, 25, u32)



/******************************************************************************/
#define HDMI_TX_IOC_MAGIC               'h'
/******************************************************************************/
#define HDMI_GET_LINK_STATUS            _IOR(HDMI_TX_IOC_MAGIC, 0x90, u32)
#define HDMI_GET_SINK_CAPABILITY        _IOR(HDMI_TX_IOC_MAGIC, 0x91, struct sink_capabilities)
#define HDMI_SET_VIDEO_CONFIG           _IOW(HDMI_TX_IOC_MAGIC, 0x92, struct Video_Settings)
#define HDMI_GET_VIDEO_CONFIG           _IOR(HDMI_TX_IOC_MAGIC, 0x93, struct Video_Settings)
#define HDMI_SET_AUDIO_CONFIG           _IOW(HDMI_TX_IOC_MAGIC, 0x94, struct Audio_Settings)
#define HDMI_GET_AUDIO_CONFIG           _IOR(HDMI_TX_IOC_MAGIC, 0x95, struct Audio_Settings)
#define HDMI_SET_AUDIO_MUTE             _IOW(HDMI_TX_IOC_MAGIC, 0x96, u32)
#define HDMI_SET_AV_MUTE                _IOW(HDMI_TX_IOC_MAGIC, 0x97, u32)
#define HDMI_SET_ENABLE                 _IOW(HDMI_TX_IOC_MAGIC, 0x98, u32)
#define HDMI_SET_HDCP                   _IOW(HDMI_TX_IOC_MAGIC, 0x99, u32)
#define HDMI_GET_HDCP                   _IOR(HDMI_TX_IOC_MAGIC, 0xA0, u32)
#define HDMI_SET_HDCP_UNAUTH_FORCE      _IO(HDMI_TX_IOC_MAGIC, 0xA1)
#define HDMI_SET_HDCP_RNG_SEED          _IOW(HDMI_TX_IOC_MAGIC, 0xA2, u32)
#define HDMI_GET_HDCP_RNG_SEED          _IOR(HDMI_TX_IOC_MAGIC, 0xA3, u32)
#define HDMI_SET_CEC_ONETOUCHPLAY       _IO(HDMI_TX_IOC_MAGIC, 0xA4)

/******************************************************************************/
#define TVOUT_IOC_MAGIC               't'
/******************************************************************************/
#define TVOUT_CONFIGURE                 _IOW(TVOUT_IOC_MAGIC, 0x90, struct tv_settings)
#define TVOUT_SET_ENABLE                _IOW(TVOUT_IOC_MAGIC, 0x91, u32)
#define TVOUT_GET_ENABLE                _IOR(TVOUT_IOC_MAGIC, 0x92, u32)

/******************************************************************************/
#define GPU_MAGIC_NUMBER              'g'
/******************************************************************************/
#define GPU_USED_NUM_INC                _IOWR(GPU_MAGIC_NUMBER, 0, u32)
#define GPU_BUF_PHY_BASE                _IOWR(GPU_MAGIC_NUMBER, 1, u32)
//#define GPU_PRINT_STRING                _IOWR(GPU_MAGIC_NUMBER, 2, u32)


/******************************************************************************/
#define CHARGE_IOC_MAGIC               'c'
/******************************************************************************/
#define CHARGE_SET_DISABLE                _IO(CHARGE_IOC_MAGIC, 0)
#define CHARGE_GET_STATUS_AND_ENABLE      _IOR(CHARGE_IOC_MAGIC, 1, u32)
#define CHARGE_GET_VOLUME                 _IOR(CHARGE_IOC_MAGIC, 2, u8)

/******************************************************************************/
#define USB_MONITOR_MAGIC              'u'
/******************************************************************************/
#define MNT_SET_UDISK_PID               _IOW(USB_MONITOR_MAGIC, 0x0, pid_t)

#define MNT_GET_DOCK_PLUG_STAT          _IO(USB_MONITOR_MAGIC, 0x1)
#define MNT_GET_DOCK_POWER_STAT         _IO(USB_MONITOR_MAGIC, 0x2)

#define MNT_GET_USBLINE_PLUG_STAT       _IO(USB_MONITOR_MAGIC, 0x3)
#define MNT_GET_USBLINE_TYPE            _IO(USB_MONITOR_MAGIC, 0x4)
#define MNT_GET_USBLINE_WORK_STAT       _IOR(USB_MONITOR_MAGIC, 0x5, unsigned int )
#define MNT_GET_USBLINE_DATA_STAT       _IOR(USB_MONITOR_MAGIC, 0x6, unsigned int )

#define MNT_GET_UDEVICE_PLUG_STAT       _IO(USB_MONITOR_MAGIC, 0x7)

#define __MNT_GET_DOCK_PLUG_STAT          _IO(USB_MONITOR_MAGIC, 0x8)
#define __MNT_GET_DOCK_POWER_STAT         _IO(USB_MONITOR_MAGIC, 0x9)
#define __MNT_GET_USBLINE_PLUG_STAT       _IO(USB_MONITOR_MAGIC, 0xa)
#define __MNT_GET_USBLINE_TYPE            _IO(USB_MONITOR_MAGIC, 0xb)
#define __MNT_GET_USBLINE_PLUG_STAT_NODOCK       _IO(USB_MONITOR_MAGIC, 0xc)
#define __MNT_GET_USBLINE_TYPE_NODOCK            _IO(USB_MONITOR_MAGIC, 0xd)
#define MNT_GET_UPGRADE_STAT       		_IOR(USB_MONITOR_MAGIC, 0xE, unsigned int )
#define MNT_SET_UPGRADE_STAT       		_IOW(USB_MONITOR_MAGIC, 0xF, unsigned int )

/******************************************************************************/
#define CARD_MAGIC              'r'
/******************************************************************************/
#define CARD_SET_UDISK_PID       _IOW(CARD_MAGIC, 0x0, pid_t)

#define CARD_GET_PLUG_STAT       _IO(CARD_MAGIC, 0x1)

/******************************************************************************/
#define NAND_MAGIC		'n'
/******************************************************************************/
#define BOOT_ERASE		_IOW(NAND_MAGIC, 0x0, unsigned int)
#define BOOT_READ		_IOW(NAND_MAGIC, 0x1, unsigned int)
#define BOOT_WRITE		_IOW(NAND_MAGIC, 0x2, unsigned int) 
#define BOOT_GETINFO		_IOW(NAND_MAGIC, 0x3, unsigned int)
#define BOOT_ERASE_ALL		_IO(NAND_MAGIC, 0x4)
#define BOOT_TEST_BADSUBLK	_IOW(NAND_MAGIC, 0x5, unsigned int)
#define BOOT_TEST_NEEDESALL	_IO(NAND_MAGIC, 0x6)
#define BOOT_GET_MBR		_IOW(NAND_MAGIC, 0x7, unsigned int)
#define FORCE_FLUSH		_IOW(NAND_MAGIC, 0x8, unsigned int)

/******************************************************************************/
#define HANTRO_MAGIC              'o'
/******************************************************************************/
#define HANTRO_QUERY       _IOW(HANTRO_MAGIC, 0x0, u32)


#ifdef __cplusplus
}
#endif

#endif
