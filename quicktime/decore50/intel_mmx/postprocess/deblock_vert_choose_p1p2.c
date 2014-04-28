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



/* This function chooses the "endstops" for the vertial LPF9 filter: p1 and p2 */
/* We also convert these to 16-bit values here */
INLINE void deblock_vert_choose_p1p2(uint8_t *v, int stride, uint64_t *p1p2, int QP) {
	uint64_t *pmm1, *pmm2;
	uint64_t mm_b_qp;
	#ifdef PP_SELF_CHECK
	int i;
	#endif

	/* load QP into every one of the 8 bytes in mm_b_qp */
	((uint32_t *)&mm_b_qp)[0] = 
	((uint32_t *)&mm_b_qp)[1] = 0x01010101 * QP; 

	pmm1 = (uint64_t *)(&(v[0*stride]));
	pmm2 = (uint64_t *)(&(v[8*stride]));

	__asm {
		push eax
		push ebx
		push ecx
		
		mov eax, pmm1
		mov ebx, pmm2
		mov ecx, p1p2

	/* p1 */
		pxor     mm7, mm7             /* mm7 = 0                       */
		movq     mm0, [eax]          /* mm0 = *pmm1 = v[l0]           */
		movq     mm2, mm0             /* mm2 = mm0 = v[l0]             */
		add      eax, stride         /* pmm1 += stride                */
		movq     mm1, [eax]          /* mm1 = *pmm1 = v[l1]           */
		movq     mm3, mm1             /* mm3 = mm1 = v[l1]             */
		psubusb  mm0, mm1             /* mm0 -= mm1                    */
		psubusb  mm1, mm2             /* mm1 -= mm2                    */
		por      mm0, mm1             /* mm0 |= mm1                    */
		psubusb  mm0, mm_b_qp         /* mm0 -= QP                     */
		/* now a zero byte in mm0 indicates use v0 else use v1              */
		pcmpeqb  mm0, mm7             /* zero bytes to ff others to 00 */
		movq     mm1, mm0             /* make a copy of mm0            */
		/* now ff byte in mm0 indicates use v0 else use v1                  */
		pandn    mm0, mm3             /* mask v1 into 00 bytes in mm0  */
		pand     mm1, mm2             /* mask v0 into ff bytes in mm0  */
		por      mm0, mm1             /* mm0 |= mm1                    */
		movq     mm1, mm0             /* make a copy of mm0            */
		/* Now we have our result, p1, in mm0.  Next, unpack.               */
		punpcklbw mm0, mm7            /* low bytes to mm0              */
		punpckhbw mm1, mm7            /* high bytes to mm1             */
		/* Store p1 in memory                                               */
		movq     [ecx], mm0           /* low words to p1p2[0]          */
		movq     8[ecx], mm1          /* high words to p1p2[1]         */
	/* p2 */
		movq     mm1, [ebx]          /* mm1 = *pmm2 = v[l8]           */
		movq     mm3, mm1             /* mm3 = mm1 = v[l8]             */
		add      ebx, stride         /* pmm2 += stride                */
		movq     mm0, [ebx]          /* mm0 = *pmm2 = v[l9]           */
		movq     mm2, mm0             /* mm2 = mm0 = v[l9]             */
		psubusb  mm0, mm1             /* mm0 -= mm1                    */
		psubusb  mm1, mm2             /* mm1 -= mm2                    */
		por      mm0, mm1             /* mm0 |= mm1                    */
		psubusb  mm0, mm_b_qp         /* mm0 -= QP                     */
		/* now a zero byte in mm0 indicates use v0 else use v1              */
		pcmpeqb  mm0, mm7             /* zero bytes to ff others to 00 */
		movq     mm1, mm0             /* make a copy of mm0            */
		/* now ff byte in mm0 indicates use v0 else use v1                  */
		pandn    mm0, mm3             /* mask v1 into 00 bytes in mm0  */
		pand     mm1, mm2             /* mask v0 into ff bytes in mm0  */
		por      mm0, mm1             /* mm0 |= mm1                    */
		movq     mm1, mm0             /* make a copy of mm0            */
		/* Now we have our result, p2, in mm0.  Next, unpack.               */
		punpcklbw mm0, mm7            /* low bytes to mm0              */
		punpckhbw mm1, mm7            /* high bytes to mm1             */
		/* Store p2 in memory                                               */
		movq     16[ecx], mm0         /* low words to p1p2[2]          */
		movq     24[ecx], mm1         /* high words to p1p2[3]         */

		pop ecx
		pop ebx
		pop eax



	};

	#ifdef PP_SELF_CHECK
	/* check p1 and p2 have been calculated correctly */
	/* p2 */
	for (i=0; i<8; i++) {
		if ( ((ABS(v[9*stride+i] - v[8*stride+i]) - QP > 0) ? v[8*stride+i] : v[9*stride+i])
		     != ((uint16_t *)(&(p1p2[2])))[i] ) {
			 printf("ERROR: problem with P2\n");
		}
	}
	/* p1 */
	for (i=0; i<8; i++) {
		if ( ((ABS(v[0*stride+i] - v[1*stride+i]) - QP > 0) ? v[1*stride+i] : v[0*stride+i])
		     != ((uint16_t *)(&(p1p2[0])))[i] ) {
			 printf("ERROR: problem with P1\n");
		}
	}
	#endif

}

