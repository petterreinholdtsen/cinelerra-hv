#ifndef DEC_TRANSFER_H_
#define DEC_TRANSFER_H_


#include "dec_portab.h"


/* plain c */

void dec_transfer_16to8copy(uint8_t * const dst,
					const int16_t * const src,
					uint32_t stride);

void dec_transfer_16to8add(uint8_t * const dst,
					const int16_t * const src,
					uint32_t stride);


/* mmx */

void dec_transfer_16to8copy_mmx(uint8_t * const dst,
					const int16_t * const src,
					uint32_t stride);

void dec_transfer_16to8add_mmx(uint8_t * const dst,
					const int16_t * const src,
					uint32_t stride);


#endif /* _TRANSFER_H_ */