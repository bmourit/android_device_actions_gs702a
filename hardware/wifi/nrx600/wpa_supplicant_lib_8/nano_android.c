/*
 * WPA Supplicant - Driver interaction with nanoradio driver
 * (c) 2009 nanoradio A.B>
 */

#include "includes.h"
//#include "wireless_copy.h"
#include <linux/wireless.h>

#include "common.h"
#include "driver.h"
#include "driver_wext.h"
#include "l2_packet.h"

#include "nrx_lib.h"
#include "nrx_priv.h"
#include "nrx_proto.h"

/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_get_mac_addr
Routine Description: return WLAN MAC address
Arguments: See wpa_driver_nano_driver_cmd()
Return Value: actual buffer length on success, negative on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_get_mac_addr( void *priv, char *buf, size_t buf_len )
{
	struct wpa_driver_wext_data *drv = priv;
	nrx_context ctx;
	int status;
	u8 mac_addr[6];
	size_t len = sizeof(mac_addr);

	wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);

	status = nrx_init_context(&ctx, drv->ifname);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_init_context failed.\n");
		return -1;
	}

	status = nrx_get_mib_val(ctx, "2.1.1.1", (void*) mac_addr, &len);
	nrx_free_context(ctx);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_get_mib_val failed, exit code %d (%s)\n",
							  status, strerror(status));
		return -1;
	}

	if (len != sizeof(mac_addr)) {
		wpa_printf(MSG_DEBUG, "nrx_get_mib_val returned %d bytes.\n", len);
		return -1;
	}

	return snprintf(buf, buf_len, "Macaddr = " MACSTR "\n", MAC2STR(mac_addr));
}

/**
 * wpa_driver_nano_get_ssid - Get SSID, SIOCGIWESSID
 * @priv: Pointer to private nano data from wpa_driver_nano_init()
 * @ssid: Buffer for the SSID; must be at least 32 bytes long
 * Returns: SSID length on success, -1 on failure
 */
int wpa_driver_nano_get_ssid(void *priv, u8 *ssid)
{
	struct wpa_driver_wext_data *drv = priv;
	struct iwreq iwr;
	int ret = 0;

	os_memset(&iwr, 0, sizeof(iwr));
	os_strlcpy(iwr.ifr_name, drv->ifname, IFNAMSIZ);
	iwr.u.essid.pointer = (caddr_t) ssid;
	iwr.u.essid.length = 32;

	if (ioctl(drv->ioctl_sock, SIOCGIWESSID, &iwr) < 0) {
		wpa_printf(MSG_ERROR, "ioctl[SIOCGIWESSID]");
		ret = -1;
	} else {
		ret = iwr.u.essid.length;
		if (ret > 32)
			ret = 32;
		/* Some drivers include nul termination in the SSID, so let's
		 * remove it here before further processing. WE-21 changes this
		 * to explicitly require the length _not_ to include nul
		 * termination. */
		if (ret > 0 && ssid[ret - 1] == '\0' &&
		    drv->we_version_compiled < 21)
			ret--;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_get_rssi
Routine Description: returns empty string if not connected,
                     returns string "<ssid> rssi <rssi>\n" if connected
Arguments: priv - pointer to private data structure
Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_get_rssi( void *priv, char *buf, size_t buf_len )
{
	extern int wpa_driver_nano_get_ssid(void *priv, u8 *ssid);

	struct wpa_driver_wext_data *drv = priv;
	nrx_context ctx;
	int status;
	size_t len;
	int32_t rssi;

	wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);

	if (buf_len < 33)
		return (int) buf_len; /* indicate insufficient buffer length */
		
	status = wpa_driver_nano_get_ssid(priv, (u8 *) buf);
	if (status < 0) {
		wpa_printf(MSG_DEBUG, "wpa_driver_nano_get_ssid failed.\n");
		return -1;
	}

	len = (size_t) status;
	if (len == 0) {
		wpa_printf(MSG_DEBUG, "wpa_driver_nano_get_ssid returned null SSID.\n");
		*buf = 0;
		return 0; /* return empty string */
	}

	status = nrx_init_context(&ctx, drv->ifname);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_init_context failed.\n");
		return -1;
	}

	status = nrx_get_rssi(ctx, &rssi, NRX_DT_BEACON );
	nrx_free_context(ctx);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_get_rssi failed, exit code %d (%s)\n",
							  status, strerror(status));
		return -1;
	}

	len += snprintf(buf + len, buf_len - len, " rssi %d\n", rssi);
	return len;
}

/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_get_linkspeed
Routine Description: returns RSSI if connected, empty string if not connected.
Arguments: priv - pointer to private data structure
Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_get_linkspeed( void *priv, char *buf, size_t buf_len )
{
	struct wpa_driver_wext_data *drv = priv;
	nrx_context ctx;
	int status;
	nrx_rate_t rx_rate;
	uint32_t linkspeed;

	wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);

	status = nrx_init_context(&ctx, drv->ifname);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_init_context failed.\n");
		return -1;
	}

	status = nrx_get_rx_rate(ctx, &rx_rate);
	nrx_free_context(ctx);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_get_rx_rate failed, exit code %d (%s)\n",
							  status, strerror(status));
		return -1;
	}

	/* rx_rate is in IEEE units (500kbps)
	 * Convert it to Mbps and drop fractional part,
	 * as Android GUI does not support decimal digits anyway. */
	linkspeed = ((uint32_t) rx_rate) / 2;
	return snprintf(buf, buf_len, "LinkSpeed %u\n", linkspeed);
}

/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_disable_default_scan_job
Routine Description: Disables the default scan job.
                     Auxilaury function, called by wpa_driver_nano_scan_passive
					 and/or wpa_driver_nano_scan_active
Arguments: priv - pointer to private data structure
Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
static int wpa_driver_nano_disable_default_scan_job(void *priv)
{
	struct wpa_driver_wext_data *drv = priv;
	nrx_context ctx;
	int status;

	status = nrx_init_context(&ctx, drv->ifname);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_init_context failed.\n");
		return -1;
	}

	status  = nrx_set_scan_job_state(ctx, 0, NRX_SCAN_JOB_STATE_SUSPENDED);
	nrx_free_context(ctx);
	if (status) {
		wpa_printf(MSG_DEBUG, "nrx_set_scan_job_state failed, exit code %d (%s)\n",
							  status, strerror(status));
		return -1;
	}
	
	return 0;
}

/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_scan_passive
Routine Description: Selects passive scan mode.
Arguments: priv - pointer to private data structure
Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_scan_passive( void *priv, char *buf, size_t buf_len )
{
	wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);
	
//	wpa_driver_nano_disable_default_scan_job(priv);

	return 0; /* Indicate success */
}


/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_scan_active
Routine Description: Selects active scan mode.
Arguments: priv - pointer to private data structure
Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_scan_active( void *priv, char *buf, size_t buf_len )
{
	wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);

//	wpa_driver_nano_disable_default_scan_job(priv);

	return 0; /* Indicate success */
}


/*-----------------------------------------------------------------------------
Routine Name: wpa_driver_nano_driver_cmd
Routine Description: executes driver-specific commands
Arguments:
   priv - pointer to private data structure
   cmd - command
   buf - return buffer
   buf_len - buffer length
   Return Value: actual buffer length on success, -1 on failure
-----------------------------------------------------------------------------*/
int wpa_driver_nano_driver_cmd( void *priv, char *cmd, char *buf, size_t buf_len )
{
	int ret;

	wpa_printf(MSG_DEBUG, "%s ENTRY, cmd = %s", __func__, cmd);
	memset(buf, 0, buf_len);

	if( os_strcasecmp(cmd, "macaddr") == 0 )
		ret = wpa_driver_nano_get_mac_addr(priv, buf, buf_len);
    else if( os_strcasecmp(cmd, "rssi-approx") == 0 )
        ret = wpa_driver_nano_get_rssi(priv, buf, buf_len);
	else if( os_strcasecmp(cmd, "rssi") == 0 )
		ret = wpa_driver_nano_get_rssi(priv, buf, buf_len);
	else if( os_strcasecmp(cmd, "linkspeed") == 0 )
		ret = wpa_driver_nano_get_linkspeed(priv, buf, buf_len);
	else if ( os_strcasecmp(cmd, "scan-passive") == 0 )
		ret = wpa_driver_nano_scan_passive(priv, buf, buf_len);
	else if ( os_strcasecmp(cmd, "scan-active") == 0 )
		ret = wpa_driver_nano_scan_active(priv, buf, buf_len);
	else {
		wpa_printf(MSG_DEBUG, "unknown command");
		ret = -1;
	}

	if (ret >= (int) buf_len) {
		wpa_printf(MSG_DEBUG, "insufficient buffer length");
		memset(buf, 0, buf_len);
		ret = -1;
	}

	if (ret < 0)
		wpa_printf(MSG_DEBUG, "%s FAILED", __func__);
	else
		wpa_printf(MSG_DEBUG, "%s EXIT '%s', len = %d", __func__, buf, ret);

	return ret;
}
