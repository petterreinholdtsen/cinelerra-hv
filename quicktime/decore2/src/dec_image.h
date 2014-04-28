#ifndef DEC_IMAGE_H_
#define DEC_IMAGE_H_

#include "dec_portab.h"

#define EDGE_SIZE  32


typedef struct
{
	uint8_t * y;
	uint8_t * u;
	uint8_t * v;
} IMAGE;


uint32_t image_create(IMAGE * image, uint32_t edged_width, uint32_t edged_height);
void image_destroy(IMAGE * image, uint32_t edged_width, uint32_t edged_height);


#endif /* _IMAGE_H_ */