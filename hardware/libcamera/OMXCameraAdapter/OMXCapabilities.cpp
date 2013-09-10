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
* @file OMXCap.cpp
*
* This file implements the OMX Capabilities feature.
*
*/
#include "CameraHalDebug.h"

#include "CameraHal.h"
#include "OMXCameraAdapter.h"
#include "ErrorUtils.h"
#include "ActCameraParameters.h"

#include "CameraConfigs.h"

namespace android
{

#undef LOG_TAG

// Maintain a separate tag for OMXCameraAdapter logs to isolate issues OMX specific
#define LOG_TAG "CameraHAL"

#undef TRUE
#undef FALSE

/************************************
 * global constants and variables
 *************************************/

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))
#define FPS_MIN 5
#define FPS_STEP 5
#define FPS_MAX 60

static const char PARAM_SEP[] = ",";
static const int PARAM_SEP_CHAR = ',';
static const uint32_t VFR_OFFSET = 8;
static const char VFR_BACKET_START[] = "(";
static const char VFR_BRACKET_END[] = ")";
static const char FRAMERATE_COUNT = 10;
static const char VARFRAMERATE_COUNT = 20;
static const char WHSIZE_COUNT = 12;

/**** look up tables to translate OMX Caps to Parameter ****/


const CapFramerate OMXCameraAdapter::mFramerates [] =
{
    { 30, "30" },
    { 28, "28" },
    { 26, "26" },
    { 25, "25" },
    { 24, "24" },
    { 20, "20" },
    { 18, "18" },
    { 16, "16" },
    { 15, "15" },
};


const CapZoom OMXCameraAdapter::mZoomStages [] =
{
    { 65536, "100" },
    { 68157, "104" },
    { 70124, "107" },
    { 72745, "111" },
    { 75366, "115" },
    { 77988, "119" },
    { 80609, "123" },
    { 83231, "127" },
    { 86508, "132" },
    { 89784, "137" },
    { 92406, "141" },
    { 95683, "146" },
    { 99615, "152" },
    { 102892, "157" },
    { 106168, "162" },
    { 110100, "168" },
    { 114033, "174" },
    { 117965, "180" },
    { 122552, "187" },
    { 126484, "193" },
    { 131072, "200" },
    { 135660, "207" },
    { 140247, "214" },
    { 145490, "222" },
    { 150733, "230" },
    { 155976, "238" },
    { 161219, "246" },
    { 167117, "255" },
    { 173015, "264" },
    { 178913, "273" },
    { 185467, "283" },
    { 192020, "293" },
    { 198574, "303" },
    { 205783, "314" },
    { 212992, "325" },
    { 220201, "336" },
    { 228065, "348" },
    { 236585, "361" },
    { 244449, "373" },
    { 252969, "386" },
    { 262144, "400" },
};

const CapPixelformat OMXCameraAdapter::mPixelformats [] =
{
    { OMX_COLOR_FormatCbYCrY, CameraParameters::PIXEL_FORMAT_YUV422I },
    { OMX_COLOR_FormatYUV420SemiPlanar, CameraParameters::PIXEL_FORMAT_YUV420SP },
    { OMX_COLOR_Format16bitRGB565, CameraParameters::PIXEL_FORMAT_RGB565 },
    { OMX_COLOR_FormatYUV420Planar, CameraParameters::PIXEL_FORMAT_YUV420P },
};



const CapU32Pair OMXCameraAdapter::mVarFrameratesDefault =
{
    15, 15, "(15000,15000)"
};

// mapped values have to match with new_sensor_MSP.h
const CapU32 OMXCameraAdapter::mSensorNames [] =
{
    { 0, "ActionsBack" },
    { 1, "ActionsFront" },
};

const CapISO OMXCameraAdapter::mISOStages [] = {
    { 0, ActCameraParameters::ISO_MODE_AUTO },
    { 100, ActCameraParameters::ISO_MODE_100 },
    { 200, ActCameraParameters::ISO_MODE_200},
    { 400, ActCameraParameters::ISO_MODE_400 },
    { 800, ActCameraParameters::ISO_MODE_800 },
    { 1000, ActCameraParameters::ISO_MODE_1000 },
    { 1200, ActCameraParameters::ISO_MODE_1200 },
    { 1600, ActCameraParameters::ISO_MODE_1600 },
};


const CapResolution OMXCameraAdapter::mThumbRes [] = {
	{160,120,"128x96"},
};

const CapResolution OMXCameraAdapter::mExtendedPictureRes[] = {
	{2048,1536,"2048x1536"},
	{2560,1920,"2560x1920"},
};

const CapResolution OMXCameraAdapter::mExtendedVideoRes[] = {
	{800,600,"800x600"},
	{1280,720,"1280x720"},
};
/************************************
 * static helper functions
 *************************************/

// utility function to remove last seperator
void remove_last_sep(char* buffer)
{
    char* last_sep = NULL;
    last_sep = strrchr(buffer, PARAM_SEP_CHAR);
    if (last_sep != NULL)
    {
        unsigned int i;
        for(i = 1; i < strlen(last_sep); i++)
        {
            if(last_sep[i] != ' ')
            {
                return ;
            }
        }
        last_sep[0] = '\0';
    }
}

int OMXCameraAdapter::sizeNearWeight(int cw,int ch, int dw, int dh)
{
    int rw,rh;
    int w;
    //ratio <<16
    rw = ((dw-cw)<<16)/cw;
    rh = ((dh-ch)<<16)/ch;

    if(rw >= 0 && rh >= 0)
    {
        w = rw > rh ? rw: rh;
    }
    else if( rw <= 0 && rh <= 0)
    {
        w = rw > rh ? rh: rw;
    }
    else 
    {
        w = rw < 0 ? rh-rw : rw-rh;
    }

    w = (w<0?-w:w);

    if(cw >= dw && ch>=dh && rw == rh)
    {
        w = w>>2; 
    }
    //do not use 176x144 as default size
    if(dw<320 || dh <240)
    {
        w = w<<4;
    }
    ALOGE("cw=%d,ch=%d,dw=%d,dh=%d,w=%d",cw,ch,dw,dh,w);
    return w;
}

status_t OMXCameraAdapter::findBestSize(OMX_ACT_VARRESTYPE hwSizes[], OMX_U16 hwSizeNum, int width, int height, int *bestWidth, int *bestHeight)
{
    int ret = NO_ERROR;

    int i;
    int weight = 0;
    int minIndex = -1;
    int min = 0x7fffffff;
    for(i =0; i < hwSizeNum; i++)
    {
        weight = sizeNearWeight(width,height, hwSizes[i].nWidth,  hwSizes[i].nHeight);
        if(weight < min)
        {            
            minIndex = i;
            min = weight;
        }
    } 
    if(minIndex >= 0)
    {
        *bestWidth = hwSizes[minIndex].nWidth; 
        *bestHeight = hwSizes[minIndex].nHeight; 
    }
    else
    {
        ret = BAD_VALUE;
    }

    return ret;

}
bool OMXCameraAdapter::getCropArea(int sw, int sh, int dw, int dh, int *cw, int *ch, bool isImage)
{
    int rw, rh;
    bool needCrop = false;
    //ratio
    rw = (sw<<16)/ dw;
    rh = ( sh<<16)/dh;
    if(dw >= sw && dh >= sh)
    {

        if(rw == rh)
        {
            needCrop = false;
            *cw = sw;
            *ch = sh;
        }
        else
        {
            needCrop = true;
            if(rw < rh)
            {
                *cw = SIZE_ALIGN_DOWN_16(sw);
                *ch = SIZE_ALIGN_DOWN_16(sw*dh/dw);
            }
            else
            {
                *cw = SIZE_ALIGN_DOWN_16(dw*sh/dh);
                *ch = SIZE_ALIGN_DOWN_16(sh);
            }
        }
    }
    else if(dw < sw && dh < sh)
    {
        //picture support zoom out  by using soft encode, but preview using crop
        if(isImage && (sw <= MAX_SW_ENC_WIDTH) && (sh <= MAX_SW_ENC_HEIGHT))
        {
            if(rw == rh)
            {
                needCrop = false;
                *cw = sw;
                *ch = sh;
            }
            else
            {
                needCrop = true;
                if(rw >= rh)
                {
                    *cw = SIZE_ALIGN_DOWN_16(sh*dw/dh);
                    *ch = SIZE_ALIGN_DOWN_16(sh);
                }
                else
                {
                    *cw = SIZE_ALIGN_DOWN_16(sw);
                    *ch = SIZE_ALIGN_DOWN_16(sw*dh/dw);
                }

            }
        }
        else
        {
            needCrop = true;
            *cw = SIZE_ALIGN_DOWN_16(dw);
            *ch = SIZE_ALIGN_DOWN_16(dh);
        }
    }
    else if( dw > sw )
    {
        needCrop = true;
        *cw = SIZE_ALIGN_DOWN_16(sw);
        *ch = SIZE_ALIGN_DOWN_16(dh*sw/dw);
    }
    else
    {
        needCrop = true;
        *cw = SIZE_ALIGN_DOWN_16(dw*sh/dh);
        *ch = SIZE_ALIGN_DOWN_16(sh);
    }
    return needCrop;
}

status_t OMXCameraAdapter::findBestExSize(OMX_ACT_VARRESTYPE hwSizes[], OMX_U16 hwSizeNum, int width, int height, ExtendedResolution *exres, bool isImage)
{
    int ret = NO_ERROR;

    int i;
    int weight = 0;
    int minIndex = -1;
    int min = 0x7fffffff;
    for(i =0; i < hwSizeNum; i++)
    {

        //check vce support, check max vce zoom in capability
        if((width-1)/hwSizes[i].nWidth >= MAX_CAMERA_SCALE 
            || (height-1)/hwSizes[i].nHeight >= MAX_CAMERA_SCALE)
        {
            continue;
        }
        if(isImage)
        {
             // max picture size is 5M, this is determined by alloced buffer size
             if( hwSizes[i].nWidth > MAX_PICTURE_WIDTH || hwSizes[i].nHeight > MAX_PICTURE_HEIGHT)
             {
                 continue;
             }
#if 0
             //image size bigger than soft enc size do not support zoom out, fliter this condition 
             if((( (int)hwSizes[i].nWidth > MAX_SW_ENC_WIDTH) || ((int)hwSizes[i].nHeight > MAX_SW_ENC_HEIGHT))
                 && (((int)hwSizes[i].nWidth > width) || ((int)hwSizes[i].nHeight > height)))
             {
                 continue;
             }
#endif
        }
        else
        {
#if 0
            //preview not support zoom out, fliter this condition 
            if((int)hwSizes[i].nWidth > width || (int)hwSizes[i].nHeight > height)
            {
                continue;
            }
#endif
        }

        weight = sizeNearWeight(width,height, hwSizes[i].nWidth,  hwSizes[i].nHeight);
        if(weight < min)
        {            
            minIndex = i;
            min = weight;
        }
    } 

    if(minIndex>=0)
    {
        exres->width = width;
        exres->height = height;
        exres->captureWidth = hwSizes[minIndex].nWidth;
        exres->captureHeight = hwSizes[minIndex].nHeight;

        exres->needCrop = getCropArea(exres->captureWidth, exres->captureHeight, exres->width, exres->height, (int *)&exres->cropWidth, (int *)&exres->cropHeight, isImage);

        CAMHAL_LOGEB("isImage = %d", (int)isImage);
        CAMHAL_LOGEB("exres.width = %d", (int)exres->width);
        CAMHAL_LOGEB("exres.height = %d", (int)exres->height);
        CAMHAL_LOGEB("exres.captureWidth = %d", (int)exres->captureWidth);
        CAMHAL_LOGEB("exres.captureHeight = %d", (int)exres->captureHeight);
        CAMHAL_LOGEB("exres.cropWidth = %d", (int)exres->cropWidth);
        CAMHAL_LOGEB("exres.cropHeight = %d", (int)exres->cropHeight);
        CAMHAL_LOGEB("exres.needCrop = %d", (int)exres->needCrop);
    }
    else
    {
        ret = BAD_VALUE;
    }
    return ret;
}


status_t OMXCameraAdapter::findBestFps(OMX_ACT_VARFPSTYPE hwFps[], OMX_U16 hwFpsNum, int fps, int *bestmin, int *bestmax, int *bestfps)
{
    int ret = NO_ERROR;

    int i;
    int index = -1;
    for(i =0; i < hwFpsNum; i++)
    {
        if(fps>= (int)hwFps[i].nVarFPSMin && fps <= (int)hwFps[i].nVarFPSMax)
        {
            //match
            index = i;
            break;
        }
    } 
    if(index >= 0)
    {
        *bestmin = hwFps[index].nVarFPSMin; 
        *bestmax = hwFps[index].nVarFPSMin; 
        *bestfps = *bestmax;
    }
    else
    {
        int proper;
        int min = 0xffffffff;
        int minIndex = -1;

        for(i =0; i < hwFpsNum; i++)
        {
            if(abs(fps - hwFps[i].nVarFPSMax)<min )
            {
                minIndex = i;
                min = abs(fps - hwFps[i].nVarFPSMax);
            }
        } 
        if(minIndex >= 0)
        {
            *bestmin = hwFps[minIndex].nVarFPSMin; 
            *bestmax = hwFps[minIndex].nVarFPSMin; 
            *bestfps = *bestmax;
        }
        else
        {
            *bestmin = 30; 
            *bestmax = 30; 
            *bestfps = 30;
        }
    }

    return ret;

}

/*****************************************
 * internal static function declarations
 *****************************************/

/**** Utility functions to help translate OMX Caps to Parameter ****/

status_t OMXCameraAdapter::encodePixelformatCap(OMX_COLOR_FORMATTYPE format,
        const CapPixelformat *cap,
        size_t capCount,
        char * buffer,
        size_t bufferSize)
{
    status_t ret = NO_ERROR;

    LOG_FUNCTION_NAME;

    if ( ( NULL == buffer ) || ( NULL == cap ) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }
#ifdef CAMERA_RUN_IN_EMULATOR
    if(format != OMX_COLOR_Format16bitRGB565)
    {
        return ret;
    }
#else
#if 0
    if(format == OMX_COLOR_Format16bitRGB565 || format == OMX_COLOR_FormatCbYCrY)
    {
        return ret;
    }
#else
	if(format != OMX_COLOR_FormatYUV420SemiPlanar && format != OMX_COLOR_FormatYUV420Planar)
    {
        return ret;
    }
#endif

#endif

    for ( unsigned int i = 0; i < capCount; i++ )
    {

        if ( format == cap[i].pixelformat )
        {
            strncat(buffer, cap[i].param, bufferSize - 1);
            strncat(buffer, PARAM_SEP, bufferSize - 1);
        }
    }

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

status_t OMXCameraAdapter::encodeFramerateCap(OMX_U32 framerateMax,
        OMX_U32 framerateMin,
        const CapFramerate *cap,
        size_t capCount,
        char * buffer,
        size_t bufferSize)
{
    status_t ret = NO_ERROR;
    bool minInserted = false;
    bool maxInserted = false;
    char tmpBuffer[FRAMERATE_COUNT];

    LOG_FUNCTION_NAME;

    if ( ( NULL == buffer ) || ( NULL == cap ) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }

    for ( unsigned int i = 0; i < capCount; i++ )
    {
        if ( (framerateMax >= cap[i].num) && (framerateMin <= cap[i].num) )
        {
            strncat(buffer, cap[i].param, bufferSize - 1);
            strncat(buffer, PARAM_SEP, bufferSize - 1);

            if ( cap[i].num ==  framerateMin )
            {
                minInserted = true;
            }
        }
        if ( cap[i].num ==  framerateMax )
        {
            maxInserted = true;
        }
    }

    if ( !maxInserted )
    {
        memset(tmpBuffer, 0, FRAMERATE_COUNT);
        snprintf(tmpBuffer, FRAMERATE_COUNT - 1, "%u,", ( unsigned int ) framerateMax);
        strncat(buffer, tmpBuffer, bufferSize - 1);
        strncat(buffer, PARAM_SEP, bufferSize - 1);
    }

    if ( !minInserted )
    {
        memset(tmpBuffer, 0, FRAMERATE_COUNT);
        snprintf(tmpBuffer, FRAMERATE_COUNT - 1, "%u,", ( unsigned int ) framerateMin);
        strncat(buffer, tmpBuffer, bufferSize - 1);
        strncat(buffer, PARAM_SEP, bufferSize - 1);
    }

    remove_last_sep(buffer);

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}


status_t OMXCameraAdapter::encodeCapVFramerateCap(OMX_ACT_CAPTYPE &caps,
        char *buffer,
        char *fpsBuffer,
        size_t bufferSize)
{
    status_t ret = NO_ERROR;
    uint32_t min, max;
    char tmpBuffer[VARFRAMERATE_COUNT];


    LOG_FUNCTION_NAME;

    if ( (NULL == buffer) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }

    for (unsigned int i = 0; i < caps.ulCapVarFPSModesCount; i++)
    {

        max = caps.tCapVarFPSModes[i].nVarFPSMax;
        min = caps.tCapVarFPSModes[i].nVarFPSMin;

        if ((FPS_MAX < max))
        {
            max = FPS_MAX;
        }
        if( (FPS_MIN > min))
        {
            min = FPS_MIN;
        }

        memset(&tmpBuffer, 0, sizeof(tmpBuffer));
        snprintf(tmpBuffer, sizeof(tmpBuffer), "(%u,%u)", min*CameraHal::VFR_SCALE, max*CameraHal::VFR_SCALE);
        strncat(buffer, tmpBuffer, bufferSize - 1);
        strncat(buffer, PARAM_SEP, bufferSize - 1);

        for(unsigned int j = max; j <=max; j++)
        {
            memset(&tmpBuffer, 0, sizeof(tmpBuffer));
            snprintf(tmpBuffer, sizeof(tmpBuffer), "%u", j);
            strncat(fpsBuffer, tmpBuffer, bufferSize - 1);
            strncat(fpsBuffer, PARAM_SEP, bufferSize - 1);
        }

    }

    if ((buffer[0] == '\0') || (fpsBuffer[0] == '\0'))
    {
        snprintf(buffer, bufferSize, "(%lu,%lu)",
                 mVarFrameratesDefault.num1*CameraHal::VFR_SCALE ,
                 mVarFrameratesDefault.num2*CameraHal::VFR_SCALE);

        snprintf(fpsBuffer, bufferSize, "%lu",
                 mVarFrameratesDefault.num2);
    }

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

status_t OMXCameraAdapter::encodePrvVFramerateCap(OMX_ACT_CAPTYPE &caps,
        char *buffer,
        char *fpsBuffer,
        size_t bufferSize)
{
    status_t ret = NO_ERROR;
    uint32_t min, max;
    char tmpBuffer[VARFRAMERATE_COUNT];


    LOG_FUNCTION_NAME;

    if ( (NULL == buffer) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }

    for (unsigned int i = 0; i < caps.ulPrvVarFPSModesCount; i++)
    {

        max = caps.tPrvVarFPSModes[i].nVarFPSMax;
        min = caps.tPrvVarFPSModes[i].nVarFPSMin;

        if ((FPS_MAX < max))
        {
            max = FPS_MAX;
        }
        if((FPS_MIN > min))
        {
            min = FPS_MIN;
        }

        memset(&tmpBuffer, 0, sizeof(tmpBuffer));
        snprintf(tmpBuffer, sizeof(tmpBuffer), "(%u,%u)", min*CameraHal::VFR_SCALE,max*CameraHal::VFR_SCALE);
        strncat(buffer, tmpBuffer, bufferSize - 1);
        strncat(buffer, PARAM_SEP, bufferSize - 1);

        for(unsigned int j = max; j <=max; j++)
        {
            memset(&tmpBuffer, 0, sizeof(tmpBuffer));
            snprintf(tmpBuffer, sizeof(tmpBuffer), "%u", j);
            strncat(fpsBuffer, tmpBuffer, bufferSize - 1);
            strncat(fpsBuffer, PARAM_SEP, bufferSize - 1);
        }
    }

    if ((buffer[0] == '\0') || (fpsBuffer[0] == '\0'))
    {
        snprintf(buffer, bufferSize, "(%lu,%lu)",
                 mVarFrameratesDefault.num1*CameraHal::VFR_SCALE ,
                 mVarFrameratesDefault.num2*CameraHal::VFR_SCALE);

        snprintf(fpsBuffer, bufferSize, "%lu",
                 mVarFrameratesDefault.num2);

    }

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

size_t OMXCameraAdapter::encodeZoomCap(OMX_S32 maxZoom,
                                       const CapZoom *cap,
                                       size_t capCount,
                                       char * buffer,
                                       size_t bufferSize)
{
    status_t res = NO_ERROR;
    size_t ret = 0;

    LOG_FUNCTION_NAME;

    if ( (NULL == buffer) || (NULL == cap) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }


    for ( unsigned int i = 0; i < capCount; i++ )
    {
        strncat(buffer, cap[i].param, bufferSize - 1);
        strncat(buffer, PARAM_SEP, bufferSize - 1);
    }
    ret = capCount;
    remove_last_sep(buffer);

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

status_t OMXCameraAdapter::encodeISOCap(OMX_U32 maxISO,
                      const CapISO *cap,
                      size_t capCount,
                      char * buffer,
                      size_t bufferSize) {
    status_t ret = NO_ERROR;

    LOG_FUNCTION_NAME;

    if ( (NULL == buffer) || (NULL == cap) ) {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }

    for ( unsigned int i = 0; i < capCount; i++ ) {
        if ( cap[i].num <= maxISO) {
            strncat(buffer, cap[i].param, bufferSize - 1);
            strncat(buffer, PARAM_SEP, bufferSize - 1);
        }
    }
    remove_last_sep(buffer);

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}


status_t OMXCameraAdapter::encodeSizeCap(OMX_ACT_VARRESTYPE &res,
        char * buffer,
        size_t bufferSize)
{
    status_t ret = NO_ERROR;

    char tmpBuffer[WHSIZE_COUNT];

    LOG_FUNCTION_NAME;

    if ( (NULL == buffer) )
    {
        CAMHAL_LOGEA("Invalid input arguments");
        return -EINVAL;
    }

    memset(&tmpBuffer, 0, sizeof(tmpBuffer));
    snprintf(tmpBuffer, sizeof(tmpBuffer),"%ux%u,",(unsigned int)res.nWidth,(unsigned int)res.nHeight);
    strncat(buffer, tmpBuffer, sizeof(tmpBuffer));

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}


static int resCompareFunc(const CameraResCfg *lhs, const CameraResCfg *rhs)
{
    
    if(lhs == NULL && rhs == NULL)
    {
        return 0;
    }
    else if(lhs == NULL)
    {
        return -1;
    }
    else if(rhs == NULL)
    {
        return 1;
    }

    if(lhs->width == rhs->width)
    {
        if(lhs->height == rhs->height)
        {
            return 0;
        }
        if(lhs->height > rhs->height)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else if(lhs->width >= rhs->width)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

status_t OMXCameraAdapter::insertImageSizes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps, int id)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    int i, j, k;
    char tmpBuffer[WHSIZE_COUNT];

    Vector<CameraResCfg> imageSupportedRes;

    LOG_FUNCTION_NAME;


    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for(i =0; i< caps.ulImageResCount; i++)
    {
        
        CameraResCfg resItem;
        if(caps.tImageRes[i].nWidth > MAX_PICTURE_WIDTH || caps.tImageRes[i].nHeight > MAX_PICTURE_HEIGHT)
        {
            continue;
        }

        resItem.width = caps.tImageRes[i].nWidth;
        resItem.height =  caps.tImageRes[i].nHeight;

        imageSupportedRes.add(resItem);
    }

    if ( NO_ERROR != ret )
    {
        CAMHAL_LOGEB("Error inserting supported picture sizes 0x%x", ret);
    }
    else
    {
        //for extended image size
        Vector<CameraResCfg> imageExRes = CameraConfigs::getImageExRes(id);
        if(imageExRes.size() > 0)
        {
            params->mExtendedImageResolution.clear();
            for(i =0; i < (int)imageExRes.size(); i++)
            {
                CameraResCfg resItem = imageExRes.itemAt(i);

                //whether supported by hw
                for(j =0; j < caps.ulImageResCount; j++)
                {
                    if(caps.tImageRes[j].nWidth > MAX_PICTURE_WIDTH || caps.tImageRes[j].nHeight > MAX_PICTURE_HEIGHT)
                    {
                        continue;
                    }
                    if((int)caps.tImageRes[j].nWidth == resItem.width && (int)caps.tImageRes[j].nHeight == resItem.height)
                    {
                        break;
                    }
                }
                if(j >= caps.ulImageResCount)
                {

                    ExtendedResolution imageRes;
                    ret = findBestExSize(caps.tImageRes,  caps.ulImageResCount, resItem.width, resItem.height,&imageRes, true);
                    if(!ret)
                    {
                        imageSupportedRes.add(resItem);
                        
                        params->mExtendedImageResolution.add(imageRes);
                    }
                    ret = NO_ERROR;
                }

            }
        }

        imageSupportedRes.sort(resCompareFunc);

        for(i = 0; i < (int)imageSupportedRes.size(); i++)
        {
            CameraResCfg resItem = imageSupportedRes.itemAt(i);
            memset(&tmpBuffer, 0, sizeof(tmpBuffer));
            snprintf(tmpBuffer, sizeof(tmpBuffer),"%ux%u,",(unsigned int)resItem.width,(unsigned int)resItem.height);
            strncat(supported, tmpBuffer, MAX_PROP_NAME_LENGTH);
        }
        remove_last_sep(supported);
        params->set(CameraProperties::SUPPORTED_PICTURE_SIZES, supported);
    }

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertPreviewSizes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps, int id)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    int i,j,k;
    char tmpBuffer[WHSIZE_COUNT];

    Vector<CameraResCfg> prevSupportedRes;
    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for(i =0; i< caps.ulPreviewResCount; i++)
    {
        CameraResCfg resItem;
        resItem.width = caps.tPreviewRes[i].nWidth;
        resItem.height =  caps.tPreviewRes[i].nHeight;

        prevSupportedRes.add(resItem);
    }

    if ( NO_ERROR != ret )
    {
        CAMHAL_LOGEB("Error inserting supported picture sizes 0x%x", ret);
    }
    else
    {
        //for extended image size
        Vector<CameraResCfg> prevExRes = CameraConfigs::getPrevExRes(id);
        if(prevExRes.size() > 0)
        {
            params->mExtendedVideoResolution.clear();
            for(i =0; i < (int)prevExRes.size(); i++)
            {
                CameraResCfg resItem = prevExRes.itemAt(i);

                //whether supported by hw
                for(j =0; j < caps.ulPreviewResCount; j++)
                {
                    
                    if((int)caps.tPreviewRes[j].nWidth == resItem.width && (int)caps.tPreviewRes[j].nHeight == resItem.height)
                    {
                        break;
                    }
                }
                if(j >= caps.ulPreviewResCount)
                {

                    ExtendedResolution videoRes;
                    ret = findBestExSize(caps.tPreviewRes,  caps.ulPreviewResCount, resItem.width, resItem.height,&videoRes, false);
                    if(!ret)
                    {
                        prevSupportedRes.add(resItem);
                        params->mExtendedVideoResolution.add(videoRes);
                    }
                    ret = NO_ERROR;
                }

            }
        }

        prevSupportedRes.sort(resCompareFunc);
        for(i = 0; i < (int)prevSupportedRes.size(); i++)
        {
            CameraResCfg resItem = prevSupportedRes.itemAt(i);
            memset(&tmpBuffer, 0, sizeof(tmpBuffer));
            snprintf(tmpBuffer, sizeof(tmpBuffer),"%ux%u,",(unsigned int)resItem.width,(unsigned int)resItem.height);
            strncat(supported, tmpBuffer, MAX_PROP_NAME_LENGTH);
        }
        remove_last_sep(supported);
        params->set(CameraProperties::SUPPORTED_PREVIEW_SIZES, supported);

    }

    LOG_FUNCTION_NAME;

    return ret;
}


status_t OMXCameraAdapter::insertThumbSizes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    int i;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);
#if 0
    for(i =0; i< caps.ulThumbResCount; i++)
    {
        ret = encodeSizeCap(caps.tThumbRes[i],
                            (char *)supported,
                            MAX_PROP_VALUE_LENGTH);
    }
#else
	for(i =0; i< (int)ARRAY_SIZE(mThumbRes); i++)
    {
        strncat(supported, mThumbRes[i].param, MAX_PROP_NAME_LENGTH);
		strncat(supported, PARAM_SEP, MAX_PROP_NAME_LENGTH);
    }
	
#endif

    if ( NO_ERROR != ret )
    {
        CAMHAL_LOGEB("Error inserting supported thumbnail sizes 0x%x", ret);
    }
    else
    {
        strncat(supported, "0x0", MAX_PROP_NAME_LENGTH);
        params->set(CameraProperties::SUPPORTED_THUMBNAIL_SIZES, supported);
    }

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertZoom(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    size_t zoomStageCount = 0;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    zoomStageCount = encodeZoomCap(caps.xMaxWidthZoom,
                                   mZoomStages,
                                   ARRAY_SIZE(mZoomStages),
                                   supported,
                                   MAX_PROP_VALUE_LENGTH);

    params->set(CameraProperties::SUPPORTED_ZOOM_RATIOS, supported);
    params->set(CameraProperties::SUPPORTED_ZOOM_STAGES, zoomStageCount - 1); //As per CTS requirement

    if ( 0 == zoomStageCount )
    {
        params->set(CameraProperties::ZOOM_SUPPORTED, ActCameraParameters::ZOOM_UNSUPPORTED);
        params->set(CameraProperties::SMOOTH_ZOOM_SUPPORTED, ActCameraParameters::ZOOM_UNSUPPORTED);
    }
    else
    {
        params->set(CameraProperties::ZOOM_SUPPORTED, ActCameraParameters::ZOOM_SUPPORTED);
        params->set(CameraProperties::SMOOTH_ZOOM_SUPPORTED, ActCameraParameters::ZOOM_SUPPORTED);
    }

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertImageFormats(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

#if 0
    for ( int i = 0 ; i < caps.ulImageFormatCount ; i++ )
    {
        ret = encodePixelformatCap(caps.eImageFormats[i],
                                   mPixelformats,
                                   ARRAY_SIZE(mPixelformats),
                                   supported,
                                   MAX_PROP_VALUE_LENGTH);
        if ( NO_ERROR != ret )
        {
            CAMHAL_LOGEB("Error inserting supported picture formats 0x%x", ret);
            ret = NO_ERROR;
            continue;
        }
    }
#endif

    if ( NO_ERROR == ret )
    {
        //jpeg is not supported in OMX capabilies yet
        strncat(supported, CameraParameters::PIXEL_FORMAT_JPEG, MAX_PROP_VALUE_LENGTH - 1);
        params->set(CameraProperties::SUPPORTED_PICTURE_FORMATS, supported);
    }

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertPreviewFormats(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for ( int i = 0 ; i < caps.ulPreviewFormatCount; i++ )
    {
        ret = encodePixelformatCap(caps.ePreviewFormats[i],
                                   mPixelformats,
                                   ARRAY_SIZE(mPixelformats),
                                   supported,
                                   MAX_PROP_VALUE_LENGTH);
        if ( NO_ERROR != ret )
        {
            CAMHAL_LOGEB("Error inserting supported preview formats 0x%x", ret);
            ret = NO_ERROR;
            continue;
        }
    }

    if ( NO_ERROR == ret )
    {
        // need to advertise we support YV12 format
        // We will program preview port with NV21 when we see application set YV12
        //strncat(supported, CameraParameters::PIXEL_FORMAT_YUV420P, MAX_PROP_VALUE_LENGTH - 1);
        remove_last_sep(supported);
        params->set(CameraProperties::SUPPORTED_PREVIEW_FORMATS, supported);
    }

    LOG_FUNCTION_NAME;

    return ret;
}


status_t OMXCameraAdapter::insertFramerates(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    ret = encodeFramerateCap(FPS_MAX,
                             FPS_MIN,
                             mFramerates,
                             ARRAY_SIZE(mFramerates),
                             supported,
                             MAX_PROP_VALUE_LENGTH);

    if ( NO_ERROR != ret )
    {
        CAMHAL_LOGEB("Error inserting supported preview framerates 0x%x", ret);
    }
    else
    {
        params->set(CameraProperties::SUPPORTED_PREVIEW_FRAME_RATES, supported);
    }

    LOG_FUNCTION_NAME;

    return ret;
}


status_t OMXCameraAdapter::insertCapVFramerates(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    char fpsBuffer[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);
    memset(fpsBuffer, '\0', MAX_PROP_VALUE_LENGTH);

#ifdef CAMERA_FPS_FROM_OMX

    ret = encodeCapVFramerateCap(caps,
                                 supported,
                                 fpsBuffer,
                                 MAX_PROP_VALUE_LENGTH);

    if ( NO_ERROR != ret )
    {
        CAMHAL_LOGEB("Error inserting supported preview framerates 0x%x", ret);
    }
    else
    {
        remove_last_sep(supported);
        //remove_last_sep(fpsBuffer);
        params->set(CameraProperties::IMAGE_FRAMERATE_RANGE_SUPPORTED, supported);
    }
#else
        params->set(CameraProperties::IMAGE_FRAMERATE_RANGE_SUPPORTED, "(5000,30000)");
#endif

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertPrvVFramerates(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    char fpsBuffer[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);
    memset(fpsBuffer, '\0', MAX_PROP_VALUE_LENGTH);

#ifdef CAMERA_FPS_FROM_OMX
    ret = encodePrvVFramerateCap(caps,
                                 supported,
                                 fpsBuffer,
                                 MAX_PROP_VALUE_LENGTH);

    if ( NO_ERROR != ret )
    {
    }
    else
    {
        remove_last_sep(supported);
        remove_last_sep(fpsBuffer);
        params->set(CameraProperties::PREVIEW_FRAMERATE_RANGE_SUPPORTED, supported);
        params->set(CameraProperties::SUPPORTED_PREVIEW_FRAME_RATES, fpsBuffer);
    }
#else
    params->set(CameraProperties::PREVIEW_FRAMERATE_RANGE_SUPPORTED, "(5000,30000)");
    params->set(CameraProperties::SUPPORTED_PREVIEW_FRAME_RATES, "30");
#endif

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertEVs(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

	//FIX THIS
	caps.xEVCompensationMin = 0;
	caps.xEVCompensationMax = 0;
    CAMHAL_LOGDB("caps.xEVCompensationMin=%d,caps.xEVCompensationMax=%d",(int)caps.xEVCompensationMin,(int)caps.xEVCompensationMax);

    snprintf(supported, MAX_PROP_VALUE_LENGTH, "%d", ( int ) ( caps.xEVCompensationMin));
    params->set(CameraProperties::SUPPORTED_EV_MIN, supported);

    snprintf(supported, MAX_PROP_VALUE_LENGTH, "%d", ( int ) ( caps.xEVCompensationMax));
    params->set(CameraProperties::SUPPORTED_EV_MAX, supported);

    LOG_FUNCTION_NAME;

    return ret;
}



status_t OMXCameraAdapter::insertIPPModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    if(caps.bContrastSupported)
    {
        params->set(CameraProperties::CONTRAST_SUPPORTED, CameraParameters::TRUE);
		
		params->set(CameraProperties::SUPPORTED_CONTRAST_MIN, caps.xContrastLevel.nMinVal);
		params->set(CameraProperties::SUPPORTED_CONTRAST_MAX, caps.xContrastLevel.nMaxVal);
		params->set(CameraProperties::SUPPORTED_CONTRAST_STEP, caps.xContrastLevel.nStep);

    }
    else
    {
        params->set(CameraProperties::CONTRAST_SUPPORTED, CameraParameters::FALSE);
    }

    if(caps.bSaturationSupported)
    {
        params->set(CameraProperties::SATURATION_SUPPORTED, CameraParameters::TRUE);

		params->set(CameraProperties::SUPPORTED_SATURATION_MIN, caps.xSaturationLevel.nMinVal);
		params->set(CameraProperties::SUPPORTED_SATURATION_MAX, caps.xSaturationLevel.nMaxVal);
		params->set(CameraProperties::SUPPORTED_SATURATION_STEP, caps.xSaturationLevel.nStep);
    }
    else
    {
        params->set(CameraProperties::SATURATION_SUPPORTED, CameraParameters::FALSE);
    }

    if(caps.bBrightnessSupported)
    {
        params->set(CameraProperties::BRIGHTNESS_SUPPORTED, CameraParameters::TRUE);

		params->set(CameraProperties::SUPPORTED_BRIGHTNESS_MIN, caps.xBrightnessLevel.nMinVal);
		params->set(CameraProperties::SUPPORTED_BRIGHTNESS_MAX, caps.xBrightnessLevel.nMaxVal);
		params->set(CameraProperties::SUPPORTED_BRIGHTNESS_STEP, caps.xBrightnessLevel.nStep);
    }
    else
    {
        params->set(CameraProperties::BRIGHTNESS_SUPPORTED, CameraParameters::FALSE);
    }

	params->set(CameraProperties::SHARPNESS_SUPPORTED, CameraParameters::FALSE);

	params->set(CameraProperties::DENOISE_SUPPORTED, CameraParameters::FALSE);
	if(strcmp(params->get(CameraProperties::DENOISE_SUPPORTED), CameraParameters::TRUE) == 0)
	{
		params->set(CameraProperties::SUPPORTED_DENOISE_MIN, caps.xDeNoiseLevel.nMinVal);
		params->set(CameraProperties::SUPPORTED_DENOISE_MAX, caps.xDeNoiseLevel.nMaxVal);
		params->set(CameraProperties::SUPPORTED_DENOISE_STEP, caps.xDeNoiseLevel.nStep);
	}
		
    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertWBModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for ( unsigned int i = 0 ; i < caps.ulWhiteBalanceCount ; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eWhiteBalanceModes[i], WBalLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }

    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_WHITE_BALANCE, supported);

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertEffects(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for ( unsigned int i = 0 ; i < caps.ulColorEffectCount; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eColorEffects[i], EffLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }
    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_EFFECTS, supported);

    LOG_FUNCTION_NAME;

    return ret;
}


status_t OMXCameraAdapter::insertFlashModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps, int id)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    int flashSupported = 1;

    LOG_FUNCTION_NAME;

    flashSupported = CameraConfigs::getFlashSupported(id);

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    if(caps.ulFlashCount == 0 || (!flashSupported))
    {
        params->set(CameraProperties::SUPPORTED_FLASH_MODES, "");
        params->set(CameraProperties::FLASH_SUPPORTED, CameraParameters::FALSE);
        
        goto EXIT;
    }

    for ( unsigned int i = 0 ; i < caps.ulFlashCount; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eFlashModes[i], FlashLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }

    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_FLASH_MODES, supported);
    params->set(CameraProperties::FLASH_SUPPORTED, CameraParameters::TRUE);
EXIT:
    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertSceneModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    //exp mode as scene mode
    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for ( unsigned int i = 0 ; i < caps.ulExposureModeCount; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eExposureModes[i], ExpLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }

    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_SCENE_MODES, supported);

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertFocusModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    for ( unsigned int i = 0 ; i < caps.ulFocusModeCount; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eFocusModes[i], FocusLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }

    // Check if focus is supported by camera
    if ((caps.ulFocusModeCount <= 1 &&
            caps.eFocusModes[0] == OMX_IMAGE_FocusControlOff) ||  caps.ulFocusModeCount == 0)
    {
		memset(supported, '\0', MAX_PROP_VALUE_LENGTH);
        // Focus is not supported by camera
        // Advertise this to app as infinitiy focus mode
#ifdef CAMERA_FOCUS_MODE_AUTO
        strncat(supported, CameraParameters::FOCUS_MODE_AUTO, MAX_PROP_NAME_LENGTH);
        strncat(supported, PARAM_SEP, 1);
#endif
#ifdef CAMERA_FOCUS_MODE_INFINITY
        strncat(supported, CameraParameters::FOCUS_MODE_INFINITY, MAX_PROP_NAME_LENGTH);
#endif
        params->set(CameraProperties::FOCUS_SUPPORTED, CameraParameters::FALSE);
    }
    else
    {
        params->set(CameraProperties::FOCUS_SUPPORTED, CameraParameters::TRUE);
        
    }
    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_FOCUS_MODES, supported);

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertFlickerModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

	if(caps.ulFlickerCount == 0)
	{
        params->set(CameraProperties::SUPPORTED_ANTIBANDING, CameraParameters::ANTIBANDING_OFF);
        params->set(CameraProperties::ANTIBANDING_SUPPORTED, CameraParameters::FALSE);
        goto EXIT;
    }
    for ( unsigned int i = 0 ; i < caps.ulFlickerCount; i++ )
    {
        p = getLUTvalue_OMXtoHAL(caps.eFlicker[i], FlickerLUT);
        if ( NULL != p )
        {
            strncat(supported, p, MAX_PROP_NAME_LENGTH);
            strncat(supported, PARAM_SEP, 1);
        }
    }
    remove_last_sep(supported);
    params->set(CameraProperties::SUPPORTED_ANTIBANDING, supported);
    params->set(CameraProperties::ANTIBANDING_SUPPORTED, CameraParameters::TRUE);

EXIT:
    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertISOModes(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps) {
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];

    LOG_FUNCTION_NAME;
	
    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);
	if(caps.bISONoiseFilterSupported == OMX_FALSE)
	{
        params->set(CameraProperties::ISO_SUPPORTED, CameraParameters::FALSE);
		params->set(CameraProperties::SUPPORTED_ISO_VALUES, ActCameraParameters::ISO_MODE_AUTO);
	}
	else
	{
	    ret = encodeISOCap(caps.nSensitivityMax,
	                       mISOStages,
	                       ARRAY_SIZE(mISOStages),
	                       supported,
	                       MAX_PROP_VALUE_LENGTH);
	    if ( NO_ERROR != ret ) {
	    } else {
	        params->set(CameraProperties::SUPPORTED_ISO_VALUES, supported);
	    }
        params->set(CameraProperties::ISO_SUPPORTED, CameraParameters::TRUE);
	        CAMHAL_LOGEA("ISO_SUPPORTED is true");
	}

    LOG_FUNCTION_NAME;

    return ret;
}


status_t OMXCameraAdapter::insertAreas(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;

    LOG_FUNCTION_NAME;

    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

#ifdef CAMERA_FOCUS_AREA
    if ((caps.ulFocusModeCount <= 1 &&
            caps.eFocusModes[0] == OMX_IMAGE_FocusControlOff) ||  caps.ulFocusModeCount == 0)
    {
        params->set(CameraProperties::MAX_FOCUS_AREAS, 0);
		params->set(CameraProperties::MAX_REAL_FOCUS_AREAS, 0);
    }
    else
    {
		if(caps.ulAreasFocusCount <= 0)
		{
			//autofocus, default to support focus area
			int i = 0;
			for(i=0; i < caps.ulFocusModeCount; i++)
			{
				if(caps.eFocusModes[i] == OMX_IMAGE_FocusControlAuto)
				{
					break;
				}
			}
			if(i < caps.ulFocusModeCount)
			{
				params->set(CameraProperties::MAX_FOCUS_AREAS, 1);
			}
			else
			{
				params->set(CameraProperties::MAX_FOCUS_AREAS, 0);
			}
			params->set(CameraProperties::MAX_REAL_FOCUS_AREAS, 0);

		}
		else
		{
	        sprintf(supported, "%d", caps.ulAreasFocusCount);
	        params->set(CameraProperties::MAX_FOCUS_AREAS, supported);
			params->set(CameraProperties::MAX_REAL_FOCUS_AREAS, supported);
		}
    }
#else
    params->set(CameraProperties::MAX_FOCUS_AREAS, 0);
	params->set(CameraProperties::MAX_REAL_FOCUS_AREAS, 0);
#endif
    CAMHAL_LOGDB("Maximum supported focus areas %s", supported);

    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertLocks(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps)
{
    status_t ret = NO_ERROR;

    LOG_FUNCTION_NAME

    params->set(CameraProperties::AUTO_EXPOSURE_LOCK_SUPPORTED, ((caps.bExposureLockSupported == OMX_TRUE)? DEFAULT_LOCK_SUPPORTED : DEFAULT_LOCK_UNSUPPORTED));
    params->set(CameraProperties::AUTO_WHITEBALANCE_LOCK_SUPPORTED, ((caps.bWhiteBalanceLockSupported == OMX_TRUE)? DEFAULT_LOCK_SUPPORTED : DEFAULT_LOCK_UNSUPPORTED));
	params->set(CameraProperties::AUTO_FOCUS_LOCK_SUPPORTED, ((caps.bFocusLockSupported == OMX_TRUE)? DEFAULT_LOCK_SUPPORTED : DEFAULT_LOCK_UNSUPPORTED));
    LOG_FUNCTION_NAME

    return ret;
}

bool OMXCameraAdapter::isResolutionValid(unsigned int width, unsigned int height, const char *supportedResolutions)
{
    bool ret = true;
    status_t status = NO_ERROR;
    char tmpBuffer[PARAM_BUFFER+1];
    char *pos = NULL;

    LOG_FUNCTION_NAME;

    if ( NULL == supportedResolutions )
    {
        CAMHAL_LOGEA("Invalid supported resolutions string");
        ret = false;
        goto exit;
    }

    status = snprintf(tmpBuffer, PARAM_BUFFER, "%dx%d", width, height);
    if ( 0 > status )
    {
        CAMHAL_LOGEA("Error encountered while generating validation string");
        ret = false;
        goto exit;
    }

    pos = strstr(supportedResolutions, tmpBuffer);
    if ( NULL == pos )
    {
        ret = false;
    }
    else
    {
        ret = true;
    }

exit:

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

bool OMXCameraAdapter::isParameterValid(const char *param, const char *supportedParams)
{
    bool ret = true;
    char *pos = NULL;

    LOG_FUNCTION_NAME;

    if ( NULL == supportedParams )
    {
        CAMHAL_LOGEA("Invalid supported parameters string");
        ret = false;
        goto exit;
    }

    if ( NULL == param )
    {
        CAMHAL_LOGEA("Invalid parameter string");
        ret = false;
        goto exit;
    }

    pos = strstr(supportedParams, param);
    if ( NULL == pos )
    {
        ret = false;
    }
    else
    {
        ret = true;
    }

exit:

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

bool OMXCameraAdapter::isParameterValid(int param, const char *supportedParams)
{
    bool ret = true;
    char *pos = NULL;
    status_t status;
    char tmpBuffer[PARAM_BUFFER + 1];

    LOG_FUNCTION_NAME;

    if ( NULL == supportedParams )
    {
        CAMHAL_LOGEA("Invalid supported parameters string");
        ret = false;
        goto exit;
    }

    status = snprintf(tmpBuffer, PARAM_BUFFER, "%d", param);
    if ( 0 > status )
    {
        CAMHAL_LOGEA("Error encountered while generating validation string");
        ret = false;
        goto exit;
    }

    pos = strstr(supportedParams, tmpBuffer);
    if ( NULL == pos )
    {
        ret = false;
    }
    else
    {
        ret = true;
    }

exit:

    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

status_t OMXCameraAdapter::insertDefaults(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps, int id)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;
    char *pos;

    int defaultFps = 30;
    int fps=30, defaultFpsMin=30,defaultFpsMax=30;
    CameraResCfg defaultPrevRes;
    CameraResCfg defaultImageRes;

    LOG_FUNCTION_NAME;

    //get configs
    {
        int bestWidth=DEFAULT_PREVIEW_WIDTH, bestHeight = DEFAULT_PREVIEW_HEIGHT;
        ret = CameraConfigs::getDefaultPrevRes(id, &defaultPrevRes);
        if(ret)
        {
            findBestSize(caps.tPreviewRes,  caps.ulPreviewResCount, DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT, &bestWidth, &bestHeight);
        }
        else
        {
            findBestSize(caps.tPreviewRes,  caps.ulPreviewResCount, defaultPrevRes.width, defaultPrevRes.height, &bestWidth, &bestHeight);
        }
        defaultPrevRes.width = bestWidth;
        defaultPrevRes.height = bestHeight;
        ret = NO_ERROR;
    }
    ALOGE("defaultPrevRes,w=%d, h=%d", defaultPrevRes.width, defaultPrevRes.height);

    {
        int bestWidth=DEFAULT_PICTURE_WIDTH, bestHeight = DEFAULT_PICTURE_HEIGHT;
        ret = CameraConfigs::getDefaultImageRes(id, &defaultImageRes);    
        if(ret)
        {
            findBestSize(caps.tPreviewRes,  caps.ulPreviewResCount, DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT, &bestWidth, &bestHeight);
        }
        else
        {
            findBestSize(caps.tPreviewRes,  caps.ulPreviewResCount, defaultImageRes.width, defaultImageRes.height, &bestWidth, &bestHeight);

        }
        defaultImageRes.width = bestWidth;
        defaultImageRes.height = bestHeight;
        ret = NO_ERROR;
    }
    ALOGE("defaultImageRes,w=%d, h=%d", defaultImageRes.width, defaultImageRes.height);
    fps = CameraConfigs::getDefaultFps(id);    
    
    findBestFps(caps.tPrvVarFPSModes, caps.ulPrvVarFPSModesCount, fps, &defaultFpsMin, &defaultFpsMax, &defaultFps);

    if(strcmp(params->get(CameraProperties::ISO_SUPPORTED), CameraParameters::TRUE) != 0)
    {
	    params->set(CameraProperties::ISO_MODE, DEFAULT_ISO_MODE);
    }
    if( strcmp(params->get(CameraProperties::ANTIBANDING_SUPPORTED), CameraParameters::TRUE) == 0)
    { 
        params->set(CameraProperties::ANTIBANDING, DEFAULT_ANTIBANDING);
    }
    else
    {
        params->set(CameraProperties::ANTIBANDING, CameraParameters::ANTIBANDING_OFF);
    }
    params->set(CameraProperties::BRIGHTNESS, DEFAULT_BRIGHTNESS);
    params->set(CameraProperties::CONTRAST, DEFAULT_CONTRAST);
    params->set(CameraProperties::EFFECT, DEFAULT_EFFECT);
    if(caps.xEVCompensationMin == 0 && caps.xEVCompensationMax == 0)
    {
        params->set(CameraProperties::EV_COMPENSATION, 0);
        params->set(CameraProperties::SUPPORTED_EV_STEP, 0);
    }
    else
    {
        params->set(CameraProperties::EV_COMPENSATION, DEFAULT_EV_COMPENSATION);
        params->set(CameraProperties::SUPPORTED_EV_STEP, DEFAULT_EV_STEP);
    }
    params->set(CameraProperties::EXPOSURE_MODE, DEFAULT_EXPOSURE_MODE);
    params->set(CameraProperties::DENOISE, DEFAULT_DENOISE);
	if(strcmp(params->get(CameraProperties::FLASH_SUPPORTED),CameraParameters::TRUE) == 0)
	{
	    params->set(CameraProperties::FLASH_MODE, DEFAULT_FLASH_MODE);
	}
    else
    {
	    params->set(CameraProperties::FLASH_MODE, "");
    }
    pos = strstr(params->get(CameraProperties::SUPPORTED_FOCUS_MODES), DEFAULT_FOCUS_MODE_PREFERRED);
    if ( NULL != pos )
    {
        params->set(CameraProperties::FOCUS_MODE, DEFAULT_FOCUS_MODE_PREFERRED);
    }
    else
    {
        params->set(CameraProperties::FOCUS_MODE, DEFAULT_FOCUS_MODE);
    }
    params->set(CameraProperties::JPEG_QUALITY, DEFAULT_JPEG_QUALITY);
	
    params->set(CameraProperties::JPEG_THUMBNAIL_QUALITY, DEFAULT_THUMBNAIL_QUALITY);
	if(isParameterValid(DEFAULT_THUMBNAIL_SIZE, params->get(CameraProperties::SUPPORTED_THUMBNAIL_SIZES)))
	{
	    params->set(CameraProperties::JPEG_THUMBNAIL_SIZE, DEFAULT_THUMBNAIL_SIZE);
	}
	else
	{
		
        params->set(CameraProperties::JPEG_THUMBNAIL_SIZE, "128x96");
	}
	//TODO
    params->set(CameraProperties::PICTURE_FORMAT, DEFAULT_PICTURE_FORMAT);
	
    if(caps.ulImageResCount > 0)
    {
        memset(&supported, 0 ,sizeof(supported));
        snprintf((char *)&supported, sizeof(supported) -1, "%dx%d", (unsigned int)defaultImageRes.width, (unsigned int)defaultImageRes.height);
        params->set(CameraProperties::PICTURE_SIZE, (char *)&supported);
    }
    else
    {
        params->set(CameraProperties::PICTURE_SIZE, "640x480");
    }
	
	//TODO
    params->set(CameraProperties::PREVIEW_FORMAT, DEFAULT_PREVIEW_FORMAT);
	//TODO
	
#ifdef CAMERA_FPS_FROM_OMX
    if(caps.ulPrvVarFPSModesCount>0)
    {
        memset(&supported, 0 ,sizeof(supported));
        snprintf((char *)&supported, sizeof(supported) -1, "%d", (unsigned int)defaultFps);
        params->set(CameraProperties::PREVIEW_FRAME_RATE, (char *)&supported);
    }
    else
    {
        params->set(CameraProperties::PREVIEW_FRAME_RATE, "30");
    }

   
    if(caps.ulPrvVarFPSModesCount>0)
    {
        memset(&supported, 0 ,sizeof(supported));
        snprintf((char *)&supported, sizeof(supported) -1, "%d,%d", (unsigned int)defaultFpsMin*CameraHal::VFR_SCALE,(unsigned int)defaultFpsMax*CameraHal::VFR_SCALE);
        params->set(CameraProperties::PREVIEW_FRAMERATE_RANGE, (char *)&supported);
    }
    else
    {
        params->set(CameraProperties::PREVIEW_FRAMERATE_RANGE, "30000,30000");
    }
#else
    params->set(CameraProperties::PREVIEW_FRAME_RATE, "30");
    params->set(CameraProperties::PREVIEW_FRAMERATE_RANGE, "5000,30000");

#endif
	
    if(caps.ulPreviewResCount > 0)
    {
        memset(&supported, 0 ,sizeof(supported));
        snprintf((char *)&supported, sizeof(supported) -1, "%dx%d", (unsigned int)defaultPrevRes.width, (unsigned int)defaultPrevRes.height);
        params->set(CameraProperties::PREVIEW_SIZE, (char *)&supported);
    }
    else
    {
        params->set(CameraProperties::PREVIEW_SIZE, "640x480");
    }
	
    params->set(CameraProperties::REQUIRED_PREVIEW_BUFS, DEFAULT_NUM_PREV_BUFS);
    params->set(CameraProperties::REQUIRED_IMAGE_BUFS, DEFAULT_NUM_PIC_BUFS);
    params->set(CameraProperties::SATURATION, DEFAULT_SATURATION);
    params->set(CameraProperties::SCENE_MODE, DEFAULT_SCENE_MODE);
    params->set(CameraProperties::SHARPNESS, DEFAULT_SHARPNESS);
    params->set(CameraProperties::VSTAB, DEFAULT_VSTAB);
    params->set(CameraProperties::VSTAB_SUPPORTED, DEFAULT_VSTAB_SUPPORTED);
    params->set(CameraProperties::WHITEBALANCE, DEFAULT_WB);
    params->set(CameraProperties::ZOOM, DEFAULT_ZOOM);
    params->set(CameraProperties::MAX_FD_HW_FACES, DEFAULT_MAX_FD_HW_FACES);
    params->set(CameraProperties::MAX_FD_SW_FACES, DEFAULT_MAX_FD_SW_FACES);
    params->set(CameraProperties::AUTO_EXPOSURE_LOCK, DEFAULT_AE_LOCK);
    params->set(CameraProperties::AUTO_WHITEBALANCE_LOCK, DEFAULT_AWB_LOCK);
    if(id == 0)
    {
        params->set(CameraProperties::FOCAL_LENGTH, DEFAULT_FOCAL_LENGTH_PRIMARY);
    }
    else
    {
        params->set(CameraProperties::FOCAL_LENGTH, DEFAULT_FOCAL_LENGTH_SECONDARY);
    }
    params->set(CameraProperties::FOCUS_DISTANCES, DEFAULT_FOCUS_DISTANCES);
    params->set(CameraProperties::HOR_ANGLE, DEFAULT_HOR_ANGLE);
    params->set(CameraProperties::VER_ANGLE, DEFAULT_VER_ANGLE);
    params->set(CameraProperties::VIDEO_SNAPSHOT_SUPPORTED, DEFAULT_VIDEO_SNAPSHOT_SUPPORTED);

	params->set(CameraProperties::SUPPORTED_VIDEO_SIZES, params->get(CameraProperties::SUPPORTED_PREVIEW_SIZES));

    params->set(CameraProperties::VIDEO_SIZE, params->get(CameraProperties::PREVIEW_SIZE));

	
    params->set(CameraProperties::PREFERRED_PREVIEW_SIZE_FOR_VIDEO, params->get(CameraProperties::PREVIEW_SIZE));

    params->set(CameraProperties::MAX_NUM_METERING_AREAS, DEFAULT_MAX_NUM_METERING_AREAS);


    LOG_FUNCTION_NAME;

    return ret;
}

status_t OMXCameraAdapter::insertSenMount(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps,int id)
{
    status_t ret = NO_ERROR;
    char supported[MAX_PROP_VALUE_LENGTH];
    const char *p;
    unsigned int i = 0;
    int rotation = 0;

    LOG_FUNCTION_NAME;


    memset(supported, '\0', MAX_PROP_VALUE_LENGTH);

    // 1) Look up and assign sensor name
    if ( id >= (int)ARRAY_SIZE(mSensorNames) )
    {
        p = "UNKNOWN_SENSOR";
    }
    else
    {
        p = mSensorNames[id].param;
    }
    strncat(supported, p, MAX_PROP_NAME_LENGTH);
    params->set(CameraProperties::CAMERA_NAME, supported);


    // 2)assign sensor index
    params->set(CameraProperties::CAMERA_SENSOR_INDEX, id);
    

    // 3) Assign mounting rotation
    rotation = CameraConfigs::getCameraRotation();
    if(id == 0)
    {
        //params->set(CameraProperties::ORIENTATION_INDEX, DEFAULT_SENSOR_ROTATION);
        params->set(CameraProperties::ORIENTATION_INDEX, rotation);
    }
    else if(id == 1)
    {
        //params->set(CameraProperties::ORIENTATION_INDEX, DEFAULT_SENSOR_ROTATION_SEC);
        rotation = (360-rotation)%360;
        params->set(CameraProperties::ORIENTATION_INDEX, rotation);
    }
    else
    {
        params->set(CameraProperties::ORIENTATION_INDEX, "0");
    }

    LOG_FUNCTION_NAME;
    return ret;
}

status_t OMXCameraAdapter::insertCapabilities(CameraProperties::Properties* params, OMX_ACT_CAPTYPE &caps, int id)
{
    status_t ret = NO_ERROR;
    LOG_FUNCTION_NAME;

    if ( NO_ERROR == ret )
    {
        ret = insertPreviewFormats(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertImageFormats(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertPreviewSizes(params, caps, id);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertImageSizes(params, caps,id);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertThumbSizes(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertWBModes(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertEffects(params, caps);
    }
    //zoom ignore
    if ( NO_ERROR == ret )
    {
        ret = insertZoom(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertFlickerModes(params, caps);
    }

	if ( NO_ERROR == ret )
    {
        ret = insertISOModes(params, caps);
    }
	
    if ( NO_ERROR == ret )
    {
        ret = insertSceneModes(params, caps);
    }


    if ( NO_ERROR == ret )
    {
        ret = insertEVs(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertFocusModes(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertFlashModes(params, caps, id);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertIPPModes(params, caps);
    }


    if ( NO_ERROR == ret )
    {
        ret = insertPrvVFramerates(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertCapVFramerates(params, caps);
    }

    //if ( NO_ERROR == ret ) {
    //    ret = insertFramerates(params, caps);
    //}

    if ( NO_ERROR == ret)
    {
        ret = insertAreas(params, caps);
    }


    if ( NO_ERROR == ret )
    {
        ret = insertSenMount(params, caps, id);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertLocks(params, caps);
    }

    if ( NO_ERROR == ret )
    {
        ret = insertDefaults(params, caps,id);
    }


    LOG_FUNCTION_NAME_EXIT;

    return ret;
}

/*****************************************
 * public exposed function declarations
 *****************************************/
#define DUMP_CAPS_INTS(count,value)   \
    do{	\
        DUMP_CAPS_LOGB("dumpCaps---%s:", #value); \
        DUMP_CAPS_LOGB("\t %s= %d", #count,(int)count); \
        for(int i = 0 ; i < count; i++)		\
        {									\
            DUMP_CAPS_LOGB("\t %s[%d] = %d", #value, i, (int)value[i]);  \
        }	\
    }while(0)


#define DUMP_CAPS_RES(count,res)   \
    do{ \
        DUMP_CAPS_LOGB("dumpCaps---%s:", #res); \
        DUMP_CAPS_LOGB("\t %s= %d", #count, (int)count); \
        for(int i = 0 ; i < count; i++)		\
        {									\
            DUMP_CAPS_LOGB("\t %s[%d] = %dx%d", #res,i, (int)res[i].nWidth, (int)res[i].nHeight);  \
        }	\
    }while(0)

#define DUMP_CAPS_VFP(count,vfp)   \
    do{ \
        DUMP_CAPS_LOGB("dumpCaps---%s:", #vfp); \
        DUMP_CAPS_LOGB("\t %s= %d", #count, (int)count); \
        for(int i = 0 ; i < count; i++)		\
        {									\
            DUMP_CAPS_LOGB("\t %s[%d] = %dx%d", #vfp,i, (int)vfp[i].nVarFPSMin, (int)vfp[i].nVarFPSMax);  \
        }	\
    }while(0)


#define DUMP_CAPS_LEVLE(level)   \
	DUMP_CAPS_LOGB("dumpCaps---%s: min=%d, max=%d, step=%d", #level, (int)level.nMinVal, (int)level.nMaxVal, (int)level.nStep) 


#define DUMP_CAPS_BOOL(val)   \
	DUMP_CAPS_LOGB("dumpCaps---%s: %s", #val, (val==OMX_TRUE ? "true" :"false"))

#define DUMP_CAPS_INT(val)   \
	DUMP_CAPS_LOGB("dumpCaps---%s: %d", #val, (int)val)
	
void OMXCameraAdapter::dumpCaps(OMX_ACT_CAPTYPE &caps)
{
	DUMP_CAPS_INTS(caps.ulPreviewFormatCount, caps.ePreviewFormats);
	DUMP_CAPS_INTS(caps.ulImageFormatCount, caps.eImageFormats);
	DUMP_CAPS_RES(caps.ulPreviewResCount, caps.tPreviewRes);
	DUMP_CAPS_RES(caps.ulImageResCount, caps.tImageRes);
	DUMP_CAPS_RES(caps.ulThumbResCount, caps.tThumbRes);
	DUMP_CAPS_INTS(caps.ulWhiteBalanceCount, caps.eWhiteBalanceModes);
	DUMP_CAPS_INTS(caps.ulColorEffectCount, caps.eColorEffects);
	DUMP_CAPS_INT(caps.xMaxWidthZoom);
	DUMP_CAPS_INT(caps.xMaxHeightZoom);
	DUMP_CAPS_INTS(caps.ulFlickerCount, caps.eFlicker);
	DUMP_CAPS_INTS(caps.ulExposureModeCount, caps.eExposureModes);
	DUMP_CAPS_BOOL(caps.bISONoiseFilterSupported);
	DUMP_CAPS_INT(caps.xEVCompensationMin);
	DUMP_CAPS_INT(caps.xEVCompensationMax);
	DUMP_CAPS_INT(caps.nSensitivityMax);
	DUMP_CAPS_INTS(caps.ulFocusModeCount, caps.eFocusModes);
	DUMP_CAPS_INTS(caps.ulFlashCount, caps.eFlashModes);
	DUMP_CAPS_BOOL(caps.bContrastSupported);
	DUMP_CAPS_BOOL(caps.bSaturationSupported);
	DUMP_CAPS_BOOL(caps.bBrightnessSupported);
	DUMP_CAPS_BOOL(caps.bWhiteBalanceLockSupported);
	DUMP_CAPS_BOOL(caps.bExposureLockSupported);
	DUMP_CAPS_BOOL(caps.bFocusLockSupported);
	DUMP_CAPS_LEVLE(caps.xBrightnessLevel);
	DUMP_CAPS_LEVLE(caps.xDeNoiseLevel);
	DUMP_CAPS_LEVLE(caps.xSaturationLevel);
	DUMP_CAPS_LEVLE(caps.xContrastLevel);
	DUMP_CAPS_VFP(caps.ulPrvVarFPSModesCount, caps.tPrvVarFPSModes);
	DUMP_CAPS_VFP(caps.ulCapVarFPSModesCount, caps.tCapVarFPSModes);
	DUMP_CAPS_INT(caps.ulAreasFocusCount);
	
}
status_t OMXCameraAdapter::getCaps(CameraProperties::Properties* params, OMX_HANDLETYPE handle, int id)
{
    status_t ret = NO_ERROR;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_ACT_CAPTYPE* caps = NULL;

    LOG_FUNCTION_NAME;

    caps = new  OMX_ACT_CAPTYPE();

	

    if (!caps)
    {
        CAMHAL_LOGEB("Error allocating buffer for caps %d", eError);
        ret = -ENOMEM;
        goto EXIT;
    }

    // initialize structures to be passed to OMX Camera
    OMX_INIT_STRUCT_PTR (caps, OMX_ACT_CAPTYPE);
	
	//nPortIndex for select camera
    caps->nSensorIndex= id;

    // Get capabilities from OMX Camera
    eError =  OMX_GetConfig(handle, (OMX_INDEXTYPE) OMX_ACT_IndexConfigCamCapabilities, caps);
    if ( OMX_ErrorNone != eError || caps->ulPreviewResCount == 0)
    {
        CAMHAL_LOGEB("Error during capabilities query 0x%x", eError);
        ret = UNKNOWN_ERROR;
        goto EXIT;
    }
    else
    {
        CAMHAL_LOGDA("OMX capability query success");
    }

    // Translate and insert Ducati capabilities to CameraProperties
    if ( NO_ERROR == ret )
    {
		dumpCaps(*caps);
        ret = insertCapabilities(params, *caps, id);
    }

EXIT:
    if (caps)
    {
        delete caps;
        caps = NULL;
    }

    LOG_FUNCTION_NAME_EXIT;
    return ret;
}

};

