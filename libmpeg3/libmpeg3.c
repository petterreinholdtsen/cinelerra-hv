#include "libmpeg3.h"
#include "mpeg3private.h"
#include "mpeg3protos.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int mpeg3_major()
{
	return MPEG3_MAJOR;
}

int mpeg3_minor()
{
	return MPEG3_MINOR;
}

int mpeg3_release()
{
	return MPEG3_RELEASE;
}



mpeg3_t* mpeg3_new(char *path)
{
	int i;
	mpeg3_t *file = calloc(1, sizeof(mpeg3_t));
	file->cpus = 1;
	file->fs = mpeg3_new_fs(path);
//	file->have_mmx = mpeg3_mmx_test();
// Late compilers don't produce usable code.
	file->have_mmx = 0;
	file->demuxer = mpeg3_new_demuxer(file, 0, 0, -1);
	file->seekable = 1;
	return file;
}

int mpeg3_delete(mpeg3_t *file)
{
	int i;

	for(i = 0; i < file->total_vstreams; i++)
		mpeg3_delete_vtrack(file, file->vtrack[i]);

	for(i = 0; i < file->total_astreams; i++)
		mpeg3_delete_atrack(file, file->atrack[i]);

	mpeg3_delete_fs(file->fs);
	mpeg3_delete_demuxer(file->demuxer);

	if(file->frame_offsets)
	{
		for(i = 0; i < file->total_vstreams; i++)
		{
			free(file->frame_offsets[i]);
			free(file->keyframe_numbers[i]);
		}

		free(file->frame_offsets);
		free(file->keyframe_numbers);
		free(file->total_frame_offsets);
		free(file->total_keyframe_numbers);
	}

	if(file->sample_offsets)
	{
		for(i = 0; i < file->total_astreams; i++)
			free(file->sample_offsets[i]);

		free(file->sample_offsets);
		free(file->total_sample_offsets);
	}
	if(file->channel_counts)
		free(file->channel_counts);

	free(file);
	return 0;
}

int mpeg3_check_sig(char *path)
{
	mpeg3_fs_t *fs;
	u_int32_t bits;
	char *ext;
	int result = 0;

	fs = mpeg3_new_fs(path);
	if(mpeg3io_open_file(fs))
	{
/* File not found */
		return 0;
	}

	bits = mpeg3io_read_int32(fs);
/* Test header */
	if(bits == MPEG3_TOC_PREFIX)
	{
		result = 1;
	}
	else
	if((((bits >> 24) & 0xff) == MPEG3_SYNC_BYTE) ||
		(bits == MPEG3_PACK_START_CODE) ||
		((bits & 0xfff00000) == 0xfff00000) ||
		((bits & 0xffff0000) == 0xffe30000) ||
		(bits == MPEG3_SEQUENCE_START_CODE) ||
		(bits == MPEG3_PICTURE_START_CODE) ||
		(((bits & 0xffff0000) >> 16) == MPEG3_AC3_START_CODE) ||
		((bits >> 8) == MPEG3_ID3_PREFIX) ||
		(bits == MPEG3_RIFF_CODE) ||
        (bits == MPEG3_IFO_PREFIX))
	{
		result = 1;

		ext = strrchr(path, '.');
		if(ext)
		{
/* Test file extension. */
			if(strncasecmp(ext, ".ifo", 4) && 
            	strncasecmp(ext, ".mp2", 4) && 
				strncasecmp(ext, ".mp3", 4) &&
				strncasecmp(ext, ".m1v", 4) &&
				strncasecmp(ext, ".m2v", 4) &&
				strncasecmp(ext, ".m2s", 4) &&
				strncasecmp(ext, ".mpg", 4) &&
				strncasecmp(ext, ".vob", 4) &&
				strncasecmp(ext, ".mpeg", 4) &&
				strncasecmp(ext, ".ac3", 4))
				result = 0;
		}
	}

	mpeg3io_close_file(fs);
	mpeg3_delete_fs(fs);
	return result;
}


static uint32_t read_int32(unsigned char *buffer, int *position)
{
	uint32_t temp;

	if(MPEG3_LITTLE_ENDIAN)
	{
		((unsigned char*)&temp)[3] = buffer[(*position)++];
		((unsigned char*)&temp)[2] = buffer[(*position)++];
		((unsigned char*)&temp)[1] = buffer[(*position)++];
		((unsigned char*)&temp)[0] = buffer[(*position)++];
	}
	else
	{
		((unsigned char*)&temp)[0] = buffer[(*position)++];
		((unsigned char*)&temp)[1] = buffer[(*position)++];
		((unsigned char*)&temp)[2] = buffer[(*position)++];
		((unsigned char*)&temp)[3] = buffer[(*position)++];
	}
	
	return temp;
}

static uint64_t read_int64(unsigned char *buffer, int *position)
{
	uint64_t temp;

	if(MPEG3_LITTLE_ENDIAN)
	{
		((unsigned char*)&temp)[7] = buffer[(*position)++];
		((unsigned char*)&temp)[6] = buffer[(*position)++];
		((unsigned char*)&temp)[5] = buffer[(*position)++];
		((unsigned char*)&temp)[4] = buffer[(*position)++];
		((unsigned char*)&temp)[3] = buffer[(*position)++];
		((unsigned char*)&temp)[2] = buffer[(*position)++];
		((unsigned char*)&temp)[1] = buffer[(*position)++];
		((unsigned char*)&temp)[0] = buffer[(*position)++];
	}
	else
	{
		((unsigned char*)&temp)[0] = buffer[(*position)++];
		((unsigned char*)&temp)[1] = buffer[(*position)++];
		((unsigned char*)&temp)[2] = buffer[(*position)++];
		((unsigned char*)&temp)[3] = buffer[(*position)++];
		((unsigned char*)&temp)[4] = buffer[(*position)++];
		((unsigned char*)&temp)[5] = buffer[(*position)++];
		((unsigned char*)&temp)[6] = buffer[(*position)++];
		((unsigned char*)&temp)[7] = buffer[(*position)++];
	}

	return temp;
}






static int read_toc(mpeg3_t *file, int *atracks_return, int *vtracks_return)
{
	unsigned char *buffer;
	int file_type;
	int position = 4;
	int stream_type;
	int i, j;
	int is_vfs = 0;
	int vfs_len = strlen(RENDERFARM_FS_PREFIX);
	int toc_version;
	int64_t current_byte = 0;

// Fix title paths for Cinelerra VFS
	if(!strncmp(file->fs->path, RENDERFARM_FS_PREFIX, vfs_len))
		is_vfs = 1;

	buffer = malloc(mpeg3io_total_bytes(file->fs));
	mpeg3io_seek(file->fs, 0);
	mpeg3io_read_data(buffer, mpeg3io_total_bytes(file->fs), file->fs);

// Test version
	if((toc_version = buffer[position++]) != MPEG3_TOC_VERSION)
	{
		fprintf(stderr,
			"read_toc: invalid TOC version %x\n", 
			toc_version);
		return 1;
	}

//printf("read_toc %lld\n", mpeg3io_total_bytes(file->fs));

// File type
	file_type = buffer[position++];
	switch(file_type)
	{
		case FILE_TYPE_PROGRAM:
			file->is_program_stream = 1;
			break;
		case FILE_TYPE_TRANSPORT:
			file->is_transport_stream = 1;
			break;
		case FILE_TYPE_AUDIO:
			file->is_audio_stream = 1;
			break;
		case FILE_TYPE_VIDEO:
			file->is_video_stream = 1;
			break;
	}

//printf("read_toc 10\n");

// Stream ID's
	while((stream_type = buffer[position]) != TITLE_PATH)
	{
		int offset;
		int stream_id;

//printf("read_toc %d %x\n", position, buffer[position]);
		position++;
		offset = read_int32(buffer, &position);
		stream_id = read_int32(buffer, &position);

		if(stream_type == STREAM_AUDIO)
		{
			file->demuxer->astream_table[offset] = stream_id;
		}

		if(stream_type == STREAM_VIDEO)
		{
			file->demuxer->vstream_table[offset] = stream_id;
		}
	}



//printf("read_toc 10\n");


// Titles
	while(buffer[position] == TITLE_PATH)
	{
		char string[MPEG3_STRLEN];
		int string_len = 0;
		mpeg3_title_t *title;
		FILE *test_fd;

// Construct title path from VFS prefix and path.
		position++;
		if(is_vfs)
		{
			strcpy(string, RENDERFARM_FS_PREFIX);
			string_len = vfs_len;
		}
		while(buffer[position] != 0) string[string_len++] = buffer[position++];
		string[string_len++] = 0;
		position++;

// Test title availability
		test_fd = fopen(string, "r");
		if(test_fd) 
		{
			fclose(test_fd);
		}
		else
		{
// Try concatenating title and toc directory if title is not absolute and
// toc path has a directory section.
			if((!is_vfs && string[0] != '/') ||
				(is_vfs && string[vfs_len] != '/'))
			{
// Get toc filename without path
				char *ptr = strrchr(file->fs->path, '/');
				if(ptr)
				{
					char string2[MPEG3_STRLEN];

// Stack filename on toc path
					strcpy(string2, file->fs->path);
					if(!is_vfs)
						strcpy(&string2[ptr - file->fs->path + 1], string);
					else
						strcpy(&string2[ptr - file->fs->path + 1], string + vfs_len);

					test_fd = fopen(string2, "r");
					if(test_fd)
					{
						fclose(test_fd);
						strcpy(string, string2);
					}
					else
					{
						fprintf(stderr, 
							"read_toc: failed to open %s or %s\n",
							string,
							string2);
						return 1;
					}
				}
				else
				{
					fprintf(stderr,
						"read_toc: failed to open %s\n", 
						string);
					return 1;
				}
			}
			else
			{
				fprintf(stderr, 
					"read_toc: failed to open %s\n", 
					string);
				return 1;
			}
		}

		title = 
			file->demuxer->titles[file->demuxer->total_titles++] = 
			mpeg3_new_title(file, string);

		title->total_bytes = read_int64(buffer, &position);
		title->start_byte = current_byte;
		title->end_byte = title->start_byte + title->total_bytes;
		current_byte = title->end_byte;

// Cells
		title->cell_table_size = 
			title->cell_table_allocation = 
			read_int32(buffer, &position);
		title->cell_table = calloc(title->cell_table_size, sizeof(mpeg3demux_cell_t));
		for(i = 0; i < title->cell_table_size; i++)
		{
			title->cell_table[i].start_byte = read_int64(buffer, &position);
			title->cell_table[i].end_byte = read_int64(buffer, &position);
			title->cell_table[i].program = read_int32(buffer, &position);
		}
	}
//printf("read_toc 10\n");



// Audio streams
// Skip ATRACK_COUNT
	position++;
	*atracks_return = read_int32(buffer, &position);
//printf("read_toc 10\n");

// Skip VTRACK_COUNT
	position++;
	*vtracks_return = read_int32(buffer, &position);
//printf("read_toc 10\n");


	if(*atracks_return)
	{
		file->channel_counts = calloc(sizeof(int), *atracks_return);
		file->sample_offsets = malloc(sizeof(int64_t*) * *atracks_return);
		file->total_sample_offsets = malloc(sizeof(int*) * *atracks_return);

		for(i = 0; i < *atracks_return; i++)
		{
			file->channel_counts[i] = read_int32(buffer, &position);
			file->total_sample_offsets[i] = read_int32(buffer, &position);
			file->sample_offsets[i] = malloc(file->total_sample_offsets[i] * sizeof(int64_t));
			for(j = 0; j < file->total_sample_offsets[i]; j++)
			{
				file->sample_offsets[i][j] = read_int64(buffer, &position);
//printf("samples %llx\n", file->sample_offsets[i][j]);
			}
		}
	}
//printf("read_toc 10\n");

	if(*vtracks_return)
	{
		file->frame_offsets = malloc(sizeof(int64_t*) * *vtracks_return);
		file->total_frame_offsets = malloc(sizeof(int*) * *vtracks_return);
		file->keyframe_numbers = malloc(sizeof(int64_t*) * *vtracks_return);
		file->total_keyframe_numbers = malloc(sizeof(int*) * *vtracks_return);

		for(i = 0; i < *vtracks_return; i++)
		{
			file->total_frame_offsets[i] = read_int32(buffer, &position);
			file->frame_offsets[i] = malloc(file->total_frame_offsets[i] * sizeof(int64_t));
			for(j = 0; j < file->total_frame_offsets[i]; j++)
			{
				file->frame_offsets[i][j] = read_int64(buffer, &position);
//printf("frame %llx\n", file->frame_offsets[i][j]);
			}


			file->total_keyframe_numbers[i] = read_int32(buffer, &position);
			file->keyframe_numbers[i] = malloc(file->total_keyframe_numbers[i] * sizeof(int64_t));
			for(j = 0; j < file->total_keyframe_numbers[i]; j++)
			{
				file->keyframe_numbers[i][j] = read_int64(buffer, &position);
			}
		}
	}
//printf("read_toc 10\n");

	free(buffer);
//printf("read_toc 10\n");



//printf("read_toc 1\n");
	mpeg3demux_open_title(file->demuxer, 0);
//printf("read_toc 10\n");

//printf("read_toc 2 %llx\n", mpeg3demux_tell(file->demuxer));
	return 0;
}




mpeg3_t* mpeg3_open_copy(char *path, mpeg3_t *old_file)
{
	mpeg3_t *file = 0;
	unsigned int bits;
	int i, done;
/* The table of contents may have fewer tracks than are in the demuxer */
/* This limits the track count */
	int toc_atracks = 0x7fffffff;
	int toc_vtracks = 0x7fffffff;

/* Initialize the file structure */
	file = mpeg3_new(path);







//printf("mpeg3_open_copy 1 %s\n", path);

/* Need to perform authentication before reading a single byte. */
	if(mpeg3io_open_file(file->fs))
	{
		mpeg3_delete(file);
		return 0;
	}













/* =============================== Create the title objects ========================= */
	bits = mpeg3io_read_int32(file->fs);
//printf("mpeg3_open 1 %p %d %d %d %d\n", old_file, file->is_transport_stream, file->is_program_stream, file->is_video_stream, file->is_audio_stream);

	if(bits == MPEG3_TOC_PREFIX)   /* TOC  */
	{
/* Table of contents for another title set */
		if(!old_file)
		{
			if(read_toc(file, &toc_atracks, &toc_vtracks))
			{
				mpeg3io_close_file(file->fs);
				mpeg3_delete(file);
				return 0;
			}
		}
		mpeg3io_close_file(file->fs);
	}
	else
// IFO file
    if(bits == MPEG3_IFO_PREFIX)
    {
    	if(!old_file)
		{
			if(mpeg3_read_ifo(file, 0))
        	{
				mpeg3_delete(file);
				mpeg3io_close_file(file->fs);
				return 0;
        	}
		}
		file->is_ifo_file = 1;
		mpeg3io_close_file(file->fs);
    }
    else
	if(((bits >> 24) & 0xff) == MPEG3_SYNC_BYTE)
	{
/* Transport stream */
		file->is_transport_stream = 1;
	}
	else
	if(bits == MPEG3_PACK_START_CODE)
	{
/* Program stream */
/* Determine packet size empirically */
		file->is_program_stream = 1;
	}
	else
	if((bits & 0xfff00000) == 0xfff00000 ||
		(bits & 0xffff0000) == 0xffe30000 ||
		((bits >> 8) == MPEG3_ID3_PREFIX) ||
		(bits == MPEG3_RIFF_CODE))
	{
/* MPEG Audio only */
		file->is_audio_stream = 1;
	}
	else
	if(bits == MPEG3_SEQUENCE_START_CODE ||
		bits == MPEG3_PICTURE_START_CODE)
	{
/* Video only */
		file->is_video_stream = 1;
	}
	else
	if(((bits & 0xffff0000) >> 16) == MPEG3_AC3_START_CODE)
	{
/* AC3 Audio only */
		file->is_audio_stream = 1;
	}
	else
	{
		mpeg3_delete(file);
		fprintf(stderr, "mpeg3_open: not an MPEG 2 stream\n");
		return 0;
	}

/*
 * printf("mpeg3_open 2 %p %d %d %d %d\n", 
 * old_file, 
 * file->is_transport_stream, 
 * file->is_program_stream, 
 * file->is_video_stream, 
 * file->is_audio_stream);
 */












// Configure packet size
	if(file->is_transport_stream)
		file->packet_size = MPEG3_TS_PACKET_SIZE;
	else
	if(file->is_program_stream)
		file->packet_size = 0;
	else
	if(file->is_audio_stream)
		file->packet_size = MPEG3_DVD_PACKET_SIZE;
	else
	if(file->is_video_stream)
		file->packet_size = MPEG3_DVD_PACKET_SIZE;













//printf("mpeg3_open 1\n");

/* Create titles */
/* Copy timecodes from an old demuxer */
	if(old_file && mpeg3_get_demuxer(old_file))
	{
		mpeg3demux_copy_titles(file->demuxer, mpeg3_get_demuxer(old_file));
		file->is_transport_stream = old_file->is_transport_stream;
		file->is_program_stream = old_file->is_program_stream;
	}
	else
/* Start from scratch */
	if(!file->demuxer->total_titles)
	{
		mpeg3demux_create_title(file->demuxer, 0, 0);
	}



//printf("mpeg3_open 50\n");











/* Generate tracks */
	if(file->is_transport_stream || file->is_program_stream)
	{
/* Create video tracks */
		for(i = 0; 
			i < MPEG3_MAX_STREAMS && file->total_vstreams < toc_vtracks; 
			i++)
		{
			if(file->demuxer->vstream_table[i])
			{
				file->vtrack[file->total_vstreams] = 
					mpeg3_new_vtrack(file, 
						i, 
						file->demuxer, 
						file->total_vstreams);
				if(file->vtrack[file->total_vstreams]) 
					file->total_vstreams++;

			}
		}

/* Create audio tracks */
		for(i = 0; i < MPEG3_MAX_STREAMS && file->total_astreams < toc_atracks; i++)
		{
			if(file->demuxer->astream_table[i])
			{
				file->atrack[file->total_astreams] = mpeg3_new_atrack(file, 
					i, 
					file->demuxer->astream_table[i], 
					file->demuxer,
					file->total_astreams);
				if(file->atrack[file->total_astreams]) file->total_astreams++;
			}
		}
	}
	else
	if(file->is_video_stream)
	{
/* Create video tracks */
		file->vtrack[0] = mpeg3_new_vtrack(file, 
			-1, 
			file->demuxer, 
			0);
		if(file->vtrack[0]) file->total_vstreams++;
	}
	else
	if(file->is_audio_stream)
	{
/* Create audio tracks */

		file->atrack[0] = mpeg3_new_atrack(file, 
			-1, 
			AUDIO_UNKNOWN, 
			file->demuxer,
			0);
		if(file->atrack[0]) file->total_astreams++;
	}




	mpeg3io_close_file(file->fs);
	return file;
}

mpeg3_t* mpeg3_open(char *path)
{
	return mpeg3_open_copy(path, 0);
}

int mpeg3_close(mpeg3_t *file)
{
/* File is closed in the same procedure it is opened in. */
	mpeg3_delete(file);
	return 0;
}

int mpeg3_set_cpus(mpeg3_t *file, int cpus)
{
	int i;
	file->cpus = cpus;
	for(i = 0; i < file->total_vstreams; i++)
		mpeg3video_set_cpus(file->vtrack[i]->video, cpus);
	return 0;
}

int mpeg3_set_mmx(mpeg3_t *file, int use_mmx)
{
	int i;
	file->have_mmx = use_mmx;
	for(i = 0; i < file->total_vstreams; i++)
		mpeg3video_set_mmx(file->vtrack[i]->video, use_mmx);
	return 0;
}

int mpeg3_has_audio(mpeg3_t *file)
{
	return file->total_astreams > 0;
}

int mpeg3_total_astreams(mpeg3_t *file)
{
	return file->total_astreams;
}

int mpeg3_audio_channels(mpeg3_t *file,
		int stream)
{
	if(file->total_astreams)
		return file->atrack[stream]->channels;
	return -1;
}

int mpeg3_sample_rate(mpeg3_t *file,
		int stream)
{
	if(file->total_astreams)
		return file->atrack[stream]->sample_rate;
	return -1;
}

long mpeg3_get_sample(mpeg3_t *file,
		int stream)
{
	if(file->total_astreams)
		return file->atrack[stream]->current_position;
	return -1;
}

int mpeg3_set_sample(mpeg3_t *file, 
		long sample,
		int stream)
{
	if(file->total_astreams)
	{
//printf(__FUNCTION__ " 1 %d %d\n", file->atrack[stream]->current_position, sample);
		file->atrack[stream]->current_position = sample;
		mpeg3audio_seek_sample(file->atrack[stream]->audio, sample);
		return 0;
	}
	return -1;
}

long mpeg3_audio_samples(mpeg3_t *file,
		int stream)
{
	if(file->total_astreams)
		return file->atrack[stream]->total_samples;
	return -1;
}

char* mpeg3_audio_format(mpeg3_t *file, int stream)
{
	if(stream < file->total_astreams)
	{
		switch(file->atrack[stream]->format)
		{
			case AUDIO_UNKNOWN: return "Unknown"; break;
			case AUDIO_MPEG:    return "MPEG"; break;
			case AUDIO_AC3:     return "AC3"; break;
			case AUDIO_PCM:     return "PCM"; break;
			case AUDIO_AAC:     return "AAC"; break;
			case AUDIO_JESUS:   return "Vorbis"; break;
		}
	}
	return "";
}

int mpeg3_has_video(mpeg3_t *file)
{
	return file->total_vstreams > 0;
}

int mpeg3_total_vstreams(mpeg3_t *file)
{
	return file->total_vstreams;
}

int mpeg3_video_width(mpeg3_t *file,
		int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->width;
	return -1;
}

int mpeg3_video_height(mpeg3_t *file,
		int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->height;
	return -1;
}

float mpeg3_aspect_ratio(mpeg3_t *file, int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->aspect_ratio;
	return 0;
}

double mpeg3_frame_rate(mpeg3_t *file,
		int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->frame_rate;
	return -1;
}

long mpeg3_video_frames(mpeg3_t *file,
		int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->total_frames;
	return -1;
}

long mpeg3_get_frame(mpeg3_t *file,
		int stream)
{
	if(file->total_vstreams)
		return file->vtrack[stream]->current_position;
	return -1;
}

int mpeg3_set_frame(mpeg3_t *file, 
		long frame,
		int stream)
{
	if(file->total_vstreams)
	{
		file->vtrack[stream]->current_position = frame;
		mpeg3video_seek_frame(file->vtrack[stream]->video, frame);
		return 0;
	}
	return -1;
}

int mpeg3_seek_byte(mpeg3_t *file, int64_t byte)
{
	int i;

//	file->percentage_pts = -1;
	for(i = 0; i < file->total_vstreams; i++)
	{
		file->vtrack[i]->current_position = 0;
		mpeg3video_seek_byte(file->vtrack[i]->video, byte);
	}

	for(i = 0; i < file->total_astreams; i++)
	{
		file->atrack[i]->current_position = 0;
		mpeg3audio_seek_byte(file->atrack[i]->audio, byte);
	}

	return 0;
}

/*
 * double mpeg3_get_percentage_pts(mpeg3_t *file)
 * {
 * 	return file->percentage_pts;
 * }
 * 
 * void mpeg3_set_percentage_pts(mpeg3_t *file, double pts)
 * {
 * }
 */


int mpeg3_previous_frame(mpeg3_t *file, int stream)
{
	file->last_type_read = 2;
	file->last_stream_read = stream;

	if(file->total_vstreams)
		return mpeg3video_previous_frame(file->vtrack[stream]->video);

	return 0;
}

int64_t mpeg3_tell_byte(mpeg3_t *file)
{
	int64_t result = 0;
	if(file->last_type_read == 1)
	{
		result = mpeg3demux_tell_byte(file->atrack[file->last_stream_read]->demuxer);
	}

	if(file->last_type_read == 2)
	{
		result = mpeg3demux_tell_byte(file->vtrack[file->last_stream_read]->demuxer);
	}
	return result;
}

int64_t mpeg3_get_bytes(mpeg3_t *file)
{
	return mpeg3demux_movie_size(file->demuxer);
}

double mpeg3_get_time(mpeg3_t *file)
{
	double atime = 0, vtime = 0;

	if(file->is_transport_stream || file->is_program_stream)
	{
/* Timecode only available in transport stream */
		if(file->last_type_read == 1)
		{
			atime = mpeg3demux_get_time(file->atrack[file->last_stream_read]->demuxer);
		}
		else
		if(file->last_type_read == 2)
		{
			vtime = mpeg3demux_get_time(file->vtrack[file->last_stream_read]->demuxer);
		}
	}
	else
	{
/* Use percentage and total time */
		if(file->total_astreams)
		{
			atime = mpeg3demux_tell_byte(file->atrack[0]->demuxer) * 
						mpeg3_audio_samples(file, 0) / 
						mpeg3_sample_rate(file, 0) /
						mpeg3_get_bytes(file);
		}

		if(file->total_vstreams)
		{
			vtime = mpeg3demux_tell_byte(file->vtrack[0]->demuxer) *
						mpeg3_video_frames(file, 0) / 
						mpeg3_frame_rate(file, 0) /
						mpeg3_get_bytes(file);
		}
	}

	return MAX(atime, vtime);
}

int mpeg3_end_of_audio(mpeg3_t *file, int stream)
{
	int result = 0;
	if(!file->atrack[stream]->channels) return 1;
	result = mpeg3demux_eof(file->atrack[stream]->demuxer);
	return result;
}

int mpeg3_end_of_video(mpeg3_t *file, int stream)
{
	int result = 0;
	result = mpeg3demux_eof(file->vtrack[stream]->demuxer);
	return result;
}


int mpeg3_drop_frames(mpeg3_t *file, long frames, int stream)
{
	int result = -1;

	if(file->total_vstreams)
	{
		result = mpeg3video_drop_frames(file->vtrack[stream]->video, 
						frames);
		if(frames > 0) file->vtrack[stream]->current_position += frames;
		file->last_type_read = 2;
		file->last_stream_read = stream;
	}
	return result;
}

int mpeg3_colormodel(mpeg3_t *file, int stream)
{
	if(file->total_vstreams)
	{
		return mpeg3video_colormodel(file->vtrack[stream]->video);
	}
	return 0;
}

int mpeg3_set_rowspan(mpeg3_t *file, int bytes, int stream)
{
	if(file->total_vstreams)
	{
		file->vtrack[stream]->video->row_span = bytes;
	}
	return 0;
}


int mpeg3_read_frame(mpeg3_t *file, 
		unsigned char **output_rows, 
		int in_x, 
		int in_y, 
		int in_w, 
		int in_h, 
		int out_w, 
		int out_h, 
		int color_model,
		int stream)
{
	int result = -1;
//printf("mpeg3_read_frame 1 %d\n", file->vtrack[stream]->current_position);

	if(file->total_vstreams)
	{
		result = mpeg3video_read_frame(file->vtrack[stream]->video, 
					file->vtrack[stream]->current_position, 
					output_rows,
					in_x, 
					in_y, 
					in_w, 
					in_h, 
					out_w,
					out_h,
					color_model);
//printf(__FUNCTION__ " 2\n");
		file->last_type_read = 2;
		file->last_stream_read = stream;
		file->vtrack[stream]->current_position++;
	}

//printf("mpeg3_read_frame 2 %d\n", file->vtrack[stream]->current_position);
	return result;
}

int mpeg3_read_yuvframe(mpeg3_t *file,
		char *y_output,
		char *u_output,
		char *v_output,
		int in_x, 
		int in_y,
		int in_w,
		int in_h,
		int stream)
{
	int result = -1;

//printf("mpeg3_read_yuvframe 1\n");
	if(file->total_vstreams)
	{
		result = mpeg3video_read_yuvframe(file->vtrack[stream]->video, 
					file->vtrack[stream]->current_position, 
					y_output,
					u_output,
					v_output,
					in_x,
					in_y,
					in_w,
					in_h);
		file->last_type_read = 2;
		file->last_stream_read = stream;
		file->vtrack[stream]->current_position++;
	}
//printf("mpeg3_read_yuvframe 100\n");
	return result;
}

int mpeg3_read_yuvframe_ptr(mpeg3_t *file,
		char **y_output,
		char **u_output,
		char **v_output,
		int stream)
{
	int result = -1;

	if(file->total_vstreams)
	{
		result = mpeg3video_read_yuvframe_ptr(file->vtrack[stream]->video, 
					file->vtrack[stream]->current_position, 
					y_output,
					u_output,
					v_output);
		file->last_type_read = 2;
		file->last_stream_read = stream;
		file->vtrack[stream]->current_position++;
	}
	return result;
}

int mpeg3_read_audio(mpeg3_t *file, 
		float *output_f, 
		short *output_i, 
		int channel, 
		long samples,
		int stream)
{
	int result = -1;

	if(file->total_astreams)
	{
		result = mpeg3audio_decode_audio(file->atrack[stream]->audio, 
					output_f, 
					output_i, 
					channel, 
					samples);
		file->last_type_read = 1;
		file->last_stream_read = stream;
		file->atrack[stream]->current_position += samples;
	}

	return result;
}

int mpeg3_reread_audio(mpeg3_t *file, 
		float *output_f, 
		short *output_i, 
		int channel, 
		long samples,
		int stream)
{
	if(file->total_astreams)
	{
		mpeg3_set_sample(file, 
			file->atrack[stream]->current_position - samples,
			stream);
		file->last_type_read = 1;
		file->last_stream_read = stream;
		return mpeg3_read_audio(file, 
			output_f, 
			output_i, 
			channel, 
			samples,
			stream);
	}
	return -1;
}

int mpeg3_read_audio_chunk(mpeg3_t *file, 
		unsigned char *output, 
		long *size, 
		long max_size,
		int stream)
{
	int result = 0;
	if(file->total_astreams)
	{
		result = mpeg3audio_read_raw(file->atrack[stream]->audio, output, size, max_size);
		file->last_type_read = 1;
		file->last_stream_read = stream;
	}
	return result;
}

int mpeg3_read_video_chunk(mpeg3_t *file, 
		unsigned char *output, 
		long *size, 
		long max_size,
		int stream)
{
	int result = 0;
	if(file->total_vstreams)
	{
		result = mpeg3video_read_raw(file->vtrack[stream]->video, output, size, max_size);
		file->last_type_read = 2;
		file->last_stream_read = stream;
	}
	return result;
}
