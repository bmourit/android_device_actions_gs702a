#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include "mbr_info.h"
#include "asoc_ioctl.h"

#define COMPILED_FOR_BUSYBOX

#ifdef COMPILED_FOR_BUSYBOX
#include "busybox.h"
#endif

#ifndef COMPILED_FOR_BUSYBOX
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sched.h>
#endif

#define CHECKSUM_FIRSTSTART_FLAG_FILE "/configfs/firststartflag.bin"
#define CHECKSUM_CONFIRM_FLAG_FILE "/configfs/checksumflag.bin"

#define CHECKSUM_DISKB_FILE "/sys/block/actb/checksum"
#define CHECKSUM_DISKC_FILE "/sys/block/actc/checksum"
#define CHECKSUM_DISKD_FILE "/sys/block/actd/checksum"
#define CHECKSUM_DISKF_FILE "/sys/block/actf/checksum"
#define CHECKSUM_DISKH_FILE "/sys/block/acth/checksum"

#define CHECKSUM_DISKB_FILE_SAVED "/configfs/checksum_b.bin"
#define CHECKSUM_DISKD_FILE_SAVED "/configfs/checksum_d.bin"
#define CHECKSUM_DISKF_FILE_SAVED "/configfs/checksum_f.bin"
#define CHECKSUM_DISKH_FILE_SAVED "/configfs/checksum_h.bin"

#define CHECKSUM_FILE "/configfs/checksum.bin"
#define OTHERS_DEVS_CKRESULT_PATH "/configfs/checksum_result.bin"

#define DEVB_PATH "/dev/actb"
#define DEVC_PATH "/dev/actc"

#define DEVD_PATH "/dev/actd"
#define DEVE_PATH "/dev/acte"

#define DEVF_PATH "/dev/actf"
#define DEVG_PATH "/dev/actg"

#define DEVH_PATH "/dev/acth"
#define DEVI_PATH "/dev/acti"

#define CHECKSUM_FAILED_MASK_D 0x01
#define CHECKSUM_FAILED_MASK_F 0x02
#define CHECKSUM_FAILED_MASK_H 0x04

#define CHECKSUM_PASSED		0
#define CHECK_BUF_SIZE 		1024
#define MAX_CMD_LENGTH		100
#define RESOURCE_DEV_CKFLAG	0

#define DEBUG

#ifdef DEBUG

#define DBL_ERR         0x1
#define DBL_WARNING     0x2
#define DBL_INFO        0x3
#define DBL_LOG         0x4

#define DEBUG_LEVEL	DBL_WARNING

#define PRINT(fmt, args...) printf(fmt, ## args)
#define PRINT_DBG(DebugLevel, fmt, args...) \
    do {                                                            \
        if (DebugLevel <= DEBUG_LEVEL) {                            \
            PRINT(fmt, ##args);                                     \
        }                                                           \
    } while (0)

#else
#define PRINT(fmt, args...)
#define PRINT_DBG(DebugLevel, fmt, args...)  do {} while (0)
#endif

char ck2rec_cmd[MAX_CMD_LENGTH];
static partition_info_t partition_info_from_nand[MAX_PARTITION];
static unsigned int sysfs_begin_item;
static unsigned int sysfs_end_item;

unsigned int CalDevCheckSum(char * dev_path);
unsigned int GetDevSavedCkSum(char * ckdev_save_path);
int SetCkSumRt(unsigned char CkSumResultMask);
int CSDevCheckSum(char * dev_path, char * ckdev_save_path);
int CheckAndRecoveryRootfs(void);
int CheckAndRecoveryOthers(void);

static int GetMbrFromKern(partition_info_t * partition_info_tbl) 
{
#ifdef DBG_BOOT
	int j = 0;
#endif
	int ret;
	int flash_fp;
	flash_fp = open("/dev/acta", O_RDWR);
	if (flash_fp < 0)
	{
		printf("open /dev/acta error.!!\n");
	}
	ret = ioctl(flash_fp, BOOT_GET_MBR, partition_info_tbl);
#ifdef DBG_BOOT
	PRINT("\n-----------------------\n");
	while(j < MBR_SIZE) {
		PRINT("%02x ", *((unsigned char *)partition_info_tbl + j));
		j++;
		if( j%16 == 0 )
			PRINT("\n");
	}
	PRINT("\n-----------------------\n");
#endif
	return ret;
}

static void PartInfotoItemNum(unsigned int *begin_num, unsigned int *end_num,
		unsigned int phy_num, partition_info_t * partition_info)
{
	unsigned int i;
	for (i = 0; i < MAX_PARTITION; i++) {
		if(partition_info[i].flash_ptn == phy_num) {
			*begin_num = i;
			break;
		}
	}
	for (i = 0; i < MAX_PARTITION; i++) {
		if (partition_info[i].flash_ptn == phy_num + 1) {
			*end_num = i - 1;
			break;
		}
		if (partition_info[i].partition_cap == 0xffffffff) {
			*end_num = i;
			break;
		}
	}
}

unsigned int CalDevCheckSum(char * dev_path)
{
	FILE *fp = NULL;
	unsigned char * check_buf = NULL;
	unsigned int check_sum = 0;
	unsigned int tmp_checksum = 0;
	int i, j = 0;
	int read_cnt;
	fp = fopen(dev_path, "rb");
	if (fp == NULL) {
		PRINT_DBG(DBL_ERR, "fail to open file.\n");
		exit(0);
	}
	check_buf = malloc(CHECK_BUF_SIZE);
	memset(check_buf, 0x0, CHECK_BUF_SIZE);
	if (check_buf == NULL) {
		PRINT_DBG(DBL_ERR, "fail to malloc.\n");
		exit(0);
	}
	while(!feof(fp)) {
		read_cnt = fread(check_buf, CHECK_BUF_SIZE, 1, fp);
		if (read_cnt != 1)
		{
			//PRINT_DBG(DBL_INFO, "reach the file end.\n");
			goto check_sum_end;
		}
		for (i = 0; i < CHECK_BUF_SIZE/(sizeof(unsigned int)); i++)
			tmp_checksum += ((unsigned int *)check_buf)[i];
		check_sum += tmp_checksum;
		j++;
		//PRINT_DBG(DBL_INFO, "record %d, tmp_checksum is %x\n", j, tmp_checksum);
		tmp_checksum = 0;
	}
check_sum_end:
	PRINT_DBG(DBL_INFO, "%s Check sum is %x.\n", dev_path, check_sum);
	fclose(fp);
	free(check_buf);
	return check_sum;
}

unsigned int GetDevSavedCkSum(char * ckdev_save_path)
{
	int read_cnt;
	int cksum_dev_fd;
	unsigned char tmp_ckbuf[4];
	unsigned int cksum_dev_saved;
	cksum_dev_fd = open(ckdev_save_path, O_RDWR | O_CREAT);
	if (cksum_dev_fd < 0) {
		PRINT_DBG(DBL_INFO, "Can't open %s\n", ckdev_save_path);
		return 0;
	}
	read_cnt = read(cksum_dev_fd, tmp_ckbuf, 4);
	cksum_dev_saved = (unsigned int)(tmp_ckbuf[0] | (tmp_ckbuf[1] << 8) | (tmp_ckbuf[2] <<16) | (tmp_ckbuf[3] << 24));
	close(cksum_dev_fd);
	return cksum_dev_saved;
}

int SetCkSumRt(unsigned char CkSumResultMask)
{
	int cksum_rt_fd;
	unsigned char tmp_ckresult = 0;
	tmp_ckresult = tmp_ckresult | CkSumResultMask;
	cksum_rt_fd = open(OTHERS_DEVS_CKRESULT_PATH, O_RDWR | O_SYNC | O_CREAT);
	if (cksum_rt_fd < 0) {
		PRINT_DBG(DBL_INFO, "Can't open %s\n", OTHERS_DEVS_CKRESULT_PATH);
		return -1;
	}
	write(cksum_rt_fd, &tmp_ckresult, 1);
	close(cksum_rt_fd);
	return 0;
}

int CSDevCheckSum(char * dev_path, char * ckdev_save_path)
{
	int cksum_dev_fd;
	unsigned int dev_cksum;
	unsigned char tmp_ckbuf[4];
	dev_cksum = CalDevCheckSum(dev_path);
	//PRINT_DBG(DBL_INFO, "dev cksum %x\n", devb_cksum);
	cksum_dev_fd = open(ckdev_save_path, O_RDWR | O_SYNC | O_CREAT);
	if (cksum_dev_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", ckdev_save_path);
		return -1;
	}
	
	tmp_ckbuf[0] = dev_cksum & 0xff;
	tmp_ckbuf[1] = (dev_cksum & 0xff00) >> 8;
	tmp_ckbuf[2] = (dev_cksum & 0xff0000) >> 16;
	tmp_ckbuf[3] = (dev_cksum & 0xff000000) >> 24;
	write(cksum_dev_fd, tmp_ckbuf, 4);
	close(cksum_dev_fd);
	return 0;
}

int CheckAndRecoveryRootfs(void)
{
	//int i;
	int read_cnt, cksum_fd, cksum_b_fd, cksum_c_fd;
	unsigned int cksum_standard, cksum_diskb, cksum_diskc;
	unsigned char ckbuf[4];

	/* read checksum file in configfs */
	cksum_fd = open(CHECKSUM_DISKB_FILE_SAVED, O_RDONLY);
	if (cksum_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", CHECKSUM_FILE);
		return -1;
	}
	read_cnt = read(cksum_fd, ckbuf, 4);
	/* for (i = 0; i < 20; i++) 
		PRINT_DBG("0x%x ", ckbuf[i]); */
	cksum_standard = (unsigned int)(ckbuf[0] | (ckbuf[1] << 8) | (ckbuf[2] << 16) | (ckbuf[3] << 24));
	PRINT_DBG(DBL_INFO, "read devB checksum from backfile: %x\n", cksum_standard);
	
	/* read checksum file calculated by flash driver */
	cksum_b_fd = open(CHECKSUM_DISKB_FILE, O_RDONLY);
	if (cksum_b_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", CHECKSUM_DISKB_FILE);
		return -1;
	}
	read_cnt = read(cksum_b_fd, ckbuf, 4);
	/* for (i = 0; i < 20; i++) 
		PRINT_DBG("0x%x ", ckbuf[i]); */
	cksum_diskb = (unsigned int)(ckbuf[0] | (ckbuf[1] << 8) | (ckbuf[2] << 16) | (ckbuf[3] << 24));
	PRINT_DBG(DBL_INFO, "read devB checksum from devfile: %x\n", cksum_diskb);

	/* compare the two checksum, to decide if we should recovery the disk */
	if (cksum_standard != cksum_diskb) {
		PRINT_DBG(DBL_WARNING, "check sum failed!! Recovery ...\n");
		PRINT_DBG(DBL_WARNING, "check the backup DEVimg ... \n");
		
		/* read checksum file calculated by flash driver */
		cksum_c_fd = open(CHECKSUM_DISKC_FILE, O_RDONLY);
		if (cksum_c_fd < 0) {
			PRINT_DBG(DBL_ERR, "Can't open %s\n", CHECKSUM_DISKC_FILE);
			return -1;
		}
		read_cnt = read(cksum_c_fd, ckbuf, 4);
		/* for (i = 0; i < 20; i++) 
		   PRINT_DBG("0x%x ", ckbuf[i]); */
		cksum_diskc = (unsigned int)(ckbuf[0] | (ckbuf[1] << 8) | (ckbuf[2] << 16) | (ckbuf[3] << 24));
		PRINT_DBG(DBL_INFO, "read devC checksum from devfile: %x\n", cksum_diskc);
		if (cksum_standard != cksum_diskc)
		{
			PRINT_DBG(DBL_WARNING, "Check Failed! Enter ADFU mod ...\n");
			close(cksum_fd);
			close(cksum_b_fd);
			close(cksum_c_fd);
			system("insmod /enter_adfu.ko");
			return -1;
		} else {
			PRINT_DBG(DBL_WARNING, "Check Passed! Enter Recovery mod ...\n");
			sprintf(ck2rec_cmd, "dd if=/dev/actc of=/dev/actb bs=1M count=%d",
					partition_info_from_nand[sysfs_begin_item].partition_cap);
			PRINT_DBG(DBL_WARNING, "ck2rec_cmd: %s\n", ck2rec_cmd);
			system(ck2rec_cmd);
			close(cksum_c_fd);
		}
	} else {
		PRINT_DBG(DBL_INFO, "check sum passed!! Disk Safe.\n");
	}
	close(cksum_fd);
	close(cksum_b_fd);
	return 0;
}

int CheckAndRecoveryOthers(void)
{
	int cksum_rt_fd;
	int read_cnt;
	unsigned char tmp_ckresult;
	cksum_rt_fd = open(OTHERS_DEVS_CKRESULT_PATH, O_RDWR | O_SYNC | O_CREAT);
	if (cksum_rt_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", OTHERS_DEVS_CKRESULT_PATH);
		return -1;
	}
	read_cnt = read(cksum_rt_fd, &tmp_ckresult, 1);
	if ((tmp_ckresult & CHECKSUM_FAILED_MASK_D) != 0) {
		unsigned int tmp_checksum_devd;
		unsigned int tmp_checksum_devd_cur;
		unsigned int tmp_checksum_devd_saved;
		
		PRINT_DBG(DBL_WARNING, "DevD checksum failed, Check the Backup...\n");
		
		tmp_checksum_devd_cur = CalDevCheckSum(DEVD_PATH);
		PRINT_DBG(DBL_WARNING, "The checksum of Current-DevD is: %x\n", tmp_checksum_devd_cur);
		
		tmp_checksum_devd_saved = GetDevSavedCkSum(CHECKSUM_DISKD_FILE_SAVED);
		PRINT_DBG(DBL_WARNING, "The saved-checksum of DevD is: %x\n", tmp_checksum_devd_saved);
		
		if (tmp_checksum_devd_cur == tmp_checksum_devd_saved) {
			
			PRINT_DBG(DBL_WARNING, "WEIRD!! The CUR-CKSUM of DevD PASSED!!\n");
			lseek(cksum_rt_fd, 0, 0);
			tmp_ckresult = tmp_ckresult & (~0x1);
			write(cksum_rt_fd, &tmp_ckresult, 1);
			goto recovery_end;	
		} else {
			
			PRINT_DBG(DBL_WARNING, "INDEED!! The CUR-CKSUM of DevD FAILED!!\n");
		
		}
		
		tmp_checksum_devd = CalDevCheckSum(DEVE_PATH);
		PRINT_DBG(DBL_WARNING, "deve-ck1 devd-ck2 : %x %x\n",
				tmp_checksum_devd, tmp_checksum_devd_saved);
		
		if (tmp_checksum_devd != tmp_checksum_devd_saved) {
			PRINT_DBG(DBL_WARNING, "DevD BACKUP CHECK FAILED, Enter ADFU mod ...\n");
			close(cksum_rt_fd);
			system("insmod /enter_adfu.ko");
			return -1;
		} else {
		
			PRINT_DBG(DBL_WARNING, "DevD BACKUP CHECK PASSED, Enter ROCOVERY mod ...\n");
			sprintf(ck2rec_cmd, "dd if=/dev/acte of=/dev/actd bs=1M count=%d",
					partition_info_from_nand[sysfs_begin_item + 2].partition_cap);
			PRINT_DBG(DBL_WARNING, "ck2rec_cmd: %s\n", ck2rec_cmd);
			system(ck2rec_cmd);
			PRINT_DBG(DBL_WARNING, "Recoverying end, clear MASK...\n");
			lseek(cksum_rt_fd, 0, 0);
			tmp_ckresult = tmp_ckresult & (~0x1);
			write(cksum_rt_fd, &tmp_ckresult, 1);
		}
	} else {
		PRINT_DBG(DBL_INFO, "DevD clean, passed!!\n");
	}
	if ((tmp_ckresult & CHECKSUM_FAILED_MASK_F) != 0) {
		unsigned int tmp_checksum_devf;
		unsigned int tmp_checksum_devf_cur;
		unsigned int tmp_checksum_devf_saved;
		
		PRINT_DBG(DBL_WARNING, "DevF checksum failed, Check the Backup...\n");
		
		tmp_checksum_devf_cur = CalDevCheckSum(DEVF_PATH);
		PRINT_DBG(DBL_WARNING, "The checksum of Current-DevF is: %x\n", tmp_checksum_devf_cur);
		
		tmp_checksum_devf_saved = GetDevSavedCkSum(CHECKSUM_DISKF_FILE_SAVED);
		PRINT_DBG(DBL_WARNING, "The saved-checksum of DevF is: %x\n", tmp_checksum_devf_saved);
	
		if (tmp_checksum_devf_cur == tmp_checksum_devf_saved) {
			
			PRINT_DBG(DBL_WARNING, "WEIRD!! The CUR-CKSUM of DevF PASSED!!\n");
			lseek(cksum_rt_fd, 0, 0);
			tmp_ckresult = tmp_ckresult & (~0x2);
			write(cksum_rt_fd, &tmp_ckresult, 1);
			goto recovery_end;	
		} else {
			
			PRINT_DBG(DBL_WARNING, "INDEED!! The CUR-CKSUM of DevF FAILED!!\n");
		
		}



		tmp_checksum_devf = CalDevCheckSum(DEVG_PATH);
		PRINT_DBG(DBL_WARNING, "devg-ck1 devf-ck2 : %x %x\n",
				tmp_checksum_devf, tmp_checksum_devf_saved);
		if (tmp_checksum_devf != tmp_checksum_devf_saved) {
			PRINT_DBG(DBL_WARNING, "DevF BACKUP CHECK FAILED, Enter ADFU mod ...\n");
			close(cksum_rt_fd);
			system("insmod /enter_adfu.ko");
			return -1;
		} else {
			PRINT_DBG(DBL_WARNING, "DevF BACKUP CHECK PASSED, Enter ROCOVERY mod ...\n");
			sprintf(ck2rec_cmd, "dd if=/dev/actg of=/dev/actf bs=1M count=%d",
					partition_info_from_nand[sysfs_begin_item + 4].partition_cap);
			PRINT_DBG(DBL_WARNING, "ck2rec_cmd %s\n", ck2rec_cmd);
			system(ck2rec_cmd);
			PRINT_DBG(DBL_WARNING, "Recoverying end, clear MASK...\n");
			lseek(cksum_rt_fd, 0, 0);
			tmp_ckresult = tmp_ckresult & (~0x2);
			write(cksum_rt_fd, &tmp_ckresult, 1);
		}
	} else {
		PRINT_DBG(DBL_INFO, "DevF clean, passed!!\n");
	}
#if RESOURCE_DEV_CKFLAG
	/* for future use!! */
	if ((tmp_ckresult & CHECKSUM_FAILED_MASK_H) != 0) {
		PRINT_DBG("DevH checksum failed, recoverying ...\n");
		PRINT_DBG("Recoverying end, clear MASK...\n");
		lseek(cksum_rt_fd, 0, 0);
		tmp_ckresult = tmp_ckresult & (~0x4);
		write(cksum_rt_fd, &tmp_ckresult, 1);
	} else {
		PRINT_DBG("DevH clean, passed!!\n");
	}
#endif

recovery_end:
	close(cksum_rt_fd);
	return 0;
}

#ifdef COMPILED_FOR_BUSYBOX
int ck2recovery_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int ck2recovery_main(int argc, char **argv)
#else
int main(int argc, char ** argv)
#endif
{
	//int i;
	int read_cnt;
	
	int cksum_flag_fd;
	int first_start_flag_fd;
	
	unsigned int checksum_devd;  /*  */
	unsigned int checksum_devd_saved;
	unsigned int checksum_devf;  /*  */
	unsigned int checksum_devf_saved;
	unsigned int checksum_devh;  /*  */
	unsigned int checksum_devh_saved;

	unsigned char ck_flag;
	unsigned char first_start_flag;
	//unsigned char * ckbuf;
	/* mount /dev/actf /configfs*/
	
#ifndef COMPILED_FOR_BUSYBOX
	nice(19);
#endif
	/****************************************************/
	/* if first start, Cal and Save the checksum of DEVs*/
	PRINT_DBG(DBL_INFO, "******** initialize checking ********\n");
	first_start_flag_fd = open(CHECKSUM_FIRSTSTART_FLAG_FILE, O_RDWR | O_SYNC);
	if (first_start_flag_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", CHECKSUM_FIRSTSTART_FLAG_FILE);
		return -1;
	}
	read_cnt = read(first_start_flag_fd, &first_start_flag, 1);
	PRINT_DBG(DBL_INFO, "read first start flag : %x\n", first_start_flag);
	if (first_start_flag != 0) {	
		int write_cnt;
		PRINT_DBG(DBL_INFO, "******** first start ********\n");
		CSDevCheckSum(DEVB_PATH, CHECKSUM_DISKB_FILE_SAVED);
		
		CSDevCheckSum(DEVD_PATH, CHECKSUM_DISKD_FILE_SAVED);
		CSDevCheckSum(DEVF_PATH, CHECKSUM_DISKF_FILE_SAVED);
#if RESOURCE_DEV_CKFLAG
		CSDevCheckSum(DEVH_PATH, CHECKSUM_DISKD_FILE_SAVED);
#endif
		lseek(first_start_flag_fd, 0, SEEK_SET);
		first_start_flag = 0;
		write_cnt = write(first_start_flag_fd, &first_start_flag, 1);
		PRINT_DBG(DBL_INFO, "******** first start end ********\n\n\n");
	
	} else {
		PRINT_DBG(DBL_INFO, "not first start, skip this stage.\n");
	}
	close(first_start_flag_fd);
	PRINT_DBG(DBL_INFO, "******** initialize checking end ********\n\n\n");
	/****************************************************/
	
	/*********************************************************************************/
	cksum_flag_fd = open(CHECKSUM_CONFIRM_FLAG_FILE, O_RDONLY);
	if (cksum_flag_fd < 0) {
		PRINT_DBG(DBL_ERR, "Can't open %s\n", CHECKSUM_CONFIRM_FLAG_FILE);
		return -1;
	}
	read_cnt = read(cksum_flag_fd, &ck_flag, 1);
	close(cksum_flag_fd);
	PRINT_DBG(DBL_INFO, "read confirm flag: %x\n", ck_flag);
	if (ck_flag == 0) {
		PRINT_DBG(DBL_INFO, "Skip Check, do nothing ...\n");
		return 0;
	}

	GetMbrFromKern(partition_info_from_nand); 
	PartInfotoItemNum(&sysfs_begin_item, &sysfs_end_item, 1, partition_info_from_nand);
	memset(ck2rec_cmd, '\0', MAX_CMD_LENGTH);

	/** ck_flag != 0, it means that the things we will do bellow are wanted by some clients **/
	if (strcmp("fc-stage", argv[1]) == 0) {
		PRINT_DBG(DBL_INFO, "******** first stage start ********\n");
		if (CheckAndRecoveryRootfs() < 0)
			PRINT_DBG(DBL_ERR, "Err Occured while rootfs check&recoverying !!\n\n");
		else
			PRINT_DBG(DBL_INFO, "Rootfs Check & Recovery end.\n\n");
		if (CheckAndRecoveryOthers() < 0)
			PRINT_DBG(DBL_ERR, "Err Occured while Others check&recoverying !!\n");
		else
			PRINT_DBG(DBL_INFO, "Others Check & Recovery end.\n");
		/*********************************************************************************/
		PRINT_DBG(DBL_INFO, "******** first stage end ********\n\n\n");
		return 0;
	} else if (strcmp("sc-stage", argv[1]) == 0) {
		PRINT_DBG(DBL_INFO, "******** second stage start ********\n");
		/*********************************************************************************/
		/*** Second stage, run in lowlevel ***/
		/* CheckSum the "Others",  when check err, mark and reboot */
		
		/* dev-D */
		checksum_devd = CalDevCheckSum(DEVD_PATH);
		checksum_devd_saved = GetDevSavedCkSum(CHECKSUM_DISKD_FILE_SAVED);
		PRINT_DBG(DBL_INFO, "devd-ck1 devd-ck2 : %x %x\n", checksum_devd, checksum_devd_saved);
		if (checksum_devd != checksum_devd_saved) {
			/* check fail, mark and reboot */
			PRINT_DBG(DBL_WARNING, "Something wrong with DevD!! system will auto-reboot and reovery!!\n");
			if (SetCkSumRt(CHECKSUM_FAILED_MASK_D) < 0)
				PRINT_DBG(DBL_ERR, "Fail to save Dev-D checksum result.\n");
			PRINT_DBG(DBL_WARNING, "Please type REBOOT\n");
			goto quit_ck2rec;
			//system("reboot");
		}
		
		/* dev-F */
		checksum_devf = CalDevCheckSum(DEVF_PATH);
		checksum_devf_saved = GetDevSavedCkSum(CHECKSUM_DISKF_FILE_SAVED);
		PRINT_DBG(DBL_INFO, "devf-ck1 devf-ck2 : %x %x\n", checksum_devf, checksum_devf_saved);
		if (checksum_devf != checksum_devf_saved) {
			/* check fail, mark and reboot */
			PRINT_DBG(DBL_WARNING, "Something wrong with DevF!! system will auto-reboot and reovery!!\n");
			if (SetCkSumRt(CHECKSUM_FAILED_MASK_F) < 0)
				PRINT_DBG(DBL_ERR, "Fail to save Dev-F checksum result.\n");
			PRINT_DBG(DBL_WARNING, "Please type REBOOT\n");
			goto quit_ck2rec;
			//system("reboot");
		}
#if RESOURCE_DEV_CKFLAG
		/* for future use */
		/* dev-H */
		checksum_devh = CalDevCheckSum(DEVH_PATH);
		checksum_devh_saved = GetDevSavedCkSum(CHECKSUM_DISKH_FILE_SAVED);
		PRINT_DBG("devh-ck1 devh-ck2 : %x %x\n", checksum_devh, checksum_devh_saved);
		if (checksum_devh != checksum_devh_saved) {
			/* check fail, mark and reboot */
			PRINT_DBG("Something wrong with DevH!! system will auto-reboot and reovery!!\n");
			if (SetCkSumRt(CHECKSUM_FAILED_MASK_H) < 0)
				PRINT_DBG("Fail to save Dev-H checksum result.\n");
			PRINT_DBG("Please type REBOOT\n");
			goto quit_ck2rec;
			//system("reboot");
		}
#endif
		/* check pass, mark and quit */
		if (SetCkSumRt(CHECKSUM_PASSED) < 0)
			PRINT_DBG(DBL_ERR, "Fail to save checksum result.\n");
		
		PRINT_DBG(DBL_INFO, "******** second stage end ********\n");
		return 0;
	} else {
		PRINT_DBG(DBL_ERR, "nosence param!!\n");
quit_ck2rec:
		return 0;
	}	
	/*********************************************************************************/
}
