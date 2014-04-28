#ifndef _DECORE2_H_
#define _DECORE2_H_

#ifdef __cplusplus
extern "C" {
#endif 

// options
#define DEC_OPT_DECODE	0
#define DEC_OPT_CREATE	1
#define DEC_OPT_DESTROY	2

// return values
#define DEC_FAIL		-1
#define DEC_OK			0
#define	DEC_MEMORY		1
#define DEC_BAD_FORMAT		2

// colorspaces
#define DEC_CSP_RGB24 	0
#define DEC_CSP_YV12	1
#define DEC_CSP_YUY2	2
#define DEC_CSP_UYVY	3
#define DEC_CSP_I420	4
#define DEC_CSP_RGB32 	1000
#define DEC_CSP_YVYU	1002

typedef struct 
{
	int width;
	int height;
	void *handle;		// will be filled by encore
} DEC_PARAM;


typedef struct
{
	void * bitstream;
	int length;
	void * dst;
	int colorspace;
	int stride;
	int render;
} DEC_FRAME;


int decore(void * handle,
		int dec_opt,
		void *param1,
		void *param2);


#ifdef __cplusplus
}
#endif 


#endif /* _DECORE2_H_ */