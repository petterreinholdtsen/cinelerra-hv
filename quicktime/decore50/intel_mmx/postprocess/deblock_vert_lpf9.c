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
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
*
**/

/***

References:  
 * ISO/IEC 14496-2
 * MoMuSys-FDIS-V1.0-990812
 * Intel Architecture Software Developer's Manual
   Volume 2: Instruction Set Reference

***/

#include "postprocess_mmx.h"

/* John Funnell, December 2000 */


const static uint64_t mm_fours  = 0x0004000400040004;


/* Vertical 9-tap low-pass filter for use in "DC" regions of the picture */
INLINE void deblock_vert_lpf9(uint64_t *v_local, uint64_t *p1p2, uint8_t *v, int stride) {

	#ifdef PP_SELF_CHECK
	int j, k;
	uint8_t selfcheck[64], *vv;
	int p1, p2, psum;
	/* define semi-constants to enable us to move up and down the picture easily... */
	int l1 = 1 * stride;
	int l2 = 2 * stride;
	int l3 = 3 * stride;
	int l4 = 4 * stride;
	int l5 = 5 * stride;
	int l6 = 6 * stride;
	int l7 = 7 * stride;
	int l8 = 8 * stride;
	#endif


	#ifdef PP_SELF_CHECK
	/* generate a self-check version of the filter result in selfcheck[64] */
	for (j=0; j<8; j++) { /* loop left->right */
		vv = &(v[j]);
		p1 = ((uint16_t *)(&(p1p2[0+j/4])))[j%4]; /* yuck! */
		p2 = ((uint16_t *)(&(p1p2[2+j/4])))[j%4]; /* yuck! */
		/* the above may well be endian-fussy */
		psum = p1 + p1 + p1 + vv[l1] + vv[l2] + vv[l3] + vv[l4] + 4; 
		selfcheck[j+8*0] = (((psum + vv[l1]) << 1) - (vv[l4] - vv[l5])) >> 4; 
		psum += vv[l5] - p1; 
		selfcheck[j+8*1] = (((psum + vv[l2]) << 1) - (vv[l5] - vv[l6])) >> 4; 
		psum += vv[l6] - p1; 
		selfcheck[j+8*2] = (((psum + vv[l3]) << 1) - (vv[l6] - vv[l7])) >> 4; 
		psum += vv[l7] - p1; 
		selfcheck[j+8*3] = (((psum + vv[l4]) << 1) + p1 - vv[l1] - (vv[l7] - vv[l8])) >> 4; 
		psum += vv[l8] - vv[l1];  
		selfcheck[j+8*4] = (((psum + vv[l5]) << 1) + (vv[l1] - vv[l2]) - vv[l8] + p2) >> 4; 
		psum += p2 - vv[l2];  
		selfcheck[j+8*5] = (((psum + vv[l6]) << 1) + (vv[l2] - vv[l3])) >> 4; 
		psum += p2 - vv[l3]; 
		selfcheck[j+8*6] = (((psum + vv[l7]) << 1) + (vv[l3] - vv[l4])) >> 4; 
		psum += p2 - vv[l4]; 
		selfcheck[j+8*7] = (((psum + vv[l8]) << 1) + (vv[l4] - vv[l5])) >> 4; 
	}
	#endif

	/* vertical DC filter in MMX  
		mm2 - p1/2 left
		mm3 - p1/2 right
		mm4 - psum left
		mm5 - psum right */
	/* alternate between using mm0/mm1 and mm6/mm7 to accumlate left/right */

	__asm {
		push eax
		push ebx
		push ecx

		mov eax, p1p2
		mov ebx, v_local
		mov ecx, v
	
	/* load p1 left into mm2 and p1 right into mm3 */
		movq   mm2, [eax]                  /* mm2 = p1p2[0]               0 1 2w3 4 5 6 7    */
		add   ecx, stride                    /* ecx points at v[1*stride]   0 1 2 3 4 5 6 7    */     

		movq   mm3, 8[eax]                 /* mm3 = p1p2[1]               0 1 2 3w4 5 6 7    */

		movq   mm4, mm_fours                /* mm4 = 0x0004000400040004    0 1 2 3 4w5 6 7    */
	/* psum = p1 + p1 + p1 + vv[1] + vv[2] + vv[3] + vv[4] + 4 */
	/* psum left will be in mm4, right in mm5          */
		movq   mm5, mm4                     /* mm5 = 0x0004000400040004    0 1 2 3 4 5w6 7    */

		paddsw mm4, 16[ebx]                 /* mm4 += vv[1] left           0 1 2 3 4m5 6 7    */
		paddw  mm5, mm3                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

		paddsw mm4, 32[ebx]                 /* mm4 += vv[2] left           0 1 2 3 4m5 6 7    */
		paddw  mm5, mm3                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

		paddsw mm4, 48[ebx]                 /* mm4 += vv[3] left           0 1 2 3 4m5 6 7    */
		paddw  mm5, mm3                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

		paddsw mm5, 24[ebx]                 /* mm5 += vv[1] right          0 1 2 3 4 5m6 7    */
		paddw  mm4, mm2                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

		paddsw mm5, 40[ebx]                 /* mm5 += vv[2] right          0 1 2 3 4 5m6 7    */
		paddw  mm4, mm2                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

		paddsw mm5, 56[ebx]                 /* mm5 += vv[3] right          0 1 2 3 4 5m6 7    */
		paddw  mm4, mm2                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

		paddsw mm4, 64[ebx]                 /* mm4 += vv[4] left           0 1 2 3 4m5 6 7    */

		paddsw mm5, 72[ebx]                 /* mm5 += vv[4] right          0 1 2 3 4 5m6 7    */
	/* v[1] = (((psum + vv[1]) << 1) - (vv[4] - vv[5])) >> 4 */
	/* compute this in mm0 (left) and mm1 (right)   */
		movq   mm0, mm4                     /* mm0 = psum left             0w1 2 3 4 5 6 7    */ 

		paddsw mm0, 16[ebx]                 /* mm0 += vv[1] left           0m1 2 3 4 5 6 7    */
		movq   mm1, mm5                     /* mm1 = psum right            0 1w2 3 4 5r6 7    */ 

		paddsw mm1, 24[ebx]                 /* mm1 += vv[1] right          0 1 2 3 4 5 6 7    */
		psllw  mm0, 1                       /* mm0 <<= 1                   0m1 2 3 4 5 6 7    */

		psubsw mm0, 64[ebx]                 /* mm0 -= vv[4] left           0m1 2 3 4 5 6 7    */
		psllw  mm1, 1                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

		psubsw mm1, 72[ebx]                 /* mm1 -= vv[4] right          0 1m2 3 4 5 6 7    */

		paddsw mm0, 80[ebx]                 /* mm0 += vv[5] left           0m1 2 3 4 5 6 7    */

		paddsw mm1, 88[ebx]                 /* mm1 += vv[5] right          0 1m2 3 4 5 6 7    */
		psrlw  mm0, 4                       /* mm0 >>= 4                   0m1 2 3 4 5 6 7    */
/* psum += vv[5] - p1 */ 
		paddsw mm4, 80[ebx]                 /* mm4 += vv[5] left           0 1 2 3 4m5 6 7    */
		psrlw  mm1, 4                       /* mm1 >>= 4                   0 1m2 3 4 5 6 7    */

		paddsw mm5, 88[ebx]                 /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
		psubsw mm4, [eax]                  /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */

		packuswb mm0, mm1                   /* pack mm1, mm0 to mm0        0m1 2 3 4 5 6 7    */
		psubsw mm5, 8[eax]                 /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */

	/* v[2] = (((psum + vv[2]) << 1) - (vv[5] - vv[6])) >> 4 */
	/* compute this in mm6 (left) and mm7 (right)   */
		movq   mm6, mm4                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm6, 32[ebx]                 /* mm6 += vv[2] left           0 1 2 3 4 5 6 7    */
		movq   mm7, mm5                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm7, 40[ebx]                 /* mm7 += vv[2] right          0 1 2 3 4 5 6 7    */
		psllw  mm6, 1                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

		psubsw mm6, 80[ebx]                 /* mm6 -= vv[5] left           0 1 2 3 4 5 6 7    */
		psllw  mm7, 1                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

		psubsw mm7, 88[ebx]                 /* mm7 -= vv[5] right          0 1 2 3 4 5 6 7    */

		movq   [ecx], mm0                     /* v[1*stride] = mm0           0 1 2 3 4 5 6 7    */

		paddsw mm6, 96[ebx]                 /* mm6 += vv[6] left           0 1 2 3 4 5 6 7    */
		add   ecx, stride                    /* ecx points at v[2*stride]   0 1 2 3 4 5 6 7    */     

		paddsw mm7, 104[ebx]                /* mm7 += vv[6] right          0 1 2 3 4 5 6 7    */
	/* psum += vv[6] - p1 */ 

		paddsw mm4, 96[ebx]                 /* mm4 += vv[6] left           0 1 2 3 4 5 6 7    */
		psrlw  mm6, 4                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

		paddsw mm5, 104[ebx]                /* mm5 += vv[6] right          0 1 2 3 4 5 6 7    */
		psrlw  mm7, 4                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm4, [eax]                  /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */
		packuswb mm6, mm7                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

		psubsw mm5, 8[eax]                 /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */
	/* v[3] = (((psum + vv[3]) << 1) - (vv[6] - vv[7])) >> 4 */
	/* compute this in mm0 (left) and mm1 (right)    */

		movq   mm0, mm4                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm0, 48[ebx]                 /* mm0 += vv[3] left           0 1 2 3 4 5 6 7    */
		movq   mm1, mm5                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm1, 56[ebx]                 /* mm1 += vv[3] right          0 1 2 3 4 5 6 7    */
		psllw  mm0, 1                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

		psubsw mm0, 96[ebx]                 /* mm0 -= vv[6] left           0 1 2 3 4 5 6 7    */
		psllw  mm1, 1                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

		psubsw mm1, 104[ebx]                /* mm1 -= vv[6] right          0 1 2 3 4 5 6 7    */

		movq   [ecx], mm6                     /* v[2*stride] = mm6           0 1 2 3 4 5 6 7    */
		paddsw mm0, 112[ebx]                /* mm0 += vv[7] left           0 1 2 3 4 5 6 7    */

		paddsw mm1, 120[ebx]                /* mm1 += vv[7] right          0 1 2 3 4 5 6 7    */
		add   ecx, stride                    /* ecx points at v[3*stride]   0 1 2 3 4 5 6 7    */     

	/* psum += vv[7] - p1 */ 
		paddsw mm4, 112[ebx]                /* mm4 += vv[5] left           0 1 2 3 4 5 6 7    */
		psrlw  mm0, 4                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

		paddsw mm5, 120[ebx]                /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
		psrlw  mm1, 4                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm4, [eax]                  /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */
		packuswb mm0, mm1                   /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

		psubsw mm5, 8[eax]                 /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */
	/* v[4] = (((psum + vv[4]) << 1) + p1 - vv[1] - (vv[7] - vv[8])) >> 4 */
	/* compute this in mm6 (left) and mm7 (right)    */
		movq   [ecx], mm0                     /* v[3*stride] = mm0           0 1 2 3 4 5 6 7    */
		movq   mm6, mm4                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm6, 64[ebx]                 /* mm6 += vv[4] left           0 1 2 3 4 5 6 7    */
		movq   mm7, mm5                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm7, 72[ebx]                 /* mm7 += vv[4] right          0 1 2 3 4 5 6 7    */
		psllw  mm6, 1                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm6, [eax]                  /* mm6 += p1 left              0 1 2 3 4 5 6 7    */
		psllw  mm7, 1                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm7, 8[eax]                 /* mm7 += p1 right             0 1 2 3 4 5 6 7    */

		psubsw mm6, 16[ebx]                 /* mm6 -= vv[1] left           0 1 2 3 4 5 6 7    */

		psubsw mm7, 24[ebx]                 /* mm7 -= vv[1] right          0 1 2 3 4 5 6 7    */

		psubsw mm6, 112[ebx]                /* mm6 -= vv[7] left           0 1 2 3 4 5 6 7    */

		psubsw mm7, 120[ebx]                /* mm7 -= vv[7] right          0 1 2 3 4 5 6 7    */

		paddsw mm6, 128[ebx]                /* mm6 += vv[8] left           0 1 2 3 4 5 6 7    */
		add   ecx, stride                    /* ecx points at v[4*stride]   0 1 2 3 4 5 6 7    */     

		paddsw mm7, 136[ebx]                /* mm7 += vv[8] right          0 1 2 3 4 5 6 7    */
	/* psum += vv[8] - vv[1] */ 

		paddsw mm4, 128[ebx]                /* mm4 += vv[5] left           0 1 2 3 4 5 6 7    */
		psrlw  mm6, 4                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

		paddsw mm5, 136[ebx]                /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
		psrlw  mm7, 4                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm4, 16[ebx]                 /* mm4 -= vv[1] left           0 1 2 3 4 5 6 7    */
		packuswb mm6, mm7                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

		psubsw mm5, 24[ebx]                 /* mm5 -= vv[1] right          0 1 2 3 4 5 6 7    */
	/* v[5] = (((psum + vv[5]) << 1) + (vv[1] - vv[2]) - vv[8] + p2) >> 4 */
	/* compute this in mm0 (left) and mm1 (right)    */
		movq   mm0, mm4                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm0, 80[ebx]                 /* mm0 += vv[5] left           0 1 2 3 4 5 6 7    */
		movq   mm1, mm5                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm1, 88[ebx]                 /* mm1 += vv[5] right          0 1 2 3 4 5 6 7    */
		psllw  mm0, 1                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm0, 16[eax]                /* mm0 += p2 left              0 1 2 3 4 5 6 7    */
		psllw  mm1, 1                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm1, 24[eax]                /* mm1 += p2 right             0 1 2 3 4 5 6 7    */

		paddsw mm0, 16[ebx]                 /* mm0 += vv[1] left           0 1 2 3 4 5 6 7    */
		movq   [ecx], mm6                     /* v[4*stride] = mm6           0 1 2 3 4 5 6 7    */

		paddsw mm1, 24[ebx]                 /* mm1 += vv[1] right          0 1 2 3 4 5 6 7    */

		psubsw mm0, 32[ebx]                 /* mm0 -= vv[2] left           0 1 2 3 4 5 6 7    */

		psubsw mm1, 40[ebx]                 /* mm1 -= vv[2] right          0 1 2 3 4 5 6 7    */

		psubsw mm0, 128[ebx]                /* mm0 -= vv[8] left           0 1 2 3 4 5 6 7    */

		psubsw mm1, 136[ebx]                /* mm1 -= vv[8] right          0 1 2 3 4 5 6 7    */
	/* psum += p2 - vv[2] */ 
		paddsw mm4, 16[eax]                /* mm4 += p2 left              0 1 2 3 4 5 6 7    */
		add   ecx, stride                   /* ecx points at v[5*stride]   0 1 2 3 4 5 6 7    */     

		paddsw mm5, 24[eax]                /* mm5 += p2 right             0 1 2 3 4 5 6 7    */

		psubsw mm4, 32[ebx]                 /* mm4 -= vv[2] left           0 1 2 3 4 5 6 7    */

		psubsw mm5, 40[ebx]                 /* mm5 -= vv[2] right          0 1 2 3 4 5 6 7    */
	/* v[6] = (((psum + vv[6]) << 1) + (vv[2] - vv[3])) >> 4 */
	/* compute this in mm6 (left) and mm7 (right)    */
		movq   mm6, mm4                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm6, 96[ebx]                 /* mm6 += vv[6] left           0 1 2 3 4 5 6 7    */
		movq   mm7, mm5                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm7, 104[ebx]                /* mm7 += vv[6] right          0 1 2 3 4 5 6 7    */
		psllw  mm6, 1                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm6, 32[ebx]                 /* mm6 += vv[2] left           0 1 2 3 4 5 6 7    */
		psllw  mm7, 1                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm7, 40[ebx]                 /* mm7 += vv[2] right          0 1 2 3 4 5 6 7    */
		psrlw  mm0, 4                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm6, 48[ebx]                 /* mm6 -= vv[3] left           0 1 2 3 4 5 6 7    */
		psrlw  mm1, 4                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm7, 56[ebx]                 /* mm7 -= vv[3] right          0 1 2 3 4 5 6 7    */
		packuswb mm0, mm1                   /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

		movq   [ecx], mm0                     /* v[5*stride] = mm0           0 1 2 3 4 5 6 7    */
	/* psum += p2 - vv[3] */ 

		paddsw mm4, 16[eax]                /* mm4 += p2 left              0 1 2 3 4 5 6 7    */
		psrlw  mm6, 4                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

		paddsw mm5, 24[eax]                /* mm5 += p2 right             0 1 2 3 4 5 6 7    */
		psrlw  mm7, 4                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm4, 48[ebx]                 /* mm4 -= vv[3] left           0 1 2 3 4 5 6 7    */
		add   ecx, stride                    /* ecx points at v[6*stride]   0 1 2 3 4 5 6 7    */     

		psubsw mm5, 56[ebx]                 /* mm5 -= vv[3] right           0 1 2 3 4 5 6 7    */
	/* v[7] = (((psum + vv[7]) << 1) + (vv[3] - vv[4])) >> 4 */
	/* compute this in mm0 (left) and mm1 (right)     */
		movq   mm0, mm4                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm0, 112[ebx]                /* mm0 += vv[7] left           0 1 2 3 4 5 6 7    */
		movq   mm1, mm5                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm1, 120[ebx]                /* mm1 += vv[7] right          0 1 2 3 4 5 6 7    */
		psllw  mm0, 1                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm0, 48[ebx]                 /* mm0 += vv[3] left           0 1 2 3 4 5 6 7    */
		psllw  mm1, 1                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm1, 56[ebx]                 /* mm1 += vv[3] right          0 1 2 3 4 5 6 7    */
		packuswb mm6, mm7                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

		psubsw mm0, 64[ebx]                 /* mm0 -= vv[4] left           0 1 2 3 4 5 6 7    */

		psubsw mm1, 72[ebx]                 /* mm1 -= vv[4] right          0 1 2 3 4 5 6 7    */
		psrlw  mm0, 4                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

		movq   [ecx], mm6                     /* v[6*stride] = mm6           0 1 2 3 4 5 6 7    */
	/* psum += p2 - vv[4] */ 
		paddsw mm4, 16[eax]                /* mm4 += p2 left               0 1 2 3 4 5 6 7    */

		paddsw mm5, 24[eax]                /* mm5 += p2 right              0 1 2 3 4 5 6 7    */
		add    ecx, stride                    /* ecx points at v[7*stride]   0 1 2 3 4 5 6 7    */     

		psubsw mm4, 64[ebx]                 /* mm4 -= vv[4] left            0 1 2 3 4 5 6 7    */
		psrlw  mm1, 4                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

		psubsw mm5, 72[ebx]                 /* mm5 -= vv[4] right           0 1 2 3 4 5 6 7    */
	/* v[8] = (((psum + vv[8]) << 1) + (vv[4] - vv[5])) >> 4 */
	/* compute this in mm6 (left) and mm7 (right)     */
		movq   mm6, mm4                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */ 

		paddsw mm6, 128[ebx]                /* mm6 += vv[8] left           0 1 2 3 4 5 6 7    */
		movq   mm7, mm5                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */ 

		paddsw mm7, 136[ebx]                /* mm7 += vv[8] right          0 1 2 3 4 5 6 7    */
		psllw  mm6, 1                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm6, 64[ebx]                 /* mm6 += vv[4] left           0 1 2 3 4 5 6 7    */
		psllw  mm7, 1                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

		paddsw mm7, 72[ebx]                 /* mm7 += vv[4] right          0 1 2 3 4 5 6 7    */
		packuswb mm0, mm1                   /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

		psubsw mm6, 80[ebx]                 /* mm6 -= vv[5] left           0 1 2 3 4 5 6 7    */

		psubsw mm7, 88[ebx]                 /* mm7 -= vv[5] right          0 1 2 3 4 5 6 7    */
		psrlw  mm6, 4                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

		movq   [ecx], mm0                   /* v[7*stride] = mm0           0 1 2 3 4 5 6 7    */
		psrlw  mm7, 4                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

		packuswb mm6, mm7                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

		add   ecx, stride                   /* ecx points at v[8*stride]   0 1 2 3 4 5 6 7    */     

		nop                                 /*                             0 1 2 3 4 5 6 7    */     

		movq   [ecx], mm6                   /* v[8*stride] = mm6           0 1 2 3 4 5 6 7    */


		pop ecx
		pop ebx
		pop eax


	};
	
	#ifdef PP_SELF_CHECK
	/* use the self-check version of the filter result in selfcheck[64] to verify the filter output */
	for (k=0; k<8; k++) { /* loop top->bottom */
		for (j=0; j<8; j++) { /* loop left->right */
			vv = &(v[(k+1)*stride + j]);
			if (*vv != selfcheck[j+8*k]) {
				printf("ERROR: problem with vertical LPF9 filter in row %d\n", k+1);
			}
		}
	}
	#endif

}


