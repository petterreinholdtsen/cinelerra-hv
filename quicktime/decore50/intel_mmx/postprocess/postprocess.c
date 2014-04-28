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

/* John Funnell, 2000,2001 */






/* entry point for MMX postprocessing */
void postprocess(unsigned char * src[], int src_stride,
                 unsigned char * dst[], int dst_stride, 
                 int horizontal_size,   int vertical_size, 
                 QP_STORE_T *QP_store,  int QP_stride,
					  int mode) {
					  
	uint8_t *puc_src;
	uint8_t *puc_dst;
	uint8_t *puc_flt;
	QP_STORE_T *QP_ptr;
	int y, i;


	/* this loop is (hopefully) going to improve performance */
	/* loop down the picture, copying and processing in vertical stripes, each four pixels high */
	for (y=0; y<vertical_size; y+= 4) {
		
		if (!(mode & PP_DONT_COPY)) {
			puc_src = &((src[0])[y*src_stride]);
			puc_dst = &((dst[0])[y*dst_stride]);

			/* First copy source to destination... */
			fast_copy(puc_src, src_stride, puc_dst, dst_stride, horizontal_size, 4);
		}
		
		if (mode & PP_DEBLOCK_Y_H) {
			puc_flt = &((dst[0])[y*dst_stride]);  
			QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
			deblock_horiz(puc_flt, horizontal_size,      dst_stride, QP_ptr, QP_stride, 0);
		}

		if (mode & PP_DEBLOCK_Y_V) { 
			if ( (y%8) && (y-4)>5 )   {
				puc_flt = &((dst[0])[(y-4)*dst_stride]);  
				QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
				deblock_vert( puc_flt, horizontal_size,   dst_stride, QP_ptr, QP_stride, 0);
			}
		}

//		if (mode & PP_DERING_Y) {
//			dering(       puc_flt, horizontal_size,   dst_stride, QP_ptr, QP_stride, 0);
//		}

	} /* for loop */

	/* now we're going to do U and V assuming 4:2:0 */
	horizontal_size >>= 1;
	vertical_size   >>= 1;
	src_stride      >>= 1;
	dst_stride      >>= 1;


	/* loop U then V */
	for (i=1; i<=2; i++) {

	for (y=0; y<vertical_size; y+= 4) {
		
		if (!(mode & PP_DONT_COPY)) {
			puc_src = &((src[i])[y*src_stride]);
			puc_dst = &((dst[i])[y*dst_stride]);

			/* First copy source to destination... */
			fast_copy(puc_src, src_stride, puc_dst, dst_stride, horizontal_size, 4);
		}
		
		if (mode & PP_DEBLOCK_C_H) {
			puc_flt = &((dst[i])[y*dst_stride]);  
			QP_ptr  = &(QP_store[(y>>3)*QP_stride]);
			deblock_horiz(puc_flt, horizontal_size,      dst_stride, QP_ptr, QP_stride, 1);
		}

		if (mode & PP_DEBLOCK_C_V) { 
			if ( (y%8) && (y-4)>5 )   {
				puc_flt = &((dst[i])[(y-4)*dst_stride]);  
				QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
				deblock_vert( puc_flt, horizontal_size,   dst_stride, QP_ptr, QP_stride, 1);
			}
		}

//		if (mode & PP_DERING_Y) {
//			dering(       puc_flt, horizontal_size,   dst_stride, QP_ptr, QP_stride, 1);
//		}

	} /* stripe loop */

	} /* U,V loop */

	do_emms();


}

