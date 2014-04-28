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


/* horizontal deblocking filter used in default (non-DC) mode */
INLINE void deblock_horiz_default_filter(uint8_t *v, int stride, int QP) {
	int a3_0, a3_1, a3_2, d;
	int q1, q;
	int y;

	/* 
	   scalar implementation for the time being - going to MMX may be complex for 
		little gain? I have some ideas how do go to MMX but anyone's welcome to try!   

		The compiler used for this module can make a big difference to performance.
		this seems to be fastest when compiled using pgcc.  I haven't tried the 
		latest Intel compiler yet.
		
		I suspect that the benefits from SIMD here are neglible since the scalar
		version is easily able to break out of the loop and save CPU time.  SIMD
		would have a lot more inertia and may have to complete all calculations
		even if they are not needed.  Still, branches are expensive so, well
		SIMD/MMX is worth a try!
		
		JF
	*/

	for (y=0; y<4; y++) {

		q1 = v[4] - v[5];
		q = q1 / 2;
		if (q) {

			a3_0  = q1;
			a3_0 += a3_0 << 2;
			a3_0 = 2*(v[3]-v[6]) - a3_0;
			
			/* apply the 'delta' function first and check there is a difference to avoid wasting time */
			if (ABS(a3_0) < 8*QP) {
				a3_1  = v[3]-v[2];
				a3_2  = v[7]-v[8];
				a3_1 += a3_1 << 2;
				a3_2 += a3_2 << 2;
				a3_1 += (v[1]-v[4]) << 1;
				a3_2 += (v[5]-v[8]) << 1;
				d = ABS(a3_0) - MIN(ABS(a3_1), ABS(a3_2));
		
				if (d > 0) { /* energy across boundary is greater than in one or both of the blocks */
					d += d<<2;
					d = (d + 32) >> 6; 
	
					if (d > 0) {
	
						//d *= SIGN(-a3_0);
					
						/* clip d in the range 0 ... q */
						if (q > 0) {
							if (a3_0 < 0) {
								//d = d<0 ? 0 : d;
								d = d>q ? q : d;
								v[4] -= d;
								v[5] += d;
							}
						} else {
							if (a3_0 > 0) {
								//d = d>0 ? 0 : d;
								d = (-d)<q ? q : (-d);
								v[4] -= d;
								v[5] += d;
							}
						}
					}
				}
			}
		}

		#ifdef PP_SELF_CHECK
		/* no selfcheck written for this yet */
		#endif;

		v += stride;
	}


}
