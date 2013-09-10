
BOARD_WIFI_VENDOR := $(BOARD_WIFI_VENDOR)
WIFI_DRIVER_FIRMWARE_PATH :=
ifneq ($(strip $(BOARD_WIFI_VENDOR)),)
BOARD_HAVE_WIFI := true
else
BOARD_HAVE_WIFI := false
endif

ifeq ($(strip $(BOARD_WIFI_VENDOR)), mt6620)

WPA_SUPPLICANT_VERSION:=VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
#WIFI_DRIVER_FIRMWARE_PATH       := "/misc/modules"
#WIFI_DRIVER_MODULE_FOLDER_PATH  := "/system/lib/modules"
#WIFI_DRIVER_MODULE_PATH         := $(WIFI_DRIVER_FIRMWARE_PATH)"/wlan_mt6620.ko"
#WIFI_DRIVER_MODULE_NAME 		 := "wlan_mt6620"


else ifeq ($(strip $(BOARD_WIFI_VENDOR)), mt5931)

WPA_SUPPLICANT_VERSION			:= VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER 	:= NL80211
WIFI_DRIVER_FIRMWARE_PATH       := "/misc/modules"
WIFI_DRIVER_MODULE_FOLDER_PATH  := "/system/lib/modules"
WIFI_DRIVER_MODULE_PATH         := $(WIFI_DRIVER_FIRMWARE_PATH)"/wlan_mt5931.ko"
WIFI_DRIVER_MODULE_NAME 		:= "wlan_mt5931"


else ifeq ($(strip $(BOARD_WIFI_VENDOR)), realtek)

WPA_SUPPLICANT_VERSION 				:= VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER 		:= NL80211
CONFIG_DRIVER_WEXT 					:= y
BOARD_WPA_SUPPLICANT_PRIVATE_LIB 	:= lib_driver_cmd_rtl
BOARD_HOSTAPD_DRIVER        		:= NL80211
BOARD_HOSTAPD_PRIVATE_LIB   		:= lib_driver_cmd_rtl
BOARD_WLAN_DEVICE := rtl8192cu
#BOARD_WLAN_DEVICE := rtl8192du
#BOARD_WLAN_DEVICE := rtl8192ce
#BOARD_WLAN_DEVICE := rtl8192de
#BOARD_WLAN_DEVICE := rtl8723as
#BOARD_WLAN_DEVICE := rtl8723au
#BOARD_WLAN_DEVICE := rtl8188es

# wifi.c defines
WIFI_DRIVER_FIRMWARE_PATH       := "/misc/modules"
WIFI_DRIVER_MODULE_FOLDER_PATH  := "/system/lib/modules"
WIFI_DRIVER_MODULE_PATH         := $(WIFI_DRIVER_FIRMWARE_PATH)"/wlan.ko"
WIFI_DRIVER_MODULE_NAME 		:= "wlan"
WIFI_DRIVER_MODULE_ARG    		:= "ifname=wlan0 if2name=p2p0"

#WIFI_DRIVER_MODULE_ARG    := ""
WIFI_FIRMWARE_LOADER      := ""
WIFI_DRIVER_FW_PATH_STA   := ""
WIFI_DRIVER_FW_PATH_AP    := ""
WIFI_DRIVER_FW_PATH_P2P   := ""
WIFI_DRIVER_FW_PATH_PARAM := ""


else ifeq ($(strip $(BOARD_WIFI_VENDOR)), rtl8723as_vt)

WPA_SUPPLICANT_VERSION 				:= VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER 		:= NL80211
CONFIG_DRIVER_WEXT 					:= y
BOARD_WPA_SUPPLICANT_PRIVATE_LIB 	:= lib_driver_cmd_rtl
BOARD_HOSTAPD_DRIVER        		:= NL80211
BOARD_HOSTAPD_PRIVATE_LIB   		:= lib_driver_cmd_rtl
#BOARD_WLAN_DEVICE := rtl8192cu
#BOARD_WLAN_DEVICE := rtl8192du
#BOARD_WLAN_DEVICE := rtl8192ce
#BOARD_WLAN_DEVICE := rtl8192de
BOARD_WLAN_DEVICE := rtl8723as
#BOARD_WLAN_DEVICE := rtl8723au
#BOARD_WLAN_DEVICE := rtl8188es

# wifi.c defines
WIFI_DRIVER_FIRMWARE_PATH       := "/misc/modules"
WIFI_DRIVER_MODULE_FOLDER_PATH  := "/system/lib/modules"
WIFI_DRIVER_MODULE_PATH         := $(WIFI_DRIVER_FIRMWARE_PATH)"/wlan_8723as_vt.ko"
WIFI_DRIVER_MODULE_NAME 	:= "wlan_8723as_vt"
WIFI_DRIVER_MODULE_ARG    := "ifname=wlan0 if2name=p2p0"

#WIFI_DRIVER_MODULE_ARG    := ""
WIFI_FIRMWARE_LOADER      := ""
WIFI_DRIVER_FW_PATH_STA   := ""
WIFI_DRIVER_FW_PATH_AP    := ""
WIFI_DRIVER_FW_PATH_P2P   := ""
WIFI_DRIVER_FW_PATH_PARAM := ""


else ifeq ($(strip $(BOARD_WIFI_VENDOR)), nanoradio)

WPA_SUPPLICANT_VERSION          	:= VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER     	:= WEXT
BOARD_WPA_SUPPLICANT_PRIVATE_LIB	:= lib_nano_cmd_nrx
BOARD_HOSTAPD_DRIVER            	:= NANO
WPA_SUPPLICANT_TYPE 				:= open
WIFI_DRIVER_IFACE               	:= "wlan0"
BOARD_WLAN_DEVICE					:= nrx600

# wifi.c defines
WIFI_DRIVER_FIRMWARE_PATH       := "/misc/modules"
WIFI_DRIVER_MODULE_FOLDER_PATH  := "/system/lib/modules"
WIFI_DRIVER_MODULE_PATH         := $(WIFI_DRIVER_MODULE_FOLDER_PATH)"/nano.ko"
WIFI_DRIVER_MODULE_NAME 		:= "nano"
#WIFI_DRIVER_MODULE_ARG			:= "nrx_ifname="$(WIFI_DRIVER_IFACE)" 

nrx_settings_path="$(WIFI_DRIVER_MODULE_FOLDER_PATH)"/settings.bin"
WIFI_DRIVER_MODULE_ARG			:= "nrx_ifname="$(WIFI_DRIVER_IFACE)""
WIFI_DRIVER_SUPP_CONFIG_TEMPLATE:= "/system/etc/wifi/wpa_supplicant.conf"
WIFI_DRIVER_SUPP_CONFIG_FILE    := "/data/misc/wifi/wpa_supplicant.conf"
WIFI_DRIVER_SUPP_IFACE_DIR      := "/data/misc/wifi/sockets"
WIFI_DRIVER_STATUS_PATH         := "/data/misc/wifi/driver_status"
HOSTAPD_CONFIG_TEMPLATE_DIR     := "/system/etc/wifi/hostapd_conf"
HOSTAPD_CONFIG_FILE             := "/data/misc/wifi/hostapd_softap.conf"

WIFI_DRIVER_FW_PATH_STA := $(WIFI_DRIVER_MODULE_FOLDER_PATH)"/fw/nrx600-sta.fw"
WIFI_DRIVER_FW_PATH_AP := $(WIFI_DRIVER_MODULE_FOLDER_PATH)"/fw/nrx600-softap.fw"
WIFI_DRIVER_FW_PATH_PARAM := "/proc/driver/"$(WIFI_DRIVER_IFACE)"/fw_path"
WIFI_FIRMWARE_LOADER := "wlan_loader"

ifeq ($(BOARD_WLAN_DEVICE), nrx600)
WIFI_SUPPORT_NRX600 		:= true
WIFI_SUPPORT_NRX700 		:= false
else ifeq ($(BOARD_WLAN_DEVICE), nrx700)
WIFI_SUPPORT_NRX600 		:= false
WIFI_SUPPORT_NRX700 		:= true
else
WIFI_SUPPORT_NRX600 		:= true
WIFI_SUPPORT_NRX700 		:= true
endif


else ifeq ($(strip $(BOARD_WIFI_VENDOR)), broadcom)

BOARD_WPA_SUPPLICANT_DRIVER 		:= NL80211
WPA_SUPPLICANT_VERSION      		:= VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB 	:= lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER        		:= NL80211
BOARD_HOSTAPD_PRIVATE_LIB   		:= lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE           		:= ap6210
BOARD_WLAN_DEVICE_REV       		:= ap6210
WIFI_DRIVER_FW_PATH_PARAM   		:= "/sys/module/wlan_bcmdhd/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_STA     		:= "/system/etc/firmware/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_P2P     		:= "/system/etc/firmware/fw_bcmdhd_p2p.bin"
WIFI_DRIVER_FW_PATH_AP      		:= "/system/etc/firmware/fw_bcmdhd_apsta.bin"
WIFI_DRIVER_MODULE_PATH     		:= "/misc/modules/wlan_bcmdhd.ko"
WIFI_DRIVER_MODULE_NAME     		:= "wlan_bcmdhd"
WIFI_DRIVER_MODULE_ARG      		:= "iface_name=wlan firmware_path=/system/etc/firmware/fw_bcmdhd.bin nvram_path=/system/etc/firmware/nvram.txt"

endif  # BOARD_WIFI_VENDOR