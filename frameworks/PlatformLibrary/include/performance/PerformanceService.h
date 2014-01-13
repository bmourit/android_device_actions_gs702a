/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_PERFORMANCESERVICE_H
#define ANDROID_PERFORMANCESERVICE_H

#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/Vector.h>
#include <utils/KeyedVector.h>
#include <binder/IServiceManager.h>

#include "performance/IPerformanceService.h"

#ifdef __cplusplus
namespace android {
#endif

/* obtain structures and constants from the kernel header */
#include <linux/PerformanceState.h>

#ifdef __cplusplus
}   // namespace android
#endif


namespace android {

class PerformanceService : public BnPerformanceService
{
public:

	static inline void instantiate() {
	    defaultServiceManager()->addService(
	            String16("performanceservice"), new PerformanceService());
	}

	virtual bool getMaxPerformance();
	virtual bool putMaxPerformance();
	virtual bool singleThreadAccelerate(int pid);
	virtual bool cleanAllVmCaches();
	virtual bool setCpuFreqRange(const sp<IBinder> who, int min, int max);
	virtual bool restoreCpuFreqRange(const sp<IBinder> who);
	virtual bool enbleAutoAdjustBacklight();
	virtual bool disableAutoAdjustBacklight();
	virtual bool setAlign();
	virtual bool clearAlign();
	virtual bool notifier(int pid);
	virtual bool setOpt();
	virtual bool clearOpt();
	virtual bool getSupport();
	
private:
    PerformanceService();
    int maxCpuFreq;
    int minCpuFreq;
    int maxPerformaceCount;
    int maxPerformaceOpen;
    void cpuFreqRangInit();
    virtual    ~PerformanceService();

};

// ----------------------------------------------------------------------------

}; // namespace android

#endif
