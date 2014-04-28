#include "dec_portab.h"



void init_yuv2rgb();

void YV12toRGB32_generic(uint8_t *puc_y, int stride_y,
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
                unsigned int _stride_out);


void YV12toRGB24_generic(uint8_t *puc_y, int stride_y,
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
               unsigned int _stride_out);

void YV12toYV12_generic(uint8_t *puc_y, int stride_y,
                uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
                uint8_t *puc_out, int width_y, int height_y,
                unsigned int _stride_out);

void yuv_to_yuy2(uint8_t *puc_y, int stride_y,
  uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, int width_y, int height_y,
	unsigned int stride_out);


void yuv_to_yuyv(uint8_t *dst, int dst_stride,
				 uint8_t *y_src, uint8_t *v_src, uint8_t * u_src, int src_stride,
				 int width, int height);

void yuv_to_uyvy(uint8_t *dst, int dst_stride,
				 uint8_t *y_src, uint8_t *v_src, uint8_t * u_src, int src_stride,
				 int width, int height);



