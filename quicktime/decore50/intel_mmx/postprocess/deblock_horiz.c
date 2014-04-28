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

/* this is to help us inline our functions */ 
#ifdef PP_COMPONENTS_INLINE
 #ifndef INLINE
 #define INLINE static __inline
 #endif
 #include "deblock_horiz_DC_on.c"
 #include "deblock_horiz_useDC.c"
 #include "deblock_horiz_default_filter.c"
 #include "deblock_horiz_lpf9.c"
#endif

/* this is a horizontal deblocking filter - i.e. it will smooth _vertical_ block edges */
void deblock_horiz(uint8_t *image, int width, int stride, QP_STORE_T *QP_store, int QP_stride, int chromaFlag) {
	int x, y;
	int QP;
	uint8_t *v;
	int useDC, DC_on;
	#ifdef PREFETCH_AHEAD_H
	void *prefetch_addr;
	#endif

	y = 0;

	/* loop over image's pixel rows , four at a time */
//	for (y=0; y<height; y+=4) {	
		
		/* loop over every block boundary in that row */
		for (x=8; x<width; x+=8) {
		
			/* extract QP from the decoder's array of QP values */
			QP = chromaFlag ? QP_store[y/8*QP_stride+x/8]
			                : QP_store[y/16*QP_stride+x/16];	

			/* v points to pixel v0, in the left-hand block */
			v = &(image[y*stride + x]) - 5;

			#ifdef PREFETCH_AHEAD_V
			/* try a prefetch PREFETCH_AHEAD_V bytes ahead on all eight rows... experimental */
			prefetch_addr = v + PREFETCH_AHEAD_V;
			__asm {
				push eax
				push ebx
				mov eax, prefetch_addr
				mov ebx, stride
				add      eax , ebx        /* prefetch_addr+= stride */
				prefetcht0 [eax]           
				add      eax , ebx        /* prefetch_addr+= stride */
				prefetcht0 [eax]           
				add      eax , ebx        /* prefetch_addr+= stride */
				prefetcht0 [eax]           
				add      eax , ebx        /* prefetch_addr+= stride */
				prefetcht0 [eax]           
				pop ebx
				pop eax
			};
			#endif

			/* first decide whether to use default or DC offet mode */ 
			useDC = deblock_horiz_useDC(v, stride);

			if (useDC) { /* use DC offset mode */
				
				DC_on = deblock_horiz_DC_on(v, stride, QP);

				if (DC_on) {

					deblock_horiz_lpf9(v, stride, QP); 

					#ifdef SHOWDECISIONS_H
					if (!chromaFlag) {
						v[0*stride + 4] = 
						v[1*stride + 4] = 
						v[2*stride + 4] = 
						v[3*stride + 4] = 255;  
					}
					#endif
				}

			} else {     /* use default mode */
			
				deblock_horiz_default_filter(v, stride, QP);

				#ifdef SHOWDECISIONS_H
				if (!chromaFlag) {
					v[0*stride + 4] = 
					v[1*stride + 4] = 
					v[2*stride + 4] = 
					v[3*stride + 4] = 0;  
				}
				#endif

			}
		}
	__asm { 
		emms
	};
}

