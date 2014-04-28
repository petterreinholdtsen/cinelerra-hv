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



/* function using MMX to copy an 8-pixel wide column and unpack to 16-bit values */
/* n is the number of rows to copy - this muxt be even */
INLINE void deblock_vert_copy_and_unpack(int stride, uint8_t *source, uint64_t *dest, int n) {
	uint64_t *pmm1 = (uint64_t *)source;
	uint64_t *pmm2 = (uint64_t *)dest;
	int i = -n / 2;
	#ifdef PP_SELF_CHECK
	int j, k;
	#endif

	/* copy block to local store whilst unpacking to 16-bit values */
	__asm {
		push edi
		push eax
		push ebx
		
		mov edi, i
		mov eax, pmm1
		mov ebx, pmm2

		pxor   mm7, mm7                        /* set mm7 = 0                     */
	deblock_v_L1:                             /* now p1 is in mm1                */	
		movq   mm0, [eax]                     /* mm0 = v[0*stride]               */							

		#ifdef PREFETCH_ENABLE
		prefetcht0 0[ebx]                 
		#endif
		add   eax, stride                    /* p_data += stride                */
		movq   mm1, mm0                        /* mm1 = v[0*stride]               */							
		punpcklbw mm0, mm7                     /* unpack low bytes (left hand 4)  */

		movq   mm2, [eax]                     /* mm2 = v[0*stride]               */							
		punpckhbw mm1, mm7                     /* unpack high bytes (right hand 4)*/

		movq   mm3, mm2                        /* mm3 = v[0*stride]               */							
		punpcklbw mm2, mm7                     /* unpack low bytes (left hand 4)  */

		movq   [ebx], mm0                     /* v_local[n] = mm0 (left)         */
		add   eax, stride                    /* p_data += stride                */

		movq   8[ebx], mm1                    /* v_local[n+8] = mm1 (right)      */
		punpckhbw mm3, mm7                     /* unpack high bytes (right hand 4)*/

		movq   16[ebx], mm2                   /* v_local[n+16] = mm2 (left)      */

		movq   24[ebx], mm3                   /* v_local[n+24] = mm3 (right)     */

		add   ebx, 32                        /* p_data2 += 8                    */
		
		add   i, 1                            /* increment loop counter          */
		jne    deblock_v_L1             


		pop ebx
		pop eax
		pop edi








	};

	#ifdef PP_SELF_CHECK
	/* check that MMX copy has worked correctly */
	for (k=0; k<n; k++) {
		for (j=0; j<8; j++) {
			if ( ((uint16_t *)dest)[k*8+j] != source[k*stride+j] ) {
				printf("ERROR: MMX copy block is flawed at (%d, %d)\n", j, k);  
			} 
		}
	}
	#endif

}



