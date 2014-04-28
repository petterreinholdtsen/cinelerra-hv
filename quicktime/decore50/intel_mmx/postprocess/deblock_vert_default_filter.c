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



/* Vertical deblocking filter for use in non-flat picture regions */
INLINE void deblock_vert_default_filter(uint8_t *v, int stride, int QP) {
	uint64_t *pmm1;
	const uint64_t mm_0020 = 0x0020002000200020;
	uint64_t mm_8_x_QP;
	int i;
	
	#ifdef PP_SELF_CHECK
	/* define semi-constants to enable us to move up and down the picture easily... */
	int l1 = 1 * stride;
	int l2 = 2 * stride;
	int l3 = 3 * stride;
	int l4 = 4 * stride;
	int l5 = 5 * stride;
	int l6 = 6 * stride;
	int l7 = 7 * stride;
	int l8 = 8 * stride;
	int x, y, a3_0_SC, a3_1_SC, a3_2_SC, d_SC, q_SC;
	uint8_t selfcheck[8][2];
	#endif

	#ifdef PP_SELF_CHECK
	/* compute selfcheck matrix for later comparison */
	for (x=0; x<8; x++) {
		a3_0_SC = 2*v[l3+x] - 5*v[l4+x] + 5*v[l5+x] - 2*v[l6+x];	
		a3_1_SC = 2*v[l1+x] - 5*v[l2+x] + 5*v[l3+x] - 2*v[l4+x];	
		a3_2_SC = 2*v[l5+x] - 5*v[l6+x] + 5*v[l7+x] - 2*v[l8+x];	
		q_SC    = (v[l4+x] - v[l5+x]) / 2;

		if (ABS(a3_0_SC) < 8*QP) {

			d_SC = ABS(a3_0_SC) - MIN(ABS(a3_1_SC), ABS(a3_2_SC));
			if (d_SC < 0) d_SC=0;
				
			d_SC = (5*d_SC + 32) >> 6; 
			d_SC *= SIGN(-a3_0_SC);
							
			//printf("d_SC[%d] preclip=%d\n", x, d_SC);
			/* clip d in the range 0 ... q */
			if (q_SC > 0) {
				d_SC = d_SC<0    ? 0    : d_SC;
				d_SC = d_SC>q_SC ? q_SC : d_SC;
			} else {
				d_SC = d_SC>0    ? 0    : d_SC;
				d_SC = d_SC<q_SC ? q_SC : d_SC;
			}
						
		} else {
			d_SC = 0;		
		}
		selfcheck[x][0] = v[l4+x] - d_SC;
		selfcheck[x][1] = v[l5+x] + d_SC;
	}
	#endif

	((uint32_t *)&mm_8_x_QP)[0] = 
	 ((uint32_t *)&mm_8_x_QP)[1] = 0x00080008 * QP; 
	
	/* working in 4-pixel wide columns, left to right */
	for (i=0; i<2; i++) { /*i=0 in left, i=1 in right */

		/* v should be 64-bit aligned here */
		pmm1 = (uint64_t *)(&(v[4*i]));
		/* pmm1 will be 32-bit aligned but this doesn't matter as we'll use movd not movq */

		__asm {
			push ecx
			mov ecx, pmm1

			pxor      mm7, mm7               /* mm7 = 0000000000000000    0 1 2 3 4 5 6 7w   */
			add      ecx, stride           /* %0 += stride              0 1 2 3 4 5 6 7    */ 

			movd      mm0, [ecx]            /* mm0 = v1v1v1v1v1v1v1v1    0w1 2 3 4 5 6 7    */
			punpcklbw mm0, mm7               /* mm0 = __v1__v1__v1__v1 L  0m1 2 3 4 5 6 7r   */

			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 
			movd      mm1, [ecx]            /* mm1 = v2v2v2v2v2v2v2v2    0 1w2 3 4 5 6 7    */

			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 
			punpcklbw mm1, mm7               /* mm1 = __v2__v2__v2__v2 L  0 1m2 3 4 5 6 7r   */

			movd      mm2, [ecx]            /* mm2 = v3v3v3v3v3v3v3v3    0 1 2w3 4 5 6 7    */
			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 

			punpcklbw mm2, mm7               /* mm2 = __v3__v3__v3__v3 L  0 1 2m3 4 5 6 7r   */

			movd      mm3, [ecx]            /* mm3 = v4v4v4v4v4v4v4v4    0 1 2 3w4 5 6 7    */

			punpcklbw mm3, mm7               /* mm3 = __v4__v4__v4__v4 L  0 1 2 3m4 5 6 7r   */

			psubw     mm1, mm2               /* mm1 = v2 - v3          L  0 1m2r3 4 5 6 7    */

			movq      mm4, mm1               /* mm4 = v2 - v3          L  0 1r2 3 4w5 6 7    */
			psllw     mm1, 2                 /* mm1 = 4 * (v2 - v3)    L  0 1m2 3 4 5 6 7    */

			paddw     mm1, mm4               /* mm1 = 5 * (v2 - v3)    L  0 1m2 3 4r5 6 7    */
			psubw     mm0, mm3               /* mm0 = v1 - v4          L  0m1 2 3r4 5 6 7    */

			psllw     mm0, 1                 /* mm0 = 2 * (v1 - v4)    L  0m1 2 3 4 5 6 7    */

			psubw     mm0, mm1               /* mm0 = a3_1             L  0m1r2 3 4 5 6 7    */

			pxor      mm1, mm1               /* mm1 = 0000000000000000    0 1w2 3 4 5 6 7    */

			pcmpgtw   mm1, mm0               /* is 0 > a3_1 ?          L  0r1m2 3 4 5 6 7    */

			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 
			pxor      mm0, mm1               /* mm0 = ABS(a3_1) step 1 L  0m1r2 3 4 5 6 7    */

			psubw     mm0, mm1               /* mm0 = ABS(a3_1) step 2 L  0m1r2 3 4 5 6 7    */

			movd      mm1, [ecx]            /* mm1 = v5v5v5v5v5v5v5v5    0 1w2 3 4 5 6 7    */

			punpcklbw mm1, mm7               /* mm1 = __v5__v5__v5__v5 L  0 1m2 3 4 5 6 7r   */


			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 
			psubw     mm3, mm1               /* mm3 = v4 - v5          L  0 1r2 3m4 5 6 7    */

			movd      mm4, [ecx]            /* mm4 = v6v6v6v6v6v6v6v6    0 1 2 3 4w5 6 7    */

			punpcklbw mm4, mm7               /* mm4 = __v6__v6__v6__v6 L  0 1 2 3 4m5 6 7r   */

			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 

			movd      mm5, [ecx]            /* mm5 = v7v7v7v7v7v7v7v7    0 1 2 3 4 5w6 7    */
			psubw     mm2, mm4               /* mm2 = v3 - v6          L  0 1 2m3 4r5 6 7    */

			punpcklbw mm5, mm7               /* mm5 = __v7__v7__v7__v7 L  0 1 2 3 4 5m6 7r   */

			add      ecx, stride           /* ecx += stride              0 1 2 3 4 5 6 7    */ 
			psubw     mm5, mm4               /* mm5 = v7 - v6          L  0 1 2 3 4r5m6 7    */

			movq      mm4, mm5               /* mm4 = v7 - v6          L  0 1 2 3 4w5r6 7    */

			psllw     mm4, 2                 /* mm4 = 4 * (v7 - v6)    L  0 1 2 3 4 5m6 7    */

			paddw     mm5, mm4               /* mm5 = 5 * (v7 - v6)    L  0 1 2 3 4r5m6 7    */

			movd      mm4, [ecx]            /* mm4 = v8v8v8v8v8v8v8v8    0 1 2 3 4w5 6 7    */

			punpcklbw mm4, mm7               /* mm4 = __v8__v8__v8__v8 L  0 1 2 3 4m5 6 7r   */



			psubw     mm1, mm4               /* mm1 = v5 - v8          L  0 1m2 3 4r5 6 7    */

			pxor      mm4, mm4               /* mm4 = 0000000000000000    0 1 2 3 4w5 6 7    */
			psllw     mm1, 1                 /* mm1 = 2 * (v5 - v8)    L  0 1m2 3 4 5 6 7    */

			paddw     mm1, mm5               /* mm1 = a3_2             L  0 1m2 3 4 5r6 7    */

			pcmpgtw   mm4, mm1               /* is 0 > a3_2 ?          L  0 1r2 3 4m5 6 7    */

			pxor      mm1, mm4               /* mm1 = ABS(a3_2) step 1 L  0 1m2 3 4r5 6 7    */

			psubw     mm1, mm4               /* mm1 = ABS(a3_2) step 2 L  0 1m2 3 4r5 6 7    */
			/* at this point, mm0 = ABS(a3_1), mm1 = ABS(a3_2), mm2 = v3 - v6, mm3 = v4 - v5 */

			movq      mm4, mm1               /* mm4 = ABS(a3_2)        L  0 1r2 3 4w5 6 7    */

			pcmpgtw   mm1, mm0               /* is ABS(a3_2) > ABS(a3_1)  0r1m2 3 4 5 6 7    */

			pand      mm0, mm1               /* min() step 1           L  0m1r2 3 4 5 6 7    */

			pandn     mm1, mm4               /* min() step 2           L  0 1m2 3 4r5 6 7    */

			por       mm0, mm1               /* min() step 3           L  0m1r2 3 4 5 6 7    */
			/* at this point, mm0 = MIN( ABS(a3_1), ABS(a3_2), mm2 = v3 - v6, mm3 = v4 - v5 */
			movq      mm1, mm3               /* mm1 = v4 - v5          L  0 1w2 3r4 5 6 7    */
			psllw     mm3, 2                 /* mm3 = 4 * (v4 - v5)    L  0 1 2 3m4 5 6 7    */

			paddw     mm3, mm1               /* mm3 = 5 * (v4 - v5)    L  0 1r2 3m4 5 6 7    */
			psllw     mm2, 1                 /* mm2 = 2 * (v3 - v6)    L  0 1 2m3 4 5 6 7    */

			psubw     mm2, mm3               /* mm2 = a3_0             L  0 1 2m3r4 5 6 7    */
			/* at this point, mm0 = MIN( ABS(a3_1), ABS(a3_2), mm1 = v4 - v5, mm2 = a3_0 */
			movq      mm4, mm2               /* mm4 = a3_0             L  0 1 2r3 4w5 6 7    */
			pxor      mm3, mm3               /* mm3 = 0000000000000000    0 1 2 3w4 5 6 7    */

			pcmpgtw   mm3, mm2               /* is 0 > a3_0 ?          L  0 1 2r3m4 5 6 7    */

			movq      mm2, mm_8_x_QP         /* mm4 = 8*QP                0 1 2w3 4 5 6 7    */
			pxor      mm4, mm3               /* mm4 = ABS(a3_0) step 1 L  0 1 2 3r4m5 6 7    */

			psubw     mm4, mm3               /* mm4 = ABS(a3_0) step 2 L  0 1 2 3r4m5 6 7    */
	
			/* compare a3_0 against 8*QP */
			pcmpgtw   mm2, mm4               /* is 8*QP > ABS(d) ?     L  0 1 2m3 4r5 6 7    */

			pand      mm2, mm4               /* if no, d = 0           L  0 1 2m3 4r5 6 7    */

			movq      mm4, mm2               /* mm2 = a3_0             L  0 1 2r3 4w5 6 7    */
			
			/* at this point, mm0 = MIN( ABS(a3_1), ABS(a3_2), mm1 = v4 - v5, mm2 = a3_0 , mm3 = SGN(a3_0), mm4 = ABS(a3_0) */
			psubusw   mm4, mm0               /* mm0 = (A3_0 - a3_0)    L  0r1 2 3 4m5 6 7    */

			movq      mm0, mm4               /* mm0=ABS(d)             L  0w1 2 3 4r5 6 7    */

			psllw     mm0, 2                 /* mm0 = 4 * (A3_0-a3_0)  L  0m1 2 3 4 5 6 7    */

			paddw     mm0, mm4               /* mm0 = 5 * (A3_0-a3_0)  L  0m1 2 3 4r5 6 7    */

			paddw     mm0, mm_0020           /* mm0 += 32              L  0m1 2 3 4 5 6 7    */

			psraw     mm0, 6                 /* mm0 >>= 6              L  0m1 2 3 4 5 6 7    */
			/* at this point, mm0 = ABS(d), mm1 = v4 - v5, mm3 = SGN(a3_0) */
	
			pxor      mm2, mm2               /* mm2 = 0000000000000000    0 1 2w3 4 5 6 7    */

			pcmpgtw   mm2, mm1               /* is 0 > (v4 - v5) ?     L  0 1r2m3 4 5 6 7    */

			pxor      mm1, mm2               /* mm1 = ABS(mm1) step 1  L  0 1m2r3 4 5 6 7    */

			psubw     mm1, mm2               /* mm1 = ABS(mm1) step 2  L  0 1m2r3 4 5 6 7    */

			psraw     mm1, 1                 /* mm1 >>= 2              L  0 1m2 3 4 5 6 7    */
			/* OK at this point, mm0 = ABS(d), mm1 = ABS(q), mm2 = SGN(q), mm3 = SGN(-d) */
			movq      mm4, mm2               /* mm4 = SGN(q)           L  0 1 2r3 4w5 6 7    */

			pxor      mm4, mm3               /* mm4 = SGN(q) ^ SGN(-d) L  0 1 2 3r4m5 6 7    */

			movq      mm5, mm0               /* mm5 = ABS(d)           L  0r1 2 3 4 5w6 7    */

			pcmpgtw   mm5, mm1               /* is ABS(d) > ABS(q) ?   L  0 1r2 3 4 5m6 7    */

			pand      mm1, mm5               /* min() step 1           L  0m1 2 3 4 5r6 7    */

			pandn     mm5, mm0               /* min() step 2           L  0 1r2 3 4 5m6 7    */

			por       mm1, mm5               /* min() step 3           L  0m1 2 3 4 5r6 7    */

			pand      mm1, mm4               /* if signs differ, set 0 L  0m1 2 3 4r5 6 7    */

			pxor      mm1, mm2               /* Apply sign step 1      L  0m1 2r3 4 5 6 7    */

			psubw     mm1, mm2               /* Apply sign step 2      L  0m1 2r3 4 5 6 7    */
			/* at this point we have d in mm1 */
		


		pop ecx



		};

		if (i==0) {
			__asm {
				movq mm6, mm1
			};
		}

	}
	
	/* add d to rows l4 and l5 in memory... */
	pmm1 = (uint64_t *)(&(v[4*stride])); 
	__asm {
		push ecx
		mov ecx, pmm1
		packsswb  mm6, mm1             
		movq      mm0, [ecx]                
		psubb     mm0, mm6               
		movq      [ecx], mm0                
		add       ecx, stride                    /* %0 += stride              0 1 2 3 4 5 6 7    */ 
		paddb     mm6, [ecx]                
		movq      [ecx], mm6                
		pop ecx




	};



	#ifdef PP_SELF_CHECK
	/* do selfcheck */
	for (x=0; x<8; x++) {
		for (y=0; y<2; y++) {
			if (selfcheck[x][y] != v[l4+x+y*stride]) {
				printf("ERROR: problem with vertical default filter in col %d, row %d\n", x, y);	
				printf("%d should be %d\n", v[l4+x+y*stride], selfcheck[x][y]);	
				
			}
		}
	}
	#endif

	


}





