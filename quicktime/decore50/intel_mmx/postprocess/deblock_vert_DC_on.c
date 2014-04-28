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



/* decide whether the DC filter should be turned on accoding to QP */
INLINE int deblock_vert_DC_on(uint8_t *v, int stride, int QP) {
	uint64_t QP_x_2;
	uint8_t *ptr1;
	uint8_t *ptr2;
	int DC_on;
	#ifdef PP_SELF_CHECK
	int i, DC_on2;
	#endif

	ptr1 = &(v[1*stride]);
	ptr2 = &(v[8*stride]);

	#ifdef PP_SELF_CHECK
	DC_on2 = 1;
	for (i=0; i<8; i++) {
		if (ABS(v[i+1*stride]-v[i+8*stride]) > 2 *QP) DC_on2 = 0;
	}
	#endif

	/* load 2*QP into every byte in QP_x_2 */
	((uint32_t *)(&QP_x_2))[0] =
	((uint32_t *)(&QP_x_2))[1] = 0x02020202 * QP; 

	/* MMX assembler to compute DC_on */
	__asm {
		push eax
		push ebx
		
		mov eax, ptr1
		mov ebx, ptr2

		movq     mm0, [eax]               /* mm0 = v[l1]                   */
		movq     mm1, mm0                 /* mm1 = v[l1]                   */
		movq     mm2, [ebx]               /* mm2 = v[l8]                   */
		psubusb  mm0, mm2                 /* mm0 -= mm2                    */
		psubusb  mm2, mm1                 /* mm2 -= mm1                    */
		por      mm0, mm2                 /* mm0 |= mm2                    */
		psubusb  mm0, QP_x_2              /* mm0 -= 2 * QP                 */
		movq     mm1, mm0                 /* mm1 = mm0                     */
		psrlq    mm0, 32                  /* shift mm0 right 32 bits       */
		por      mm0, mm1                 /*                               */
		movd     DC_on, mm0               /* this is actually !DC_on       */

		pop ebx
		pop eax

	};
				
	DC_on = !DC_on; /* the above assembler computes the opposite sense! */
	
	#ifdef PP_SELF_CHECK
	if (DC_on != DC_on2) {
		printf("ERROR: MMX version of DC_on is incorrect\n");
	}
	#endif

	return DC_on;
}


