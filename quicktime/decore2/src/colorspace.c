/*---[start projectmayo code]------------------------------------------ */
/**************************************************************************
 *                                                                        *
 * This code has been developed by John Funnell. This software is an      *
 * implementation of a part of one or more MPEG-4 Video tools as          *
 * specified in ISO/IEC 14496-2 standard.  Those intending to use this    *
 * software module in hardware or software products are advised that its  *
 * use may infringe existing patents or copyrights, and any such use      *
 * would be at such party's own risk.  The original developer of this     *
 * software module and his/her company, and subsequent editors and their  *
 * companies (including Project Mayo), will have no liability for use of  *
 * this software or modifications or derivatives thereof.                 *
 *                                                                        *
 * Project Mayo gives users of the Codec a license to this software       *
 * module or modifications thereof for use in hardware or software        *
 * products claiming conformance to the MPEG-4 Video Standard as          *
 * described in the Open DivX license.                                    *
 *                                                                        *
 * The complete Open DivX license can be found at                         *
 * http://www.projectmayo.com/opendivx/license.php                        *
 *                                                                        *
 **************************************************************************/
/**
*  Copyright (C) 2001 - Project Mayo
 *
 * John Funnell 
 * Andrea Graziani
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
*
**/


#include "dec_colorspace.h"





/***

  /  2568      0   3343  \              
 |   2568  -0c92  -1a1e   | / 65536 * 8 
  \  2568   40cf      0  /              

    Y -= 16;
    U -= 128;
    V -= 128;

    R = (0x2568*Y + 0x0000*V + 0x3343*U) / 0x2000;
    G = (0x2568*Y - 0x0c92*V - 0x1a1e*U) / 0x2000;
    B = (0x2568*Y + 0x40cf*V + 0x0000*U) / 0x2000;

    R = R>255 ? 255 : R;
    R = R<0   ?   0 : R;

    G = G>255 ? 255 : G;
    G = G<0   ?   0 : G;

    B = B>255 ? 255 : B;
    B = B<0   ?   0 : B;

***/

#define _S(a)		(a)>255 ? 255 : (a)<0 ? 0 : (a)

#define _R(y,u,v) (0x2568*(y)  			       + 0x3343*(u)) /0x2000
#define _G(y,u,v) (0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u)) /0x2000
#define _B(y,u,v) (0x2568*(y) + 0x40cf*(v))					     /0x2000

struct lookuptable
{
    int32_t m_plY[256];
    int32_t m_plRV[256];
    int32_t m_plGV[256];
    int32_t m_plGU[256];
    int32_t m_plBU[256];
};
static struct lookuptable lut;


void init_yuv2rgb()
{
    int i;
    for(i=0; i<256; i++)
    {
	if(i>=16)
	    if(i>240)
		lut.m_plY[i]=lut.m_plY[240];
	    else
		lut.m_plY[i]=298*(i-16);
	else
	    lut.m_plY[i]=0;
	if((i>=16) && (i<=240))
	{
	    lut.m_plRV[i]=408*(i-128);
	    lut.m_plGV[i]=-208*(i-128);
	    lut.m_plGU[i]=-100*(i-128);
	    lut.m_plBU[i]=517*(i-128);
	}
	else if(i<16)
	{
	    lut.m_plRV[i]=408*(16-128);
	    lut.m_plGV[i]=-208*(16-128);
	    lut.m_plGU[i]=-100*(16-128);
	    lut.m_plBU[i]=517*(16-128);
	}
	else
	{
	    lut.m_plRV[i]=lut.m_plRV[240];
	    lut.m_plGV[i]=lut.m_plGV[240];
	    lut.m_plGU[i]=lut.m_plGU[240];
	    lut.m_plBU[i]=lut.m_plBU[240];
	}
    }
}

/* all stride values are in _pixels_ */

void YV12toYV12_generic(uint8_t *puc_y, int stride_y,
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
                unsigned int _stride_out)
{
	int i;
//printf("YV12toYV12_generic 1 %d %d %d\n", width_y, stride_y, height_y);
	for(i = 0; i < height_y; i++)
	{
		memcpy(puc_out + i * _stride_out, 
			puc_y + i * stride_y, 
			_stride_out);
	}

//printf("YV12toYV12_generic 1 %d %d %d\n", width_y, stride_y, height_y);
	for(i = 0; i < height_y / 2; i++)
	{
		memcpy(puc_out + _stride_out * height_y + i * _stride_out / 2,
			puc_v + i * stride_uv, 
			_stride_out / 2);
	}

//printf("YV12toYV12_generic 1 %d %d %d\n", width_y, stride_y, height_y);
	for(i = 0; i < height_y / 2; i++)
	{
		memcpy(puc_out + _stride_out * height_y * 5 / 4 + i * _stride_out / 2,
			puc_u + i * stride_uv, 
			_stride_out / 2);
	}
//printf("YV12toYV12_generic 2\n");
}


void YV12toRGB32_generic(uint8_t *puc_y, int stride_y, 
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
		unsigned int _stride_out) 
{

	int x, y;
	int stride_diff = 4 * (_stride_out - width_y);

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	for (y=0; y<height_y; y++) 
	{
		for (x=0; x<width_y; x++)
		{
			signed int _r,_g,_b; 
			signed int r, g, b;
			signed int y, u, v;

			y = puc_y[x] - 16;
			u = puc_u[x>>1] - 128;
			v = puc_v[x>>1] - 128;

			_r = _R(y,u,v);
			_g = _G(y,u,v);
			_b = _B(y,u,v);

			r = _S(_r);
			g = _S(_g);
			b = _S(_b);

			puc_out[0] = r;
			puc_out[1] = g;
			puc_out[2] = b;
			puc_out[3] = 0;

			puc_out+=4;
		}

		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_diff;
	}
}

/***/

// This be done more efficiently 
// ( we spend almost as much time only in here
// as DivX 3.11 spends on all decoding )
void YV12toRGB24_generic(uint8_t *puc_y, int stride_y, 
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
								unsigned int _stride_out) 
{

	int x, y;
	int stride_diff = 6*_stride_out - 3*width_y;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	for (y=0; y<height_y; y+=2) 
	{
		uint8_t* pY=puc_y;
		uint8_t* pY1=puc_y+stride_y;
		uint8_t* pU=puc_u;
		uint8_t* pV=puc_v;
		uint8_t* pOut2=puc_out+3*_stride_out;
		for (x=0; x<width_y; x+=2)
		{
			int R, G, B;
			int Y;
			unsigned int tmp;
			R=lut.m_plRV[*pV];
			G=lut.m_plGV[*pV];
			pV++;
			G+=lut.m_plGU[*pU];
			B=lut.m_plBU[*pU];
			pU++;
#define PUT_COMPONENT(p,v,i) 	\
    tmp=(unsigned int)(v); 	\
    if(tmp < 0x10000) 		\
	p[i]=tmp>>8; 		\
    else			\
	p[i]=(tmp >> 24) ^ 0xff; 
			Y=lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, B+Y, 0);
			PUT_COMPONENT(puc_out, G+Y, 1);
			PUT_COMPONENT(puc_out, R+Y, 2);
			Y=lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, B+Y, 3);
			PUT_COMPONENT(puc_out, G+Y, 4);
			PUT_COMPONENT(puc_out, R+Y, 5);
			Y=lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, B+Y, 0);
			PUT_COMPONENT(pOut2, G+Y, 1);
			PUT_COMPONENT(pOut2, R+Y, 2);
			Y=lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, B+Y, 3);
			PUT_COMPONENT(pOut2, G+Y, 4);
			PUT_COMPONENT(pOut2, R+Y, 5);
			puc_out+=6;
			pOut2+=6;
		}

		puc_y   += 2*stride_y;
		puc_u   += stride_uv;
		puc_v   += stride_uv;
		puc_out += stride_diff;
	}
}



// conversion from 4:2:0 to yuv2, 16 bit yuv output
//
void yuv_to_yuy2(uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, int width_y, int height_y,
	unsigned int stride_out) 
{ 
	int y;
	uint8_t* puc_out2;
	unsigned int stride_diff = 4 * stride_out - 2 * width_y; // expressed in bytes

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}
	puc_out2 = puc_out + 2 * stride_out;
	for (y=height_y/2; y; y--) {
		register uint8_t *py, *py2, *pu, *pv;
		register int x;
		uint32_t tmp;

		py = puc_y;
		py2 = puc_y + stride_y;
		pu = puc_u;
		pv = puc_v;
		for (x=width_y/2; x; x--) {
			tmp = *(py++);
			tmp |= *(pu++) << 8;
			tmp |= *(py++) << 16;
			tmp |= *(pv++) << 24;
			*(uint32_t*)puc_out=tmp;
			puc_out += 4;

			tmp &= 0xFF00FF00;
			tmp |= *(py2++);
			tmp |= *(py2++) << 16;
			*(uint32_t*)puc_out2=tmp;
			puc_out2 += 4;
		}

		puc_y += 2*stride_y;
		puc_u += stride_uv;
		puc_v += stride_uv;

		puc_out += stride_diff;
		puc_out2 += stride_diff;
	}

}


/*---[end projectmayo code]------------------------------------------ */


/* yuv 4:2:0 planar -> yuyv (yuv2) packed
   
   NOTE: does not flip */

void yuv_to_yuyv(uint8_t *dst, int dst_stride,
				 uint8_t *y_src, uint8_t *v_src, uint8_t * u_src, int y_stride,
				 int width, int height)
{
	int dst_dif = 2*(dst_stride - width);
	int y_dif = y_stride - width;
	int uv_dif = y_dif / 2;

	int x, y;

	for (y =0; y < height; y+=2)
	{
		for (x = 0; x < width; x+=2)
		{
			*dst++ = *y_src++;
			*dst = *(dst + 2*dst_stride) = *u_src++;			dst++;
			*dst++ = *y_src++;
			*dst = *(dst + 2*dst_stride) = *v_src++;			dst++;
		}
		dst += dst_dif;
		y_src += y_dif;
		u_src += uv_dif;
		v_src += uv_dif;

		for (x = 0; x < width; x+=2)
		{
			*dst++ = *y_src++;
			dst++;
			*dst++ = *y_src++;
			dst++;

		}
		dst += dst_dif;
		y_src += y_dif;
	}
}


/* yuv 4:2:0 planar -> uyvy packed
   
   NOTE: does not flip */

void yuv_to_uyvy(uint8_t *dst, int dst_stride,
				 uint8_t *y_src, uint8_t *u_src, uint8_t * v_src, int y_stride,
				 int width, int height)
{

	int dst_dif = 2*(dst_stride - width);
	int y_dif = y_stride - width;
	int uv_dif = y_dif / 2;

	int x, y;

	for (y =0; y < height; y+=2)
	{
		for (x = 0; x < width; x+=2)
		{
			*dst = *(dst + 2*dst_stride) = *u_src++;			dst++;
			*dst++ = *y_src++;
			*dst = *(dst + 2*dst_stride) = *v_src++;			dst++;
			*dst++ = *y_src++;
			
		}
		dst += dst_dif;
		y_src += y_dif;
		u_src += uv_dif;
		v_src += uv_dif;

		for (x = 0; x < width; x+=2)
		{
			dst++;
			*dst++ = *y_src++;
			dst++;
			*dst++ = *y_src++;
		}
		dst += dst_dif;
		y_src += y_dif;
	}
}





