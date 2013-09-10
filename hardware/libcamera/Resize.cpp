#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CameraHalDebug.h"
#include "Resize.h"


#define ALIGN16(n) (((n)+0xf)&(~0xf))
namespace android
{
void RGB565Resize(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
	uint32_t row, col;
	uint32_t wStepFixed16b;
	uint32_t hStepFixed16b;

	uint16_t* in_rgb;
	uint16_t* out_rgb;
	uint32_t ox, oy, ow, oh;
	uint32_t cx, cy, cw, ch;
	uint32_t iw, ih;

	uint32_t stride;

	uint32_t wCoef;
	uint32_t hCoef;
	uint32_t hc1, hc2, wc1, wc2, offsetX, offsetY;
	uint32_t pixel1, pixel2, pixel3, pixel4;

	uint8_t r, g, b;

	if (!out || !in || !resize) {
		CAMHAL_LOGEA("Image resize parameters is invalid");
		return;
	}

	in_rgb = (uint16_t*)in->rgb;
	out_rgb = (uint16_t*)out->rgb;

	ox = 0;
	oy = 0;
	ow = resize->outputW;
	oh = resize->outputH;
	iw = resize->inputW;
	ih = resize->inputH;

	cx = resize->inputCrop.x;
	cy = resize->inputCrop.y;
	cw = resize->inputCrop.w;
	ch = resize->inputCrop.h;

	stride = iw;

	/* make sure valid input size */
	if (iw < 1 || ih < 1 || stride < 1) {
		return;
	}

	in_rgb = in_rgb + cy * stride + cx;

	wStepFixed16b = ((cw - 2) << 16) / (ow - 1);
	hStepFixed16b = ((ch - 2) << 16) / (oh - 1);

	hCoef = 0;
	////////////////////////////for Y//////////////////////////
	for (row = 0; row < oh; row++) {
		offsetY = (hCoef >> 16);
		hc2 = (hCoef >> 9) & 127;
		hc1 = 128 - hc2;

		wCoef = 0;
		for (col = 0; col < ow; col++) {
			offsetX = (wCoef >> 16);
			wc2 = (wCoef >> 9) & 127;
			wc1 = 128 - wc2;

			pixel1 = *(in_rgb + offsetY * stride + offsetX);
			pixel2 = *(in_rgb + (offsetY + 1) * stride + offsetX);
			pixel3 = *(in_rgb + offsetY * stride + offsetX + 1);
			pixel4 = *(in_rgb + (offsetY + 1) * stride + offsetX + 1);

			r= ((((pixel1 >> 11) & 0x1f) * hc1 + ((pixel2 >> 11) & 0x1f)
							* hc2) * wc1 + (((pixel3 >> 11) & 0x1f) * hc1
							+ ((pixel4 >> 11) & 0x1f) * hc2) * wc2) >> 14;
			g = ((((pixel1 >> 5) & 0x3f) * hc1 + ((pixel2 >> 5) & 0x3f) * hc2)
					* wc1 + (((pixel3 >> 5) & 0x3f) * hc1 + ((pixel4 >> 5)
					& 0x3f) * hc2) * wc2) >> 14;
			b = ((((pixel1) & 0x1f) * hc1 + ((pixel2) & 0x1f) * hc2)
							* wc1 + (((pixel3) & 0x1f) * hc1
							+ ((pixel4) & 0x1f) * hc2) * wc2) >> 14;

			*out_rgb = (r << 11) + (g << 5) + (b);
			out_rgb++;

			wCoef += wStepFixed16b;
		}
		hCoef += hStepFixed16b;
	}

	return;
}

void YUYVResize(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {

	uint32_t row, col;
	uint32_t wStepFixed16b;
	uint32_t hStepFixed16b;

	uint16_t* in_yuv;
	uint8_t* out_yuv;
	uint32_t ox, oy, ow, oh;
	uint32_t cx, cy, cw, ch;
	uint32_t iw, ih;

	uint32_t stride;

	uint32_t wCoef;
	uint32_t hCoef;
	uint32_t hc1, hc2, wc1, wc2, offsetX, offsetY;
	uint32_t pixel1, pixel2, pixel3, pixel4;
	uint32_t pixely1, pixely2, pixely3, pixely4;
	uint32_t pixelu1=0, pixelu2=0;
	uint32_t pixelv1=0, pixelv2=0;

	uint8_t y, u, v;

	in_yuv = (uint16_t *)in->y;
	out_yuv = out->y;

	ox = 0;
	oy = 0;
	ow = resize->outputW;
	oh = resize->outputH;
	iw = resize->inputW;
	ih = resize->inputH;

	cx = resize->inputCrop.x;
	cy = resize->inputCrop.y;
	cw = resize->inputCrop.w;
	ch = resize->inputCrop.h;

	stride = iw ;

	/* make sure valid input size */
	if (iw < 1 || ih < 1 || stride < 1) {
		return;
	}

	in_yuv = in_yuv + cy * stride + cx;

	wStepFixed16b = ((cw - 2) << 16) / (ow - 1);
	hStepFixed16b = ((ch - 2) << 16) / (oh - 1);

	hCoef = 0;
	////////////////////////////for Y//////////////////////////
	for (row = 0; row < oh; row++) {
		offsetY = (hCoef >> 16);
		hc2 = (hCoef >> 9) & 127;
		hc1 = 128 - hc2;

		wCoef = 0;
		for (col = 0; col < ow; col++) {
			//y
			offsetX = (wCoef >> 16);
			wc2 = (wCoef >> 9) & 127;
			wc1 = 128 - wc2;

			if (!(offsetX & 0x1)) {
				pixel1 = *((uint32_t *) (in_yuv + offsetY * stride + offsetX));
				pixel2 = *((uint32_t *) (in_yuv + (offsetY + 1) * stride + offsetX));

				pixely1 = (pixel1) & 0xff;
				pixely3 = (pixel1 >> 16) & 0xff;
				pixely2 = (pixel2) & 0xff;
				pixely4 = (pixel2 >> 16) & 0xff;

                if(!(col & 0x1))
                {
                    pixelu1 = (pixel1 >> 8) & 0xff;
                    pixelu2 = (pixel2 >> 8) & 0xff;
                }
                else
                {
                    pixelv1 = (pixel1 >> 24) & 0xff;
                    pixelv2 = (pixel2 >> 24) & 0xff;
                }

			}
			else
			{
				pixel1 = *(in_yuv + offsetY * stride + offsetX);
				pixel2 = *(in_yuv + (offsetY + 1) * stride + offsetX);
				pixel3 = *(in_yuv + offsetY * stride + offsetX + 1);
				pixel4 = *(in_yuv + (offsetY + 1) * stride + offsetX + 1);

				pixely1 = (pixel1 ) & 0xff;
				pixely2 = (pixel2 ) & 0xff;
				pixely3 = (pixel3 ) & 0xff;
				pixely4 = (pixel4 ) & 0xff;

                if(!(col & 0x1))
                {
                    pixelu1 = (pixel3>>8) & 0xff;
                    pixelu2 = (pixel4>>8) & 0xff;
                }
                else
                {
                    pixelv1 = (pixel1>>8) & 0xff;
                    pixelv2 = (pixel2>>8) & 0xff;
                }
			}

			y = ((pixely1 * hc1 + pixely2 * hc2) * wc1 + (pixely3 * hc1 + pixely4
					* hc2) * wc2) >> 14;

			*out_yuv = y;
			out_yuv++;

			if (!(col & 0x1)) {
				u = ((pixelu1 * hc1 + pixelu2 * hc2) * (wc1 + wc2)) >> 14;

				*out_yuv = u;
				out_yuv++;
			} else {
				v = ((pixelv1 * hc1 + pixelv2 * hc2) * (wc1 +wc2)) >> 14;

				*out_yuv = v;
				out_yuv++;
			}
			wCoef += wStepFixed16b;
		}
		hCoef += hStepFixed16b;
	}
	return;
}

void _YUV420Resize(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    uint32_t row, col;
    uint32_t wStepFixed16b;
    uint32_t hStepFixed16b;

    uint32_t ox, oy, ow, oh;
    uint32_t cx, cy, cw, ch;
    uint32_t iw, ih;

    uint8_t* inY;
    uint8_t* inU;
    uint8_t* inV;

    uint8_t* outY;
    uint8_t* outU;
    uint8_t* outV;

    uint8_t* outYLineHead;
    uint8_t* outULineHead;
    uint8_t* outVLineHead;

    uint32_t wCoef;
    uint32_t hCoef;
    uint32_t hc1, hc2, wc1, wc2, offsetX, offsetY;
    uint32_t pixel1, pixel2, pixel3, pixel4;

    uint8_t y, u, v;

    if (!out || !in || !resize) {
        CAMHAL_LOGEA("Image resize parameters is invalid");
        return;
    }

    inY = in->y;
    inU = in->u;
    inV = in->v;

    outY = out->y;
    outU = out->u;
    outV = out->v;

    ox = 0;
    oy = 0;
    ow = resize->outputW;
    oh = resize->outputH;
    iw = resize->inputW;
    ih = resize->inputH;
    cx = resize->inputCrop.x;
    cy = resize->inputCrop.y;
    cw = resize->inputCrop.w;
    ch = resize->inputCrop.h;


    CAMHAL_LOGDB("%s, iw=%d,ih=%d,ow=%d,oh=%d, cx=%d,cy=%d,cw=%d,ch=%d",__func__,iw,ih,ow,oh,cx,cy,cw,ch);
    /* make sure valid input size */
    if (iw < 1 || ih < 1) {
        CAMHAL_LOGVA("resizeFrameVideoNV12-");
        return;
    }

    //skip crop
    inY = inY + cy * in->yStride + cx;
    inU = inU + (cy >> 1) * in->uStride + (cx >> 1) * in->du;
    inV = inV + (cy >> 1) * in->vStride + (cx >> 1) * in->dv;

    wStepFixed16b = ((cw - 2) << 16) / (ow - 1);
    hStepFixed16b = ((ch - 2) << 16) / (oh - 1);

    hCoef = 0;
    ////////////////////////////for Y//////////////////////////
    outYLineHead = outY;
    outULineHead = outU;
    outVLineHead = outV;

    for (row = 0; row < oh; row++) {
        offsetY = (hCoef >> 16);
        hc2 = (hCoef >> 9) & 127;
        hc1 = 128 - hc2;

        wCoef = 0;
        for (col = 0; col < ow; col++) {
            //y
            offsetX = (wCoef >> 16);
            wc2 = (wCoef >> 9) & 127;
            wc1 = 128 - wc2;

            pixel1 = *(inY + offsetY * in->yStride + offsetX);
            pixel2 = *(inY + (offsetY + 1) * in->yStride + offsetX);
            pixel3 = *(inY + offsetY * in->yStride + offsetX + 1);
            pixel4 = *(inY + (offsetY + 1) * in->yStride + offsetX + 1);

            y = ((pixel1 * hc1 + pixel2 * hc2) * wc1 + (pixel3 * hc1 + pixel4
                    * hc2) * wc2) >> 14;

            *outY = y;
            outY++;



            if (row % 2 == 0 && col % 2 == 0) {
                //u
                pixel1
                    = *(inU + (offsetY >> 1) * in->uStride + (offsetX >> 1)
                        * in->du);
                pixel2 = *(inU + ((offsetY + 1) >> 1) * in->uStride
                    + (offsetX >> 1) * in->du);
                pixel3 = *(inU + (offsetY >> 1) * in->uStride
                    + ((offsetX + 1) >> 1) * in->du);
                pixel4 = *(inU + ((offsetY + 1) >> 1) * in->uStride
                    + ((offsetX + 1) >> 1) * in->du);

                u = ((pixel1 * hc1 + pixel2 * hc2) * wc1 + (pixel3 * hc1
                        + pixel4 * hc2) * wc2) >> 14;

                *outU = u;
                outU += out->du;

                //v
                pixel1
                    = *(inV + (offsetY >> 1) * in->vStride + (offsetX >> 1)
                        * in->dv);
                pixel2 = *(inV + ((offsetY + 1) >> 1) * in->vStride
                    + (offsetX >> 1) * in->dv);
                pixel3 = *(inV + (offsetY >> 1) * in->vStride
                    + ((offsetX + 1) >> 1) * in->dv);
                pixel4 = *(inV + ((offsetY + 1) >> 1) * in->vStride
                    + ((offsetX + 1) >> 1) * in->dv);

                v = ((pixel1 * hc1 + pixel2 * hc2) * wc1 + (pixel3 * hc1
                        + pixel4 * hc2) * wc2) >> 14;

                *outV = v;
                outV += out->dv;
            }

            wCoef += wStepFixed16b;

        }
        hCoef += hStepFixed16b;

        //output stride
        outY=(out->yStride + outYLineHead);
        outYLineHead = outY;

        if(row % 2 == 0)
        {
            outU=(out->uStride + outULineHead);
            outULineHead = outU;
            outV=(out->vStride + outVLineHead);
            outVLineHead = outV;
        }

    }
}

void NVXXResize(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = resize->inputW;
    in->uStride = resize->inputW;
    in->vStride = resize->inputW;
    in->du = 2;
    in->dv = 2;
    out->yStride = resize->outputW;
    out->uStride = resize->outputW;
    out->vStride = resize->outputW;
    out->du = 2;
    out->dv = 2;
	_YUV420Resize(out, in, resize);
}

void YX12Resize(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = resize->inputW;
    in->uStride = resize->inputW>>1;
    in->vStride = resize->inputW>>1;
    in->du = 1;
    in->dv = 1;
    out->yStride = resize->outputW;
    out->uStride = resize->outputW>>1;
    out->vStride = resize->outputW>>1;
    out->du = 1;
    out->dv = 1;
	_YUV420Resize(out, in, resize);
}

void NVXXResizeIA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = ALIGN16(resize->inputW);
    in->uStride = ALIGN16(resize->inputW);
    in->vStride = ALIGN16(resize->inputW);
    in->du = 2;
    in->dv = 2;
    out->yStride = resize->outputW;
    out->uStride = resize->outputW;
    out->vStride = resize->outputW;
    out->du = 2;
    out->dv = 2;
	_YUV420Resize(out, in, resize);
}

void YX12ResizeIA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = ALIGN16(resize->inputW);
    in->uStride = ALIGN16(resize->inputW>>1);
    in->vStride = ALIGN16(resize->inputW>>1);
    in->du = 1;
    in->dv = 1;
    out->yStride = resize->outputW;
    out->uStride = resize->outputW>>1;
    out->vStride = resize->outputW>>1;
    out->du = 1;
    out->dv = 1;
	_YUV420Resize(out, in, resize);
}
void NVXXResizeOA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = resize->inputW;
    in->uStride = resize->inputW;
    in->vStride = resize->inputW;
    in->du = 2;
    in->dv = 2;
    out->yStride = ALIGN16(resize->outputW);
    out->uStride = ALIGN16(resize->outputW);
    out->vStride = ALIGN16(resize->outputW);
    out->du = 2;
    out->dv = 2;
	_YUV420Resize(out, in, resize);
}

void YX12ResizeOA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = resize->inputW;
    in->uStride = resize->inputW>>1;
    in->vStride = resize->inputW>>1;
    in->du = 1;
    in->dv = 1;
    out->yStride = ALIGN16(resize->outputW);
    out->uStride = ALIGN16(resize->outputW>>1);
    out->vStride = ALIGN16(resize->outputW>>1);
    out->du = 1;
    out->dv = 1;
	_YUV420Resize(out, in, resize);
}
void NVXXResizeA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = ALIGN16(resize->inputW);
    in->uStride = ALIGN16(resize->inputW);
    in->vStride = ALIGN16(resize->inputW);
    in->du = 2;
    in->dv = 2;
    out->yStride = ALIGN16(resize->outputW);
    out->uStride = ALIGN16(resize->outputW);
    out->vStride = ALIGN16(resize->outputW);
    out->du = 2;
    out->dv = 2;
	_YUV420Resize(out, in, resize);
}

void YX12ResizeA(RGBOrYUV_t *out, RGBOrYUV_t *in, ImageResize_t *resize) {
    in->yStride = ALIGN16(resize->inputW);
    in->uStride = ALIGN16(resize->inputW>>1);
    in->vStride = ALIGN16(resize->inputW>>1);
    in->du = 1;
    in->dv = 1;
    out->yStride = ALIGN16(resize->outputW);
    out->uStride = ALIGN16(resize->outputW>>1);
    out->vStride = ALIGN16(resize->outputW>>1);
    out->du = 1;
    out->dv = 1;
	_YUV420Resize(out, in, resize);
}
};
