package com.actions.hardware;

interface IPerformanceService
{
	boolean getMaxPerformance();
	boolean putMaxPerformance();
	boolean singleThreadAccelerate(int pid);
	boolean cleanAllVmCaches();
	boolean setCpuFreqRange(IBinder binder, int min, int max);
	boolean restoreCpuFreqRange(IBinder binder);
	boolean enbleAutoAdjustBacklight();
	boolean disableAutoAdjustBacklight();
	boolean setAlign();
	boolean clearAlign();
	boolean notifier(int pid);
	boolean setOpt();
	boolean clearOpt();
	boolean getSupport();
}
