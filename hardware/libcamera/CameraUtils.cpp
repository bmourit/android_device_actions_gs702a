#include "CameraHalDebug.h"

#include "CameraHal.h"
#include "Resize.h"

#include <OMX_IVCommon.h>

#define ALIGN16(n) (((n)+0xf)&(~0xf))

#define SIMPLE_YUV420_CONV 1

namespace android
{

OMX_COLOR_FORMATTYPE toOMXPixFormat(const char* parameters_format)
{
    OMX_COLOR_FORMATTYPE pixFormat;

    if ( parameters_format != NULL )
    {
        if (strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV422I) == 0)
        {
            CAMHAL_LOGDA("CbYCrY format selected");
            pixFormat = OMX_COLOR_FormatCbYCrY;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        {
            CAMHAL_LOGDA("YUV420SP format selected");
            pixFormat = OMX_COLOR_FormatYUV420SemiPlanar;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        {
            CAMHAL_LOGDA("YUV420P format selected");
            pixFormat = OMX_COLOR_FormatYUV420Planar;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_RGB565) == 0)
        {
            CAMHAL_LOGDA("RGB565 format selected");
            pixFormat = OMX_COLOR_Format16bitRGB565;
        }
        else
        {
            CAMHAL_LOGDA("Invalid format, CbYCrY format selected as default");
            pixFormat = OMX_COLOR_FormatCbYCrY;
        }
    }
    else
    {
        CAMHAL_LOGEA("Preview format is NULL, defaulting to CbYCrY");
        pixFormat = OMX_COLOR_FormatCbYCrY;
    }

    return pixFormat;
}

int toCameraFormat(const char* parameters_format)
{
    CameraFrame::FrameFormat pixFormat;

    if ( parameters_format != NULL )
    {
        if (strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV422I) == 0)
        {
            CAMHAL_LOGDA("CbYCrY format selected");
            pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_YUV422I;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        {
            CAMHAL_LOGDA("YUV420SP format selected");
            pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_YUV420SP;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        {
            CAMHAL_LOGDA("YUV420P format selected");
            pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_YUV420P;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_RGB565) == 0)
        {
            CAMHAL_LOGDA("RGB565 format selected");
            pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_RGB565;
        }
        else
        {
            CAMHAL_LOGDA("Invalid format, CbYCrY format selected as default");
            pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_YUV422I;
        }
    }
    else
    {
        CAMHAL_LOGEA("Preview format is NULL, defaulting to CbYCrY");
        pixFormat = CameraFrame::CAMERA_FRAME_FORMAT_YUV422I;
    }

    return pixFormat;
}

const char* getPixFormatConstant(const char* parameters_format)
{
    const char* pixFormat;

    if ( parameters_format != NULL )
    {
        if (strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV422I) == 0)
        {
            CAMHAL_LOGVA("CbYCrY format selected");
            pixFormat = (const char *) CameraParameters::PIXEL_FORMAT_YUV422I;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420SP) == 0 ||
                strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        {
            // TODO(XXX): We are treating YV12 the same as YUV420SP
            CAMHAL_LOGVA("YUV420SP format selected");
            pixFormat = (const char *) CameraParameters::PIXEL_FORMAT_YUV420SP;
        }
        else if(strcmp(parameters_format, (const char *) CameraParameters::PIXEL_FORMAT_RGB565) == 0)
        {
            CAMHAL_LOGVA("RGB565 format selected");
            pixFormat = (const char *) CameraParameters::PIXEL_FORMAT_RGB565;
        }
        else
        {
            CAMHAL_LOGEA("Invalid format, CbYCrY format selected as default");
            pixFormat = (const char *) CameraParameters::PIXEL_FORMAT_YUV422I;
        }
    }
    else
    {
        CAMHAL_LOGEA("Preview format is NULL, defaulting to CbYCrY");
        pixFormat = (const char *) CameraParameters::PIXEL_FORMAT_YUV422I;
    }

    return pixFormat;
}
size_t getBufBpp(const char* parameters_format)
{
    size_t bpp;

    if ( parameters_format != NULL )
    {
        if (strcmp(parameters_format,
                   (const char *) CameraParameters::PIXEL_FORMAT_YUV422I) == 0)
        {
            bpp = 16;
        }
        else if((strcmp(parameters_format, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0) ||
                (strcmp(parameters_format, CameraParameters::PIXEL_FORMAT_YUV420P) == 0))
        {
            bpp = 12;
        }
        else if(strcmp(parameters_format,
                       (const char *) CameraParameters::PIXEL_FORMAT_RGB565) == 0)
        {
            bpp = 16;
        }
        else
        {
            CAMHAL_LOGEA("Invalid format");
            bpp = 16;
        }
    }
    else
    {
        CAMHAL_LOGEA("Preview format is NULL");
        bpp = 16;
    }

    return bpp;
}

size_t getBufSize(const char* parameters_format, int width, int height)
{
    size_t buf_size;

    if ( parameters_format != NULL )
    {
        if (strcmp(parameters_format,
                   (const char *) CameraParameters::PIXEL_FORMAT_YUV422I) == 0)
        {
            buf_size = width * height * 2;
        }
        else if((strcmp(parameters_format, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0) ||
                (strcmp(parameters_format, CameraParameters::PIXEL_FORMAT_YUV420P) == 0))
        {
            buf_size = width * height * 3 / 2;
        }
        else if(strcmp(parameters_format,
                       (const char *) CameraParameters::PIXEL_FORMAT_RGB565) == 0)
        {
            buf_size = width * height * 2;
        }
        else
        {
            CAMHAL_LOGEA("Invalid format");
            buf_size = 0;
        }
    }
    else
    {
        CAMHAL_LOGEA("Preview format is NULL");
        buf_size = 0;
    }

    return buf_size;
}

void saveFile(unsigned char   *buff, int width, int height, int bpp)
{
    static int      counter = 1;
    int             fd = -1;
    char            fn[256];

    LOG_FUNCTION_NAME;

    fn[0] = 0;
    sprintf(fn, "/data/camera_data/preview_%d_%d_%03d.yuv", width, height, counter);
    fd = open(fn, O_CREAT | O_WRONLY | O_SYNC | O_TRUNC, 0611);
    if(fd < 0)
    {
        ALOGE("Unable to open file %s: %s", fn, strerror(fd));
        return;
    }

    CAMHAL_LOGVB("Copying from 0x%p, size=%d x %d", buff, width, height);

    //method currently supports only nv12 dumping
    int stride = width*bpp>>3;
    uint8_t *bf = (uint8_t*) buff;
    for(int i=0; i<height; i++)
    {
        write(fd, bf, stride);
        bf += stride;
    }

    close(fd);

    counter++;

    LOG_FUNCTION_NAME_EXIT;
}



bool cameraFrameResizeNV12_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  


    in.y = src_buffer;
    in.u = src_buffer +  cameraframe.mOrigWidth * cameraframe.mOrigHeight;
    in.v = in.u + 1;

    out.y = dst_buffer;
    out.u = dst_buffer +  ALIGN16(cameraframe.mEncodeWidth) * cameraframe.mEncodeHeight;
    out.v = out.u + 1;

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

    
    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;

    if((resize.inputW == resize.inputCrop.w)
        &&(resize.inputH == resize.inputCrop.h)
        &&(resize.inputCrop.x == 0)
        &&(resize.inputCrop.y==0))
    {
        memcpy(dst_buffer, src_buffer, (resize.inputW*resize.inputH*12)>>3);
    }
    else
    {
        NVXXResizeOA(&out, &in, &resize);
    }

    return true;
}


bool cameraFrameResizeNV12ToNV21_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  

    in.y = src_buffer;
    in.u = src_buffer +  cameraframe.mOrigWidth * cameraframe.mOrigHeight;
    in.v = in.u + 1;

    out.y = dst_buffer;
    out.v = dst_buffer +  ALIGN16(cameraframe.mEncodeWidth) * cameraframe.mEncodeHeight;
    out.u = out.v + 1;

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

   
    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;
    

    NVXXResizeOA(&out, &in, &resize);       
    return true;
}



bool cameraFrameResizeYUYV(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  

    in.y = src_buffer;

    out.y = dst_buffer;

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;
    

    if((resize.inputW == resize.inputCrop.w)
        &&(resize.inputH == resize.inputCrop.h)
        &&(resize.inputCrop.x == 0)
        &&(resize.inputCrop.y==0))
    {
        memcpy(dst_buffer, src_buffer, (resize.inputW*resize.inputH*16)>>3);
    }
    else
    {
        YUYVResize(&out, &in, &resize);
    }
    return true;
}


bool cameraFrameResizeYU12_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  

    in.y = src_buffer;
    in.u = src_buffer +  cameraframe.mOrigWidth * cameraframe.mOrigHeight;
    in.v = in.u + ((cameraframe.mOrigWidth * cameraframe.mOrigHeight)>>2);

    out.y = dst_buffer;
    out.u = dst_buffer +  ALIGN16(cameraframe.mEncodeWidth) * cameraframe.mEncodeHeight;
    out.v = out.u + (ALIGN16(cameraframe.mEncodeWidth>>1) * (cameraframe.mEncodeHeight>>1));

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

   
    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;
    

    if((resize.inputW == resize.inputCrop.w)
        &&(resize.inputH == resize.inputCrop.h)
        &&(resize.inputCrop.x == 0)
        &&(resize.inputCrop.y==0))
    {
        memcpy(dst_buffer, src_buffer, (resize.inputW*resize.inputH*12)>>3);
    }
    else
    {
        YX12ResizeOA(&out, &in, &resize); 
    }
    return true;
}


bool cameraFrameResizeYU12ToYV12_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  

    in.y = src_buffer;
    in.u = src_buffer +  cameraframe.mOrigWidth * cameraframe.mOrigHeight;
    in.v = in.u + ((cameraframe.mOrigWidth * cameraframe.mOrigHeight)>>2);

    out.y = dst_buffer;
    out.v = dst_buffer +  ALIGN16(cameraframe.mEncodeWidth) * cameraframe.mEncodeHeight;
    out.u = out.v + (ALIGN16(cameraframe.mEncodeWidth>>1) * (cameraframe.mEncodeHeight>>1));

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

    
    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;
    

    YX12ResizeOA(&out, &in, &resize);   
    return true;
}



bool cameraFrameResizeRGB565(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    RGBOrYUV_t in, out;
    ImageResize_t resize;

    if (!src_buffer || !dst_buffer)
    {
        return false;
    }  

    in.rgb = src_buffer;

    out.rgb = dst_buffer;

    resize.inputW = cameraframe.mOrigWidth;
    resize.inputH = cameraframe.mOrigHeight;
    resize.inputCrop.x = cameraframe.mXOff;
    resize.inputCrop.y = cameraframe.mYOff;
    resize.inputCrop.w =  cameraframe.mWidth;
    resize.inputCrop.h = cameraframe.mHeight;

   
    resize.outputW = cameraframe.mEncodeWidth;
    resize.outputH = cameraframe.mEncodeHeight;
    

    if((resize.inputW == resize.inputCrop.w)
        &&(resize.inputH == resize.inputCrop.h)
        &&(resize.inputCrop.x == 0)
        &&(resize.inputCrop.y == 0))
    {
        memcpy(dst_buffer, src_buffer, (resize.inputW*resize.inputH*16)>>3);
    }
    else
    {
        RGB565Resize(&out, &in, &resize); 
    }
    return true;
}

bool cameraFrameResize(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer)
{
    //struct timeval start_time;
    if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_RGB565)
    {
        return cameraFrameResizeRGB565(cameraframe, src_buffer, dst_buffer);
    }
    else if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_YUV420P)
    {
#if 0
        return cameraFrameResizeYU12(cameraframe, src_buffer, dst_buffer);
#else
		return cameraFrameResizeYU12ToYV12_OA(cameraframe, src_buffer, dst_buffer);
#endif
    }
    else if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_YUV420SP)
    {
#if 0
        return cameraFrameResizeNV12(cameraframe, src_buffer, dst_buffer);
#else
		return cameraFrameResizeNV12ToNV21_OA(cameraframe, src_buffer, dst_buffer);

#endif
    }
    else if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_YUV422I)
    {
        return cameraFrameResizeYUYV(cameraframe, src_buffer, dst_buffer);
    }
    else
    {
        return false;
    }
    //cameraDeltaTime("cameraFrameResize ", &start_time);
}


bool cameraFrameToNV21YV12_OA(CameraFrame &cameraframe, unsigned char* src_buffer, unsigned char* dst_buffer, int src_len,bool forvce)
{


    int stride;
    int w, h;
    int row,col;
    unsigned char *src,*dst;
    int padY = 0;

    w = cameraframe.mEncodeWidth;
    h = cameraframe.mEncodeHeight;

    if(forvce)
    {
        stride = (w+0xf)&(~0xf);
        padY = ((h+0xf)&(~0xf)) - h;
    }
    else
    {
        stride = w;
        padY = 0;
    }
    src = src_buffer ;
    dst = dst_buffer;

    if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_YUV420P)
    {

        //copy y
        for(row = 0; row < h; row++)
        {
            memcpy(dst, src, w);
            src+=stride;
            dst+=ALIGN16(w);
        }
        src+=stride*padY;

#if 1
        if(forvce)
        {
            //vce not support YU12 resize, the data is NV12 actually
            //should convert to YV12
            uint8_t *srcUV = src;

            //v
            for(row = 0; row < (h>>1); row++)
            {
                for (col = 0; col < (w>>1) ; col++)
                {
                    dst[col] = src[(col<<1)+1];
                }
                src=stride+src;
                dst=ALIGN16(w>>1)+dst;

            }
            //u
            src = srcUV;
            for(row = 0; row < (h>>1); row++)
            {
                for (col = 0; col < (w>>1) ; col++)
                {
                    dst[col] = src[col<<1];
                }
                src=stride+src;
                dst=ALIGN16(w>>1)+dst;
            }
        }
        else
#endif
        {
            //copy u
            dst = dst_buffer + w*h+((w*h)>>2);
            for(row = 0; row < (h>>1); row++)
            {
                memcpy(dst, src, w>>1);
                src+=(stride>>1);
                dst+=ALIGN16(w>>1);
            }
            src+=(stride*padY)>>2;

            //copy v
            dst = dst_buffer + w*h;
            for(row = 0; row < (h>>1); row++)
            {
                memcpy(dst, src, w>>1);
                src+=(stride>>1);
                dst+=ALIGN16(w>>1);
            }
        }

    }
    else if(cameraframe.mFormat == CameraFrame::CAMERA_FRAME_FORMAT_YUV420SP)
    {
        //copy y
        for(row = 0; row < h; row++)
        {
            memcpy(dst, src, w);
            src+=stride;
            dst+=ALIGN16(w);
        }
        src+=stride*padY;

#if SIMPLE_YUV420_CONV
        //copy uv to vu
        for(row = 0; row < (h>>1); row++)
        {
            memcpy(dst, src+1, w-1);
            dst[w-1]=dst[w-2];
            src+=stride;
            dst+=ALIGN16(w);
        }

#else
        //copy uv to vu
        for(row = 0; row < (h>>1); row++)
        {
            for (col = 0; col < (w>>1) ; col++)
            {
                dst[col<<1] = src[(col<<1)+1];
                dst[(col<<1)+1] = src[col<<1];
            }
            src=stride+src;
            dst=ALIGN16(w)+dst;
        }
#endif

    }
    else
    {
        //not 420p and 420sp, not supported,just copy it
        memcpy(dst_buffer, src_buffer, src_len);
    }


    return true;

}     



void cameraStartTime(struct timeval *start)
{
    gettimeofday(start, NULL);
}

void cameraDeltaTime(const char *str, struct timeval *start)
{
    struct timeval end;

    unsigned long long elapsed;
    gettimeofday(&end, NULL);
    elapsed = end.tv_sec - start->tv_sec;
    elapsed *= 1000000;
    elapsed += end.tv_usec - start->tv_usec;

    CAMHAL_LOGDB("deltaTime: %s :%llu.%llu ms", str, ( elapsed /1000 ), ( elapsed % 1000 ));
}


unsigned long long cameraGetMs()
{
    struct timeval time;
    unsigned long long ms;

    gettimeofday(&time, NULL);
    ms = time.tv_sec;
    ms *= 1000;
    ms += time.tv_usec /1000;

    return ms;
}        


void clearUnalignedBuf(unsigned char *buf, int w, int h, int length,int f)
{
    int i=0;
    unsigned char *p = NULL;
    int alignedlne = 0;
    int unalignedlne = 0;

    if(w%32 == 0)
    {
        return;
    }
    unalignedlne = w%32;
    alignedlne = w-unalignedlne;
    if(f == CameraFrame::CAMERA_FRAME_FORMAT_YUV420SP)
    {
        //y
        p= buf+alignedlne;
        for(i = 0; i < h; i++)
        {
            memset(p,0,unalignedlne); 
            p+=w;
        }
        //uv
        p= buf+w*h+alignedlne;
        for(i = 0; i < (h>>1); i++)
        {
            memset(p,128,unalignedlne); 
            p+=w;
        }

    }
    else if(f == CameraFrame::CAMERA_FRAME_FORMAT_YUV420P)
    {
        //y
        p= buf+alignedlne;
        for(i = 0; i < h; i++)
        {
            memset(p,0,unalignedlne); 
            p+=w;
        }
        //u
        p= buf+w*h+(alignedlne>>1);
        for(i = 0; i < (h>>1); i++)
        {
            memset(p,128,(unalignedlne>>1)); 
            p+=(w>>1);
        }
        //v
        p= buf+w*h*5/4+(alignedlne>>1);
        for(i = 0; i < (h>>1); i++)
        {
            memset(p,128,(unalignedlne>>1)); 
            p+=(w>>1);
        }
    }
    else
    {
        memset(buf, 0, length);
    }

}

};

