
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>
#include <dlfcn.h>
#include <ctype.h>

#include "CameraHalDebug.h"

#include "cutils/properties.h"

#include "CameraResTable.h"
#include "CameraConfigs.h"

namespace android
{

const char CameraConfigs::BACK_PREV_EXRES[] = "ro.camerahal.prevres0";
const char CameraConfigs::BACK_IMAGE_EXRES[] = "ro.camerahal.imageres0";
const char CameraConfigs::FRONT_PREV_EXRES[] = "ro.camerahal.prevres1";
const char CameraConfigs::FRONT_IMAGE_EXRES[] = "ro.camerahal.imageres1";
const char CameraConfigs::BCAK_DEFAULT_PREV_RES[] = "ro.camerahal.prevresdft0";
const char CameraConfigs::BCAK_DEFAULT_IMAGE_RES[] = "ro.camerahal.imageresdft0";
const char CameraConfigs::FRONT_DEFAULT_PREV_RES[] ="ro.camerahal.prevresdft1";
const char CameraConfigs::FRONT_DEFAULT_IMAGE_RES[] = "ro.camerahal.imageresdft1";
const char CameraConfigs::SENSOR_ROTATION[] = "ro.camerahal.configorientation";
const char CameraConfigs::BACK_DEFAULT_FPS[] = "ro.camerahal.fpsdft0";
const char CameraConfigs::FRONT_DEFAULT_FPS[] = "ro.camerahal.fpsdft1";

const char CameraConfigs::BACK_FLASH[] = "ro.camerahal.flash0";
const char CameraConfigs::FRONT_FLASH[] = "ro.camerahal.flash1";


int CameraConfigs::getCameraRotation()
{
    char value[PROPERTY_VALUE_MAX];

    property_get(CameraConfigs::SENSOR_ROTATION, value, "0");

    if(strcmp(value, "0") != 0 
        && strcmp(value, "90") != 0
        && strcmp(value, "180") != 0
        && strcmp(value, "270") != 0)
    {
        memset(value, 0 , PROPERTY_VALUE_MAX);
        value[0] = '0';
    }      

    return atoi(value);
}

static char * myTrim(char *str)
{
    char *last,*cur;
    if(str==NULL)
    {
        return NULL;
    }
    for( ;isspace(*str); str++);
    for(last=cur=str; *cur!='\0'; cur++)
    {
        if(!isspace(*cur))
        {
            last=cur;
        }
    }
    *(++last)=0;
    return str;
}

static char * myUpcase(char *str)
{
    char *cur;
    if(str==NULL)
    {
        return NULL;
    }
    for(cur = str; *cur != '\0'; cur++)
    {
        if(*cur>='a' && *cur <= 'z')
        {
            *cur += ('A'-'a');
        }
    }
    
    return str;
} 

static int parserResList(char *prop, Vector<CameraResCfg> *reslist)
{
    char *a;
    char *b;
    const CameraResItem *resItem;
    CameraResCfg res;

    reslist->clear();
    a = prop;

    if(a == NULL || a[0]=='\0')
    {
        return 0;
    }

    for (;;) {
        b = strchr(a, ',');
        if (b == NULL) {

            a =  myTrim(a);
            a =  myUpcase(a);
            if(a != NULL)
            {
                ALOGE("parserResList:%s",a);
                resItem = CameraResTable::getResByName(a);
                if(resItem)
                {
                    res.width = resItem->width;
                    res.height = resItem->height;
                    reslist->add(res);
                }
            }

            break;
        }

        *b = 0;
        a =  myTrim(a);
        a =  myUpcase(a);
        if(a != NULL)
        {
            ALOGE("parserResList:%s",a);
            resItem = CameraResTable::getResByName(a);
            if(resItem)
            {
                res.width = resItem->width;
                res.height = resItem->height;
                reslist->add(res);
            }
        }
        a = b+1;
    }

    return 0;
    
}
static int parserRes(char *prop, CameraResCfg *res)
{
    char *a;
    char *b;
    const  CameraResItem *resItem;

    a = prop;

    if(a == NULL || a[0]=='\0')
    {
        return -1;
    }
    a = myTrim(a);
    a = myUpcase(a);

    ALOGE("parserRes:%s",a);
    resItem = CameraResTable::getResByName(a);
    if(resItem)
    {
        res->width = resItem->width;
        res->height = resItem->height;
        return 0;
    }

    return -1;
}


Vector<CameraResCfg> CameraConfigs::getPrevExRes(int id)
{
    char value[PROPERTY_VALUE_MAX];
    Vector<CameraResCfg> resList;

    if(id == 0)
    {
        property_get(CameraConfigs::BACK_PREV_EXRES, value, "");
    }
    else
    {
        property_get(CameraConfigs::FRONT_PREV_EXRES, value, "");
    }
    ALOGE("getPrevExRes:%s",value);

    parserResList(value, &resList);
    return resList; 

}

Vector<CameraResCfg> CameraConfigs::getImageExRes(int id)
{
    char value[PROPERTY_VALUE_MAX];
    Vector<CameraResCfg> resList;

    if(id == 0)
    {
        property_get(CameraConfigs::BACK_IMAGE_EXRES, value, "");
    }
    else
    {
        property_get(CameraConfigs::FRONT_IMAGE_EXRES, value, "");
    }

    ALOGE("getImageExRes:%s",value);
    parserResList(value, &resList);
    return resList; 
}

int CameraConfigs::getDefaultPrevRes(int id, CameraResCfg *res)
{
    char value[PROPERTY_VALUE_MAX];
    int ret = 0;

    if(id == 0)
    {
        property_get(CameraConfigs::BCAK_DEFAULT_PREV_RES, value, "");
    }
    else
    {
        property_get(CameraConfigs::FRONT_DEFAULT_PREV_RES, value, "");
    }

    ret = parserRes(value, res);
    return ret; 
}

int CameraConfigs::getDefaultImageRes(int id,  CameraResCfg *res)
{
    char value[PROPERTY_VALUE_MAX];
    int ret = 0;

    if(id == 0)
    {
        property_get(CameraConfigs::BCAK_DEFAULT_IMAGE_RES, value, "");
    }
    else
    {
        property_get(CameraConfigs::FRONT_DEFAULT_IMAGE_RES, value, "");
    }

    ret = parserRes(value, res);
    return ret; 
}


int CameraConfigs::getDefaultFps(int id)
{
    char value[PROPERTY_VALUE_MAX];
    int fps;

    if(id == 0)
    {
        property_get(CameraConfigs::BACK_DEFAULT_FPS, value, "30");
    }
    else
    {
        property_get(CameraConfigs::FRONT_DEFAULT_FPS, value, "30");

    }

    fps = atoi(value);
    if(fps<5 || fps>60)
    {
        fps = 0;
    }

    return fps;
}

int CameraConfigs::getFlashSupported(int id)
{
    char value[PROPERTY_VALUE_MAX];
    int flash = 1;

    if(id == 0)
    {
        property_get(CameraConfigs::BACK_FLASH, value, "1");
    }
    else
    {
        property_get(CameraConfigs::FRONT_FLASH, value, "1");
    }

    flash = atoi(value);

    return flash;
}
};

