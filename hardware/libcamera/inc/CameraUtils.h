
#include "CameraHal.h"

#include <OMX_IVCommon.h>

#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H



namespace android
{

class CameraFrame;

OMX_COLOR_FORMATTYPE toOMXPixFormat(const char* parameters_format);
int toCameraFormat(const char* parameters_format);
const char* getPixFormatConstant(const char* parameters_format);
size_t getBufSize(const char* parameters_format, int width, int height);
size_t getBufBpp(const char* parameters_format);

void saveFile(unsigned char   *buff, int width, int height, int bpp);
bool cameraFrameResize(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer);
bool cameraFrameToNV21YV12_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer, int src_len,bool forvce);
void cameraDeltaTime(const char *str, struct timeval *start);
void cameraStartTime(struct timeval *start);

unsigned long long cameraGetMs();

void clearUnalignedBuf(unsigned char *buf, int w, int h, int length,int f);
}

#endif
