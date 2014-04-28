#ifndef DEC_MBCODING_H_
#define DEC_MBCODING_H_

#include "dec_portab.h"
#include "dec_bitstream.h"
#include "decoder.h"

int get_mcbpc_intra(BITSTREAM * bs);
int get_mcbpc_inter(BITSTREAM * bs);
int get_cbpy(BITSTREAM * bs, int intra);
int get_mv(BITSTREAM * bs, uint32_t scale_fac, uint32_t fcode);

int get_dc_dif(BITSTREAM * bs, uint32_t dc_size);
int get_intra_coeff(BITSTREAM * bs, int *run, int *last);

#endif /* _MBCODING_H_ */