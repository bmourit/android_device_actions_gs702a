#include <sys/mman.h> 
#include "common_def.h"
#include <fcntl.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <signal.h>

#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include  <linux/input.h>
#include <linux/fb.h>

#include "config.h"
#include "fbutils.h"

static int palette [] =
{
	0x000000, 0xffe080, 0xffffff, 0xe0c0a0
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))


unsigned char * display_buffer;
unsigned char * wlt_disp_buf[MAX_PHOTO_CNT];

void upgrade_put_string_center(void)
{
	static int i = 0;
	if(0 == (i%4)){
		put_string_center(xres/2,yres/2,"Upgrading....",1);
	}
	else if (1 == (i%4)){
		put_string_center(xres/2,yres/2,"Upgrading....",2);
	}
	else if (2 == (i%4)){
		put_string_center(xres/2,yres/2,"Upgrading....",3);
	}
	else	{
		put_string_center(xres/2,yres/2,"Upgrading....",4);
	}		
	i++;
	alarm(1);
}

int display_init(void)
{
	int i =0;
	if (open_framebuffer()) {
		close_framebuffer();
		exit(1);
	}
		for (i = 0; i < NR_COLORS; i++)
		setcolor (i, palette [i]);
	signal(SIGALRM, upgrade_put_string_center);
	alarm(1);
	return 0;
}

int display_no_firmware(void)
{
	alarm(0);
	clear_framebuffer();
	put_string_center(xres/2, yres/2, "No upgrade firmware found!", 2);	
	return 0;
}

int display_upgrade_success(void)
{
	alarm(0);
	clear_framebuffer();
	put_string_center(xres/2,yres/2,"Upgrad Succeed!",4);
	close_framebuffer();
	return 0;
}

int display_upgrade_fail(void)
{
	alarm(0);
	clear_framebuffer();
	put_string_center(xres/2,yres/2,"Upgrading Failed",5);
	close_framebuffer();
	return 0;
}
