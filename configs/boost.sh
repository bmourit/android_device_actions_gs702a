#!/sbin/sh
#
# Boost CPU and GPU for init.d script
#
export CPUBOOST=/sys/devices/system/cpu/cpufreq/user/boost
export GPUBOOST=/sys/devices/system/cpu/cpufreq/gpufreq/policy

# Activate CPU boost to 1200MHz
echo 1 > $CPUBOOST

# Activate GPU boost 
echo 1 > $GPUBOOST

exit 0
