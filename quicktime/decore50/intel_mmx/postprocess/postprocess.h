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
// postprocess.h //

/***

References:  
 * ISO/IEC 14496-2
 * MoMuSys-FDIS-V1.0-990812
 * Intel Architecture Software Developer's Manual
   Volume 2: Instruction Set Reference

***/

/*

 Currently this contains only the deblocking filter.  The vertial   
 deblocking filter operates over eight pixel-wide columns at once.  The
 horizontal deblocking filter works on four horizontals row at a time. 

 Picture height must be multiple of 8, width a multiple of 16
 
*/


#ifndef POSTPROCESS_H
#define POSTPROCESS_H



/* we're not using <inttypes.h> for now, even on Linux */
#define  int8_t             char
#define uint8_t    unsigned char
#define  int16_t            short
#define uint16_t   unsigned short
#define  int32_t            int
#define uint32_t   unsigned int
#ifdef WIN32
#define  int64_t            __int64
#define uint64_t   unsigned __int64
#else
#define  int64_t            long long
#define uint64_t   unsigned long long
#endif


/**** Compile-time options ****/

/* the following parameters allow for some tuning of the postprocessor */
#define DEBLOCK_HORIZ_USEDC_THR    (28 -  12)
#define DEBLOCK_VERT_USEDC_THR     (56 - 24)




/* Type to use for QP. This may depend on the decoder's QP store implementation */
#define QP_STORE_T int



/**** Function prototype - entry point for postprocessing ****/
void postprocess(unsigned char * src[], int src_stride,
                 unsigned char * dst[], int dst_stride, 
                 int horizontal_size,   int vertical_size, 
                 QP_STORE_T *QP_store,  int QP_stride,
					  int mode);


//#define SHOWDECISIONS_V
//#define SHOWDECISIONS_H




/**** mode flags to control postprocessing actions ****/
#define PP_DEBLOCK_Y_H  0x00000001  /* Luma horizontal deblocking   */
#define PP_DEBLOCK_Y_V  0x00000002  /* Luma vertical deblocking     */
#define PP_DEBLOCK_C_H  0x00000004  /* Chroma horizontal deblocking */
#define PP_DEBLOCK_C_V  0x00000008  /* Chroma vertical deblocking   */
#define PP_DERING_Y     0x00000010  /* Luma deringing               */
#define PP_DERING_C     0x00000020  /* Chroma deringing             */
#define PP_DONT_COPY    0x10000000  /* Postprocessor will not copy src -> dst */
                                    /* instead, it will operate on dst only   */





#endif
