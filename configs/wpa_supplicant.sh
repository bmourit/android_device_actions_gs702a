#!/system/bin/sh

/system/bin/usb_mode.sh 1
sleep 1
/system/bin/wpa_supplicant -Dwext -iwlan0 -c/data/misc/wifi/wpa_supplicant.conf -e/data/misc/wifi/entropy.bin
