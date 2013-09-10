#include <stdio.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include "common_def.h"
#include "../../include/monitor.h"
#include "asoc_ioctl.h"


#ifdef COMPILED_FOR_BUSYBOX
int udisk_upgrade_main(void) MAIN_EXTERNALLY_VISIBLE;
int udisk_upgrade_main(void)
#else
int main()
#endif
{
	int ret=0;
	char fw_name[50];
	char info_name[50];
	int i=0;
	
	printf("Busybox udisk_upgrade start! \r\n");

	system("busybox insmod /lib/modules/mmc_core.ko");
	system("busybox insmod /lib/modules/atv5201_mmc.ko");
	system("busybox insmod /lib/modules/mmc_block.ko ");

	
	system("cd  /");
	system("mkdir -p /media");
	system("mkdir -p /firmware");
	system("mknod /dev/loop0 b 7 0");
	system("mknod /dev/loop1 b 7 1");
	system("busybox umount /proc");
	system("mount -t proc proc /proc >/dev/null 2>&1");
	
	system("mknod  /dev/sd_upgrade b 179 0");
	sprintf(fw_name, "/dev/sd_upgrade");
	if (access(fw_name, F_OK) != 0)	
	{
		printf("::%s is not exist\r\n", fw_name);
	} else {
		printf("::%s is  exist\r\n", fw_name);
	}
	
       for (i = 0 ; i<6; i++) 
	{
               if (!access("/sys/block/mmcblk0/dev", R_OK)) 
		 {
                       printf("access sys mmcblk0 ok(%d)\n", i);
                       break;
               }
               sleep(1);
       }

	ret = system("mount -t vfat /dev/sd_upgrade /media > /dev/null 2>&1");	
	printf("ret = %d\n", ret);	
	//system("mount -t vfat /dev/sd_upgrade /media");
	sprintf(fw_name, "/media/UPGRADE_FAST.FW");
	//if(0 != ret)
	if (access(fw_name, F_OK) != 0)
	{		
		//system("umount /media");
		system("rm /dev/sd_upgrade");
		system("mknod -m 777 /dev/sd_upgrade b 179 1");
		ret = system("mount -t vfat /dev/sd_upgrade /media > /dev/null 2>&1");	
		printf("ret = %d\n", ret);	
		//system("mount -t vfat /dev/sd_upgrade /media");
		//if(-1 == ret)
		if (access(fw_name, F_OK) != 0)
		{
			printf("There is no UPGRADE_FAST.FW \n");
			system("umount /media");
			system("rm /dev/sd_upgrade");
			display_no_firmware();			
			return -1;
		} else {
			printf("UPGRADE_FAST.FW EXIST\n");
		}
	} else {
		printf("UPGRADE_FAST.FW EXIST\n");
	}
  
	system("mount -o loop /media/UPGRADE_FAST.FW /firmware");

	sprintf(info_name, "/firmware/udisk_upgrade");
	if (0 == access(info_name, F_OK))
	{
		printf("find udisk_upgrade\n");
		system("cp /firmware/udisk_upgrade /usr/udisk_upgrade");
		printf("run udisk_upgrade\n");
		system("/usr/udisk_upgrade");
	} else {
		printf("There is no udisk_upgrade");
        }

	
	system("busybox rmmod /lib/modules/mmc_block.ko ");
	system("busybox rmmod /lib/modules/atv5201_mmc.ko");
	system("busybox rmmod /lib/modules/mmc_core.ko");

	return 0;
}
