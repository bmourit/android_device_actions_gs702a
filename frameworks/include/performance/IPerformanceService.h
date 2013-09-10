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

#ifndef ANDROID_IPERFORMANCESERVICE_H
#define ANDROID_IPERFORMANCESERVICE_H

#include <utils/Errors.h>  // for status_t
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <utils/String8.h>
#include <utils/Vector.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

namespace android {

class IPerformanceService: public IInterface
{
public:
    DECLARE_META_INTERFACE(PerformanceService);

    static inline sp<IPerformanceService> connect()
   	{
       	sp<IPerformanceService> PerformanceService;
   			ALOGE("IPerformanceService Connect\n");

	   		sp<IServiceManager> sm = defaultServiceManager();
	   		sp<IBinder> binder;

	   		do {
	   			binder = sm->getService(String16("performanceservice"));
	   			if (binder != 0) {
	   				break;
	   			 }
	   			 ALOGW("performanceservice not published, waiting...");
	   			 usleep(500000); // 0.5 s
	   		} while(true);

   		PerformanceService = interface_cast<IPerformanceService>(binder);
   		ALOGE_IF(PerformanceService == 0, "no PerformanceService!?");
   		return PerformanceService;
   	}

	virtual bool setAlign() = 0;
	virtual bool clearAlign() = 0;
	virtual bool setOpt() = 0;
	virtual bool clearOpt() = 0;
    //return false if already in the scence
    virtual bool getMaxPerformance() = 0;
    //return false if still in the scence
    virtual bool putMaxPerformance() = 0;
    virtual bool singleThreadAccelerate(int pid) = 0;
    virtual bool cleanAllVmCaches() = 0;
    virtual bool setCpuFreqRange(const sp<IBinder> who, int min, int max) = 0;
    virtual bool restoreCpuFreqRange(const sp<IBinder> who) = 0;
    virtual bool enbleAutoAdjustBacklight() = 0;
    virtual bool disableAutoAdjustBacklight() = 0;
    virtual bool notifier(int pid) = 0;
    virtual bool getSupport() = 0;
};

// ----------------------------------------------------------------------------

class BnPerformanceService: public BnInterface<IPerformanceService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif
