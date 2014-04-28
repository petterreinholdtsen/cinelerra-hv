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

/* decide DC mode or default mode in assembler */
INLINE  int deblock_vert_useDC(uint8_t *v, int stride) {
	const uint64_t mask   = 0xfefefefefefefefe;
	uint32_t mm_data1;
	uint64_t *pmm1;
	int eq_cnt, useDC;
	#ifdef PP_SELF_CHECK
	int useDC2, i, j;
	#endif

	#ifdef PP_SELF_CHECK
	/* C-code version for testing */
	eq_cnt = 0;
	for (j=1; j<8; j++) {
		for (i=0; i<8; i++) {
			if (ABS(v[j*stride+i] - v[(j+1)*stride+i]) <= 1) eq_cnt++;
		}
	}
	useDC2 = (eq_cnt > DEBLOCK_VERT_USEDC_THR); 
	#endif
			
	/* starting pointer is at v[stride] == v1 in mpeg4 notation */
	pmm1 = (uint64_t *)(&(v[stride]));

	/* first load some constants into mm4, mm6, mm7 */
	__asm {
		push eax
		mov eax, pmm1

		movq mm6, mask               /*mm6 = 0xfefefefefefefefe       */
		pxor mm7, mm7                /*mm7 = 0x0000000000000000       */

		movq mm2, [eax]             /* mm2 = *p_data                 */
		pxor mm4, mm4                /*mm4 = 0x0000000000000000       */

		add   eax, stride          /* p_data += stride              */
		movq   mm3, mm2              /* mm3 = *p_data                 */






	};

	__asm {

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		movq   mm0, mm3             /* mm0 = mm3                     */

		movq   mm3, mm2             /* mm3 = *p_data                 */
		movq   mm1, mm0             /* mm1 = mm0                     */

		psubusb mm0, mm2            /* mm0 -= mm2                    */
		add   eax, stride         /* p_data += stride              */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm0, mm2             /* mm0 |= mm2                    */

		pand   mm0, mm6             /* mm0 &= 0xfefefefefefefefe     */      
		pcmpeqb mm0, mm4            /* is mm0 == 0 ?                 */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		psubb  mm7, mm0             /* mm7 has running total of eqcnts */

		movq   mm5, mm3             /* mm5 = mm3                     */
		movq   mm3, mm2             /* mm3 = *p_data                 */

		movq   mm1, mm5             /* mm1 = mm5                     */
		psubusb mm5, mm2            /* mm5 -= mm2                    */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm5, mm2             /* mm5 |= mm2                    */

		add   eax, stride         /* p_data += stride              */
		pand   mm5, mm6             /* mm5 &= 0xfefefefefefefefe     */      

		pcmpeqb mm5, mm4            /* is mm0 == 0 ?                 */
		psubb  mm7, mm5             /* mm7 has running total of eqcnts */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		movq   mm0, mm3             /* mm0 = mm3                     */

		movq   mm3, mm2             /* mm3 = *p_data                 */
		movq   mm1, mm0             /* mm1 = mm0                     */

		psubusb mm0, mm2            /* mm0 -= mm2                    */
		add   eax, stride         /* p_data += stride              */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm0, mm2             /* mm0 |= mm2                    */

		pand   mm0, mm6             /* mm0 &= 0xfefefefefefefefe     */      
		pcmpeqb mm0, mm4            /* is mm0 == 0 ?                 */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		psubb  mm7, mm0             /* mm7 has running total of eqcnts */

		movq   mm5, mm3             /* mm5 = mm3                     */
		movq   mm3, mm2             /* mm3 = *p_data                 */

		movq   mm1, mm5             /* mm1 = mm5                     */
		psubusb mm5, mm2            /* mm5 -= mm2                    */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm5, mm2             /* mm5 |= mm2                    */

		add   eax, stride         /* p_data += stride              */
		pand   mm5, mm6             /* mm5 &= 0xfefefefefefefefe     */      

		pcmpeqb mm5, mm4            /* is mm0 == 0 ?                 */
		psubb  mm7, mm5             /* mm7 has running total of eqcnts */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		movq   mm0, mm3             /* mm0 = mm3                     */

		movq   mm3, mm2             /* mm3 = *p_data                 */
		movq   mm1, mm0             /* mm1 = mm0                     */

		psubusb mm0, mm2            /* mm0 -= mm2                    */
		add   eax, stride         /* p_data += stride              */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm0, mm2             /* mm0 |= mm2                    */

		pand   mm0, mm6             /* mm0 &= 0xfefefefefefefefe     */      
		pcmpeqb mm0, mm4            /* is mm0 == 0 ?                 */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		psubb  mm7, mm0             /* mm7 has running total of eqcnts */

		movq   mm5, mm3             /* mm5 = mm3                     */
		movq   mm3, mm2             /* mm3 = *p_data                 */

		movq   mm1, mm5             /* mm1 = mm5                     */
		psubusb mm5, mm2            /* mm5 -= mm2                    */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm5, mm2             /* mm5 |= mm2                    */

		add   eax, stride         /* p_data += stride              */
		pand   mm5, mm6             /* mm5 &= 0xfefefefefefefefe     */      

		pcmpeqb mm5, mm4            /* is mm0 == 0 ?                 */
		psubb  mm7, mm5             /* mm7 has running total of eqcnts */

		movq   mm2, [eax]          /* mm2 = *p_data                 */
		movq   mm0, mm3             /* mm0 = mm3                     */

		movq   mm3, mm2             /* mm3 = *p_data                 */
		movq   mm1, mm0             /* mm1 = mm0                     */

		psubusb mm0, mm2            /* mm0 -= mm2                    */
		add   eax, stride         /* p_data += stride              */

		psubusb mm2, mm1            /* mm2 -= mm1                    */
		por    mm0, mm2             /* mm0 |= mm2                    */

		pand   mm0, mm6             /* mm0 &= 0xfefefefefefefefe     */      
		pcmpeqb mm0, mm4            /* is mm0 == 0 ?                 */

		psubb  mm7, mm0             /* mm7 has running total of eqcnts */

		pop eax	
		
	
	
	
	
	                                       
	};
			
	/* now mm7 contains negative eq_cnt for all 8-columns */
	/* copy this to mm_data1                              */
	/* sum all 8 bytes in mm7 */
	__asm {
		movq    mm1, mm7            /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psrlq   mm7, 32             /* mm7 >>= 32            0 1 2 3 4 5 6 7m   */

		paddb   mm7, mm1            /* mm7 has running total of eqcnts */

		movq mm1, mm7               /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psrlq   mm7, 16             /* mm7 >>= 16            0 1 2 3 4 5 6 7m   */

		paddb   mm1, mm7            /* mm7 has running total of eqcnts */

		movq mm7, mm1               /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
		psrlq   mm7, 8              /* mm7 >>= 8             0 1 2 3 4 5 6 7m   */

		paddb   mm7, mm1            /* mm7 has running total of eqcnts */

		movd mm_data1, mm7          /* mm_data1 = mm7       */

	};

	eq_cnt = mm_data1 & 0xff;
			
	useDC = (eq_cnt  > DEBLOCK_VERT_USEDC_THR);			
			
	#ifdef PP_SELF_CHECK
	if (useDC != useDC2) printf("ERROR: MMX version of useDC is incorrect\n");
	#endif
	
	return useDC;
}

