#include <stdlib.h>

#include "decore2.h"

#include "decoder.h"
#include "dec_bitstream.h"

//#include "header.h"
#include "dec_mbcoding.h"
#include "dec_mbprediction.h"
#include "dec_quantize.h"
#include "dec_dct.h"
#include "dec_transfer.h"
#include "dec_colorspace.h"


/* shift to mbcoding later */

static const uint16_t scan_tables[3][64] =
{
	{	// zig_zag_scan
	    0,	1,	8,	16, 9,	2,	3,	10,
		17, 24, 32, 25, 18, 11, 4,	5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13, 6,	7,	14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63
	},

	{	// horizontal_scan
	    0,	1,	2,	3,	8,	9,	16, 17,
	    10, 11,	4,	5,	6,	7,	15, 14,
	    13, 12, 19, 18, 24, 25, 32, 33,
	    26, 27, 20, 21, 22, 23, 28, 29,
	    30, 31, 34, 35, 40, 41, 48, 49,
	    42, 43, 36, 37, 38, 39, 44, 45,
	    46, 47, 50, 51, 56, 57, 58, 59,
	    52, 53, 54, 55, 60, 61, 62, 63
	},
	
	{	// vertical_scan
	    0, 8, 16, 24, 1, 9, 2, 10,
	    17, 25, 32, 40, 48, 56, 57, 49,
	    41, 33, 26, 18, 3, 11, 4, 12,
	    19, 27, 34, 42, 50, 58, 35, 43,
	    51, 59, 20, 28, 5, 13, 6, 14,
	    21, 29, 36, 44, 52, 60, 37, 45,
	    53, 61, 22, 30, 7, 15, 23, 31,
	    38, 46, 54, 62, 39, 47, 55, 63
	}
};




int decoder_create(DEC_PARAM * param)
{
	DECODER * dec;

	//DEBUG("create_start\n");

	dec = malloc(sizeof(DECODER));
	if (dec == NULL) 
	{
		return DEC_MEMORY;
	}
	param->handle = dec;

	dec->width = param->width;
	dec->height = param->height;
	DEBUGI2("w,h", dec->width, dec->height)

	dec->mb_width = (dec->width + 15) / 16;
	dec->mb_height = (dec->height + 15) / 16;
	DEBUGI2("mb_w,mb_h", dec->mb_width, dec->mb_height)

	dec->edged_width = 16 * dec->mb_width + 2 * EDGE_SIZE;
	dec->edged_height = 16 * dec->mb_height + 2 * EDGE_SIZE;
	DEBUGI2("ew,eh", dec->edged_width, dec->edged_height)
	
	image_create(&dec->cur, dec->edged_width, dec->edged_height);
	image_create(&dec->refn, dec->edged_width, dec->edged_height);
	image_create(&dec->refh, dec->edged_width, dec->edged_height);
	image_create(&dec->refv, dec->edged_width, dec->edged_height);
	image_create(&dec->refhv, dec->edged_width, dec->edged_height);

	dec->mbs = malloc(sizeof(MACROBLOCK) * dec->mb_width * dec->mb_height);
	if (dec->mbs == NULL)
	{
		return DEC_MEMORY;
	}

	init_yuv2rgb();
	dec_idct_int32_init();

	return DEC_OK;
}


int decoder_destroy(DECODER * dec)
{
	free(dec->mbs);
	image_destroy(&dec->refhv, dec->edged_width, dec->edged_height);
	image_destroy(&dec->refv, dec->edged_width, dec->edged_height);
	image_destroy(&dec->refh, dec->edged_width, dec->edged_height);
	image_destroy(&dec->refn, dec->edged_width, dec->edged_height);
	image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
	free(dec);

	return DEC_OK;
}

#define MODE_INTER                      0
#define MODE_INTER_Q                    1
#define MODE_INTER4V                    2
#define MODE_INTRA                      3
#define MODE_INTRA_Q                    4
#define MODE_INTER4V_Q          5
#define MODE_STUFFING			7

static const int32_t dquant_table[4] =
{
	-1, -2, 1, 2
};


uint32_t get_dc_scaler(uint32_t quant, uint32_t lum)
{

	if(quant > 0 && quant < 5) {
		return 8;
	}

	if(quant < 25 && !lum) {
		return (quant + 13) >> 1;
	}

	if(quant < 9) {
		return quant << 1;
	}

    if(quant < 25) {
        return quant + 8;
	}

	if(lum)
		return (quant << 1) - 16;
	else
        return quant - 6;
}



#define DEQUANT_INTRA		dec_dequant_intra
#define IDCT				dec_idct_int32	
#define TRANSFER_16TO8COPY	dec_transfer_16to8copy



static int get_dc_size_lum(BITSTREAM * bs)
{
	int code;

	if (bs_show(bs, 11) == 1) {
		bs_skip(bs, 11);
		return 12;
	}
	if (bs_show(bs, 10) == 1) {
		bs_skip(bs, 10);
		return 11;
	}
	if (bs_show(bs, 9) == 1) {
		bs_skip(bs, 9);
		return 10;
	}
	if (bs_show(bs, 8) == 1) {
		bs_skip(bs, 8);
		return 9;
	}
	if (bs_show(bs, 7) == 1) {
		bs_skip(bs, 7);
		return 8;
	}
	if (bs_show(bs, 6) == 1) {
		bs_skip(bs, 6);
		return 7;
	}  
	if (bs_show(bs, 5) == 1) {
		bs_skip(bs, 5);
		return 6;
	}
	if (bs_show(bs, 4) == 1) {
		bs_skip(bs, 4);
		return 5;
	}

	code = bs_show(bs, 3);

//printf("get_dc_size_lum 1 %x %02x\n", code, bs_show(bs, 16));
	if (code == 1) {
		bs_skip(bs, 3);
		return 4;
	} else if (code == 2) {
		bs_skip(bs, 3);
		return 3;
	} else if (code == 3) {
		bs_skip(bs, 3);
//printf("get_dc_size_lum 2 %x %02x\n", code, bs_show(bs, 16));
		return 0;
	}

	code = bs_show(bs, 2);

	if (code == 2) {
		bs_skip(bs, 2);
		return 2;
	} else if (code == 3) {
		bs_skip(bs, 2);
		return 1;
	}     
//printf("get_dc_size_lum 2 %02x\n", bs_show(bs, 16));

	return 0;
}

static int get_dc_size_chrom(BITSTREAM * bs)
{
	if (bs_show(bs, 12) == 1) {
		bs_skip(bs, 12);
		return 12;
	}
	if (bs_show(bs, 11) == 1) {
		bs_skip(bs, 11);
		return 11;
	}
	if (bs_show(bs, 10) == 1) {
		bs_skip(bs, 10);
		return 10;
	}
	if (bs_show(bs, 9) == 1) {
		bs_skip(bs, 9);
		return 9;
	}
	if (bs_show(bs, 8) == 1) {
		bs_skip(bs, 8);
		return 8;
	}
	if (bs_show(bs, 7) == 1) {
		bs_skip(bs, 7);
		return 7;
	}
	if (bs_show(bs, 6) == 1) {
		bs_skip(bs, 6);
		return 6;
	}
	if (bs_show(bs, 5) == 1) {
		bs_skip(bs, 5);
		return 5;
	}
	if (bs_show(bs, 4) == 1) {
		bs_skip(bs, 4);
		return 4;
	} 
	if (bs_show(bs, 3) == 1) {
		bs_skip(bs, 3);
		return 3;
	} 

	return (3 - bs_get(bs, 2));
}



void decoder_iframe(DECODER * dec, BITSTREAM * bs, int quant)
{
	int16_t block[6][64];
	uint32_t x,y;

//printf("decoder_iframe 1\n");
	for (y = 0; y < dec->mb_height; y++)
	{
//printf("decoder_iframe 2\n");
		for (x = 0; x < dec->mb_width; x++)
		{
			MACROBLOCK * mb = &dec->mbs[y*dec->mb_width + x];

			uint32_t mcbpc;
			uint32_t cbpc;
			uint32_t acpred_flag;
			uint32_t cbpy;
			uint32_t cbp;
			uint32_t k;
			
//printf("decoder_iframe 3 %02x\n", bs_show(bs, 16));
			
			mcbpc = get_mcbpc_intra(bs);
			//DEBUGI2("(i,j)",x,y);
			mb->mode = mcbpc & 7;
			cbpc = (mcbpc >> 4);       // & 3; necessary?
//printf("decoder_iframe 4 %02x\n", bs_show(bs, 16));

			acpred_flag = bs_get1(bs);
			//DEBUGI("acpred_flag",acpred_flag);
//printf("decoder_iframe 3\n");

			if (mb->mode == MODE_STUFFING)
			{
				DEBUG("-------------STUFFING----------------");
				continue; // ???
			}
//printf("decoder_iframe 3\n");

			cbpy = get_cbpy(bs, 1);
			//DEBUGI("cbpy",cbpy);
			cbp = (cbpy << 2) | cbpc;
//printf("decoder_iframe 3\n");

			mb->quant = quant;
			if (mb->mode == MODE_INTRA_Q)
			{
				mb->quant += dquant_table[bs_get(bs,2)];
			}
//printf("decoder_iframe 3\n");
//printf("decoder_iframe 5 %02x\n", bs_show(bs, 16));

			for (k = 0; k < 6; k++)
			{
				int dc_size;
				int dc_dif;
				uint32_t dcscalar;
				const uint16_t * scan;
				
				int16_t data[64];

				bzero(block[k], 64 * sizeof(int16_t));		// clear

				// ac/dc prediction
				dcscalar = get_dc_scaler(mb->quant, k < 4);

				// get dc block
//printf("decoder_iframe %02x\n", dcscalar);

				if (k < 4)
				{
//printf("decoder_iframe 6 %02x\n", bs_show(bs, 16));
					dc_size = get_dc_size_lum(bs);
//printf("decoder_iframe 7 %02x\n", bs_show(bs, 16));
				}
				else
				{
					dc_size = get_dc_size_chrom(bs);
				}



				dc_dif = (dc_size ? get_dc_dif(bs, dc_size) : 0 );
				if (dc_size > 8)
				{
					bs_skip(bs, 1);		// marker
				}
				block[k][0] = dc_dif;
//printf("decoder_iframe 4\n");













				predict_dc(dec->mbs, x, y, dec->mb_width, k, block[k], dcscalar, mb->quant);

				// DEBUGI3("DC ", block[k][0], block[k][0], 0);

				if (!acpred_flag)
				{
					mb->acpred_directions[k] = 0;
				}
				scan = scan_tables[ mb->acpred_directions[k] ];
//printf("decoder_iframe 6\n");

				if (cbp & (1 << (5-k)))			// coded
				{
					int p;
					int level;
					int run;
					int last;

					p = 1;
//printf("decoder_iframe 6 %02x\n", bs_show(bs, 12));
					do
					{
						level = get_intra_coeff(bs, &run, &last);
//printf("decoder_iframe 6.1 %p %d %d\n", bs->pos, run, last);
//printf("decoder_iframe 6.2\n");
						//DEBUGI3("% ", level, run, last);
						if (run == -1)
						{
							DEBUG("invalid run");
						}
						p += run;
//printf("decoder_iframe 6.3 %d %d\n", level, p);
						block[k][scan[p]] = level;
						p++;







//printf("decoder_iframe 6.4 %d\n", level);











					} while (!last);

				}
//printf("decoder_iframe 7\n");

				predict_ac(dec->mbs, x, y, dec->mb_width, k, block[k], dcscalar, mb->quant);
//printf("decoder_iframe 8\n");

			
				DEQUANT_INTRA(data, block[k], mb->quant, dcscalar);
//printf("decoder_iframe 9\n");

				IDCT(data);
//printf("decoder_iframe 10\n");

				if (k < 4)
				{
					TRANSFER_16TO8COPY(dec->cur.y + (16*y*dec->edged_width) + 16*x + (4*(k&2)*dec->edged_width) + 8*(k&1), data, dec->edged_width);
				} 
				else if (k == 4)
				{
					TRANSFER_16TO8COPY(dec->cur.u+ 8*y*(dec->edged_width/2) + 8*x, data, (dec->edged_width/2));
				}
				else
				{
					TRANSFER_16TO8COPY(dec->cur.v + 8*y*(dec->edged_width/2) + 8*x, data, (dec->edged_width/2));
				}
			}
//printf("decoder_iframe 11 %02x\n", bs_show(bs, 16));

			// if (i == 5) return;
		}
	}
//printf("decoder_iframe 12\n");
	EMMS();
}



void decoder_pframe(DECODER * dec, BITSTREAM * bs, int quant, int fcode)
{
//	int16_t block[6][64];
	uint32_t x,y;

	for (y = 0; y < dec->mb_height; y++)
	{
		for (x = 0; x < dec->mb_width; x++)
		{
			MACROBLOCK * mb = &dec->mbs[y*dec->mb_width + x];

			if (!bs_get1(bs))			// not_coded
			{
				uint32_t mcbpc;
				uint32_t cbpc;
				uint32_t acpred_flag;
				uint32_t cbpy;
				uint32_t cbp;
				uint32_t k;
				uint32_t intra;


				mcbpc = get_mcbpc_inter(bs);
				mb->mode = mcbpc & 7;
				cbpc = (mcbpc >> 4);       // &3 necessary?

				intra = (mb->mode == MODE_INTRA || mb->mode == MODE_INTRA_Q);
				
				if (intra)
				{
					acpred_flag = bs_get1(bs);
				}

				if (mb->mode == MODE_STUFFING)
				{
					DEBUG("-------------STUFFING----------------");
					continue;
				}

				cbpy = get_cbpy(bs, intra);
				cbp = (cbpy << 2) | cbpc;

				mb->quant = quant;
				if (mb->mode == MODE_INTER_Q || mb->mode == MODE_INTRA_Q)
				{
					mb->quant += dquant_table[bs_get(bs,2)];
				}
				
				if (mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q)
				{
					mb->mvs[0].x = get_mv(bs, 0, fcode);
					mb->mvs[0].y = get_mv(bs, 0, fcode);
				}
				else if (mb->mode == MODE_INTER4V /* || mb->mode == MODE_INTER4V_Q */)
				{
					int z;
					for (z = 0; z < 4; z++)
					{
						mb->mvs[z].x = get_mv(bs, 0, fcode);
						mb->mvs[z].y = get_mv(bs, 0, fcode);
					}
				}
				else  // MODE_INTRA
				{
					for (k = 0; k < 4; k++)
					{
						mb->mvs[0].x = 0;
						mb->mvs[0].y = 0;
					}
				}

				// do inter/intra block
			}
			else
			{
				
			}
			// if (i == 5) return;
		}
	}
	EMMS();
}


int decoder_decode(DECODER * dec, DEC_FRAME * frame)
{
	BITSTREAM bs;
	uint32_t rounding;
	uint32_t quant;
	uint32_t fcode;

	bs_init(&bs, frame->bitstream);
	
        bs_vol(&bs, dec);

	switch (bs_vop(&bs, dec, &rounding, &quant, &fcode))
	{
	case P_VOP :
//		printf("P_VOP\n");
		decoder_pframe(dec, &bs, quant, fcode);
		break;

	case I_VOP :
//		printf("I_VOP\n");
		decoder_iframe(dec, &bs, quant);
		break;
	
	case N_VOP :
//		printf("N_VOP\n");		// vop not coded, 
		// set image cur=ref and proceed to colorspace
		break;

	default :
		printf("BAD_VOP\n");
		return DEC_FAIL;
	}

	DEBUGI("color",frame->colorspace);

	switch(frame->colorspace)
	{

	case DEC_CSP_RGB24 :		// force flip
		YV12toRGB24_generic(dec->cur.y, dec->edged_width,
		        dec->cur.u, dec->cur.v, dec->edged_width/2,
				frame->dst, dec->width, -(int32_t)dec->height,
				frame->stride);
		break;

	case DEC_CSP_RGB32 :		// force flip
		YV12toRGB32_generic(dec->cur.y, dec->edged_width,
		        dec->cur.u, dec->cur.v, dec->edged_width/2,
				frame->dst, dec->width, -(int32_t)dec->height,
				frame->stride);
		break;

	case DEC_CSP_YUY2 :
		yuv_to_yuyv(frame->dst, frame->stride, 
				dec->cur.y, dec->cur.u, dec->cur.v, dec->edged_width, 
				dec->width, dec->height);
		break;

	case DEC_CSP_UYVY :
		yuv_to_uyvy(frame->dst, frame->stride, 
				dec->cur.y, dec->cur.u, dec->cur.v, dec->edged_width, 
				dec->width, dec->height);
		break;

	case DEC_CSP_YV12 :
		YV12toYV12_generic(dec->cur.y, dec->edged_width,
		        dec->cur.u, dec->cur.v, dec->edged_width/2,
				frame->dst, dec->width, dec->height,
				frame->stride);
		break;

	default :
		return DEC_FAIL;
	}

	return DEC_OK;
}



				/* {
					uint32_t z;
					for (z = 0; z < 8; z++)
					{
						DEBUGI8("%", block[k][z*8], block[k][z*8 + 1], block[k][z*8 + 2], block[k][z*8 + 3],
								 block[k][z*8 + 4], block[k][z*8 + 5], block[k][z*8 + 6], block[k][z*8 + 7]);
					}
				} */
