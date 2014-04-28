
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include "asset.h" 
extern "C" 
{
#include "avcodec.h"
#include "avformat.h"
}
#include "bcsignals.h"
#include "clip.h"
#include "file.h"
#include "fileffmpeg.h"
#include "mutex.h"
#include <unistd.h>
#include "videodevice.inc"

#include <string.h>

Mutex* FileFFMPEG::ffmpeg_lock = new Mutex("FileFFMPEG::ffmpeg_lock");

FileFFMPEG::FileFFMPEG(Asset *asset, File *file)
 : FileBase(asset, file)
{
	reset();
	if(asset->format == FILE_UNKNOWN)
		asset->format = FILE_FFMPEG;
}

FileFFMPEG::~FileFFMPEG()
{
	close_file();
}

void FileFFMPEG::reset()
{
	ffmpeg_file_context = 0;
	ffmpeg_format = 0;
	ffmpeg_frame = 0;
	ffmpeg_samples = 0;
	audio_index = -1;
	video_index = -1;
	current_frame = 0;
	current_sample = 0;
	decoded_frame = 0;
	decoded_sample = 0;
	first_frame = 1;
}

char* FileFFMPEG::get_format_string(Asset *asset)
{
	unsigned char test[16];
	const int debug = 0;
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);
	FILE *in = fopen(asset->path, "r");
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);
	char *format_string = 0;
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);

	if(in)
	{
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);
		fread(test, sizeof(test), 1, in);
// printf("FileFFMPEG %d %02x %02x %02x %02x \n", 
// __LINE__, 
// test[0], 
// test[1], 
// test[2], 
// test[3]);
// Matroska
		if(test[0] == 0x1a &&
			test[1] == 0x45 &&
			test[2] == 0xdf &&
			test[3] == 0xa3)
		{
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);
			format_string = "matroska";
		}
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);

		fclose(in);
if(debug) printf("FileFFMPEG::get_format_string %d\n", __LINE__);
		return format_string;
	}

	return 0;
}

int FileFFMPEG::check_sig(Asset *asset)
{
	ffmpeg_lock->lock("FileFFMPEG::check_sig");
	avcodec_init();
    avcodec_register_all();
    av_register_all();
	
	AVFormatContext *ffmpeg_file_context = 0;
    AVFormatParameters params;
	bzero(&params, sizeof(params));
	int result = av_open_input_file(
		&ffmpeg_file_context, 
		asset->path, 
		0, 
		0, 
		&params);

	if(result >= 0)
	{
//printf("FileFFMPEG::check_sig %d result=%d\n", __LINE__, result);
		av_close_input_file(ffmpeg_file_context);
		ffmpeg_lock->unlock();
		return 1;
	}
	else
	{
		ffmpeg_lock->unlock();
		return 0;
	}
	
	
// 	char *format_string = get_format_string(asset);
// 	AVInputFormat *ffmpeg_format = 0;
// 
// 
// 	if(format_string)
// 	{
//     	ffmpeg_format = av_find_input_format(format_string);
// // printf("FileFFMPEG::check_sig path=%s ffmpeg_format=%p\n",
// // asset->path,
// // ffmpeg_format);
// 	}
// 
// 	ffmpeg_lock->unlock();
// 	if(!ffmpeg_format) 
// 		return 0;
// 	else
// 		return 1;
}

int FileFFMPEG::open_file(int rd, int wr)
{
	const int debug = 0;
	int result = 0;
    AVFormatParameters params;
	bzero(&params, sizeof(params));

	ffmpeg_lock->lock("FileFFMPEG::open_file");
	avcodec_init();
    avcodec_register_all();
    av_register_all();

	if(rd)
	{
// 		char *format_string = get_format_string(asset);
// if(debug) printf("FileFFMPEG::open_file format_string=%s\n", format_string);
// 		if(format_string)
// 		{
// 			ffmpeg_format = av_find_input_format(format_string);
// 		}
// 		else
// 		{
// 			ffmpeg_lock->unlock();
// 			return 1;
// 		}


// if(debug) printf("FileFFMPEG::open_file ffmpeg_format=%p\n", ffmpeg_format);
// 		if(ffmpeg_format)
// 		{
// 			ffmpeg_format = 0;
			result = av_open_input_file(
				(AVFormatContext**)&ffmpeg_file_context, 
				asset->path, 
//				(AVInputFormat*)ffmpeg_format, 
				0,
				0, 
				&params);
// 		}
// 		else
// 		{
// 			ffmpeg_lock->unlock();
// 			return 1;
// 		}

if(debug) printf("FileFFMPEG::open_file %d result=%d\n", __LINE__, result);

		if(result >= 0)
		{
			result = av_find_stream_info((AVFormatContext*)ffmpeg_file_context);
		}
		else
		{
			ffmpeg_lock->unlock();
			return 1;
		}
if(debug) printf("FileFFMPEG::open_file %d result=%d\n", __LINE__, result);

// Convert format to asset
		if(result >= 0)
		{
			result = 0;
			asset->format = FILE_FFMPEG;
			for(int i = 0; i < ((AVFormatContext*)ffmpeg_file_context)->nb_streams; i++)
			{
				AVStream *stream = ((AVFormatContext*)ffmpeg_file_context)->streams[i];
       			AVCodecContext *decoder_context = stream->codec;
        		switch(decoder_context->codec_type) 
				{
        			case CODEC_TYPE_AUDIO:
if(debug) printf("FileFFMPEG::open_file %d i=%d audio_index=%d\n", __LINE__, i, audio_index);
            			if(audio_index < 0)
						{
							audio_index = i;
							asset->audio_data = 1;
							asset->channels = decoder_context->channels;
							asset->sample_rate = decoder_context->sample_rate;
							asset->audio_length = (int64_t)(((AVFormatContext*)ffmpeg_file_context)->duration * 
								asset->sample_rate / 
								AV_TIME_BASE);
							asset->bits = 16;
if(debug) printf("FileFFMPEG::open_file %d decoder_context->codec_id=%d\n", __LINE__, decoder_context->codec_id);
							AVCodec *codec = avcodec_find_decoder(decoder_context->codec_id);
							if(!codec)
							{
								printf("FileFFMPEG::open_file: audio codec 0x%x not found.\n", 
									decoder_context->codec_id);
								asset->audio_data = 0;
								audio_index = -1;
							}
							else
							{
								avcodec_thread_init(decoder_context, file->cpus);
								avcodec_open(decoder_context, codec);
							}
						}
            			break;

        			case CODEC_TYPE_VIDEO:
if(debug) printf("FileFFMPEG::open_file %d i=%d video_index=%d\n", __LINE__, i, video_index);
            			if(video_index < 0)
						{
							video_index = i;
							asset->video_data = 1;
							asset->layers = 1;
							asset->width = decoder_context->width;
							asset->height = decoder_context->height;
							asset->frame_rate = 
								(double)stream->r_frame_rate.num /
								stream->r_frame_rate.den;
// 								(double)decoder_context->time_base.den / 
// 								decoder_context->time_base.num;
							asset->video_length = (int64_t)(((AVFormatContext*)ffmpeg_file_context)->duration *
								asset->frame_rate / 
								AV_TIME_BASE);
							asset->aspect_ratio = 
								(double)decoder_context->sample_aspect_ratio.num / 
								decoder_context->sample_aspect_ratio.den;
if(debug) printf("FileFFMPEG::open_file %d decoder_context->codec_id=%d\n", 
__LINE__, 
decoder_context->codec_id);
							AVCodec *codec = avcodec_find_decoder(decoder_context->codec_id);
							avcodec_thread_init(decoder_context, file->cpus);
							avcodec_open(decoder_context, codec);
						}
            			break;

        			default:
            			break;
        		}
			}

			if(debug) 
			{
				printf("FileFFMPEG::open_file %d audio_index=%d video_index=%d\n",
					__LINE__,
					audio_index,
					video_index);
				asset->dump();
			}
		}
		else
		{
			ffmpeg_lock->unlock();
			return 1;
		}
	}

	ffmpeg_lock->unlock();
	return result;
}

int FileFFMPEG::close_file()
{
	ffmpeg_lock->lock("FileFFMPEG::close_file");
	if(ffmpeg_file_context)
		av_close_input_file((AVFormatContext*)ffmpeg_file_context);
	if(ffmpeg_frame) av_free(ffmpeg_frame);
	if(ffmpeg_samples) free(ffmpeg_samples);
	ffmpeg_file_context = 0;
	reset();
	ffmpeg_lock->unlock();
}


int64_t FileFFMPEG::get_memory_usage()
{
	return 0;
}


int FileFFMPEG::colormodel_supported(int colormodel)
{
	return colormodel;
}

int FileFFMPEG::get_best_colormodel(Asset *asset, int driver)
{
	switch(driver)
	{
		case PLAYBACK_X11:
			return BC_RGB888;
		case PLAYBACK_X11_XV:
		case PLAYBACK_ASYNCHRONOUS:
			return BC_YUV420P;
		case PLAYBACK_X11_GL:
			return BC_YUV888;
		default:
			return BC_YUV420P;
	}
}

void FileFFMPEG::dump_context(void *ptr)
{
	AVCodecContext *context = (AVCodecContext*)ptr;

	printf("FileFFMPEG::dump_context %d\n", __LINE__);
	printf("    bit_rate=%d\n", context->bit_rate);
	printf("    bit_rate_tolerance=%d\n", context->bit_rate_tolerance);
	printf("    flags=%d\n", context->flags);
	printf("    sub_id=%d\n", context->sub_id);
	printf("    me_method=%d\n", context->me_method);
	printf("    extradata_size=%d\n", context->extradata_size);
	printf("    time_base.num=%d\n", context->time_base.num);
	printf("    time_base.den=%d\n", context->time_base.den);
	printf("    width=%d\n", context->width);
	printf("    height=%d\n", context->height);
	printf("    gop_size=%d\n", context->gop_size);
	printf("    pix_fmt=%d\n", context->pix_fmt);
	printf("    rate_emu=%d\n", context->rate_emu);
	printf("    sample_rate=%d\n", context->sample_rate);
	printf("    channels=%d\n", context->channels);
	printf("    sample_fmt=%d\n", context->sample_fmt);
	printf("    frame_size=%d\n", context->frame_size);
	printf("    frame_number=%d\n", context->frame_number);
	printf("    real_pict_num=%d\n", context->real_pict_num);
	printf("    delay=%d\n", context->delay);
	printf("    qcompress=%d\n", context->qcompress);
	printf("    qblur=%d\n", context->qblur);
	printf("    qmin=%d\n", context->qmin);
	printf("    qmax=%d\n", context->qmax);
	printf("    max_qdiff=%d\n", context->max_qdiff);
	printf("    max_b_frames=%d\n", context->max_b_frames);
	printf("    b_quant_factor=%d\n", context->b_quant_factor);
	printf("    b_frame_strategy=%d\n", context->b_frame_strategy);
	printf("    hurry_up=%d\n", context->hurry_up);
	printf("    rtp_payload_size=%d\n", context->rtp_payload_size);
	printf("    codec_id=%d\n", context->codec_id);
	printf("    codec_tag=%d\n", context->codec_tag);
	printf("    workaround_bugs=%d\n", context->workaround_bugs);
	printf("    error_resilience=%d\n", context->error_resilience);
	printf("    has_b_frames=%d\n", context->has_b_frames);
	printf("    block_align=%d\n", context->block_align);
	printf("    parse_only=%d\n", context->parse_only);
	printf("    idct_algo=%d\n", context->idct_algo);
	printf("    slice_count=%d\n", context->slice_count);
	printf("    slice_offset=%p\n", context->slice_offset);
	printf("    error_concealment=%d\n", context->error_concealment);
	printf("    dsp_mask=%p\n", context->dsp_mask);
	printf("    bits_per_sample=%d\n", context->bits_per_sample);
	printf("    slice_flags=%d\n", context->slice_flags);
	printf("    xvmc_acceleration=%d\n", context->xvmc_acceleration);
	printf("    antialias_algo=%d\n", context->antialias_algo);
	printf("    thread_count=%d\n", context->thread_count);
	printf("    skip_top=%d\n", context->skip_top);
	printf("    profile=%d\n", context->profile);
	printf("    level=%d\n", context->level);
	printf("    lowres=%d\n", context->lowres);
	printf("    coded_width=%d\n", context->coded_width);
	printf("    coded_height=%d\n", context->coded_height);
	printf("    request_channels=%d\n", context->request_channels);
}


int FileFFMPEG::read_frame(VFrame *frame)
{
	int error = 0;
	ffmpeg_lock->lock("FileFFMPEG::read_frame");
	AVStream *stream = ((AVFormatContext*)ffmpeg_file_context)->streams[video_index];
	AVCodecContext *decoder_context = stream->codec;

//printf("FileFFMPEG::read_frame\n");
//dump_context(stream->codec);
	if(first_frame)
	{
		first_frame = 0;
		int got_it = 0;

		while(!got_it && !error)
		{
			AVPacket packet;
			error = av_read_frame((AVFormatContext*)ffmpeg_file_context, 
				&packet);
			if(!error && packet.size > 0)
			{
				if(packet.stream_index == video_index)
				{

					if(!ffmpeg_frame)
						ffmpeg_frame = avcodec_alloc_frame();
					int got_picture = 0;


                	avcodec_get_frame_defaults((AVFrame*)ffmpeg_frame);
		        	int result = avcodec_decode_video(
						decoder_context,
                    	(AVFrame*)ffmpeg_frame, 
						&got_picture,
                    	packet.data, 
						packet.size);
					if(((AVFrame*)ffmpeg_frame)->data[0] && got_picture) got_it = 1;
				}
			}
		}

		error = 0;
	}

#define SEEK_THRESHOLD 16

	if(current_frame != file->current_frame &&
		(file->current_frame < current_frame ||
		file->current_frame > current_frame + SEEK_THRESHOLD))
	{
// printf("FileFFMPEG::read_frame %d current_frame=%lld file->current_frame=%lld\n", 
// __LINE__, 
// current_frame, 
// file->current_frame);

		int64_t timestamp = (int64_t)((double)file->current_frame * 
			stream->time_base.den /
			stream->time_base.num /
			asset->frame_rate);
// Want to seek to the nearest keyframe and read up to the current frame
// but ffmpeg doesn't support that kind of precision.
// Unfortunately this crashes.
		av_seek_frame((AVFormatContext*)ffmpeg_file_context, 
			video_index, 
			timestamp, 
			AVSEEK_FLAG_ANY);
		current_frame = file->current_frame - 1;
	}

//sleep(1);
	int got_it = 0;
// Read frames until we catch up to the current position.
// 	if(current_frame >= file->current_frame - SEEK_THRESHOLD &&
// 		current_frame < file->current_frame - 1)
// 	{
// 		printf("FileFFMPEG::read_frame %d current_frame=%lld file->current_frame=%lld\n", 
// 			__LINE__,
// 			current_frame,
// 			file->current_frame);
// 	}

	while(current_frame < file->current_frame && !error)
	{
		got_it = 0;
		while(!got_it && !error)
		{
			AVPacket packet;

			error = av_read_frame((AVFormatContext*)ffmpeg_file_context, 
				&packet);

			if(!error && packet.size > 0)
			{
				if(packet.stream_index == video_index)
				{

					if(!ffmpeg_frame)
						ffmpeg_frame = avcodec_alloc_frame();
					int got_picture = 0;
                	avcodec_get_frame_defaults((AVFrame*)ffmpeg_frame);


// printf("FileFFMPEG::read_frame %d current_frame=%lld ffmpeg_frame=%p packet.data=%p packet.size=%d\n",
// __LINE__,
// file->current_frame, 
// ffmpeg_frame, 
// packet.data, 
// packet.size);
// for(int i = 0; i < decoder_context->extradata_size; i++)
// printf("0x%02x, ", decoder_context->extradata[i]);
// printf("\n");
// 
// if(file->current_frame >= 200 && file->current_frame < 280)
// {
// char string[1024];
// sprintf(string, "/tmp/debug%03lld", file->current_frame);
// FILE *out = fopen(string, "w");
// fwrite(packet.data, packet.size, 1, out);
// fclose(out);
// }


		        	int result = avcodec_decode_video(
						decoder_context,
                    	(AVFrame*)ffmpeg_frame, 
						&got_picture,
                    	packet.data, 
						packet.size);
//printf("FileFFMPEG::read_frame %d result=%d\n", __LINE__, result);
					if(((AVFrame*)ffmpeg_frame)->data[0] && got_picture) got_it = 1;
//printf("FileFFMPEG::read_frame result=%d got_it=%d\n", result, got_it);
				}
			}
		}

		if(got_it) current_frame++;
	}
//PRINT_TRACE

// Convert colormodel
	if(got_it)
	{	
		int input_cmodel;
		AVFrame *input_frame = (AVFrame*)ffmpeg_frame;

		switch(decoder_context->pix_fmt)
		{
			case PIX_FMT_YUV420P:
				input_cmodel = BC_YUV420P;
				break;
			case PIX_FMT_YUV422:
				input_cmodel = BC_YUV422;
				break;
			case PIX_FMT_YUV422P:
				input_cmodel = BC_YUV422P;
				break;
			case PIX_FMT_YUV410P:
				input_cmodel = BC_YUV9P;
				break;
			default:
				fprintf(stderr, 
					"quicktime_ffmpeg_decode: unrecognized color model %d\n", 
					decoder_context->pix_fmt);
				input_cmodel = BC_YUV420P;
				break;
		}



		unsigned char **input_rows = 
			(unsigned char**)malloc(sizeof(unsigned char*) * 
			decoder_context->height);


		for(int i = 0; i < decoder_context->height; i++)
			input_rows[i] = input_frame->data[0] + 
				i * 
				decoder_context->width * 
				cmodel_calculate_pixelsize(input_cmodel);


		cmodel_transfer(frame->get_rows(), /* Leave NULL if non existent */
			input_rows,
			frame->get_y(), /* Leave NULL if non existent */
			frame->get_u(),
			frame->get_v(),
			input_frame->data[0], /* Leave NULL if non existent */
			input_frame->data[1],
			input_frame->data[2],
			0,        /* Dimensions to capture from input frame */
			0, 
			decoder_context->width, 
			decoder_context->height,
			0,       /* Dimensions to project on output frame */
			0, 
			frame->get_w(), 
			frame->get_h(),
			input_cmodel, 
			frame->get_color_model(),
			0,         /* When transfering BC_RGBA8888 to non-alpha this is the background color in 0xRRGGBB hex */
			input_frame->linesize[0],       /* For planar use the luma rowspan */
			frame->get_w());
		free(input_rows);
	}
//PRINT_TRACE


	ffmpeg_lock->unlock();
	return error;
}

int FileFFMPEG::read_samples(double *buffer, int64_t len)
{
	int error = 0;
	ffmpeg_lock->lock("FileFFMPEG::read_samples");
	AVStream *stream = ((AVFormatContext*)ffmpeg_file_context)->streams[audio_index];
	AVCodecContext *decoder_context = stream->codec;
	const int debug = 0;

	update_pcm_history(len);

// printf("FileFFMPEG::read_samples %d decode_start=%lld decode_end=%lld\n",
// __LINE__,
// decode_start,
// decode_end);

// Seek occurred
	if(decode_start != decode_end)
	{
		int64_t timestamp = (int64_t)((double)file->current_sample * 
			stream->time_base.den /
			stream->time_base.num /
			asset->sample_rate);
// Want to seek to the nearest keyframe and read up to the current frame
// but ffmpeg doesn't support that kind of precision.
		av_seek_frame((AVFormatContext*)ffmpeg_file_context, 
			video_index, 
			timestamp, 
			AVSEEK_FLAG_ANY);
		current_sample = file->current_sample;
		decode_end = decode_start;
	}

//PRINT_TRACE
	int got_it = 0;
	int accumulation = 0;
// Read frames until the requested range is decoded.
	while(accumulation < decode_len && !error)
	{
//printf("FileFFMPEG::read_samples %d accumulation=%d\n", __LINE__, accumulation);
		AVPacket packet;
		error = av_read_frame((AVFormatContext*)ffmpeg_file_context, 
			&packet);
		unsigned char *packet_ptr = packet.data;
		int packet_len = packet.size;
if(debug) printf("FileFFMPEG::read_samples %d error=%d packet_len=%d\n", 
__LINE__, 
error, 
packet_len);

		if(packet.stream_index == audio_index)
		{
			while(packet_len > 0 && !error)
			{
				int data_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
                if(!ffmpeg_samples) ffmpeg_samples = (short*)realloc(ffmpeg_samples, data_size);
if(debug) printf("FileFFMPEG::read_samples %d decoder_context=%p ffmpeg_samples=%p data_size=%d packet.size=%d packet.data=%p codec_id=%d\n", 
__LINE__, 
decoder_context,
ffmpeg_samples,
data_size,
packet_len,
packet_ptr,
decoder_context->codec_id);
//av_log_set_level(AV_LOG_DEBUG);
				int bytes_decoded = avcodec_decode_audio2(decoder_context, 
					ffmpeg_samples, 
					&data_size,
                    packet_ptr, 
					packet_len);
if(debug) PRINT_TRACE
//				if(bytes_decoded < 0) error = 1;
				packet_ptr += bytes_decoded;
				packet_len -= bytes_decoded;
if(debug) printf("FileFFMPEG::read_samples %d bytes_decoded=%d data_size=%d\n", 
__LINE__, 
bytes_decoded,
data_size);
//				if(data_size <= 0)
//					break;
				int samples_decoded = data_size / 
					asset->channels / 
					sizeof(short);
// Transfer decoded samples to ring buffer
				append_history(ffmpeg_samples, samples_decoded);
// static FILE *fd = 0;
// if(!fd) fd = fopen("/tmp/test.pcm", "w");
// fwrite(ffmpeg_samples, data_size, 1, fd);
				accumulation += samples_decoded;
			}
		}
if(debug) PRINT_TRACE
	}

if(debug) PRINT_TRACE
	read_history(buffer, 
		file->current_sample, 
		file->current_channel,
		len);

if(debug) PRINT_TRACE
	current_sample += len;
	ffmpeg_lock->unlock();
	return error;
}






