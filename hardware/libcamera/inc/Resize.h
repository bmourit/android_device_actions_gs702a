#ifndef CAMERA_RESIZE_H
#define CAMERA_RESIZE_H

namespace android
{


const uint8_t bWeights[8][8][4] =
{
    {   {64, 0, 0, 0}, {56, 0, 0, 8}, {48, 0, 0,16}, {40, 0, 0,24},
        {32, 0, 0,32}, {24, 0, 0,40}, {16, 0, 0,48}, { 8, 0, 0,56}
    },

    {   {56, 8, 0, 0}, {49, 7, 1, 7}, {42, 6, 2,14}, {35, 5, 3,21},
        {28, 4, 4,28}, {21, 3, 5,35}, {14, 2, 6,42}, { 7, 1, 7,49}
    },

    {   {48,16, 0, 0}, {42,14, 2, 6}, {36,12,4 ,12}, {30,10,6 ,18},
        {24, 8, 8,24}, {18, 6,10,30}, {12,4 ,12,36}, { 6, 2,14,42}
    },

    {   {40,24,0 ,0 }, {35,21, 3, 5}, {30,18, 6,10}, {25,15, 9,15},
        {20,12,12,20}, {15, 9,15,25}, {10, 6,18,30}, { 5, 3,21,35}
    },

    {   {32,32, 0,0 }, {28,28, 4, 4}, {24,24, 8, 8}, {20,20,12,12},
        {16,16,16,16}, {12,12,20,20}, { 8, 8,24,24}, { 4, 4,28,28}
    },

    {   {24,40,0 ,0 }, {21,35, 5, 3}, {18,30,10, 6}, {15,25,15, 9},
        {12,20,20,12}, { 9,15,25,15}, { 6,10,30,18}, { 3, 5,35,21}
    },

    {   {16,48, 0,0 }, {14,42, 6, 2}, {12,36,12, 4}, {10,30,18, 6},
        {8 ,24,24,8 }, { 6,18,30,10}, { 4,12,36,12}, { 2, 6,42,14}
    },

    {   { 8,56, 0,0 }, { 7,49, 7, 1}, { 6,42,14, 2}, { 5,35,21, 3},
        { 4,28,28,4 }, { 3,21,35, 5}, { 2,14,42, 6}, { 1,7 ,49, 7}
    }
};


typedef union{
    uint8_t *rgb;
    struct{
        uint8_t *y;
        uint8_t *u;
        uint8_t *v;
        uint8_t du;
        uint8_t dv;
        uint32_t yStride;
        uint32_t uStride;
        uint32_t vStride;
    };

}RGBOrYUV_t;

typedef struct{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
}ImageCrop_t;

typedef struct{
    uint32_t inputW;
    uint32_t inputH;
    ImageCrop_t inputCrop;
    uint32_t outputW;
    uint32_t outputH;
}ImageResize_t;


void _YUV420Resize(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void NVXXResize(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void YX12Resize(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void NVXXResizeIA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void YX12ResizeIA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void NVXXResizeOA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void YX12ResizeOA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void NVXXResizeA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void YX12ResizeA(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
void RGB565Resize(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);

void YUYVResize(RGBOrYUV_t *out,
                    RGBOrYUV_t *in,
                    ImageResize_t *resize);
};
#endif //#define CAMERA_RESIZE_H_
