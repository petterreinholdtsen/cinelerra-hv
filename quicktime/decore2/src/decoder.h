#ifndef _DECODER_H_
#define _DECODER_H_

#include "dec_portab.h"
#include "decore2.h"
#include "dec_image.h"


/* Inter-coded macroblock, 1 motion vector */
#define MODE_INTER		0
/* Inter-coded macroblock + dquant */
#define MODE_INTER_Q	1
/* Inter-coded macroblock, 4 motion vectors */
#define MODE_INTER4V	2
/* Intra-coded macroblock */
#define	MODE_INTRA		3
/* Intra-coded macroblock + dquant */
#define MODE_INTRA_Q	4


typedef struct
{
	int32_t x;
	int32_t y;
} VECTOR;


#define MBPRED_SIZE  15


typedef struct
{
	VECTOR mvs[4];

    int16_t pred_values[6][MBPRED_SIZE];
    uint8_t acpred_directions[6];
    
	uint32_t mode;
	uint32_t quant;		// absolute quant

	// uint32_t cbp;
} MACROBLOCK;



typedef struct
{
	// bitstream

	uint32_t time_inc_bits;
	uint32_t quant_bits;
	uint32_t quant_type;

	// image

	uint32_t width;
	uint32_t height;
	uint32_t edged_width;
	uint32_t edged_height;
	
	IMAGE cur;
	IMAGE refn;
	IMAGE refh;
	IMAGE refv;
	IMAGE refhv;

	// macroblock

	uint32_t mb_width;
	uint32_t mb_height;
	MACROBLOCK * mbs;

	
} DECODER;


int decoder_create(DEC_PARAM * param);
int decoder_destroy(DECODER * dec);
int decoder_decode(DECODER * dec, DEC_FRAME * frame);


#endif /* _DECODER_H_ */