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

#define FLASH_DEV_PATH		"/dev/acta"

#define DEBUG

#ifdef DEBUG
#define PRINT(fmt, args...) printf(fmt, ## args)
#else
#define PRINT(fmt, args...)
#endif

#ifdef COMPILED_FOR_BUSYBOX
int flush_flash_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int flush_flash_main(int argc, char **argv)
#else
int main(int argc, char ** argv)
#endif
{
	int ret = 0;
	int flash_fp = open(FLASH_DEV_PATH, O_RDONLY);
	if (flash_fp < 0) {
		PRINT("Open Flash dev failed!!\n");
		return -1;
	}
	if (strcmp("fp", argv[1]) == 0) {
		ret = ioctl(flash_fp, FORCE_FLUSH, 1);
		if (ret < 0) {
			PRINT("flush Part1 failed!!\n");
			ret = -1;
		}
	} else if (strcmp("sp", argv[1]) == 0) {
		ret = ioctl(flash_fp, FORCE_FLUSH, 2);
		if (ret < 0) {
			PRINT("flush Part2 failed!!\n");
			ret = -1;
		}
	} else if (strcmp("ap", argv[1]) == 0) {
		ret = ioctl(flash_fp, FORCE_FLUSH, 1);
		if (ret < 0) {
			PRINT("flush Part1 failed!!\n");
			ret = -1;
		}
		ret = ioctl(flash_fp, FORCE_FLUSH, 2);
		if (ret < 0) {
			PRINT("flush Part2 failed!!\n");
			ret = -1;
		}
	} else {
		PRINT("Invalid Param!!\n");
		ret = -1;
	}
	close(flash_fp);
	return ret;
}
