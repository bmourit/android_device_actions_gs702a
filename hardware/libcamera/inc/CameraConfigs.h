
#ifndef __CAMERA_CONFIG_H__
#define __CAMERA_CONFIG_H__

#include <utils/Vector.h>

namespace android
{
typedef struct
{
    int width;
    int height;
}CameraResCfg;

class CameraConfigs
{
public:

    static int getCameraRotation();
    static Vector<CameraResCfg> getPrevExRes(int id);
    static Vector<CameraResCfg> getImageExRes(int id);

    static int getDefaultPrevRes(int id, CameraResCfg *res);
    static int getDefaultImageRes(int id, CameraResCfg *res);

    static int getDefaultFps(int id);

    static int getFlashSupported(int id);
private:
    static const char BACK_PREV_EXRES[];
    static const char BACK_IMAGE_EXRES[];
    static const char FRONT_PREV_EXRES[];
    static const char FRONT_IMAGE_EXRES[];
    static const char BCAK_DEFAULT_PREV_RES[];
    static const char BCAK_DEFAULT_IMAGE_RES[];
    static const char FRONT_DEFAULT_PREV_RES[];
    static const char FRONT_DEFAULT_IMAGE_RES[];
    static const char SENSOR_ROTATION[];
    static const char BACK_DEFAULT_FPS[];
    static const char FRONT_DEFAULT_FPS[];
    
    static const char BACK_FLASH[];
    static const char FRONT_FLASH[];
};

}

#endif

