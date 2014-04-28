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

/* top level function for vertical deblock that loops from left to right along a line */

/* John Funnell, December 2000 */


/* this is to help us inline our functions */ 
#ifdef PP_COMPONENTS_INLINE
 #ifndef INLINE
 #define INLINE static __inline
 #endif
 #include "deblock_vert_DC_on.c"
 #include "deblock_vert_choose_p1p2.c"
 #include "deblock_vert_copy_and_unpack.c"
 #include "deblock_vert_default_filter.c"
 #include "deblock_vert_lpf9.c"
 #include "deblock_vert_useDC.c"
#endif


/* this is a vertical deblocking filter - i.e. it will smooth _horizontal_ block edges */
void deblock_vert( uint8_t *image, int width, int stride, QP_STORE_T *QP_store, int QP_stride, int chromaFlag) {
	uint64_t v_local[20];
	uint64_t p1p2[4];
	int Bx, x, y;
	int QP, QPx16;
	uint8_t *v;
	int useDC, DC_on;
	#ifdef PREFETCH_AHEAD_V
	void *prefetch_addr;
	#endif

	y = 0;
	
	/* loop over image's block boundary rows */
//	for (y=8; y<height; y+=8) {	
		
		/* loop over all blocks, left to right */
		for (Bx=0; Bx<width; Bx+=8) {

			QP = chromaFlag ? QP_store[y/8*QP_stride+Bx/8]
			                : QP_store[y/16*QP_stride+Bx/16];	
			QPx16 = 16 * QP;
			v = &(image[y*stride + Bx]) - 5*stride;

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

			/* decide whether to use DC mode on a block-by-block basis */
			useDC = deblock_vert_useDC(v, stride);
						
			if (useDC) {
 				/* we are in DC mode for this block.  But we only want to filter low-energy areas */
				
				/* decide whether the filter should be on or off for this block */
				DC_on = deblock_vert_DC_on(v, stride, QP);
				
				if (DC_on) { /* use DC offset mode */
				
						v = &(image[y*stride + Bx])- 5*stride;
						
						/* copy the block we're working on and unpack to 16-bit values */
						deblock_vert_copy_and_unpack(stride, &(v[stride]), &(v_local[2]), 8);

						deblock_vert_choose_p1p2(v, stride, p1p2, QP);
					
						deblock_vert_lpf9(v_local, p1p2, v, stride); 

						#ifdef SHOWDECISIONS_V
						if (!chromaFlag) {
							v[4*stride  ] = 
							v[4*stride+1] = 
							v[4*stride+2] = 
							v[4*stride+3] = 
							v[4*stride+4] = 
							v[4*stride+5] = 
							v[4*stride+6] = 
							v[4*stride+7] = 255;
						}  
						#endif
					}
			}

			if (!useDC) { /* use the default filter */

				///* loop over every column of pixels crossing that horizontal boundary */
				//for (dx=0; dx<8; dx++) {
		
					x = Bx;// + dx;
					v = &(image[y*stride + x])- 5*stride;
			
					deblock_vert_default_filter(v, stride, QP);

				//}
				#ifdef SHOWDECISIONS_V
				if (!chromaFlag) {
					v[4*stride  ] = 
					v[4*stride+1] = 
					v[4*stride+2] = 
					v[4*stride+3] = 
					v[4*stride+4] = 
					v[4*stride+5] = 
					v[4*stride+6] = 
					v[4*stride+7] = 0;
				}  
				#endif
			}
		} 
	__asm { 
		emms
	};
}






