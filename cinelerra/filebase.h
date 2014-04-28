#ifndef FILEBASE_H
#define FILEBASE_H

#include "assets.inc"
#include "colormodels.h"
#include "edit.inc"
#include "guicast.h"
#include "file.inc"
#include "filelist.inc"
#include "overlayframe.inc"
#include "pluginbuffer.inc"
#include "strategies.inc"
#include "vframe.inc"

#include <sys/types.h>

// inherited by every file interpreter
class FileBase
{
public:
	FileBase(Asset *asset, File *file);
	virtual ~FileBase();


	friend class File;
	friend class FileList;
	friend class FrameWriter;




	int get_mode(char *mode, int rd, int wr);
	int reset_parameters();
	virtual int check_header() { return 0; };  // Test file to see if it is of this type.
	virtual int reset_parameters_derived() {};
	virtual int read_header() {};     // WAV files for getting header
	virtual int open_file(int rd, int wr) {};
	virtual int close_file();
	virtual int close_file_derived() {};
	int set_dither();
	virtual int seek_end() { return 0; };
	virtual int seek_start() { return 0; };
	virtual long get_video_position() { return 0; };
	virtual long get_audio_position() { return 0; };
	virtual int set_video_position(long x) { return 0; };
	virtual int set_audio_position(long x) { return 0; };
	virtual long get_memory_usage() { return 0; };
	virtual int write_samples(double **buffer, 
		long len) { return 0; };
	virtual int write_frames(VFrame ***frames, int len) { return 0; };
	virtual int read_compressed_frame(VFrame *buffer) { return 0; };
	virtual int write_compressed_frame(VFrame *buffers) { return 0; };
	virtual long compressed_frame_size() { return 0; };
// Doubles are used to allow resampling
	virtual int read_samples(double *buffer, long len) { return 0; };

	virtual int read_frame(VFrame *frame) { return 1; };

// Return either the argument or another colormodel which read_frame should
// use.
	virtual int colormodel_supported(int colormodel) { return BC_RGB888; };
// This file can copy compressed frames directly from the asset
	virtual int can_copy_from(Edit *edit, long position) { return 0; }; 
	virtual int get_render_strategy(ArrayList<int>* render_strategies) { return VRENDER_VPIXEL; };

protected:
// Return 1 if the render_strategy is present on the list.
	static int search_render_strategies(ArrayList<int>* render_strategies, int render_strategy);

// overlay on the frame with scaling
// Alpha values are from 0 to VMAX
	int transfer_from(VFrame *frame_out, VFrame *frame_in, 
		float in_x1, float in_y1, float in_x2, float in_y2,
		float out_x1, float out_y1, float out_x2, float out_y2, 
		int alpha, int use_alpha, int use_float, int interpolate, 
		int mode);

// convert samples into file format
	long samples_to_raw(char *out_buffer, 
							float **in_buffer, // was **buffer
							long input_len, 
							int bits, 
							int channels,
							int byte_order,
							int signed_);

// overwrites the buffer from PCM data depending on feather.
	int raw_to_samples(float *out_buffer, char *in_buffer, 
		long samples, int bits, int channels, int channel, int feather, 
		float lfeather_len, float lfeather_gain, float lfeather_slope);

// Overwrite the buffer from float data using feather.
	int overlay_float_buffer(float *out_buffer, float *in_buffer, 
		long samples, 
		float lfeather_len, float lfeather_gain, float lfeather_slope);

// convert a frame to and from file format
	long raw_to_frame(unsigned char *in_buffer,
					VFrame *out_frame, 
					int in_x1, int in_y1, int in_x2, int in_y2,
					int out_x1, int out_y1, int out_x2, int out_y2, 
					int alpha, int use_alpha, int use_float, int color_model);

	long frame_to_raw(unsigned char *out_buffer,
					VFrame *in_frame,
					int w,
					int h,
					int use_alpha,
					int use_float,
					int color_model);

// allocate a buffer for translating int to float
	int get_audio_buffer(char **buffer, long len, long bits, long channels); // audio

// Allocate a buffer for feathering floats
	int get_float_buffer(float **buffer, long len);

// allocate a buffer for translating video to VFrame
	int get_video_buffer(unsigned char **buffer, int depth); // video
	int get_row_pointers(unsigned char *buffer, unsigned char ***pointers, int depth);
	static int match4(char *in, char *out);   // match 4 bytes for a quicktime type

	long ima4_samples_to_bytes(long samples, int channels);
	long ima4_bytes_to_samples(long bytes, int channels);

	char *audio_buffer_in, *audio_buffer_out;    // for raw audio reads and writes
	float *float_buffer;          // for floating point feathering
	unsigned char *video_buffer_in, *video_buffer_out;
	unsigned char **row_pointers_in, **row_pointers_out;
	long prev_buffer_position;  // for audio determines if reading raw data is necessary
	long prev_frame_position;   // for video determines if reading raw video data is necessary
	long prev_bytes; // determines if new raw buffer is needed and used for getting memory usage
	long prev_len;
	int prev_track;
	int prev_layer;
	Asset *asset;
	int wr, rd;
	int dither;
	int internal_byte_order;
	File *file;

private:
// ==================================== video compression (broken)
	int get_bytes_per_pixel(int color_model);

// Import a row with no scaling
	int raw_to_row_direct(unsigned char *in_buffer, VPixel *out_row, 
		int w, int alpha, int use_alpha, int use_float, int color_model);
// Import a row with scaling
	int raw_to_row_scale(unsigned char *input, VPixel *output, 
		int w, int *column_table, int alpha, int use_alpha, int use_float, int color_model);

	int raw_to_pixel_alpha(unsigned char *input, VPixel *output, float a, float output_opacity);
	int raw_to_pixel_alpha(unsigned char *input, VPixel *output, int a, int output_opacity);
	int raw_to_pixel(unsigned char *input, VPixel *output);
	int grey_to_pixel_alpha(unsigned char *input, VPixel *output, float a, float output_opacity);
	int grey_to_pixel_alpha(unsigned char *input, VPixel *output, int a, int output_opacity);
	int grey_to_pixel(unsigned char *input, VPixel *output);
	int yuv_to_pixel_alpha(unsigned char *input, VPixel *output, int pixel, float a, float output_opacity);
	int yuv_to_pixel_alpha(unsigned char *input, VPixel *output, int pixel, int a, int output_opacity);
	int yuv_to_pixel_float(unsigned char *input, VPixel *output, int pixel);
	int yuv_to_pixel_int(unsigned char *input, VPixel *output, int pixel);

	int row_to_raw(unsigned char *out_buffer, VPixel *in_row, 
		int w, int use_alpha, int use_float, int color_model);
	int pixel_to_raw_alpha_float(VPixel *input, unsigned char *output);
	int pixel_to_raw_alpha_int(VPixel *input, unsigned char *output);
	int pixel_to_raw(VPixel *input, unsigned char *output);
	int pixel_to_rgba_alpha_float(VPixel *input, unsigned char *output);
	int pixel_to_rgba_alpha_int(VPixel *input, unsigned char *output);
	int pixel_to_rgba(VPixel *input, unsigned char *output);
	int pixel_to_yuv_alpha_float(VPixel *input, unsigned char *output);
	int pixel_to_yuv_alpha_int(VPixel *input, unsigned char *output);
	int pixel_to_yuv_float(VPixel *input, unsigned char *output);
	int pixel_to_yuv_int(VPixel *input, unsigned char *output);

// ================================= Audio compression
// ULAW
	float ulawtofloat(char ulaw);
	char floattoulaw(float value);
	int generate_ulaw_tables();
	int delete_ulaw_tables();
	float *ulawtofloat_table, *ulawtofloat_ptr;
	unsigned char *floattoulaw_table, *floattoulaw_ptr;

// IMA4
	int init_ima4();
	int delete_ima4();
	int ima4_decode_block(int16_t *output, unsigned char *input);
	int ima4_decode_sample(int *predictor, int nibble, int *index, int *step);
	int ima4_encode_block(unsigned char *output, int16_t *input, int step, int channel);
	int ima4_encode_sample(int *last_sample, int *last_index, int *nibble, int next_sample);

	static int ima4_step[89];
	static int ima4_index[16];
	int *last_ima4_samples;
	int *last_ima4_indexes;
	int ima4_block_size;
	int ima4_block_samples;
	OverlayFrame *overlayer;
};

#endif
