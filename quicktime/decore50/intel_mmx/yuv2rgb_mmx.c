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
// yuv2rgb_mmx.c //

/*
 MMX version of colourspace conversion   

 13 Feb 2001  - John Funnell, added -height feature to invert output, fixed 24-bit overwrite
 
 TODO:
  1. Extract common part of from 32, 24 and 16 bit conversions into inline func 
     to avoid duplication of the core MMX code.
  2. Experiment with using prefetch instructions to improve performance


 Using matrix of SMPTE 170M

 This is what we're doing: 

	 Step 1.                                 
	 Y -= 16                                 
	 U -= 128                                
	 V -= 128                                

	 Step 2.                                 
	 Y /= 219                                
	 U /= 224                                
	 V /= 224                                

	 Step 3.                                 
	 now we want the inverse of this matrix: 
	  /  0.299  0.114  0.587  \              
	 |  -0.169  0.500 -0.331   |             
	  \  0.500 -0.081 -0.419  /              

	 which is, approximately:                
	  /  2568      0   3343  \               
	 |   2568   f36e   e5e2   | / 65536 * 8  
	  \  2568   40cf      0  /               
	 including the multiplies in Step 2      

*/


#include <memory.h> // for memset() & memcpy()


#include "portab.h"
#include "yuv2rgb.h"

/**
 *
**/

#define MAXIMUM_Y_WIDTH 800


#define _USE_PREFETCH



/* static constants */

/* colourspace conversion matrix values */
static uint64_t mmw_mult_Y    = 0x2568256825682568;
static uint64_t mmw_mult_U_G  = 0xf36ef36ef36ef36e;
static uint64_t mmw_mult_U_B  = 0x40cf40cf40cf40cf;
static uint64_t mmw_mult_V_R  = 0x3343334333433343;
static uint64_t mmw_mult_V_G  = 0xe5e2e5e2e5e2e5e2;


/* various masks and other constants */
static uint64_t mmb_0x10      = 0x1010101010101010;
static uint64_t mmw_0x0080    = 0x0080008000800080;
static uint64_t mmw_0x00ff    = 0x00ff00ff00ff00ff;

static uint64_t mmw_cut_red   = 0x7c007c007c007c00;
static uint64_t mmw_cut_green = 0x03e003e003e003e0;
static uint64_t mmw_cut_blue  = 0x001f001f001f001f;



/**** YUV -> RGB conversion, 32-bit output ****/
/* if height_y is negative then the output image will be inverted */
/* 
		note: _stride_out parameter is ignored in yuv to rgb conversion 
		it's assumed that stride_out = 4 * width_y for the 32 bit color bitmap
*/
void yuv2rgb_32(uint8_t *puc_y, int stride_y, 
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
								unsigned int _stride_out) {

	int y, horiz_count;
	int stride_out = width_y * 4;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	horiz_count = -(width_y >> 3);

	for (y=0; y<height_y; y++) {
	
		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, puc_out       
			mov ebx, puc_y       
			mov ecx, puc_u       
			mov edx, puc_v
			mov edi, horiz_count
			
		horiz_loop:

			movd mm2, [ecx]
			pxor mm7, mm7

			movd mm3, [edx]
			punpcklbw mm2, mm7       ; mm2 = __u3__u2__u1__u0

			movq mm0, [ebx]          ; mm0 = y7y6y5y4y3y2y1y0  
			punpcklbw mm3, mm7       ; mm3 = __v3__v2__v1__v0

			movq mm1, mmw_0x00ff     ; mm1 = 00ff00ff00ff00ff 

			psubusb mm0, mmb_0x10    ; mm0 -= 16

			psubw mm2, mmw_0x0080    ; mm2 -= 128
			pand mm1, mm0            ; mm1 = __y6__y4__y2__y0

			psubw mm3, mmw_0x0080    ; mm3 -= 128
			psllw mm1, 3             ; mm1 *= 8

			psrlw mm0, 8             ; mm0 = __y7__y5__y3__y1
			psllw mm2, 3             ; mm2 *= 8

			pmulhw mm1, mmw_mult_Y   ; mm1 *= luma coeff 
			psllw mm0, 3             ; mm0 *= 8

			psllw mm3, 3             ; mm3 *= 8
			movq mm5, mm3            ; mm5 = mm3 = v

			pmulhw mm5, mmw_mult_V_R ; mm5 = red chroma
			movq mm4, mm2            ; mm4 = mm2 = u

			pmulhw mm0, mmw_mult_Y   ; mm0 *= luma coeff 
			movq mm7, mm1            ; even luma part

			pmulhw mm2, mmw_mult_U_G ; mm2 *= u green coeff 
			paddsw mm7, mm5          ; mm7 = luma + chroma    __r6__r4__r2__r0

			pmulhw mm3, mmw_mult_V_G ; mm3 *= v green coeff  
			packuswb mm7, mm7        ; mm7 = r6r4r2r0r6r4r2r0

			pmulhw mm4, mmw_mult_U_B ; mm4 = blue chroma
			paddsw mm5, mm0          ; mm5 = luma + chroma    __r7__r5__r3__r1

			packuswb mm5, mm5        ; mm6 = r7r5r3r1r7r5r3r1
			paddsw mm2, mm3          ; mm2 = green chroma

			movq mm3, mm1            ; mm3 = __y6__y4__y2__y0
			movq mm6, mm1            ; mm6 = __y6__y4__y2__y0

			paddsw mm3, mm4          ; mm3 = luma + chroma    __b6__b4__b2__b0
			paddsw mm6, mm2          ; mm6 = luma + chroma    __g6__g4__g2__g0
			
			punpcklbw mm7, mm5       ; mm7 = r7r6r5r4r3r2r1r0
			paddsw mm2, mm0          ; odd luma part plus chroma part    __g7__g5__g3__g1

			packuswb mm6, mm6        ; mm2 = g6g4g2g0g6g4g2g0
			packuswb mm2, mm2        ; mm2 = g7g5g3g1g7g5g3g1

			packuswb mm3, mm3        ; mm3 = b6b4b2b0b6b4b2b0
			paddsw mm4, mm0          ; odd luma part plus chroma part    __b7__b5__b3__b1

			packuswb mm4, mm4        ; mm4 = b7b5b3b1b7b5b3b1
			punpcklbw mm6, mm2       ; mm6 = g7g6g5g4g3g2g1g0

			punpcklbw mm3, mm4       ; mm3 = b7b6b5b4b3b2b1b0

			/* 32-bit shuffle.... */
			pxor mm0, mm0            ; is this needed?

			movq mm1, mm6            ; mm1 = g7g6g5g4g3g2g1g0
			punpcklbw mm1, mm0       ; mm1 = __g3__g2__g1__g0

			movq mm0, mm3            ; mm0 = b7b6b5b4b3b2b1b0
			punpcklbw mm0, mm7       ; mm0 = r3b3r2b2r1b1r0b0

			movq mm2, mm0            ; mm2 = r3b3r2b2r1b1r0b0

			punpcklbw mm0, mm1       ; mm0 = __r1g1b1__r0g0b0
			punpckhbw mm2, mm1       ; mm2 = __r3g3b3__r2g2b2

			/* 32-bit save... */
			movq  [eax], mm0         ; eax[0] = __r1g1b1__r0g0b0
			movq mm1, mm6            ; mm1 = g7g6g5g4g3g2g1g0

			movq 8[eax], mm2         ; eax[8] = __r3g3b3__r2g2b2

			/* 32-bit shuffle.... */
			pxor mm0, mm0            ; is this needed?

			punpckhbw mm1, mm0       ; mm1 = __g7__g6__g5__g4

			movq mm0, mm3            ; mm0 = b7b6b5b4b3b2b1b0
			punpckhbw mm0, mm7       ; mm0 = r7b7r6b6r5b5r4b4

			movq mm2, mm0            ; mm2 = r7b7r6b6r5b5r4b4

			punpcklbw mm0, mm1       ; mm0 = __r5g5b5__r4g4b4
			punpckhbw mm2, mm1       ; mm2 = __r7g7b7__r6g6b6

			/* 32-bit save... */
			add ebx, 8               ; puc_y   += 8;
			add ecx, 4               ; puc_u   += 4;

			movq 16[eax], mm0        ; eax[16] = __r5g5b5__r4g4b4
			add edx, 4               ; puc_v   += 4;

			movq 24[eax], mm2        ; eax[24] = __r7g7b7__r6g6b6
			
			// 0 1 2 3 4 5 6 7 rgb save order

			add eax, 32              ; puc_out += 32

			inc edi
			jne horiz_loop			

			pop edi 
			pop edx 
			pop ecx
			pop ebx 
			pop eax

			emms
						
		}
		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_out;
	}
}







/**** YUV -> RGB conversion, 24-bit output ****/
void yuv2rgb_24(uint8_t *puc_y, int stride_y, 
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
								unsigned int _stride_out) {

	int y, horiz_count;
	uint8_t *puc_out_remembered;
	int stride_out = width_y * 3;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	horiz_count = -(width_y >> 3);

	for (y=0; y<height_y; y++) {

		if (y == height_y-1) {
			/* this is the last output line - we need to be careful not to overrun the end of this line */
			uint8_t temp_buff[3*MAXIMUM_Y_WIDTH+1];
			puc_out_remembered = puc_out;
			puc_out = temp_buff; /* write the RGB to a temporary store */
		}

		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, puc_out       
			mov ebx, puc_y       
			mov ecx, puc_u       
			mov edx, puc_v
			mov edi, horiz_count
			
		horiz_loop:

			movd mm2, [ecx]
			pxor mm7, mm7

			movd mm3, [edx]
			punpcklbw mm2, mm7       ; mm2 = __u3__u2__u1__u0

			movq mm0, [ebx]          ; mm0 = y7y6y5y4y3y2y1y0  
			punpcklbw mm3, mm7       ; mm3 = __v3__v2__v1__v0

			movq mm1, mmw_0x00ff     ; mm1 = 00ff00ff00ff00ff 

			psubusb mm0, mmb_0x10    ; mm0 -= 16

			psubw mm2, mmw_0x0080    ; mm2 -= 128
			pand mm1, mm0            ; mm1 = __y6__y4__y2__y0

			psubw mm3, mmw_0x0080    ; mm3 -= 128
			psllw mm1, 3             ; mm1 *= 8

			psrlw mm0, 8             ; mm0 = __y7__y5__y3__y1
			psllw mm2, 3             ; mm2 *= 8

			pmulhw mm1, mmw_mult_Y   ; mm1 *= luma coeff 
			psllw mm0, 3             ; mm0 *= 8

			psllw mm3, 3             ; mm3 *= 8
			movq mm5, mm3            ; mm5 = mm3 = v

			pmulhw mm5, mmw_mult_V_R ; mm5 = red chroma
			movq mm4, mm2            ; mm4 = mm2 = u

			pmulhw mm0, mmw_mult_Y   ; mm0 *= luma coeff 
			movq mm7, mm1            ; even luma part

			pmulhw mm2, mmw_mult_U_G ; mm2 *= u green coeff 
			paddsw mm7, mm5          ; mm7 = luma + chroma    __r6__r4__r2__r0

			pmulhw mm3, mmw_mult_V_G ; mm3 *= v green coeff  
			packuswb mm7, mm7        ; mm7 = r6r4r2r0r6r4r2r0

			pmulhw mm4, mmw_mult_U_B ; mm4 = blue chroma
			paddsw mm5, mm0          ; mm5 = luma + chroma    __r7__r5__r3__r1

			packuswb mm5, mm5        ; mm6 = r7r5r3r1r7r5r3r1
			paddsw mm2, mm3          ; mm2 = green chroma

			movq mm3, mm1            ; mm3 = __y6__y4__y2__y0
			movq mm6, mm1            ; mm6 = __y6__y4__y2__y0

			paddsw mm3, mm4          ; mm3 = luma + chroma    __b6__b4__b2__b0
			paddsw mm6, mm2          ; mm6 = luma + chroma    __g6__g4__g2__g0
			
			punpcklbw mm7, mm5       ; mm7 = r7r6r5r4r3r2r1r0
			paddsw mm2, mm0          ; odd luma part plus chroma part    __g7__g5__g3__g1

			packuswb mm6, mm6        ; mm2 = g6g4g2g0g6g4g2g0
			packuswb mm2, mm2        ; mm2 = g7g5g3g1g7g5g3g1

			packuswb mm3, mm3        ; mm3 = b6b4b2b0b6b4b2b0
			paddsw mm4, mm0          ; odd luma part plus chroma part    __b7__b5__b3__b1

			packuswb mm4, mm4        ; mm4 = b7b5b3b1b7b5b3b1
			punpcklbw mm6, mm2       ; mm6 = g7g6g5g4g3g2g1g0

			punpcklbw mm3, mm4       ; mm3 = b7b6b5b4b3b2b1b0

			/* 32-bit shuffle.... */
			pxor mm0, mm0            ; is this needed?

			movq mm1, mm6            ; mm1 = g7g6g5g4g3g2g1g0
			punpcklbw mm1, mm0       ; mm1 = __g3__g2__g1__g0

			movq mm0, mm3            ; mm0 = b7b6b5b4b3b2b1b0
			punpcklbw mm0, mm7       ; mm0 = r3b3r2b2r1b1r0b0

			movq mm2, mm0            ; mm2 = r3b3r2b2r1b1r0b0

			punpcklbw mm0, mm1       ; mm0 = __r1g1b1__r0g0b0
			punpckhbw mm2, mm1       ; mm2 = __r3g3b3__r2g2b2

			/* 24-bit shuffle and save... */
			movd   [eax], mm0        ; eax[0] = __r0g0b0
			psrlq mm0, 32            ; mm0 = __r1g1b1

			movd  3[eax], mm0        ; eax[3] = __r1g1b1

			movd  6[eax], mm2        ; eax[6] = __r2g2b2
			

			psrlq mm2, 32            ; mm2 = __r3g3b3
	
			movd  9[eax], mm2        ; eax[9] = __r3g3b3

			/* 32-bit shuffle.... */
			pxor mm0, mm0            ; is this needed?

			movq mm1, mm6            ; mm1 = g7g6g5g4g3g2g1g0
			punpckhbw mm1, mm0       ; mm1 = __g7__g6__g5__g4

			movq mm0, mm3            ; mm0 = b7b6b5b4b3b2b1b0
			punpckhbw mm0, mm7       ; mm0 = r7b7r6b6r5b5r4b4

			movq mm2, mm0            ; mm2 = r7b7r6b6r5b5r4b4

			punpcklbw mm0, mm1       ; mm0 = __r5g5b5__r4g4b4
			punpckhbw mm2, mm1       ; mm2 = __r7g7b7__r6g6b6

			/* 24-bit shuffle and save... */
			movd 12[eax], mm0        ; eax[12] = __r4g4b4
			psrlq mm0, 32            ; mm0 = __r5g5b5
			
			movd 15[eax], mm0        ; eax[15] = __r5g5b5
			add ebx, 8               ; puc_y   += 8;

			movd 18[eax], mm2        ; eax[18] = __r6g6b6
			psrlq mm2, 32            ; mm2 = __r7g7b7
			
			add ecx, 4               ; puc_u   += 4;
			add edx, 4               ; puc_v   += 4;

			movd 21[eax], mm2        ; eax[21] = __r7g7b7
			add eax, 24              ; puc_out += 24

			inc edi
			jne horiz_loop			

			pop edi 
			pop edx 
			pop ecx
			pop ebx 
			pop eax

			emms
						
		}


		if (y == height_y-1) {
			/* last line of output - we have used the temp_buff and need to copy... */
			int x = 3 * width_y;                  /* interation counter */
			uint8_t *ps = puc_out;                /* source pointer (temporary line store) */
			uint8_t *pd = puc_out_remembered;     /* dest pointer       */
			while (x--) *(pd++) = *(ps++);	      /* copy the line      */
		}

		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_out; 

	}
}







/**** YUV -> RGB conversion, 16-bit output (two flavours) ****/

/* 5 Jan 2001  - Andrea Graziani    */

static uint64_t mask_5		= 0xf8f8f8f8f8f8f8f8;
static uint64_t mask_6		= 0xfcfcfcfcfcfcfcfc;

static uint64_t mask_blue	= 0x1f1f1f1f1f1f1f1f;


/* all stride values are in _bytes_ */
void yuv2rgb_555(uint8_t *puc_y, int stride_y, 
                 uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                 uint8_t *puc_out,	int width_y, int height_y,
				 unsigned int _stride_out) {


	int y, horiz_count;
	int stride_out = width_y * 2;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	horiz_count = -(width_y >> 3);

	for (y=0; y<height_y; y++) {
	
		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, puc_out       
			mov ebx, puc_y       
			mov ecx, puc_u       
			mov edx, puc_v
			mov edi, horiz_count
			
		horiz_loop:

			// load data
			movd mm2, [ecx]					 ; mm2 = ________u3u2u1u0
			movd mm3, [edx]					 ; mm3 = ________v3v2v1v0
			movq mm0, [ebx]          ; mm0 = y7y6y5y4y3y2y1y0  

			pxor mm7, mm7						 ; zero mm7

			// convert chroma part
			punpcklbw mm2, mm7       ; mm2 = __u3__u2__u1__u0
			punpcklbw mm3, mm7       ; mm3 = __v3__v2__v1__v0
			psubw mm2, mmw_0x0080    ; mm2 -= 128
			psubw mm3, mmw_0x0080    ; mm3 -= 128
			psllw mm2, 3             ; mm2 *= 8
			psllw mm3, 3             ; mm3 *= 8
			movq mm4, mm2            ; mm4 = mm2 = u
			movq mm5, mm3            ; mm5 = mm3 = v
			pmulhw mm2, mmw_mult_U_G ; mm2 *= u green coeff 
			pmulhw mm3, mmw_mult_V_G ; mm3 *= v green coeff  
			pmulhw mm4, mmw_mult_U_B ; mm4 = blue chroma
			pmulhw mm5, mmw_mult_V_R ; mm5 = red chroma
			paddsw mm2, mm3					 ; mm2 = green chroma

			// convert luma part
			psubusb mm0, mmb_0x10    ; mm0 -= 16
			movq mm1, mmw_0x00ff     ; mm1 = 00ff00ff00ff00ff 
			psrlw mm0, 8             ; mm0 = __y7__y5__y3__y1 luma odd
			pand mm1, mm0            ; mm1 = __y6__y4__y2__y0 luma even
			psllw mm0, 3             ; mm0 *= 8
			psllw mm1, 3             ; mm1 *= 8
			pmulhw mm0, mmw_mult_Y   ; mm0 luma odd *= luma coeff 
			pmulhw mm1, mmw_mult_Y   ; mm1 luma even *= luma coeff 

			// complete the matrix calc with the addictions
			movq mm3, mm4						 ; copy blue chroma
			movq mm6, mm5						 ; copy red chroma
			movq mm7, mm2						 ; copy green chroma
			paddsw mm3, mm0					 ; mm3 = luma odd + blue chroma
			paddsw mm4, mm1					 ; mm4 = luma even + blue chroma
			paddsw mm6, mm0					 ; mm6 = luma odd + red chroma
			paddsw mm5, mm1					 ; mm5 = luma even + red chroma
			paddsw mm7, mm0					 ; mm7 = luma odd + green chroma
			paddsw mm2, mm1					 ; mm2 = luma even + green chroma
			// clipping
			packuswb mm3, mm3
			packuswb mm4, mm4
			packuswb mm6, mm6
			packuswb mm5, mm5
			packuswb mm7, mm7
			packuswb mm2, mm2
			// interleave odd and even parts
			punpcklbw mm4, mm3			 ; mm4 = b7b6b5b4b3b2b1b0 blue
			punpcklbw mm5, mm6			 ; mm5 = r7r6r5r4r3r2r1r0 red
			punpcklbw mm2, mm7			 ; mm2 = g7g6g5g4g3g2g1g0 green

			// mask not needed bits (using 555)
			pand mm4, mask_5
			pand mm5, mask_5
			pand mm2, mask_5

			// mix colors and write

			psrlw mm4, 3						 ; mm4 = blue shifted
			pand mm4, mask_blue			 ; mask the blue again
			pxor mm7, mm7						 ; zero mm7
			movq mm1, mm4						 ; mm1 = copy blue
			movq mm3, mm5						 ; mm3 = copy red
			movq mm6, mm2						 ; mm6 = copy green

			punpckhbw mm1, mm7
			punpckhbw mm3, mm7
			punpckhbw mm6, mm7
			psllw mm6, 2						 ; shift green
			psllw mm3, 7						 ; shift red
			por mm6, mm3
			por mm6, mm1
			movq 8[eax], mm6

			punpcklbw mm2, mm7			 ; mm2 = __g3__g2__g1__g0 already masked
			punpcklbw mm5, mm7
			punpcklbw mm4, mm7
			psllw mm2, 2						 ; shift green
			psllw mm5, 7						 ; shift red
			por mm2, mm5
			por mm2, mm4
			movq [eax], mm2

			add ebx, 8               ; puc_y   += 8;
			add ecx, 4               ; puc_u   += 4;
			add edx, 4               ; puc_v   += 4;
			add eax, 16              ; puc_out += 16 // wrote 16 bytes

			inc edi
			jne horiz_loop			

			pop edi 
			pop edx 
			pop ecx
			pop ebx 
			pop eax

			emms
						
		}
		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_out;
	}
}

/***/

#define _S(a)		(a)>255 ? 255 : (a)<0 ? 0 : (a)

#define _R(y,u,v) (0x2568*(y)  			       + 0x3343*(u)) /0x2000
#define _G(y,u,v) (0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u)) /0x2000
#define _B(y,u,v) (0x2568*(y) + 0x40cf*(v))					     /0x2000

#define _mR	0x7c00
#define _mG 0x03e0
#define _mB 0x001f

#define _Ps565(r,g,b) ( ((r & 0xF8) >> 3) | (((g & 0xF8) << 3)) | (((b & 0xF8) << 8)) )


void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
								unsigned int _stride_out) 
{

	int y, horiz_count;
	unsigned short * pus_out;
	int stride_out = width_y * 2;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}
	pus_out = (unsigned short *) puc_out;

	horiz_count = -(width_y >> 3);

	for (y=0; y<height_y; y++) 
	{
		/***
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

			pus_out[0] = (unsigned short) _Ps565(r,g,b);

			pus_out++;
		}
		***/

		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, puc_out       
			mov ebx, puc_y       
			mov ecx, puc_u       
			mov edx, puc_v
			mov edi, horiz_count
			
		horiz_loop:

			// load data
			movd mm2, [ecx]					 ; mm2 = ________u3u2u1u0
			movd mm3, [edx]					 ; mm3 = ________v3v2v1v0
			movq mm0, [ebx]          ; mm0 = y7y6y5y4y3y2y1y0  

			pxor mm7, mm7						 ; zero mm7

			// convert chroma part
			punpcklbw mm2, mm7       ; mm2 = __u3__u2__u1__u0
			punpcklbw mm3, mm7       ; mm3 = __v3__v2__v1__v0
			psubw mm2, mmw_0x0080    ; mm2 -= 128
			psubw mm3, mmw_0x0080    ; mm3 -= 128
			psllw mm2, 3             ; mm2 *= 8
			psllw mm3, 3             ; mm3 *= 8
			movq mm4, mm2            ; mm4 = mm2 = u
			movq mm5, mm3            ; mm5 = mm3 = v
			pmulhw mm2, mmw_mult_U_G ; mm2 *= u green coeff 
			pmulhw mm3, mmw_mult_V_G ; mm3 *= v green coeff  
			pmulhw mm4, mmw_mult_U_B ; mm4 = blue chroma
			pmulhw mm5, mmw_mult_V_R ; mm5 = red chroma
			paddsw mm2, mm3					 ; mm2 = green chroma

			// convert luma part
			psubusb mm0, mmb_0x10    ; mm0 -= 16
			movq mm1, mmw_0x00ff     ; mm1 = 00ff00ff00ff00ff 
			psrlw mm0, 8             ; mm0 = __y7__y5__y3__y1 luma odd
			pand mm1, mm0            ; mm1 = __y6__y4__y2__y0 luma even
			psllw mm0, 3             ; mm0 *= 8
			psllw mm1, 3             ; mm1 *= 8
			pmulhw mm0, mmw_mult_Y   ; mm0 luma odd *= luma coeff 
			pmulhw mm1, mmw_mult_Y   ; mm1 luma even *= luma coeff 

			// complete the matrix calc with the addictions
			movq mm3, mm4						 ; copy blue chroma
			movq mm6, mm5						 ; copy red chroma
			movq mm7, mm2						 ; copy green chroma
			paddsw mm3, mm0					 ; mm3 = luma odd + blue chroma
			paddsw mm4, mm1					 ; mm4 = luma even + blue chroma
			paddsw mm6, mm0					 ; mm6 = luma odd + red chroma
			paddsw mm5, mm1					 ; mm5 = luma even + red chroma
			paddsw mm7, mm0					 ; mm7 = luma odd + green chroma
			paddsw mm2, mm1					 ; mm2 = luma even + green chroma
			// clipping
			packuswb mm3, mm3
			packuswb mm4, mm4
			packuswb mm6, mm6
			packuswb mm5, mm5
			packuswb mm7, mm7
			packuswb mm2, mm2
			// interleave odd and even parts
			punpcklbw mm4, mm3			 ; mm4 = b7b6b5b4b3b2b1b0 blue
			punpcklbw mm5, mm6			 ; mm5 = r7r6r5r4r3r2r1r0 red
			punpcklbw mm2, mm7			 ; mm2 = g7g6g5g4g3g2g1g0 green

			// mask not needed bits (using 555)
			pand mm4, mask_5
			pand mm5, mask_5
			pand mm2, mask_5

			// mix colors and write

			psrlw mm4, 3						 ; mm4 = red shifted
			pand mm4, mask_blue			 ; mask the blue again
			pxor mm7, mm7						 ; zero mm7
			movq mm1, mm5						 ; mm1 = copy blue
			movq mm3, mm4						 ; mm3 = copy red
			movq mm6, mm2						 ; mm6 = copy green

			punpckhbw mm1, mm7
			punpckhbw mm3, mm7
			punpckhbw mm6, mm7
			psllw mm6, 3						 ; shift green
			psllw mm1, 8						 ; shift blue
			por mm6, mm3
			por mm6, mm1
			movq 8[eax], mm6

			punpcklbw mm2, mm7			 ; mm2 = __g3__g2__g1__g0 already masked
			punpcklbw mm4, mm7
			punpcklbw mm5, mm7
			psllw mm2, 3						 ; shift green
			psllw mm5, 8						 ; shift blue
			por mm2, mm4
			por mm2, mm5
			movq [eax], mm2

			add ebx, 8               ; puc_y   += 8;
			add ecx, 4               ; puc_u   += 4;
			add edx, 4               ; puc_v   += 4;
			add eax, 16              ; puc_out += 16 // wrote 16 bytes

			inc edi
			jne horiz_loop			

			pop edi 
			pop edx 
			pop ecx
			pop ebx 
			pop eax

			emms
						
		}
		/***/

		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_out;
	}
}

/***/






 

/**** YUV -> YUV conversions (more-or-less straight copy) - still in development ****/

void yuy2_out(uint8_t *puc_y, int stride_y, 
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

/*** YUV 4:2:0 -> UYVY ***/

void uyvy_out(uint8_t *puc_y, int stride_y, 
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
			tmp = *(pu++);
			tmp |= *(py++) << 8;
			tmp |= *(pv++) << 16;
			tmp |= *(py++) << 24;
			*(uint32_t*)puc_out=tmp;
			puc_out += 4;

			tmp &= 0x00FF00FF;
			tmp |= *(py2++) << 8;
			tmp |= *(py2++) << 24;
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


/*** YUV edged -> YUV conversion (just remove the edges for YV12) ***/

void yuv12_out(uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, int width_y, int height_y,
	unsigned int stride_out) 
{ 
	int i;

	unsigned char * pauc_out[3];

	if (height_y < 0) {
		// we are flipping our output upside-down
		height_y = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	pauc_out[0] = puc_out;
	pauc_out[1] = puc_out + stride_out * height_y;
	pauc_out[2] = puc_out + stride_out * height_y * 5 / 4;

	for (i=0; i<height_y; i++) {
		
		memcpy(pauc_out[0], puc_y, width_y);

		pauc_out[0] += stride_out;
		puc_y += stride_y;
	}

	for (i=0; i<height_y/2; i++) {
		
		memcpy(pauc_out[2], puc_u, width_y/2); // U and V buffer must be inverted
		memcpy(pauc_out[1], puc_v, width_y/2);

		pauc_out[1] += stride_out/2;
		pauc_out[2] += stride_out/2;
		puc_u += stride_uv;
		puc_v += stride_uv;
	}
}

/***/
