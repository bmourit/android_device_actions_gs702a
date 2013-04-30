#!/system/bin/sh
param=$1
RESULT=0

if [ x$param = xBOOT ];then
    until [ x$RESULT == x1 ]
    do
        RESULT=`cat /sys/power/restore`
        sleep 2
    done
    dmesg
    insmod /misc/modules/atc260x_power.ko
    reboot
fi

if [ x$param = xSHUTDOWN ];then
    /system/bin/stop adbd
    sleep 1
    rmmod g_android
    rmmod dummy_udc
    rmmod xhci_hcd
    rmmod dwc3_actions
    rmmod dwc3
fi

