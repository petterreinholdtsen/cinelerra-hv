#include <stdlib.h>

#include "dec_image.h"


#define SAFETY	64
#define EDGE_SIZE2  (EDGE_SIZE/2)

uint32_t image_create(IMAGE * image, uint32_t edged_width, uint32_t edged_height)
{
	const uint32_t edged_width2 = edged_width / 2;
	const uint32_t edged_height2 = edged_height / 2;
	// const uint32_t edge_size2 = EDGE_SIZE / 2;

	/* image->plane[_Y_] = malloc(edged_width * edged_height + SAFETY);
	if (image->plane[_Y_] == NULL)
	{
		return -1;
	}
	
	image->plane[_U_] = malloc(edged_width2 * edged_height2 + SAFETY);
	if (image->plane[_U_])
	{
		free(image->plane[_Y_]);
		return -1;
	}
	image->plane[_V_] = malloc(edged_width2 * edged_height2 + SAFETY);
	if (image->plane[_V_])
	{
		free(image->plane[_U_]);
		free(image->plane[_Y_]);
		return -1;
	}	*/

	//image->plane[_Y_] += EDGE_SIZE * edged_width + EDGE_SIZE;
	//image->plane[_U_] += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
	//image->plane[_V_] += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;


	image->y = malloc(edged_width * edged_height + SAFETY);
	image->u = malloc(edged_width2 * edged_height2 + SAFETY);
	image->v = malloc(edged_width2 * edged_height2 + SAFETY);
	
	image->y += EDGE_SIZE * edged_width + EDGE_SIZE;
	image->u += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
	image->v += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;

	return 0;
}


void image_destroy(IMAGE * image, uint32_t edged_width, uint32_t edged_height)
{
	const uint32_t edged_width2 = edged_width / 2;
	const uint32_t edged_height2 = edged_height / 2;
	const uint32_t edge_size2 = EDGE_SIZE / 2;

	//free(image->plane[_V_] - (edge_size2 * edged_width2 + edge_size2));
	//free(image->plane[_U_] - (edge_size2 * edged_width2 + edge_size2));
	//free(image->plane[_Y_] - (EDGE_SIZE * edged_width + EDGE_SIZE));

	free(image->y - (EDGE_SIZE * edged_width + EDGE_SIZE) );
	free(image->u - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
	free(image->v - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
}
