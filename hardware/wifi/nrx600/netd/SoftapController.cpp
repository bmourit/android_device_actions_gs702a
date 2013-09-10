/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/wireless.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#define LOG_TAG "SoftapController"
#include <cutils/log.h>
#include <netutils/ifc.h>
#include <private/android_filesystem_config.h>
#include "wifi.h"

#include "SoftapController.h"

static const char HOSTAPD_CONF_FILE[]    = "/data/misc/wifi/hostapd.conf";

SoftapController::SoftapController() {
    mPid = 0;
    mSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSock < 0)
        ALOGE("Failed to open socket");
    memset(mIface, 0, sizeof(mIface));
}

SoftapController::~SoftapController() {
    if (mSock >= 0)
        close(mSock);
}

int SoftapController::setCommand(char *iface, const char *fname, unsigned buflen) {
    return 0;
}

int SoftapController::startDriver(char *iface) {
    int ret=0;
    
    ALOGE("nanoradio SoftapController!");
    
    if (mSock < 0) {
        ALOGE("Softap driver start - failed to open socket");
        return -1;
    }
    if (!iface || (iface[0] == '\0')) {
        ALOGE("Softap driver start - wrong interface");
        iface = mIface;
    }
    
    /*
    // if the driver is allready loaded then remove it
    if(is_wifi_driver_loaded()){
        ALOGE("Softap driver start - unload the old driver before to load it again!!!");
        // unload the driver
        wifi_unload_driver();
        
        usleep(AP_BSS_START_DELAY);
    }

    // load the driver
    ret =  wifi_load_driver();
    if (ret < 0) {
       ALOGE("Softap driver start - failed to load driver");
        return -1;
    }
    */
    
    // wait the driver to be ready
    usleep(30000);
    
    // load FW
    ret = wifi_change_fw_path(wifi_get_fw_path(WIFI_GET_FW_PATH_AP));
    if (ret) {
        ALOGE("Softap fwReload - failed: %d", ret);
    }
    else {
        usleep(1500000);
        ALOGE("Softap fwReload - Ok");
    }
    
    ALOGE("Softap driver start: %d iface:%s", ret,iface);
    return ret;
    
}

int SoftapController::stopDriver(char *iface) {
    int ret=0;

    ALOGE("Softap driver stop: %d iface:%s", ret,iface);
    return ret;
    
    /*
    if (mSock < 0) {
        ALOGE("Softap driver stop - failed to open socket");
        return -1;
    }
    if (!iface || (iface[0] == '\0')) {
        ALOGE("Softap driver stop - wrong interface");
        iface = mIface;
    }
    
    // unload the driver
    ret =  wifi_unload_driver();
    if (ret < 0) {
        ALOGE("Softap driver stop - failed to unload driver");
        return -1;
    }
    
    ALOGE("Softap driver stop: %d iface:%s", ret,iface);
    return ret;
    */
}

int SoftapController::startSoftap() {
    pid_t pid = 1;
    int ret = 0;

    if (mPid) {
        ALOGE("Softap already started");
        return 0;
    }
    if (mSock < 0) {
        ALOGE("Softap startap - failed to open socket");
        return -1;
    }
    
    if ((pid = fork()) < 0) {
        ALOGE("fork failed (%s)", strerror(errno));
        return -1;
    }

    if (!pid) {

        ensure_entropy_file_exists();
        if (execl("/system/bin/hostapd", "/system/bin/hostapd",
                  "-e", WIFI_ENTROPY_FILE,"-dd",
                  HOSTAPD_CONF_FILE, (char *) NULL)) {
            ALOGE("execl failed (%s)", strerror(errno));
        }

        ALOGE("Should never get here!");
        return -1;
    } else {
        mPid = pid;
        ALOGE("Softap startap - Ok pid:%d",mPid);
        usleep(AP_BSS_START_DELAY);
    }
    return ret;

}

int SoftapController::stopSoftap() {
    int ret = 0;

    if (mPid == 0) {
        ALOGE("Softap already stopped");
        return 0;
    }
    
    ALOGE("Stopping Softap service mPid=%d",mPid);
    kill(mPid, SIGTERM);
    waitpid(mPid, NULL, 0);

    if (mSock < 0) {
        ALOGE("Softap stopap - failed to open socket");
        return -1;
    }
    
    mPid = 0;
    usleep(AP_BSS_STOP_DELAY);
    return ret;
}

bool SoftapController::isSoftapStarted() {
    return (mPid != 0 ? true : false);
}

int SoftapController::addParam(int pos, const char *cmd, const char *arg)
{
    if (pos < 0)
        return pos;
    if ((unsigned)(pos + strlen(cmd) + strlen(arg) + 1) >= sizeof(mBuf)) {
        ALOGE("Command line is too big");
        return -1;
    }
    pos += sprintf(&mBuf[pos], "%s=%s,", cmd, arg);
    return pos;
}

/*
 * Arguments:
 *      argv[2] - wlan interface
 *      argv[3] - softap interface
 *      argv[4] - SSID
 *	argv[5] - Security
 *	argv[6] - Key
 *	argv[7] - Channel
 *	argv[8] - Preamble
 *	argv[9] - Max SCB
 */
int SoftapController::setSoftap(int argc, char *argv[]) {
    char psk_str[2*SHA256_DIGEST_LENGTH+1];
    int ret = 0, i = 0, fd;
    char *ssid, *iface;

    if (mSock < 0) {
        ALOGE("Softap set - failed to open socket");
        return -1;
    }
    if (argc < 4) {
        ALOGE("Softap set - missing arguments");
        return -1;
    }

    strncpy(mIface, argv[3], sizeof(mIface));
    iface = argv[2];

    char *wbuf = NULL;
    char *fbuf = NULL;

    if (argc > 4) {
        ssid = argv[4];
    } else {
        ssid = (char *)"AndroidAP";
    }

    asprintf(&wbuf, "interface=%s\ndriver=nano\nctrl_interface="
            "/data/misc/wifi/hostapd\nssid=%s\nchannel=6\n", iface, ssid);

    if (argc > 5) {
        if (!strcmp(argv[5], "wpa-psk")) {
            generatePsk(ssid, argv[6], psk_str);
            asprintf(&fbuf, "%swpa=1\nwpa_pairwise=TKIP\nwpa_psk=%s\n", wbuf, psk_str);
        } else if (!strcmp(argv[5], "wpa2-psk")) {
            generatePsk(ssid, argv[6], psk_str);
            asprintf(&fbuf, "%swpa=2\nrsn_pairwise=CCMP\nwpa_psk=%s\n", wbuf, psk_str);
        } else if (!strcmp(argv[5], "open")) {
            asprintf(&fbuf, "%s", wbuf);
        }
    } else {
        asprintf(&fbuf, "%s", wbuf);
    }

    fd = open(HOSTAPD_CONF_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0660);
    if (fd < 0) {
        ALOGE("Cannot update \"%s\": %s", HOSTAPD_CONF_FILE, strerror(errno));
        free(wbuf);
        free(fbuf);
        return -1;
    }
    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        ALOGE("Cannot write to \"%s\": %s", HOSTAPD_CONF_FILE, strerror(errno));
        ret = -1;
    }
    close(fd);
    free(wbuf);
    free(fbuf);

    /* Note: apparently open can fail to set permissions correctly at times */
    if (chmod(HOSTAPD_CONF_FILE, 0660) < 0) {
        ALOGE("Error changing permissions of %s to 0660: %s",
                HOSTAPD_CONF_FILE, strerror(errno));
        unlink(HOSTAPD_CONF_FILE);
        return -1;
    }

    if (chown(HOSTAPD_CONF_FILE, AID_SYSTEM, AID_WIFI) < 0) {
        ALOGE("Error changing group ownership of %s to %d: %s",
                HOSTAPD_CONF_FILE, AID_WIFI, strerror(errno));
        unlink(HOSTAPD_CONF_FILE);
        return -1;
    }

    return ret;
}

void SoftapController::generatePsk(char *ssid, char *passphrase, char *psk_str) {
    unsigned char psk[SHA256_DIGEST_LENGTH];
    int j;
    // Use the PKCS#5 PBKDF2 with 4096 iterations
    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase),
            reinterpret_cast<const unsigned char *>(ssid), strlen(ssid),
            4096, SHA256_DIGEST_LENGTH, psk);
    for (j=0; j < SHA256_DIGEST_LENGTH; j++) {
        sprintf(&psk_str[j<<1], "%02x", psk[j]);
    }
    psk_str[j<<1] = '\0';
}


/*
 * Arguments:
 *	argv[2] - interface name
 *	argv[3] - AP or STA
 */
int SoftapController::fwReloadSoftap(int argc, char *argv[])
{
	ALOGE("Nanoradio SoftapController::fwReloadSoftap");
    return 0;
}

int SoftapController::clientsSoftap(char **retbuf)
{
    int ret;

    if (mSock < 0) {
        ALOGE("Softap clients - failed to open socket");
        return -1;
    }
    *mBuf = 0;
    ret = setCommand(mIface, "AP_GET_STA_LIST", SOFTAP_MAX_BUFFER_SIZE);
    if (ret) {
        ALOGE("Softap clients - failed: %d", ret);
    } else {
        asprintf(retbuf, "Softap clients:%s", mBuf);
        ALOGE("Softap clients:%s", mBuf);
    }
    return ret;
}
