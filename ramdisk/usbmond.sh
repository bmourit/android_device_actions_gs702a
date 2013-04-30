#!/system/bin/sh
param1=$1
param2=$2
install=insmod 
uninstall=rmmod
setproperty=setprop

adb_exist=1

echo "usbmond shell: $param1 $param2"

if [ x$param1 = xUSB_B_IN ];then
	if [ "$param2" = "$adb_exist" ];then
	  /system/bin/stop adbd
	  sleep 1
	fi
	echo "force to uninstall dummy ko."
	$uninstall g_android
	$uninstall dummy_udc
	$uninstall dwc3_actions
	$uninstall dwc3

	$install /misc/modules/dwc3.ko
	$install /misc/modules/dwc3-actions.ko
	$install /misc/modules/g_android.ko
	if [ "$param2" = "$adb_exist" ];then
	  /system/bin/start adbd
	fi  
fi

if [ x$param1 = xUSB_B_OUT ];then
	if [ "$param2" = "$adb_exist" ];then
	  /system/bin/stop adbd
	  sleep 1
	fi  
	$uninstall g_android
	$uninstall dwc3_actions
	$uninstall dwc3
	
  $install /misc/modules/dummy_udc.ko
	$install /misc/modules/g_android.ko
fi

if [ x$param1 = xUSB_A_IN ];then
	$install /misc/modules/dwc3.ko host_config=1
	$install /misc/modules/dwc3-actions.ko
	$install /misc/modules/xhci-hcd.ko
fi

if [ x$param1 = xUSB_A_OUT ];then
	$uninstall xhci_hcd
	$uninstall dwc3_actions
	$uninstall dwc3
fi

