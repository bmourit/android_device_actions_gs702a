/*-------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#ifdef ANDROID
#include <cutils/properties.h>

#define LOG_TAG "wlan_loader"

#include <cutils/log.h>
#include <hardware_legacy/power.h>
#define PROGRAM_NAME    "wlan_loader"
#endif
	
/*---------------------------------------------------------*/

#ifdef ANDROID

#define ENABLE_LOG_ERROR
#define ENABLE_LOG_DEBUG

#ifdef ENABLE_LOG_ERROR
#define print_error(fmt, args...) \
    { SLOGE(fmt , ## args); }
#else
#define print_error(fmt, args...) \
    do { } while (0)
#endif /* ENABLE_LOG_ERROR */

#ifdef ENABLE_LOG_DEBUG
#define print_debug(fmt, args...) \
    { SLOGD(fmt , ## args); }
#else
#define print_debug(fmt, args...) \
    do { } while (0)
#endif /* ENABLE_LOG_MOUNT */

#else /* !ANDROID */

#define print_error printf
#define print_debug printf

#endif /* ifdef ANDROID */

/*---------------------------------------------------------*/
/// nano define

#ifndef WIFI_DRIVER_FW_PATH_PARAM
#define WIFI_DRIVER_FW_PATH_PARAM	"/proc/driver/eth0/firmware"
#endif

#ifndef WIFI_DRIVER_FW_PATH_STA
#define WIFI_DRIVER_FW_PATH_STA		 "/system/ventor/nanoradio/wifi/fw/x_mac.axf"
#endif

#ifdef WIFI_DRIVER_IFACE
#define PROC_XMAC_VERSION "/proc/driver/" WIFI_DRIVER_IFACE "/config/x-mac-version"  
#else
#define PROC_XMAC_VERSION "/proc/driver/eth0/config/x-mac-version"  
#endif

#define SHELL_EXEC "echo " WIFI_DRIVER_FW_PATH_STA " > " WIFI_DRIVER_FW_PATH_PARAM


/*---------------------------------------------------------*/

/*  Return '0' if success */
int init_driver(void)
{
  FILE *proc_xmac;
  int count =100; /* wait at most 20 seconds for completion */
  int bytes;

  SLOGE("[WLAN DEBUG] init_driver : ENTRY");
  SLOGE(SHELL_EXEC);

  system(SHELL_EXEC);
  
  //// check if the driver is up and running 
  while (count-- > 0) {
	sleep(2);
	/* open x_mac_version file */
	if((proc_xmac = fopen(PROC_XMAC_VERSION, "rb"))==NULL) {
		printf("Cannot open proc x_mac_version entry .\n");
		exit(1);
	}
  
	if (!feof(proc_xmac)) {
		char str_buf[256];
		str_buf[0]='\0'; /// clean the first char
		fgets(str_buf, 256, proc_xmac);
		str_buf[255]='\0'; ///  set the end of the string
		
		if(str_buf[0]!='\0')	/// check if we read the file 
		{
			SLOGE("[WLAN DEBUG] FW success download");
			printf("FW success download :\"%s\"\n",str_buf);
			return 0; // return success
		}

	}
	
	if(fclose(proc_xmac)==EOF) {
		printf("Error closing x_mac_version entry.\n");
		exit(1);
	}
	//printf("Debug fw unready\n"); 
  }
  
    return 1;
}

#ifdef ANDROID
int check_and_set_property(char *prop_name, char *prop_val)
{
    char prop_status[PROPERTY_VALUE_MAX];
    int count;

    for(count=4;( count != 0 );count--) {
        property_set(prop_name, prop_val);
        if( property_get(prop_name, prop_status, NULL) &&
            (strcmp(prop_status, prop_val) == 0) )
	    break;
    }
    if( count ) {
        print_debug("Set property %s = %s - Ok\n", prop_name, prop_val);
    }
    else {
        print_debug("Set property %s = %s - Fail\n", prop_name, prop_val);
    }
    return( count );
}
#endif

int main(int argc, char ** argv)
{
    SLOGE("[WLAN DEBUG] wlan_loader is called");

#ifdef ANDROID
    acquire_wake_lock(PARTIAL_WAKE_LOCK, PROGRAM_NAME);
#endif
	
    if( init_driver() != 0 ) {
	printf("init_driver() failed\n");
#ifdef ANDROID    
        check_and_set_property("wlan.driver.status", "failed"); /// set to the Android that the fw download faild
        release_wake_lock(PROGRAM_NAME);
#endif    
        return -1;
    }

#ifdef ANDROID    
    print_debug("Firmware loaded and running OK\n");
    check_and_set_property("wlan.driver.status", "ok");
    release_wake_lock(PROGRAM_NAME);
#endif    
    return 0;
}

