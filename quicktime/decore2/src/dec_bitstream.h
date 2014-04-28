#ifndef DECORE_BITSTREAM_H_
#define DECORE_BITSTREAM_H_

#include "dec_portab.h"
#include "decoder.h"


// vop coding types 
// intra, prediction, backward, sprite, not_coded
#define I_VOP	0
#define P_VOP	1
#define B_VOP	2
#define S_VOP	3
#define N_VOP	4

typedef struct
{
	uint64_t buf;
	uint32_t pos;
	unsigned char *head;
} BITSTREAM;



// header stuff
int bs_vol(BITSTREAM * bs, DECODER * dec);
int bs_vop(BITSTREAM * bs, DECODER * dec, uint32_t * rounding, uint32_t * quant, uint32_t * fcode);


void bs_init(BITSTREAM * const bs, void * const bitstream);



static void refill(BITSTREAM * const bs, const uint32_t bits)
{
	while(64 - bs->pos < bits)
	{
		bs->buf <<= 8;
		bs->buf |= *bs->head++;
		bs->pos -= 8;
	}
}



static uint32_t __inline bs_show(BITSTREAM * const bs, const uint32_t bits)
{
	refill(bs, bits);
	return (0xffffffff >> (32 - bits)) &
		(bs->buf >> (64 - bits - bs->pos));
}



static __inline void bs_skip(BITSTREAM * const bs, const uint32_t bits)
{
	refill(bs, bits);
	bs->pos += bits;
}



static __inline void bs_bytealign(BITSTREAM * const bs)
{
	bs_skip(bs, (64 - bs->pos) % 8);
}



static uint32_t __inline bs_get(BITSTREAM * const bs, const uint32_t bits)
{
	refill(bs, bits);
	uint32_t result = (0xffffffff >> (32 - bits)) &
		(bs->buf >> (64 - bits - bs->pos));
	bs->pos += bits;
	return result;
}


static uint32_t __inline bs_get1(BITSTREAM * const bs)
{
	return bs_get(bs, 1);
}


#endif /* _BITSTREAM_H_ */