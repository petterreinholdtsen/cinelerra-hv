#ifndef DEC_DCT_H_
#define DEC_DCT_H_

#include "dec_portab.h"


void idct_int32_init();
void idct_int32 (short * const block);


void idct_mmx (short * const src_result);
void idct_sse (short * const src_result);

#endif /* _DCT_H_ */

