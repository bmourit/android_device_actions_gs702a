#!/system/bin/sh
first_boot_file="/data/system/entropy.dat"
if [ -f ${first_boot_file} ];then
echo "not first boot"
else
echo "first boot"
busybox cp /vendor/app/app/*.* /data/app/
busybox chmod 777 /data/app/*.apk
fi