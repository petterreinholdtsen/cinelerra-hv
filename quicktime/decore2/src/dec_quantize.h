#ifndef DEC_QUANTIZE_H_
#define DEC_QUANTIZE_H_


#include "dec_portab.h"


/* plain c */


void dec_dequant_intra(int16_t *data,
				const int16_t *coeff,
				const uint8_t quant,
				const uint8_t dcscalar);


void dec_dequant_inter(int16_t *data,
				const int16_t *coeff,
				const uint8_t quant);


/* mmx */

void dec_dequant_intra_mmx(int16_t *data,
					const int16_t * const coeff,
					const uint32_t quant,
					const uint32_t dcscalar);

void dec_dequant_inter_mmx(int16_t * data,
					const int16_t * const coeff,
					const uint32_t quant);


#endif /* _QUANTIZE_H_ */