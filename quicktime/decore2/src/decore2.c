#include "decore2.h"
#include "decoder.h"


int decore(void * handle, int dec_opt, void * param1, void * param2)
{
	DECODER * dec = (DECODER *) handle;
	DEBUGI("decore", dec_opt);

	switch (dec_opt)
	{
	    case DEC_OPT_CREATE:
		return decoder_create((DEC_PARAM *) param1);
	
		case DEC_OPT_DESTROY:
		return decoder_destroy(dec);

	    case DEC_OPT_DECODE:
		return decoder_decode(dec, (DEC_FRAME *) param1); 

		default:
		return DEC_FAIL;
    }
}