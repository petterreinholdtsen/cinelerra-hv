#include "decoder.h"
#include "dec_mbprediction.h"



#define MIN(X, Y) ((X)<(Y)?(X):(Y))
#define MAX(X, Y) ((X)>(Y)?(X):(Y))

#define _div_div(a, b) (a>0) ? (a+(b>>1))/b : (a-(b>>1))/b

static int __inline rescale(int predict_quant, int current_quant, int coeff)
{
	return (coeff != 0) ? _div_div((coeff) * (predict_quant), (current_quant)) : 0;
}


static const int16_t default_acdc_values[15] = { 
	1024,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};





/*
  perform dc/ac prediction on a single block

  returns how much is saved by performing ac prediction

	[  diag  ] [   top   ]
	[  left  ] [ current ]


  NOTE: modified slightly, since we must perform dc prediction,
  coeff decoding, _then_ ac predicition.
*/


void predict_dc(MACROBLOCK *pMBs,
				uint32_t x, uint32_t y,	uint32_t mb_width, 
				uint32_t block, 
				int16_t dct_codes[64],
				uint32_t iDcScaler, 
				uint32_t current_quant)
{
    int16_t *left, *top, *diag, *current;

    int32_t left_quant = current_quant;
    int32_t top_quant = current_quant;

    const int16_t *pLeft = default_acdc_values;
    const int16_t *pTop = default_acdc_values;
    const int16_t *pDiag = default_acdc_values;

    int16_t *pCurrent;
    int32_t S1 = 0, S2 = 0;
    uint32_t index = x + y * mb_width;		// current macroblock
    int16_t dc_pred;
    uint8_t *acpred_direction = &pMBs[index].acpred_directions[block];
	uint32_t i;

	left = top = diag = current = 0;

	// grab left,top and diag macroblocks

	// left macroblock 

    if(x && (pMBs[index - 1].mode == MODE_INTRA 
		|| pMBs[index - 1].mode == MODE_INTRA_Q)) {

		left = pMBs[index - 1].pred_values[0];
		left_quant = pMBs[index - 1].quant;
		//DEBUGI("LEFT", *(left+MBPRED_SIZE));
	}
    
	// top macroblock
	
	if(y && (pMBs[index - mb_width].mode == MODE_INTRA 
		|| pMBs[index - mb_width].mode == MODE_INTRA_Q)) {

		top = pMBs[index - mb_width].pred_values[0];
		top_quant = pMBs[index - mb_width].quant;
		//DEBUG("TOP");
    }
    
	// diag macroblock 
	
	if(x && y && (pMBs[index - 1 - mb_width].mode == MODE_INTRA 
		|| pMBs[index - 1 - mb_width].mode == MODE_INTRA_Q)) {

		diag = pMBs[index - 1 - mb_width].pred_values[0];
		//DEBUG("DIAG");
	}

    current = pMBs[index].pred_values[0];
	pCurrent = current + block * MBPRED_SIZE;
    

	// now grab pLeft, pTop, pDiag _blocks_ 
	
	switch (block) {
	
	case 0: 
		if(left)
			pLeft = left + MBPRED_SIZE;
		
		if(top)
			pTop = top + (MBPRED_SIZE << 1);
		
		if(diag)
			pDiag = diag + 3 * MBPRED_SIZE;
		
		break;
	
	case 1:
		pLeft = current;
		left_quant = current_quant;
	//	pLeft[8] = 0;
		
		//DEBUG("--BLOCK1--");
		//DEBUGI2("pLeft[0],q:", pLeft[0], left_quant);
		//DEBUGI8("pLeft[1-7]:", pLeft[1], pLeft[2], pLeft[3], pLeft[4], pLeft[5], pLeft[6], pLeft[7], 12345);
		//DEBUGI8("pLeft[8-14]:", pLeft[8], pLeft[9], pLeft[10], pLeft[11], pLeft[12], pLeft[13], pLeft[14], 12345);
		
		
		if(top) {
			pTop = top + 3 * MBPRED_SIZE;
			pDiag = top + (MBPRED_SIZE << 1);
		}
		break;
	
	case 2:
		if(left) {
			pLeft = left + 3 * MBPRED_SIZE;
			pDiag = left + MBPRED_SIZE;
		}
		
		pTop = current;
		top_quant = current_quant;

		break;
	
	case 3:
		pLeft = current + (MBPRED_SIZE << 1);
		left_quant = current_quant;
		
		pTop = current + MBPRED_SIZE;
		top_quant = current_quant;
		
		pDiag = current;
		
		break;
	
	case 4:
		if(left)
			pLeft = left + (MBPRED_SIZE << 2);
		
		if(top)
			pTop = top + (MBPRED_SIZE << 2);
		
		if(diag)
			pDiag = diag + (MBPRED_SIZE << 2);
		
		break;
	
	case 5:
		if(left)
			pLeft = left + 5 * MBPRED_SIZE;
		if(top)
			pTop = top + 5 * MBPRED_SIZE;
		if(diag)
			pDiag = diag + 5 * MBPRED_SIZE;
		break;
	}

    
    //	determine ac prediction direction & ac/dc predictor
	// dc prediction is placed in dc_pred
	// ac predictions are  placed in pCurrent[] (for use later)

    if(abs(pLeft[0] - pDiag[0]) < abs(pDiag[0] - pTop[0])) {
		*acpred_direction = 1;             // vertical
		dc_pred = _div_div(pTop[0], iDcScaler);
		for (i = 1; i < 8; i++)
		{
			pCurrent[i] = rescale(top_quant, current_quant, pTop[i]);
		}
		//DEBUG("VERT");
	}
	else 
	{
		*acpred_direction = 2;             // horizontal
		//DEBUGI3("! ", dct_codes[0], pLeft[0], iDcScaler);
		//DEBUGI3("(i,j.k)", x, y, block);
		dc_pred = _div_div(pLeft[0], iDcScaler);
		for (i = 1; i < 8; i++)
		{
			pCurrent[i+7] = rescale(left_quant, current_quant, pLeft[i + 7]);
		}
		//DEBUGI2("left_quant, current_quant", left_quant, current_quant);
		//DEBUGI8("pCur[8-14]:", pCurrent[8], pCurrent[9], pCurrent[10], pCurrent[11], pCurrent[12], pCurrent[13], pCurrent[14], 12345);
		
		//DEBUG("HORIZ");
	}
	dct_codes[0] = (dct_codes[0] + dc_pred); // * iDcScaler; 
	//  DO THIS LATER ;
   
	
	// DEBUGI("dcp", dc_pred);
	
	// apply prediction
	/* dct_codes[0] = (dct_codes[0] + dc_pred) * iDcScaler;
	if (*acpred_direction == 1)
	{
		for (i = 1; i < 8; i++)
		{
			dct_codes[i] = _rescale(top_quant, current_quant, pTop[i]);
		}
	}
    else
    {
		for (i = 1; i < 8; i++)
		{
			dct_codes[i*8] = _rescale(left_quant, current_quant,pLeft[i + 7]);
		}
    } */

	// backup final coeffs
	
}



// dc prediction & backup

void predict_ac(MACROBLOCK *pMBs,
				uint32_t x, uint32_t y,	uint32_t mb_width, 
				uint32_t block, 
				int16_t dct_codes[64],
				uint32_t iDcScaler, 
				uint32_t current_quant)
{
    int16_t *current;
    int16_t *pCurrent;
    uint32_t index = x + y * mb_width;		// current macroblock
	uint32_t i;

	uint8_t acpred_direction = pMBs[index].acpred_directions[block];

	current = pMBs[index].pred_values[0];
	pCurrent = current + block * MBPRED_SIZE;


	pCurrent[0] = dct_codes[0] * iDcScaler;
	if (acpred_direction == 1)
	{
		for (i = 1; i < 8; i++)
		{
			int level = dct_codes[i] + pCurrent[i];
			dct_codes[i] = level;
			pCurrent[i] = level;
			pCurrent[i+7] = dct_codes[i*8];

		}
	}
	else if (acpred_direction == 2)
	{
		for (i = 1; i < 8; i++)
		{
			int level = dct_codes[i*8] + pCurrent[i+7];
			dct_codes[i*8] = level;
			pCurrent[i+7] = level;
			pCurrent[i] = dct_codes[i];
		}
	}
	else
	{
		//DEBUG("X");
		for (i = 1; i < 8; i++)
		{
			pCurrent[i] = dct_codes[i];
			pCurrent[i+7] = dct_codes[i*8];
		}
	}
}




// --------------------------------------------------------------
// motion preidiction stuff

/* calculate the pmv (predicted motion vector)
	(take the median of surrounding motion vectors)
	
	(x,y) = the macroblock
	block = the block within the macroblock
*/
void get_pmv(const MACROBLOCK * const pMBs,
							const uint32_t x, const uint32_t y,
							const uint32_t x_dim,
							const uint32_t block,
							int32_t * const pred_x, int32_t * const pred_y)
/*             
    PLEASE NOTE:
    This function has been duplicated in MBMotionEstComp and MBPrediction
    to enforce modularity
*/    
{
    int x1, x2, x3;
	int y1, y2, y3;
    int xin1, xin2, xin3;
    int yin1, yin2, yin3;
    int vec1, vec2, vec3;

    uint32_t index = x + y * x_dim;

	// first row (special case)
    if (y == 0 && (block == 0 || block == 1))
    {
		if (x == 0 && block == 0)		// first column
		{
			*pred_x = 0;
			*pred_y = 0;
			return;
		}
		if (block == 1)
		{
			VECTOR mv = pMBs[index].mvs[0];
			*pred_x = mv.x;
			*pred_y = mv.y;
			return;
		}
		// else
		{
			VECTOR mv = pMBs[index - 1].mvs[1];
			*pred_x = mv.x;
			*pred_y = mv.y;
			return;
		}
    }

	/*
		MODE_INTER, vm18 page 48
		MODE_INTER4V vm18 page 51

					(x,y-1)		(x+1,y-1)
					[   |   ]	[	|   ]
					[ 2 | 3 ]	[ 2 |   ]

		(x-1,y)		(x,y)		(x+1,y)
		[   | 1 ]	[ 0 | 1 ]	[ 0 |   ]
		[   | 3 ]	[ 2 | 3 ]	[	|   ]
	*/

    switch (block)
    {
	case 0:
		xin1 = x - 1;	yin1 = y;		vec1 = 1;
		xin2 = x;		yin2 = y - 1;	vec2 = 2;
		xin3 = x + 1;	yin3 = y - 1;	vec3 = 2;
		break;
	case 1:
		xin1 = x;		yin1 = y;		vec1 = 0;
		xin2 = x;		yin2 = y - 1;   vec2 = 3;
		xin3 = x + 1;	yin3 = y - 1;	vec3 = 2;
	    break;
	case 2:
		xin1 = x - 1;	yin1 = y;		vec1 = 3;
		xin2 = x;		yin2 = y;		vec2 = 0;
		xin3 = x;		yin3 = y;		vec3 = 1;
	    break;
	default:
		xin1 = x;		yin1 = y;		vec1 = 2;
		xin2 = x;		yin2 = y;		vec2 = 0;
		xin3 = x;		yin3 = y;		vec3 = 1;
    }


	if (xin1 < 0 || /* yin1 < 0  || */ xin1 >= (int32_t)x_dim)
	{
	    x1 = 0;
		y1 = 0;
	}
	else
	{
		const VECTOR * const mv = &(pMBs[xin1 + yin1 * x_dim].mvs[vec1]); 
		x1 = mv->x;
		y1 = mv->y;
	}

	if (xin2 < 0 || /* yin2 < 0 || */ xin2 >= (int32_t)x_dim)
	{
		x2 = 0;
		y2 = 0;
	}
	else
	{
		const VECTOR * const mv = &(pMBs[xin2 + yin2 * x_dim].mvs[vec2]); 
		x2 = mv->x;
		y2 = mv->y;
	}

	if (xin3 < 0 || /* yin3 < 0 || */ xin3 >= (int32_t)x_dim)
	{
	    x3 = 0;
		y3 = 0;
	}
	else
	{
		const VECTOR * const mv = &(pMBs[xin3 + yin3 * x_dim].mvs[vec3]); 
		x3 = mv->x;
		y3 = mv->y;
	}

	// median

	*pred_x = MIN(MAX(x1, x2), MIN(MAX(x2, x3), MAX(x1, x3)));
	*pred_y = MIN(MAX(y1, y2), MIN(MAX(y2, y3), MAX(y1, y3)));
}

void predict_motion(MACROBLOCK * mbs, uint32_t i, uint32_t j, uint32_t mb_width)
{
    MACROBLOCK * mb = &mbs[j*mb_width + i];

	//....
}