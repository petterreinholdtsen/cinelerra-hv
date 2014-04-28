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


const static uint64_t mm64_0008 = 0x0008000800080008;
const static uint64_t mm64_0101 = 0x0101010101010101;
static uint64_t mm64_temp;
const static uint64_t mm64_coefs[18] =  {
	0x0001000200040006, /* p1 left */ 0x0000000000000001, /* v1 right */
	0x0001000200020004, /* v1 left */ 0x0000000000010001, /* v2 right */
	0x0002000200040002, /* v2 left */ 0x0000000100010002, /* v3 right */
	0x0002000400020002, /* v3 left */ 0x0001000100020002, /* v4 right */
	0x0004000200020001, /* v4 left */ 0x0001000200020004, /* v5 right */
	0x0002000200010001, /* v5 left */ 0x0002000200040002, /* v6 right */
	0x0002000100010000, /* v6 left */ 0x0002000400020002, /* v7 right */
	0x0001000100000000, /* v7 left */ 0x0004000200020001, /* v8 right */
	0x0001000000000000, /* v8 left */ 0x0006000400020001  /* p2 right */
};
static uint32_t mm32_p1p2;
static uint8_t *pmm1;




/* The 9-tap low pass filter used in "DC" regions */
/* I'm not sure that I like this implementation any more...! */
INLINE void deblock_horiz_lpf9(uint8_t *v, int stride, int QP) {
	int y, p1, p2;
	#ifdef PP_SELF_CHECK
	uint8_t selfcheck[9];
	int psum;
	uint8_t *vv; 
	int i;	
	#endif

	for (y=0; y<4; y++) {
		p1 = (ABS(v[0+y*stride]-v[1+y*stride]) < QP ) ?  v[0+y*stride] : v[1+y*stride];
		p2 = (ABS(v[8+y*stride]-v[9+y*stride]) < QP ) ?  v[9+y*stride] : v[8+y*stride];

		mm32_p1p2 = 0x0101 * ((p2 << 16) + p1);

		#ifdef PP_SELF_CHECK
		/* generate a self-check version of the filter result in selfcheck[9] */
		/* low pass filtering (LPF9: 1 1 2 2 4 2 2 1 1) */
		vv = &(v[y*stride]);
		psum = p1 + p1 + p1 + vv[1] + vv[2] + vv[3] + vv[4] + 4;
		selfcheck[1] = (((psum + vv[1]) << 1) - (vv[4] - vv[5])) >> 4;
		psum += vv[5] - p1; 
		selfcheck[2] = (((psum + vv[2]) << 1) - (vv[5] - vv[6])) >> 4;
		psum += vv[6] - p1; 
		selfcheck[3] = (((psum + vv[3]) << 1) - (vv[6] - vv[7])) >> 4;
		psum += vv[7] - p1; 
		selfcheck[4] = (((psum + vv[4]) << 1) + p1 - vv[1] - (vv[7] - vv[8])) >> 4;
		psum += vv[8] - vv[1]; 
		selfcheck[5] = (((psum + vv[5]) << 1) + (vv[1] - vv[2]) - vv[8] + p2) >> 4;
		psum += p2 - vv[2]; 
		selfcheck[6] = (((psum + vv[6]) << 1) + (vv[2] - vv[3])) >> 4;
		psum += p2 - vv[3]; 
		selfcheck[7] = (((psum + vv[7]) << 1) + (vv[3] - vv[4])) >> 4;
		psum += p2 - vv[4]; 
		selfcheck[8] = (((psum + vv[8]) << 1) + (vv[4] - vv[5])) >> 4;
		#endif

		pmm1 = (&(v[y*stride-3])); /* this is 64-aligned */

		/* mm7 = 0, mm6 is left hand accumulator, mm5 is right hand acc */
		__asm {
			push eax
			push ebx
			mov eax, pmm1
			lea ebx, mm64_coefs

			#ifdef PREFETCH_ENABLE
			prefetcht0 32[ebx]                     
			#endif

			movd   mm0,   mm32_p1p2            /* mm0 = ________p2p2p1p1    0w1 2 3 4 5 6 7    */
			punpcklbw mm0, mm0                 /* mm0 = p2p2p2p2p1p1p1p1    0m1 2 3 4 5 6 7    */

			movq    mm2, qword ptr [eax]       /* mm2 = v4v3v2v1xxxxxxxx    0 1 2w3 4 5 6 7    */
			pxor    mm7, mm7                   /* mm7 = 0000000000000000    0 1 2 3 4 5 6 7w   */

			movq     mm6, mm64_0008            /* mm6 = 0008000800080008    0 1 2 3 4 5 6w7    */
			punpckhbw mm2, mm2                 /* mm2 = v4__v3__v2__v1__    0 1 2m3 4 5 6 7    */

			movq     mm64_temp, mm0            /*temp = p2p2p2p2p1p1p1p1    0r1 2 3 4 5 6 7    */

			punpcklbw mm0, mm7                 /* mm0 = __p1__p1__p1__p1    0m1 2 3 4 5 6 7    */
			movq      mm5, mm6                 /* mm5 = 0008000800080008    0 1 2 3 4 5w6r7    */

			pmullw    mm0, [ebx]               /* mm0 *= mm64_coefs[0]      0m1 2 3 4 5 6 7    */

			movq      mm1, mm2                 /* mm1 = v4v4v3v3v2v2v1v1    0 1w2r3 4 5 6 7    */
			punpcklbw mm2, mm2                 /* mm2 = v2v2v2v2v1v1v1v1    0 1 2m3 4 5 6 7    */

			punpckhbw mm1, mm1                 /* mm1 = v4v4v4v4v3v3v3v3    0 1m2 3 4 5 6 7    */

			#ifdef PREFETCH_ENABLE
			prefetcht0 32[ebx]                     
			#endif

			movq      mm3, mm2                /* mm3 = v2v2v2v2v1v1v1v1    0 1 2r3w4 5 6 7    */
			punpcklbw mm2, mm7                /* mm2 = __v1__v1__v1__v1    0 1 2m3 4 5 6 7    */

			punpckhbw mm3, mm7                /* mm3 = __v2__v2__v2__v2    0 1 2 3m4 5 6 7    */
			paddw     mm6, mm0                /* mm6 += mm0                0r1 2 3 4 5 6m7    */

			movq      mm0, mm2                /* mm0 = __v1__v1__v1__v1    0w1 2r3 4 5 6 7    */ 

			pmullw    mm0, 8[ebx]             /* mm2 *= mm64_coefs[1]      0m1 2 3 4 5 6 7    */
			movq      mm4, mm3                /* mm4 = __v2__v2__v2__v2    0 1 2 3r4w5 6 7    */ 

			pmullw    mm2, 16[ebx]            /* mm2 *= mm64_coefs[2]      0 1 2m3 4 5 6 7    */

			pmullw    mm3, 32[ebx]            /* mm3 *= mm64_coefs[4]      0 1 2 3m4 5 6 7    */

			pmullw    mm4, 24[ebx]            /* mm3 *= mm64_coefs[3]      0 1 2 3 4m5 6 7    */
			paddw     mm5, mm0                /* mm5 += mm0                0r1 2 3 4 5m6 7    */

			paddw     mm6, mm2                /* mm6 += mm2                0 1 2r3 4 5 6m7    */
			movq      mm2, mm1                /* mm2 = v4v4v4v4v3v3v3v3    0 1 2 3 4 5 6 7    */

			punpckhbw mm2, mm7                /* mm2 = __v4__v4__v4__v4    0 1 2m3 4 5 6 7r   */
			paddw     mm5, mm4                /* mm5 += mm4                0 1 2 3 4r5m6 7    */

			punpcklbw mm1, mm7                /* mm1 = __v3__v3__v3__v3    0 1m2 3 4 5 6 7r   */
			paddw     mm6, mm3                /* mm6 += mm3                0 1 2 3r4 5 6m7    */

			#ifdef PREFETCH_ENABLE
			prefetcht0 64[ebx]                   
			#endif
			movq      mm0, mm1                /* mm0 = __v3__v3__v3__v3    0w1 2 3 4 5 6 7    */ 

			pmullw    mm1, 48[ebx]            /* mm1 *= mm64_coefs[6]      0 1m2 3 4 5 6 7    */

			pmullw    mm0, 40[ebx]            /* mm0 *= mm64_coefs[5]      0m1 2 3 4 5 6 7    */
			movq      mm4, mm2                /* mm4 = __v4__v4__v4__v4    0 1 2r3 4w5 6 7    */ 

			pmullw    mm2, 64[ebx]            /* mm2 *= mm64_coefs[8]      0 1 2 3 4 5 6 7    */
			paddw     mm6, mm1                /* mm6 += mm1                0 1 2 3 4 5 6 7    */

			pmullw    mm4, 56[ebx]            /* mm4 *= mm64_coefs[7]      0 1 2 3 4m5 6 7    */
			pxor      mm3, mm3                /* mm3 = 0000000000000000    0 1 2 3w4 5 6 7    */

			movq      mm1, 8[eax]             /* mm1 = xxxxxxxxv8v7v6v5    0 1w2 3 4 5 6 7    */
			paddw     mm5, mm0                /* mm5 += mm0                0r1 2 3 4 5 6 7    */

			punpcklbw mm1, mm1                /* mm1 = v8v8v7v7v6v6v5v5    0 1m2 3m4 5 6 7    */
			paddw     mm6, mm2                /* mm6 += mm2                0 1 2r3 4 5 6 7    */

			#ifdef PREFETCH_ENABLE
			prefetcht0 96[ebx]                   
			#endif

			movq      mm2, mm1                /* mm2 = v8v8v7v7v6v6v5v5    0 1r2w3 4 5 6 7    */
			paddw     mm5, mm4                /* mm5 += mm4                0 1 2 3 4r5 6 7    */

			punpcklbw mm2, mm2                /* mm2 = v6v6v6v6v5v5v5v5    0 1 2m3 4 5 6 7    */
			punpckhbw mm1, mm1                /* mm1 = v8v8v8v8v7v7v7v7    0 1m2 3 4 5 6 7    */

			movq      mm3, mm2                /* mm3 = v6v6v6v6v5v5v5v5    0 1 2r3w4 5 6 7    */
			punpcklbw mm2, mm7                /* mm2 = __v5__v5__v5__v5    0 1 2m3 4 5 6 7r   */

			punpckhbw mm3, mm7                /* mm3 = __v6__v6__v6__v6    0 1 2 3m4 5 6 7r   */
			movq      mm0, mm2                /* mm0 = __v5__v5__v5__v5    0w1 2b3 4 5 6 7    */ 

			pmullw    mm0, 72[ebx]            /* mm0 *= mm64_coefs[9]      0m1 2 3 4 5 6 7    */
			movq      mm4, mm3                /* mm4 = __v6__v6__v6__v6    0 1 2 3 4w5 6 7    */ 

			pmullw    mm2, 80[ebx]            /* mm2 *= mm64_coefs[10]     0 1 2m3 4 5 6 7    */

			pmullw    mm3, 96[ebx]            /* mm3 *= mm64_coefs[12]     0 1 2 3m4 5 6 7    */

			pmullw    mm4, 88[ebx]            /* mm4 *= mm64_coefs[11]     0 1 2 3 4m5 6 7    */
			paddw     mm5, mm0                /* mm5 += mm0                0r1 2 3 4 5 6 7    */

			paddw     mm6, mm2                /* mm6 += mm2                0 1 2r3 4 5 6 7    */
			movq      mm2, mm1                /* mm2 = v8v8v8v8v7v7v7v7    0 1r2w3 4 5 6 7    */

			paddw     mm6, mm3                /* mm6 += mm3                0 1 2 3r4 5 6 7    */
			punpcklbw mm1, mm7                /* mm1 = __v7__v7__v7__v7    0 1m2 3 4 5 6 7r   */

			paddw     mm5, mm4                /* mm5 += mm4                0 1 2 3 4r5 6 7    */
			punpckhbw mm2, mm7                /* mm2 = __v8__v8__v8__v8    0 1 2m3 4 5 6 7    */

			#ifdef PREFETCH_ENABLE
			prefetcht0 128[ebx]                  
			#endif

			movq      mm3, mm64_temp          /* mm0 = p2p2p2p2p1p1p1p1    0 1 2 3w4 5 6 7    */
			movq      mm0, mm1                /* mm0 = __v7__v7__v7__v7    0w1r2 3 4 5 6 7    */ 

			pmullw    mm0, 104[ebx]           /* mm0 *= mm64_coefs[13]     0m1b2 3 4 5 6 7    */
			movq      mm4, mm2                /* mm4 = __v8__v8__v8__v8    0 1 2r3 4w5 6 7    */ 

			pmullw    mm1, 112[ebx]           /* mm1 *= mm64_coefs[14]     0 1w2 3 4 5 6 7    */
			punpckhbw mm3, mm7                /* mm0 = __p2__p2__p2__p2    0 1 2 3 4 5 6 7    */

			pmullw    mm2, 128[ebx]           /* mm2 *= mm64_coefs[16]     0 1b2m3 4 5 6 7    */

			pmullw    mm4, 120[ebx]           /* mm4 *= mm64_coefs[15]     0 1b2 3 4m5 6 7    */
			paddw     mm5, mm0                /* mm5 += mm0                0r1 2 3 4 5m6 7    */

			pmullw    mm3, 136[ebx]           /* mm0 *= mm64_coefs[17]     0 1 2 3m4 5 6 7    */
			paddw     mm6, mm1                /* mm6 += mm1                0 1w2 3 4 5 6m7    */

			paddw     mm6, mm2                /* mm6 += mm2                0 1 2r3 4 5 6m7    */

			paddw     mm5, mm4                /* mm5 += mm4                0 1 2 3 4r5m6 7    */
			psrlw     mm6, 4                  /* mm6 /= 16                 0 1 2 3 4 5 6m7    */

			paddw     mm5, mm3                /* mm6 += mm0                0 1 2 3r4 5m6 7    */

			psrlw     mm5, 4                  /* mm5 /= 16                 0 1 2 3 4 5m6 7    */

			packuswb  mm6, mm5                /* pack result into mm6      0 1 2 3 4 5r6m7    */

			movq      4[eax], mm6             /* v[] = mm6                 0 1 2 3 4 5 6r7    */

			pop ebx
			pop eax




		};
	
		#ifdef PP_SELF_CHECK
		for (i=1; i<=8; i++) {
			if (selfcheck[i] != v[i+y*stride]) {
				printf("ERROR: MMX version of horiz lpf9 is incorrect at %d\n", i);
			}
		}
		#endif

	}

}
