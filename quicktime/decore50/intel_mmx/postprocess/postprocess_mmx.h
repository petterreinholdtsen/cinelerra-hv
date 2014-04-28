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
// postprocess_mmx.h //

/***

References:  
 * ISO/IEC 14496-2
 * MoMuSys-FDIS-V1.0-990812
 * Intel Architecture Software Developer's Manual
   Volume 2: Instruction Set Reference

***/

/* Header to be included in all component MMX files */

#ifndef _POSTPROCESS_MMX_H_
#define _POSTPROCESS_MMX_H_


/* include the public API header file */
#include "postprocess.h" 




/*********************** self checking ***********************/

/* PP_SELF_CHECK enables exhaustive self-checking of all MMX routines */
/* self checking is broken in the "wine cl.exe" environment as printf() crashes */
//#define PP_SELF_CHECK

/* we only ever printf() if self-checking is turned on at compile time */
#ifdef PP_SELF_CHECK
 #include <stdio.h>
#endif





/******************** inlining functions ***********************/

/*
 If we want the component functions of the filter to be inlined into
 the main filter loops, then we need to arrange for their source files
 to be #include'd and for them to be identified as static inline
*/
#ifdef PP_COMPONENTS_INLINE
 #ifdef WIN32
  #define INLINE  __inline
 #else
  #define INLINE  inline
 #endif
#else
 #define INLINE
#endif 





/******************* Experimental prefetch *****************/
/* it would seem my version of Microsoft's tools do not recognise prefetch 
   instructions so this isn't working! (It worked for gcc) */
//#define PREFETCH_ENABLE

/* the optimum values (8 and 8) for these two were discovered by experimentation */
//#define PREFETCH_AHEAD_H 8
//#define PREFETCH_AHEAD_V 8






/******************* general, useful macros ****************/
#define ABS(a)     ( (a)>0 ? (a) : -(a) )
#define SIGN(a)    ( (a)<0 ? -1 : 1 )
#define MIN(a, b)  ( (a)<(b) ? (a) : (b) )
#define MAX(a, b)  ( (a)>(b) ? (a) : (b) )






/******************** component function prototypes **************/
int deblock_horiz_useDC(uint8_t *v, int stride);
int deblock_horiz_DC_on(uint8_t *v, int stride, int QP);
void deblock_horiz_lpf9(uint8_t *v, int stride, int QP);
void deblock_horiz_default_filter(uint8_t *v, int stride, int QP);
void deblock_horiz(uint8_t *image, int width, int stride, QP_STORE_T *QP_store, int QP_stride, int chromaFlag);
int deblock_vert_useDC(uint8_t *v, int stride);
int deblock_vert_DC_on(uint8_t *v, int stride, int QP);
void deblock_vert_copy_and_unpack(int stride, uint8_t *source, uint64_t *dest, int n);
void deblock_vert_choose_p1p2(uint8_t *v, int stride, uint64_t *p1p2, int QP);
void deblock_vert_lpf9(uint64_t *v_local, uint64_t *p1p2, uint8_t *v, int stride);
void deblock_vert_default_filter(uint8_t *v, int stride, int QP);
void deblock_vert( uint8_t *image, int width, int stride, QP_STORE_T *QP_store, int QP_stride, int chromaFlag);
void fast_copy(unsigned char *src, int src_stride,
                 unsigned char *dst, int dst_stride, 
                 int horizontal_size,   int vertical_size);
void do_emms();




#endif
