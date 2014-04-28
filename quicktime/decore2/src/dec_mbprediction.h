#include "dec_portab.h"
#include "decoder.h"

void predict_dc(MACROBLOCK *pMBs,
				uint32_t x, uint32_t y,	uint32_t x_dim, 
				uint32_t block, 
				int16_t dct_codes[64],
				uint32_t iDcScaler, 
				uint32_t current_quant);


void predict_ac(MACROBLOCK *pMBs,
				uint32_t x, uint32_t y,	uint32_t mb_width, 
				uint32_t block, 
				int16_t dct_codes[64],
				uint32_t iDcScaler, 
				uint32_t current_quant);


void predict_store(MACROBLOCK *pMBs,
				uint32_t x, uint32_t y,	uint32_t mb_width, 
				uint32_t block, 
				int16_t dct_codes[64],
				uint32_t iDcScaler, 
				uint32_t current_quant);
