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

/* I know this could probably be written faster... */


/* Fast copy... needs width and stride to be multiples of 16 */
void fast_copy(unsigned char *src, int src_stride,
                 unsigned char *dst, int dst_stride, 
                 int horizontal_size,   int vertical_size) {
	
	uint8_t *pmm1;
	uint8_t *pmm2;
	int x, y;

	#ifdef PP_SELF_CHECK
	int j, k;
	#endif
	
		pmm1 = src;
		pmm2 = dst;

		for (y=0; y<vertical_size; y++) {

			x = - horizontal_size / 8;

			__asm {
				push edi
				push ebx
				push ecx
				
				mov edi, x
				mov ebx, pmm1
				mov ecx, pmm2
			
			L123:                                         /*                                 */	
				movq   mm0, [ebx]                         /* mm0 = *pmm1                     */							
				movq   [ecx], mm0                         /* *pmm2 = mm0                     */							
				add   ecx, 8                              /* pmm2 +=8                        */
				#ifdef PREFETCH_ENABLE
				prefetcht0 32[ebx]                        /* prefetch ahead                  */							
				#endif
				add   ebx, 8                              /* pmm1 +=8                        */
				add   edi, 1                              /* increment loop counter          */
				jne    L123
				
				pop ecx
				pop ebx
				pop edi
				  
			};

			pmm1 += src_stride;// - horizontal_size;	
			pmm2 += dst_stride;// - horizontal_size;	

		}


	#ifdef PP_SELF_CHECK
	for (k=0; k<vertical_size; k++) {
		for (j=0; j<horizontal_size; j++) {
			if (dst[k*dst_stride + j] != src[k*src_stride + j]) {
				printf("problem with MMX fast copy - Y\n");
			}
		}
	}
	#endif

	__asm { 
		emms
	};

}

