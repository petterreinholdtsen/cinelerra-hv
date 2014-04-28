#include <stdio.h>
#include <stdlib.h>
#include "colormodels.h"
#include "funcprotos.h"
#include "jpeg.h"
#include "quicktime.h"

static int delete_codec(quicktime_video_map_t *vtrack)
{
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)vtrack->codec)->priv;
	int i;

	mjpeg_delete(codec->mjpeg);
	if(codec->buffer)
		free(codec->buffer);
	if(codec->temp_video)
		free(codec->temp_video);
	free(codec);
	return 0;
}

void quicktime_set_jpeg(quicktime_t *file, int quality, int use_float)
{
	int i;
	char *compressor;

	for(i = 0; i < file->total_vtracks; i++)
	{
		if(quicktime_match_32(quicktime_video_compressor(file, i), QUICKTIME_JPEG) ||
			quicktime_match_32(quicktime_video_compressor(file, i), QUICKTIME_MJPA) ||
			quicktime_match_32(quicktime_video_compressor(file, i), QUICKTIME_RTJ0))
		{
			quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)file->vtracks[i].codec)->priv;
			mjpeg_set_quality(codec->mjpeg, quality);
			mjpeg_set_float(codec->mjpeg, use_float);
		}
	}
}

static int decode(quicktime_t *file, 
	unsigned char **row_pointers, 
	int track)
{
	quicktime_video_map_t *vtrack = &(file->vtracks[track]);
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)vtrack->codec)->priv;
	quicktime_trak_t *trak = vtrack->track;
	mjpeg_t *mjpeg = codec->mjpeg;
	long size, field2_offset;
	int track_height = trak->tkhd.track_height;
	int track_width = trak->tkhd.track_width;
	int result = 0;
//printf("decode 1\n");

	mjpeg_set_cpus(codec->mjpeg, file->cpus);
	if(file->row_span) 
		mjpeg_set_rowspan(codec->mjpeg, file->row_span);
	else
		mjpeg_set_rowspan(codec->mjpeg, 0);

	quicktime_set_video_position(file, vtrack->current_position, track);
	size = quicktime_frame_size(file, vtrack->current_position, track);
	codec->buffer_size = size;
//printf("decode 1 %x\n", size);

	if(size > codec->buffer_allocated)
	{
		codec->buffer_allocated = size;
		codec->buffer = realloc(codec->buffer, codec->buffer_allocated);
	}

//printf("decode 2 %llx %02x %02x\n", 
//	quicktime_chunk_to_offset(trak, vtrack->current_position), codec->buffer[0], codec->buffer[1]);
	result = !quicktime_read_data(file, codec->buffer, size);
//printf("decode 3 %x\n", size);

	if(!result)
	{
		if(mjpeg_get_fields(mjpeg) == 2)
		{
//printf("decode 4\n");
			field2_offset = mjpeg_get_quicktime_field2(codec->buffer, size);
//printf("decode 5\n");
		}
		else
			field2_offset = 0;

/*
 * printf("decode 6 %02x%02x %02x%02x\n", codec->buffer[0], codec->buffer[1],
 * 	codec->buffer[field2_offset], codec->buffer[field2_offset + 1]);
 */
//printf("decode 6 %d %d %d %d %d %d\n",
//	file->in_x, file->in_y, file->in_w, file->in_h, file->out_w, file->out_h);

 		if(file->in_x == 0 && 
			file->in_y == 0 && 
			file->in_w == track_width &&
			file->in_h == track_height &&
			file->out_w == track_width &&
			file->out_h == track_height)
		{
			int i;
//printf("decode 6\n");
			mjpeg_decompress(codec->mjpeg, 
				codec->buffer, 
				size,
				field2_offset,  
				row_pointers, 
				row_pointers[0], 
				row_pointers[1], 
				row_pointers[2],
				file->color_model,
				file->cpus);
//printf("decode 7\n");
		}
		else
		{
			int i;
			unsigned char **temp_rows;
			int temp_cmodel = BC_YUV888;
			int temp_rowsize = cmodel_calculate_pixelsize(temp_cmodel) * track_width;

//printf("decode 8\n");
			if(!codec->temp_video)
				codec->temp_video = malloc(temp_rowsize * track_height);
			temp_rows = malloc(sizeof(unsigned char*) * track_height);
			for(i = 0; i < track_height; i++)
				temp_rows[i] = codec->temp_video + i * temp_rowsize;

			mjpeg_decompress(codec->mjpeg, 
				codec->buffer, 
				size,
				field2_offset,  
				temp_rows, 
				temp_rows[0], 
				temp_rows[1], 
				temp_rows[2],
				temp_cmodel,
				file->cpus);

			cmodel_transfer(row_pointers, 
				temp_rows,
				row_pointers[0],
				row_pointers[1],
				row_pointers[2],
				temp_rows[0],
				temp_rows[1],
				temp_rows[2],
				file->in_x, 
				file->in_y, 
				file->in_w, 
				file->in_h,
				0, 
				0, 
				file->out_w, 
				file->out_h,
				temp_cmodel, 
				file->color_model,
				0,
				track_width,
				file->out_w);

			free(temp_rows);
//printf("decode 9\n");
		}
	}
//printf("decode 8\n");

	return result;
}

static int encode(quicktime_t *file, unsigned char **row_pointers, int track)
{
	quicktime_video_map_t *vtrack = &(file->vtracks[track]);
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)vtrack->codec)->priv;
	quicktime_trak_t *trak = vtrack->track;
	longest offset = quicktime_position(file);
	int result = 0;
	long field2_offset;
	quicktime_atom_t chunk_atom;

	mjpeg_set_cpus(codec->mjpeg, file->cpus);

	mjpeg_compress(codec->mjpeg, 
		row_pointers, 
		row_pointers[0], 
		row_pointers[1], 
		row_pointers[2],
		file->color_model,
		file->cpus);
	if(codec->jpeg_type == JPEG_MJPA) 
		mjpeg_insert_quicktime_markers(&codec->mjpeg->output_data,
			&codec->mjpeg->output_size,
			&codec->mjpeg->output_allocated,
			2,
			&field2_offset);

	quicktime_write_chunk_header(file, trak, &chunk_atom);
	result = !quicktime_write_data(file, 
				mjpeg_output_buffer(codec->mjpeg), 
				mjpeg_output_size(codec->mjpeg));
	quicktime_write_chunk_footer(file, 
					trak,
					vtrack->current_chunk,
					&chunk_atom, 
					1);

	vtrack->current_chunk++;
	return result;
}

static int reads_colormodel(quicktime_t *file, 
		int colormodel, 
		int track)
{
	quicktime_video_map_t *vtrack = &(file->vtracks[track]);
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)vtrack->codec)->priv;

// Some JPEG_PROGRESSIVE is BC_YUV422P
	if(codec->jpeg_type == JPEG_PROGRESSIVE)
	{
		return (colormodel == BC_RGB888 ||
			colormodel == BC_YUV888 ||
			colormodel == BC_YUV420P ||
			colormodel == BC_YUV422P ||
			colormodel == BC_YUV422);
	}
	else
	{
		return (colormodel == BC_RGB888 ||
			colormodel == BC_YUV888 ||
//			colormodel == BC_YUV420P ||
			colormodel == BC_YUV422P ||
			colormodel == BC_YUV422);
// The BC_YUV420P option was provided only for mpeg2movie use.because some 
// interlaced movies were accidentally in YUV4:2:0
	}
}

static int writes_colormodel(quicktime_t *file, 
		int colormodel, 
		int track)
{
	quicktime_video_map_t *vtrack = &(file->vtracks[track]);
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)vtrack->codec)->priv;

	if(codec->jpeg_type == JPEG_PROGRESSIVE)
	{
		return (colormodel == BC_RGB888 ||
			colormodel == BC_YUV888 ||
			colormodel == BC_YUV420P);
	}
	else
	{
		return (colormodel == BC_RGB888 ||
			colormodel == BC_YUV888 ||
			colormodel == BC_YUV422P);
	}
}

static int set_parameter(quicktime_t *file, 
		int track, 
		char *key, 
		void *value)
{
	quicktime_jpeg_codec_t *codec = ((quicktime_codec_t*)file->vtracks[track].codec)->priv;
	
	if(!strcasecmp(key, "jpeg_quality"))
	{
		mjpeg_set_quality(codec->mjpeg, *(int*)value);
	}
	else
	if(!strcasecmp(key, "jpeg_usefloat"))
	{
		mjpeg_set_float(codec->mjpeg, *(int*)value);
	}
	return 0;
}

void quicktime_init_codec_jpeg(quicktime_video_map_t *vtrack)
{
	char *compressor = vtrack->track->mdia.minf.stbl.stsd.table[0].format;
	quicktime_jpeg_codec_t *codec;
	int i, jpeg_type;

	if(quicktime_match_32(compressor, QUICKTIME_JPEG))
		jpeg_type = JPEG_PROGRESSIVE;
	if(quicktime_match_32(compressor, QUICKTIME_MJPA))
		jpeg_type = JPEG_MJPA;

/* Init public items */
	((quicktime_codec_t*)vtrack->codec)->priv = calloc(1, sizeof(quicktime_jpeg_codec_t));
	((quicktime_codec_t*)vtrack->codec)->delete_vcodec = delete_codec;
	((quicktime_codec_t*)vtrack->codec)->decode_video = decode;
	((quicktime_codec_t*)vtrack->codec)->encode_video = encode;
	((quicktime_codec_t*)vtrack->codec)->decode_audio = 0;
	((quicktime_codec_t*)vtrack->codec)->encode_audio = 0;
	((quicktime_codec_t*)vtrack->codec)->reads_colormodel = reads_colormodel;
	((quicktime_codec_t*)vtrack->codec)->writes_colormodel = writes_colormodel;
	((quicktime_codec_t*)vtrack->codec)->set_parameter = set_parameter;

/* Init private items */
	codec = ((quicktime_codec_t*)vtrack->codec)->priv;
	codec->mjpeg = mjpeg_new(vtrack->track->tkhd.track_width, 
		vtrack->track->tkhd.track_height, 
		1 + (jpeg_type == JPEG_MJPA || jpeg_type == JPEG_MJPB));
	codec->jpeg_type = jpeg_type;

/* This information must be stored in the initialization routine because of */
/* direct copy rendering.  Quicktime for Windows must have this information. */
	if(quicktime_match_32(compressor, QUICKTIME_MJPA) && !vtrack->track->mdia.minf.stbl.stsd.table[0].fields)
	{
		vtrack->track->mdia.minf.stbl.stsd.table[0].fields = 2;
		vtrack->track->mdia.minf.stbl.stsd.table[0].field_dominance = 1;
	}
}