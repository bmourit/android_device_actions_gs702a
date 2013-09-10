/*
 * Copyright (C) Texas Instruments - http://www.ti.com/
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

/**
* @file CameraProperties.cpp
*
* This file maps the CameraHardwareInterface to the Camera interfaces on OMAP4 (mainly OMX).
*
*/

#include "CameraHalDebug.h"

//#include "CameraHal.h"
#include <utils/threads.h>

#include "CameraProperties.h"

namespace android
{



/*********************************************************
 CameraProperties - public function implemetation
**********************************************************/

CameraProperties::CameraProperties() 
{
    LOG_FUNCTION_NAME;

    mCamerasSupported = 0;
    mInitialized = 0;

    LOG_FUNCTION_NAME_EXIT;
}

CameraProperties::~CameraProperties()
{
    LOG_FUNCTION_NAME;

    LOG_FUNCTION_NAME_EXIT;
}
                     

// Initializes the CameraProperties class
status_t CameraProperties::initialize()
{
    LOG_FUNCTION_NAME;

    status_t ret;

    Mutex::Autolock lock(mLock);

    if(mInitialized)
        return NO_ERROR;

    ret = loadProperties();

    mInitialized = 1;

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}
// Initializes the CameraProperties class
status_t CameraProperties::initializeForced()
{
    LOG_FUNCTION_NAME;

    status_t ret;

    Mutex::Autolock lock(mLock);

    ret = loadProperties();

    mInitialized = 1;

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

extern "C" int CameraAdapter_Capabilities(CameraProperties::Properties* properties_array,
        const unsigned int starting_camera,
        const unsigned int max_camera);

///Loads all the Camera related properties
status_t CameraProperties::loadProperties()
{
    LOG_FUNCTION_NAME;

    status_t ret = NO_ERROR;

    // adapter updates capabilities and we update camera count
    mCamerasSupported = CameraAdapter_Capabilities(mCameraProps, 0, MAX_CAMERAS_SUPPORTED);

    if((int)mCamerasSupported < 0)
    {
        CAMHAL_LOGEA("error while getting capabilities");
        ret = UNKNOWN_ERROR;
    }
    else if (mCamerasSupported > MAX_CAMERAS_SUPPORTED)
    {
        CAMHAL_LOGEA("returned too many adapaters");
        ret = UNKNOWN_ERROR;
    }
    else
    {
        for (unsigned int i = 0; i < mCamerasSupported; i++)
        {
            mCameraProps[i].dump();
        }
    }

    CAMHAL_LOGDB("mCamerasSupported = %d", mCamerasSupported);
    LOG_FUNCTION_NAME_EXIT;
    return ret;
}

// Returns the number of Cameras found
int CameraProperties::camerasSupported()
{
    LOG_FUNCTION_NAME;
    return mCamerasSupported;
}

};
