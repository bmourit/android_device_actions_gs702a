

#include "includes.h"
//#include "wireless_copy.h"
#include <linux/wireless.h>

#include "common.h"
#include "driver.h"
#include "driver_wext.h"
#include "l2_packet.h"
#include <net/if_arp.h>

#include "nrx_lib.h"
#include "nrx_priv.h"
#include "nrx_proto.h"


int wpa_driver_nano_driver_cmd( void *priv, char *cmd, char *buf, size_t buf_len );

int wpa_driver_wext_driver_cmd(void *priv, char *cmd, char *buf,
                                        size_t buf_len)
{ 
  return wpa_driver_nano_driver_cmd(priv, cmd, buf, buf_len);
 
}

int wpa_driver_wext_combo_scan(void *priv, struct wpa_driver_scan_params *params)
{
  struct wpa_driver_wext_data *drv = priv;
  struct iwreq iwr;
  struct iw_scan_req req;
  const u8 *ssid = params->ssids[0].ssid;
  size_t ssid_len = params->ssids[0].ssid_len;
  char buf[64];
  int err;

  wpa_printf(MSG_DEBUG, "driver_nano_cmd_wext: %s\n", __func__);

  os_memset(&iwr, 0, sizeof(iwr));
  os_strlcpy(iwr.ifr_name, drv->ifname, IFNAMSIZ);

  if (ssid && ssid_len) {
    os_memset(buf, 0, sizeof(buf));
    os_memcpy(buf, ssid, ssid_len);
    wpa_printf(MSG_DEBUG, "driver_nano_cmd_wext: Scanning for a particular ssid = %s\n", buf);

    os_memset(&req, 0, sizeof(req));
    req.essid_len = ssid_len;
    req.bssid.sa_family = ARPHRD_ETHER;
    os_memset(req.bssid.sa_data, 0xff, ETH_ALEN);
    os_memcpy(req.essid, ssid, ssid_len);
    iwr.u.data.pointer = (caddr_t) &req;
    iwr.u.data.length = sizeof(req);
    iwr.u.data.flags = IW_SCAN_THIS_ESSID;
  }

  err = ioctl(drv->ioctl_sock, SIOCSIWSCAN, &iwr);
  if (err < 0) {
    wpa_printf(MSG_DEBUG, "driver_nano_cmd_wext: ioctl[SIOCSIWSCAN] failed, err = %d\n", err);
    return -1;
  }

  return 0;
}

int wpa_driver_signal_poll(void *priv, struct wpa_signal_info *si)
{
  struct wpa_driver_wext_data *drv = priv;
  nrx_context ctx;
  int status;
    
  wpa_printf(MSG_DEBUG, "%s ENTRY\n", __func__);
      
  status = nrx_init_context(&ctx, drv->ifname);
  if (status) {
	  wpa_printf(MSG_DEBUG, "nrx_init_context failed.\n");
	  return -1;
  }
	

  // get tx rate
  {	
    nrx_rate_t tx_rate;
   
    status = nrx_get_tx_rate(ctx, &tx_rate);
    if (status) {
	    wpa_printf(MSG_DEBUG, "nrx_get_tx_rate failed, exit code %d (%s)\n",
						      status, strerror(status));
	    goto failed;
    }

    /* tx_rate is in IEEE units (500kbps)
      * Convert it to Mbps and drop fractional part,
      * as Android GUI does not support decimal digits anyway. */
    si->current_txrate = (((uint32_t) tx_rate) / 2) * 1000;
  }
  
  // get rssi 
  {
    int32_t rssi;
    status = nrx_get_rssi(ctx, &rssi, NRX_DT_BEACON );
    if (status) {
	    wpa_printf(MSG_DEBUG, "nrx_get_rssi failed, exit code %d (%s)\n",
						      status, strerror(status));
	    goto failed;
    } 
	
    si->current_signal = rssi;
  }
  
  // get snr
  {
    int32_t snr;
    status = nrx_get_snr(ctx, &snr, NRX_DT_BEACON );
    if (status) {
	    wpa_printf(MSG_DEBUG, "nrx_get_snr failed, exit code %d (%s)\n",
						      status, strerror(status));
	    goto failed;
    } 
	
    si->current_noise = snr;
  }
  
  // set that we are above_threshold
  // TODO: we must change the above_threshold based on rssi
  si->above_threshold = 1;
  
  // get frequency
  {
    uint32_t frequency;
    nrx_get_curr_freq(ctx, &frequency);
    if (status) {
	    wpa_printf(MSG_DEBUG, "nrx_get_curr_freq failed, exit code %d (%s)\n",
						      status, strerror(status));
	    goto failed;
    } 
	
    si->frequency = frequency;
  }


  wpa_printf(MSG_DEBUG, "[%s] freq:%u noise:%d signal:%d txrate:%d threshold:%d \n", __func__,
	      si->frequency,
	      si->current_noise,
	      si->current_signal,
	      si->current_txrate,
	      si->above_threshold);

  nrx_free_context(ctx);
      
  return 0;
  
  failed:
  
  nrx_free_context(ctx);
  
  return -1;
}
