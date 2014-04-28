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

/* decide DC mode or default mode for the horizontal filter */
INLINE int deblock_horiz_useDC(uint8_t *v, int stride) {
	const uint64_t mm64_mask   = 0x00fefefefefefefe;
	uint32_t mm32_result;
	uint64_t *pmm1;
	int eq_cnt, useDC;
	#ifdef PP_SELF_CHECK
	int eq_cnt2, j, k;
	#endif

	pmm1 = (uint64_t *)(&(v[1])); /* this is a 32-bit aligned pointer, not 64-aligned */
	
	__asm {
		push eax
		mov eax, pmm1
		
		/* first load some constants into mm4, mm5, mm6, mm7 */
		movq mm6, mm64_mask          /*mm6 = 0x00fefefefefefefe       */
		pxor mm4, mm4                /*mm4 = 0x0000000000000000       */

		movq mm1, qword ptr [eax]   /* mm1 = *pmm            0 1 2 3 4 5 6 7    */
		add eax, stride            /* eax += stride/8      0 1 2 3 4 5 6 7    */

		movq mm5, mm1                /* mm5 = mm1             0 1 2 3 4 5 6 7    */
		psrlq mm1, 8                 /* mm1 >>= 8             0 1 2 3 4 5 6 7    */

		movq mm2, mm5                /* mm2 = mm5             0 1 2 3 4 5 6 7    */
		psubusb mm5, mm1             /* mm5 -= mm1            0 1 2 3 4 5 6 7    */

		movq mm3, qword ptr [eax]   /* mm3 = *pmm            0 1 2 3 4 5 6 7    */
		psubusb mm1, mm2             /* mm1 -= mm2            0 1 2 3 4 5 6 7    */

		add eax, stride            /* eax += stride/8      0 1 2 3 4 5 6 7    */
		por mm5, mm1                 /* mm5 |= mm1            0 1 2 3 4 5 6 7    */

		movq mm0, mm3                /* mm0 = mm3             0 1 2 3 4 5 6 7    */
		pand mm5, mm6                /* mm5 &= 0xfefefefefefefefe     */      

		pxor mm7, mm7                /*mm7 = 0x0000000000000000       */
		pcmpeqb mm5, mm4             /* are the bytes of mm5 == 0 ?   */

		movq mm1, qword ptr [eax]   /* mm3 = *pmm            0 1 2 3 4 5 6 7    */
		psubb mm7, mm5               /* mm7 has running total of eqcnts */

		psrlq mm3, 8                 /* mm3 >>= 8             0 1 2 3 4 5 6 7    */
		movq mm5, mm0                /* mm5 = mm0             0 1 2 3 4 5 6 7    */

		psubusb mm0, mm3             /* mm0 -= mm3            0 1 2 3 4 5 6 7    */

		add eax, stride            /* eax += stride/8      0 1 2 3 4 5 6 7    */
		psubusb mm3, mm5             /* mm3 -= mm5            0 1 2 3 4 5 6 7    */

		movq mm5, qword ptr [eax]   /* mm5 = *pmm            0 1 2 3 4 5 6 7    */
		por mm0, mm3                 /* mm0 |= mm3            0 1 2 3 4 5 6 7    */

		movq mm3, mm1                /* mm3 = mm1             0 1 2 3 4 5 6 7    */
		pand mm0, mm6                /* mm0 &= 0xfefefefefefefefe     */      

		psrlq   mm1, 8               /* mm1 >>= 8             0 1 2 3 4 5 6 7    */
		pcmpeqb mm0, mm4             /* are the bytes of mm0 == 0 ?   */

		movq mm2, mm3                /* mm2 = mm3             0 1 2 3 4 5 6 7    */
		psubb mm7, mm0               /* mm7 has running total of eqcnts */

		psubusb mm3, mm1             /* mm3 -= mm1            0 1 2 3 4 5 6 7    */

		psubusb mm1, mm2             /* mm1 -= mm2            0 1 2 3 4 5 6 7    */

		por mm3, mm1                 /* mm3 |= mm1            0 1 2 3 4 5 6 7    */
		movq mm1, mm5                /* mm1 = mm5             0 1 2 3 4 5 6 7    */

		pand    mm3, mm6             /* mm3 &= 0xfefefefefefefefe     */      
		psrlq   mm5, 8               /* mm5 >>= 8             0 1 2 3 4 5 6 7    */

		pcmpeqb mm3, mm4             /* are the bytes of mm3 == 0 ?   */
		movq    mm0, mm1             /* mm0 = mm1             0 1 2 3 4 5 6 7    */

		psubb   mm7, mm3             /* mm7 has running total of eqcnts */
		psubusb mm1, mm5             /* mm1 -= mm5            0 1 2 3 4 5 6 7    */

		psubusb mm5, mm0             /* mm5 -= mm0            0 1 2 3 4 5 6 7    */
		por     mm1, mm5             /* mm1 |= mm5            0 1 2 3 4 5 6 7    */

		pand    mm1, mm6             /* mm1 &= 0xfefefefefefefefe     */      

		pcmpeqb mm1, mm4             /* are the bytes of mm1 == 0 ?   */

		psubb   mm7, mm1             /* mm7 has running total of eqcnts */

		movq    mm1, mm7             /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psllq   mm7, 8               /* mm7 >>= 24            0 1 2 3 4 5 6 7m   */

		psrlq   mm1, 24              /* mm7 >>= 24            0 1 2 3 4 5 6 7m   */

		paddb   mm7, mm1             /* mm7 has running total of eqcnts */

		movq mm1, mm7                /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psrlq mm7, 16                /* mm7 >>= 16            0 1 2 3 4 5 6 7m   */

		paddb   mm7, mm1             /* mm7 has running total of eqcnts */

		movq mm1, mm7                /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psrlq   mm7, 8               /* mm7 >>= 8             0 1 2 3 4 5 6 7m   */

		paddb mm7, mm1               /* mm7 has running total of eqcnts */

		movd mm32_result, mm7               

		pop eax
	




	};

	eq_cnt = mm32_result & 0xff;

	#ifdef PP_SELF_CHECK
	eq_cnt2 = 0;
	for (k=0; k<4; k++) {
		for (j=1; j<=7; j++) {
			if (ABS(v[j+k*stride]-v[1+j+k*stride]) <= 1) eq_cnt2++;
		}
	}
	if (eq_cnt2 != eq_cnt) printf("ERROR: MMX version of useDC is incorrect\n");
	#endif

	useDC = eq_cnt >= DEBLOCK_HORIZ_USEDC_THR;

	return useDC;
}

