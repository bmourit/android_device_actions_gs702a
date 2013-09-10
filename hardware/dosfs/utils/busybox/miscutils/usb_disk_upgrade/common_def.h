#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include "mbr_info.h"

//#define DEBUG_DUMP_MBRC

#define BUILD_FOR_5003 0
#define BUILD_FOR_5009 1
#define BUILD_FOR_VL 0
#define BUILD_FOR_700A 1

#define USED_FOR_FBOOT		1


#if (USED_FOR_FBOOT == 1)

#define FIX_MBRC_SIZE_FASTBOOT	(90 * 1024)
#define TRD_MBRC_LEN_FASTBOOT		0x15600

#define FIX_MBRC_SIZE_NORMAL	(32 * 1024)
#define TRD_MBRC_LEN_NORMAL	0x6E00

#else

#define FIX_MBRC_SIZE	(32 * 1024)
#define TRD_MBRC_LEN	0x6E00

#endif


#define TRD_MBRC_OFFSET		0x1200

#define COMPILED_FOR_BUSYBOX

#ifdef COMPILED_FOR_BUSYBOX
#include "busybox.h"
#endif
//#define MBR_INFO_PATH
#define PHY_PART_NUM		3

#if(1 == BUILD_FOR_700A) 
#define MBRINFO_FILE		"/firmware/mbr_info_2.bin"
#else
#define MBRINFO_FILE		"/firmware/mbr_info.bin"
#endif

#define MBREC_FILE		"/firmware/mbrcf03.bin"
#define VMLINUXBIN_FILE		"/firmware/vmlinux.bin"

#define WHDBIN_FILE		"/firmware/whd_test.bin"
#define WHDDEV_PATH		"/dev/actb"
#define VMLINUXDEV_PATH		"/dev/actc"

#define ROOTFS_FILE		"/firmware/rtfs_image.bin"
#define OTHERS1_FILE		"/firmware/others1_img.bin"
#define OTHERS2_FILE		"/firmware/others2_img.bin"
#define CONFIGFS_FILE		"/firmware/configfs_img.bin"

#define FIRMWARE_FILE		"/media/ACTSDK600A.FW"
#define FIRMWARE_RANAME_FILE	"/media/ACTSDK600ARENAME.FW"
#define	UDISK_UPGRADE_FLAG_FILE	"/configfs/udupgrade_flag.bin" 
#define UDEV_PATH               "/sys/devices/platform/usb_disk/udev_part"

#define SD_PATH                 "/sys/block/mmcblk0"
#define MONITOR_DEVICE_PATH     "/dev/monitor"
//#define FIRMWARE_PATH tmp/mnt/actdisk/
#define BG_BUFFER_SIZE		800 * 480 * 2
#define PROCESS_BUFFER_SIZE	99 * 99 * 2
#define STATUS_BUFFER_SIZE	325 * 36 * 2

#define PROCESS_X_OFFSET	350
#define PROCESS_Y_OFFSET	134
#define PROCESS_X_LENGTH	99
#define PROCESS_Y_LENGTH	99

#define STATUS_X_OFFSET		237
#define STATUS_Y_OFFSET		303
#define STATUS_X_LENGTH		325
#define STATUS_Y_LENGTH		36

#define MAX_PHOTO_CNT		12
#define PROCESS_PHOTO_CNT	6

#define PHOTO_BG_PATH	"/firmware/BG.res"

#define PHOTO_0_PATH	"/firmware/WLTDISP0.res"
#define PHOTO_1_PATH	"/firmware/WLTDISP1.res"
#define PHOTO_2_PATH	"/firmware/WLTDISP2.res"
#define PHOTO_3_PATH	"/firmware/WLTDISP3.res"
#define PHOTO_4_PATH	"/firmware/WLTDISP4.res"
#define PHOTO_5_PATH	"/firmware/WLTDISP5.res"

#define PHOTO_END_0_PATH	"/firmware/WLTDISPF.res"
#define PHOTO_END_1_PATH	"/firmware/WLTDISPS.res"

#define PHOTO_STATUS_0_PATH	"/firmware/UPING.res"
#define PHOTO_STATUS_1_PATH	"/firmware/UPFAIL.res"
#define PHOTO_STATUS_2_PATH	"/firmware/UPOK.res"


#define FIRMWARE_BURNING_0		0
#define FIRMWARE_BURNING_1		1
#define FIRMWARE_BURNING_2		2
#define FIRMWARE_BURNING_3		3
#define FIRMWARE_BURNING_4		4
#define FIRMWARE_BURNING_5		5

#define FIRMWARE_END_FAIL		6
#define FIRMWARE_END_SUCCESS		7

#define FIRMWARE_BURNING_ING		8
#define FIRMWARE_BURNING_FAIL		9
#define FIRMWARE_BURNING_OK		10


#define DEBUG

#ifdef DEBUG
#define PRINT(fmt, args...) printf(fmt, ## args)
#else
#define PRINT(fmt, args...)
#endif

#define MBRINFO_TYPE	0
#define MBRC_TYPE	1
#define VMLINUX_TYPE	2

typedef struct BootPagePar {
	unsigned short PageNum;
	unsigned int SectorBitmapInPage;
} BootPagePar ;

typedef struct BootCmdParam {
	unsigned char two_plane_flag;
	/* sublk num */ 
	unsigned int BootBlkNum; 
	/* page param */
	BootPagePar PagePar; 
	unsigned char * Buf;
	unsigned char write_type;
} BootCmdParam ;

typedef struct BootErsParam {
	unsigned char two_plane_flag;
	unsigned int BootBlkNum;
} BootErsParam ;
/*
typedef struct BootFlashInfo {
	unsigned char PlaneCntPerDie;
	unsigned short PlaneOffset;
	unsigned int PageNumPerBlk;
	unsigned int SectNumPerPage;
} BootFlashInfo ;
*/

typedef struct BootFlashInfo {
	unsigned char LB_nand_flag;
	unsigned char PlaneCntPerDie;
	unsigned short PlaneOffset;
	unsigned int PageNumPerBlk;
	unsigned int SectNumPerPage;
} BootFlashInfo ;

#define NO_MBRC_SIZE

#define SECTOR_SIZE 512
#define MBRC_NUM 4
#define VMLINUX_NUM 4
#define AFINO_SIZE 2

#ifdef NO_MBRC_SIZE
//#define MBRC_SIZE_OFFSET        0X8         /* write mbrc size to mbrc, by sector */
#define FLASH_INFO_MBRC_OFFSET  0X8
#else
#define MBRC_SIZE_OFFSET        0X8         /* write mbrc size to mbrc, by sector */
#define FLASH_INFO_MBRC_OFFSET  0XC
#endif

/*MAX_VMLINUX_SIZE is the maximum size of vmlinux by sector */
#define MAX_VMLINUX_SIZE	8*1024*2     
#define VMLINUX_START_BLK   	4
#define VMLINUX_START_BLKA	2
#define VMLINUX_START_BLKB	4

//#define DEBUG_DUMP_MBRC
//#define DEBUG_DUMP_VMLINUX
//#define DEBUG_DUMP_MBR_INFO
//#define DEBUG_NO_FIRST_VMINUX


/* brom Ö§³ÖµÄ×î´óµÄpage size */
#define LB_SIZE_BROM    0x800

typedef struct UpgradeInfo_t {
	int upfp_mbrc;

#if (USED_FOR_FBOOT == 1)
	int upfp_whd;
#endif
	int upfp_vmlinux;
	int upfp_mbr_info;
	int upfp_flash;
	void * data_buf_pt;
	void * mbrc_mm;
#if (USED_FOR_FBOOT == 1)
	void * whd_mm;
#endif
	void * vmlinux_mm;
	BootFlashInfo    rawBootFlashInfo;
	unsigned int NandPageSize;
	unsigned int PageNumPerBlk;
    unsigned int NandFullBitmap;
    unsigned int NandFullBitmap2plane;
    unsigned int NandBlkSize;
    unsigned int vmlinux_size;
#if (USED_FOR_FBOOT == 1)
    unsigned int whd_size;
#endif
    unsigned int mbrc_size;
    unsigned int mbrc_page_size;
} UpgradeInfo ;

int display_init(void);
void display_screen(int signo);
int display_upgrade_process(int upgrade_status);


extern unsigned char * display_buffer;
extern int flash_fp;
extern UpgradeInfo ap_up_info;
extern int upgrade_mode;

void dump_flash_info(UpgradeInfo* up_info);
void dump_mem(unsigned char * buf, int size);
void dump_command(BootCmdParam * cmd);
unsigned int dwchecksum(char * buf, int start, int length);
unsigned short wchecksum(char * buf, int start, int length);
int alloc_UpgradeInfo(UpgradeInfo* up_info);
int free_UpgradeInfo(UpgradeInfo* up_info);
int config_mbrc(UpgradeInfo* up_info);
int write_mbrc(UpgradeInfo* up_info);
int config_vmlinux_checksum(UpgradeInfo* up_info);
int write_mbr_info_LB(UpgradeInfo* up_info);
int write_mbr_info_SB(UpgradeInfo* up_info);
int write_vmlinux(UpgradeInfo* up_info);
int dump_mbr(UpgradeInfo* up_info);
int dump_vmlinux(UpgradeInfo* up_info);
int burn_phy0area(void);
int burn_phy1area(void);
int burn_phy1area_vl(void);
void PartInfotoItemNum(unsigned int *begin_num, unsigned int *end_num, unsigned int phy_num, partition_info_t * partition_info);
int GetMbrFromUser(partition_info_t * partition_info_tbl);
int GetMbrFromKern(partition_info_t * partition_info_tbl); 
int DesideParam(void);
int set_mbrinfo(partition_info_t * partition_info_tbl);
int check_burnable(void);
int pad_vm(UpgradeInfo* up_info);
void raw_reboot(void);
int distroy_display_buffer(void);
int display_upgrade_success(void);
int display_upgrade_fail(void);
int display_upgrade_status(int upgrade_status);
int display_upgrade_process(int upgrade_process);

#endif
