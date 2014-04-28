#include "assets.h"
#include "byteorder.h"
#include "clip.h"
#include "file.h"
#include "filevorbis.h"
#include "guicast.h"
#include "mwindow.inc"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

FileVorbis::FileVorbis(Asset *asset, File *file, ArrayList<PluginServer*> *plugindb)
 : FileBase(asset, file)
{
	reset_parameters();
	if(asset->format == FILE_UNKNOWN) asset->format = FILE_VORBIS;
	asset->byte_order = 0;
	this->plugindb = plugindb;
}

FileVorbis::~FileVorbis()
{
	close_file();
}

void FileVorbis::get_parameters(BC_WindowBase *parent_window, 
	Asset *asset, 
	BC_WindowBase* &format_window,
	int audio_options,
	int video_options)
{
	if(audio_options)
	{
		VorbisConfig *window = new VorbisConfig(parent_window, asset);
		format_window = window;
		window->create_objects();
		window->run_window();
		delete window;
	}
}

int FileVorbis::check_sig(Asset *asset)
{
	return mpeg3_check_sig(asset->path);
}

int FileVorbis::reset_parameters_derived()
{
	fd = 0;
	video_out = 0;
	audio_out = 0;
	prev_track = 0;
	temp_frame = 0;
	audio_temp = 0;
	audio_allocation = 0;
	last_sample = -1;
}


// Just create the Quicktime objects since this routine is also called
// for reopening.
int FileVorbis::open_file(int rd, int wr)
{
	int result = 0;
	this->rd = rd;
	this->wr = wr;
//printf("FileVorbis::open_file: %d %d %d\n", rd, wr, asset->format);

	if(rd)
	{
		if(!(fd = mpeg3_open(asset->path)))
		{
			printf("FileVorbis::open_file %s\n", asset->path);
			result = 1;
		}
		else
		{
			mpeg3_set_cpus(fd, file->cpus);
			mpeg3_set_mmx(fd, 0);

			asset->audio_data = mpeg3_has_audio(fd);
			if(asset->audio_data)
			{
				asset->channels = 0;
				for(int i = 0; i < mpeg3_total_astreams(fd); i++)
				{
					asset->channels += mpeg3_audio_channels(fd, i);
				}
				if(!asset->sample_rate)
					asset->sample_rate = mpeg3_sample_rate(fd, 0);
				asset->audio_length = mpeg3_audio_samples(fd, 0); 
	//printf("FileVorbis::open_file 1 %d\n", asset->audio_length);
			}

			asset->video_data = mpeg3_has_video(fd);
			if(asset->video_data)
			{
				asset->layers = mpeg3_total_vstreams(fd);
				asset->width = mpeg3_video_width(fd, 0);
				asset->height = mpeg3_video_height(fd, 0);
				asset->video_length = mpeg3_video_frames(fd, 0);
				if(!asset->frame_rate)
					asset->frame_rate = mpeg3_frame_rate(fd, 0);
	//printf("FileVorbis::open_file 2 %d\n", asset->video_length);
			}
		}
	}
	if(wr && asset->format == FILE_AVORBIS)
	{
		char command_line[BCTEXTLEN];
		char *encoder_string;
		char argument_string[BCTEXTLEN];
		char exec_string[BCTEXTLEN];


		if(asset->audio_derivative == 2)
		{
			encoder_string = "toolame.plugin";
			sprintf(argument_string, " %s -s %f -b %d - %s", 
				(asset->channels >= 2) ? "-m j" : "-m m", 
				(float)asset->sample_rate / 1000,
				asset->audio_bitrate,
				asset->path);
		}
		else
		if(asset->audio_derivative == 3)
		{
			encoder_string = "lame.plugin";
			sprintf(argument_string, " %s -s %f -h -b %d - %s",
				(asset->channels >= 2) ? "-m j" : "-m m",
				(float)asset->sample_rate / 1000,
				asset->audio_bitrate,
				asset->path);
		}
		else
		{
			printf("FileVorbis::open_file: audio_derivative=%d\n", asset->audio_derivative);
			result = 1;
		}

// Look up executable plugin
		if(!result)
		{
			for(int i = 0; i < plugindb->total; i++)
			{
				if(strstr(plugindb->values[i]->title, encoder_string))
					strcpy(exec_string, plugindb->values[i]->title);
			}


			if(!strlen(exec_string))
			{
				printf("FileVorbis::open_file: couldn't find %s plugin.\n", encoder_string);
				result = 1;
			}
		}

		if(!result)
		{
			sprintf(command_line, "%s%s", exec_string, argument_string);
printf("FileVorbis::open_file %s\n", command_line);
			audio_out = new ThreadFork;
			audio_out->start_command(command_line, 1);
		}
	}

	return result;
}

int FileVorbis::close_file()
{
//printf("FileVorbis::close_file_derived 1\n");
	if(fd)
	{
		mpeg3_close(fd);
	}
	
	if(video_out)
	{
// End of sequence signal
		unsigned char data[5];
		data[0] = data[1] = data[2] = data[3] = 0xff;
		fwrite(data, 4, 1, video_out->get_stdin());
		fflush(video_out->get_stdin());
		
		delete video_out;
		video_out = 0;
	}

	if(audio_out)
	{
		fflush(audio_out->get_stdin());
		delete audio_out;
		audio_out = 0;
	}

//printf("FileVorbis::close_file_derived 1\n");
	if(temp_frame) delete temp_frame;

	if(audio_temp) delete audio_temp;

//printf("FileVorbis::close_file_derived 1\n");
	reset_parameters();
	FileBase::close_file();
//printf("FileVorbis::close_file_derived 2\n");
	return 0;
}


int FileVorbis::get_best_colormodel(int driver, int colormodel)
{
	if(colormodel > -1)
	{
		return colormodel;
	}
	else
	{
		switch(driver)
		{
			case PLAYBACK_X11:
				return BC_RGB888;
				break;
			case PLAYBACK_X11_XV:
				if(mpeg3_colormodel(fd, 0) == VORBIS3_YUV420P) return BC_YUV420P;
				if(mpeg3_colormodel(fd, 0) == VORBIS3_YUV422P) return BC_YUV422P;
				break;
			case PLAYBACK_LML:
			case PLAYBACK_BUZ:
				return BC_YUV422P;
				break;
		}
	}
	return colormodel;
}

int FileVorbis::can_copy_from(Edit *edit, long position)
{
	if(!fd) return 0;
	return 0;
}

int FileVorbis::set_audio_position(long sample)
{
	if(!fd) return 1;
	
	int channel, stream;
	to_streamchannel(file->current_channel, stream, channel);

//printf("FileVorbis::set_audio_position %d %d %d\n", sample, mpeg3_get_sample(fd, stream), last_sample);
	if(sample != mpeg3_get_sample(fd, stream) &&
		sample != last_sample)
	{
		if(sample >= 0 && sample < asset->audio_length)
			return mpeg3_set_sample(fd, sample, stream);
		else
			return 1;
	}
	return 0;
}

int FileVorbis::set_video_position(long x)
{
	if(!fd) return 1;
	if(x >= 0 && x < asset->video_length)
		return mpeg3_set_frame(fd, x, file->current_layer);
	else
		return 1;
}

void FileVorbis::new_audio_temp(long len)
{
}



int FileVorbis::write_samples(double **buffer, long len)
{
	int result = 0;

	if(audio_out)
	{
// Convert to int16
		int channels = MIN(asset->channels, 2);
		long audio_size = len * channels * 2;
		if(audio_allocation < audio_size)
		{
			if(audio_temp) delete [] audio_temp;
			audio_temp = new unsigned char[audio_size];
			audio_allocation = audio_size;
		}
		
		for(int i = 0; i < channels; i++)
		{
			int16_t *output = ((int16_t*)audio_temp) + i;
			double *input = buffer[i];
			for(int j = 0; j < len; j++)
			{
				int sample = (int)(*input * 0x7fff);
				*output = (int16_t)(CLAMP(sample, -0x8000, 0x7fff));
				output += channels;
				input++;
			}
		}
		
		result = !fwrite(audio_temp, 
			audio_size, 
			1, 
			audio_out->get_stdin());
	}

	return result;
}

int FileVorbis::write_frames(VFrame ***frames, int len)
{
	int result = 0;

	if(video_out)
	{
//printf("FileVorbis::write_frames 1 %d\n", asset->mpeg_cmodel);
		int temp_w = (int)((asset->width + 15) / 16) * 16;
		int temp_h = (int)((asset->height + 15) / 16) * 16;
		int output_cmodel = 
			(asset->mpeg_cmodel == 0) ? BC_YUV420P : BC_YUV422P;
		
// Only 1 layer is supported in Vorbis output
		for(int i = 0; i < 1; i++)
		{
			for(int j = 0; j < len; j++)
			{
				VFrame *frame = frames[i][j];
				
				
				
				if(frame->get_w() == temp_w &&
					frame->get_h() == temp_h &&
					frame->get_color_model() == output_cmodel)
				{
					result = !fwrite(frame->get_data(), 
						frame->get_data_size(), 
						1, 
						video_out->get_stdin());
				}
				else
				{
//printf("FileVorbis::write_frames 2\n");
					if(temp_frame&&
						(temp_frame->get_w() != temp_w ||
						temp_frame->get_h() != temp_h ||
						temp_frame->get_color_model() || output_cmodel))
					{
						delete temp_frame;
						temp_frame = 0;
					}
//printf("FileVorbis::write_frames 3\n");


					if(!temp_frame)
					{
						temp_frame = new VFrame(0, 
							temp_w, 
							temp_h, 
							output_cmodel);
					}

					cmodel_transfer(temp_frame->get_rows(), 
						frame->get_rows(),
						temp_frame->get_y(),
						temp_frame->get_u(),
						temp_frame->get_v(),
						frame->get_y(),
						frame->get_u(),
						frame->get_v(),
						0,
						0,
						asset->width,
						asset->height,
						0,
						0,
						asset->width,
						asset->height,
						frame->get_color_model(), 
						temp_frame->get_color_model(),
						0, 
						frame->get_w(),
						temp_w);

//printf("FileVorbis::write_frames %d %d\n", frame->get_color_model(), temp_frame->get_color_model());

// printf("FileVorbis::write_frames %p %d %d %d %d\n", temp_frame->get_data(), 
// 						temp_frame->get_data_size(),
// 						temp_frame->get_color_model(),
// 						temp_frame->get_w(),
// 						temp_frame->get_h());

// Not end of sequence signal
					unsigned char data[5];
					data[0] = data[1] = data[2] = data[3] = 0x0;
					fwrite(data, 4, 1, video_out->get_stdin());
					result = !fwrite(temp_frame->get_data(), 
						temp_frame->get_data_size(), 
						1, 
						video_out->get_stdin());
//printf("FileVorbis::write_frames 5\n");
				}
			}
		}
	}



	return result;
}

int FileVorbis::read_frame(VFrame *frame)
{
	if(!fd) return 1;
	int result = 0;
	int src_cmodel;
	
	if(mpeg3_colormodel(fd, 0) == VORBIS3_YUV420P)
		src_cmodel = BC_YUV420P;
	else
	if(mpeg3_colormodel(fd, 0) == VORBIS3_YUV422P)
		src_cmodel = BC_YUV422P;

//printf("FileVorbis::read_frame 1 %p %d\n", frame, frame->get_color_model());
	switch(frame->get_color_model())
	{
		case VORBIS3_RGB565:
		case VORBIS3_BGR888:
		case VORBIS3_BGRA8888:
		case VORBIS3_RGB888:
		case VORBIS3_RGBA8888:
		case VORBIS3_RGBA16161616:
			mpeg3_read_frame(fd, 
					frame->get_rows(), /* Array of pointers to the start of each output row */
					0,                    /* Location in input frame to take picture */
					0, 
					asset->width, 
					asset->height, 
					asset->width,                   /* Dimensions of output_rows */
					asset->height, 
					frame->get_color_model(),             /* One of the color model #defines */
					file->current_layer);
			break;

// Use Temp
		default:
// Read these directly
			if(frame->get_color_model() == src_cmodel)
			{
				mpeg3_read_yuvframe(fd,
					(char*)frame->get_y(),
					(char*)frame->get_u(),
					(char*)frame->get_v(),
					0,
					0,
					asset->width,
					asset->height,
					file->current_layer);
			}
			else
// Process through temp frame
			{
				char *y, *u, *v;
				mpeg3_read_yuvframe_ptr(fd,
					&y,
					&u,
					&v,
					file->current_layer);
				cmodel_transfer(frame->get_rows(), 
					0,
					frame->get_y(),
					frame->get_u(),
					frame->get_v(),
					(unsigned char*)y,
					(unsigned char*)u,
					(unsigned char*)v,
					0,
					0,
					asset->width,
					asset->height,
					0,
					0,
					asset->width,
					asset->height,
					src_cmodel, 
					frame->get_color_model(),
					0, 
					asset->width,
					frame->get_w());
			}
//printf("FileVorbis::read_frame 2\n");
			break;
	}
//for(int i = 0; i < frame->get_w() * 3 * 20; i++) 
//	((u_int16_t*)frame->get_rows()[0])[i] = 0xffff;
//printf("FileVorbis::read_frame 3\n");
	return result;
}

void FileVorbis::to_streamchannel(int channel, int &stream_out, int &channel_out)
{
	for(stream_out = 0, channel_out = file->current_channel; 
		stream_out < mpeg3_total_astreams(fd) && 
			channel_out >= mpeg3_audio_channels(fd, stream_out);
		channel_out -= mpeg3_audio_channels(fd, stream_out), stream_out++)
	;
}

FileVorbisBuffer* FileVorbis::oldest_buffer()
{
	int oldest_age = 0, oldest_i = 0;
	for(int i = 0; i < HISTORY_SIZE; i++)
	{
		if(history[i].age > oldest_age)
		{
			oldest_age = history[i].age;
			oldest_i = i;
		}
	}

	return &history[oldest_i];
}

int FileVorbis::read_samples(double *buffer, long len)
{
	if(!fd) return 0;

	new_audio_temp(len);

// This is directed to a FileVorbisBuffer
	float *temp_float = 0;
	int found = 0;
	int stream, channel;
	to_streamchannel(file->current_channel, stream, channel);
	
	
	
//printf("FileVorbis::read_samples 1 %ld %ld %d %d\n", file->current_sample, last_sample, len, channel);



	for(int i = 0; i < HISTORY_SIZE; i++)
	{
		history[i].age++;
	}

// Search history
	for(int i = 0; i < HISTORY_SIZE && !found; i++)
	{
		if(history[i].allocated && 
			history[i].sample == file->current_sample &&
			history[i].channel == channel &&
			history[i].stream == stream &&
			history[i].len == len)
		{
			found = 1;
			temp_float = history[i].data;
			history[i].age = 0;
//printf("FileVorbis::read_samples: got history #: %d channel: %d stream: %d len: %d\n",
//	i, history[i].channel, history[i].stream, len);
		}
	}

// Load new samples for all channels in the current stream.
// Replace oldest FileVorbisBuffers
	if(!found)
	{
//printf("FileVorbis::read_samples: reading history stream: %d\n",
//	stream);
		for(int i = 0; i < mpeg3_audio_channels(fd, stream); i++)
		{
			FileVorbisBuffer *dest = oldest_buffer();

			dest->reallocate(len);
			dest->len = len;
			dest->sample = file->current_sample;
			dest->channel = i;
			dest->stream = stream;
			dest->age = 0;
			
			if(i == 0)
				mpeg3_read_audio(fd, 
					dest->data,      /* Pointer to pre-allocated buffer of floats */
					0,      /* Pointer to pre-allocated buffer of int16's */
					i,          /* Channel to decode */
					len,         /* Number of samples to decode */
					stream);          /* Stream containing the channel */
			else
				mpeg3_reread_audio(fd, 
					dest->data,      /* Pointer to pre-allocated buffer of floats */
					0,      /* Pointer to pre-allocated buffer of int16's */
					i,          /* Channel to decode */
					len,         /* Number of samples to decode */
					stream);          /* Stream containing the channel */
		
			if(i == channel)
				temp_float = dest->data;
		}
	}


	last_sample = file->current_sample;
	for(int i = 0; i < len; i++) buffer[i] = temp_float[i];

	return 0;
}

char* FileVorbis::strtocompression(char *string)
{
	return "";
}

char* FileVorbis::compressiontostr(char *string)
{
	return "";
}










VorbisConfigAudio::VorbisConfigAudio(BC_WindowBase *parent_window, Asset *asset)
 : BC_Window(PROGRAM_NAME ": Audio Compression",
 	parent_window->get_abs_cursor_x(),
 	parent_window->get_abs_cursor_y(),
	310,
	120,
	-1,
	-1,
	0,
	0,
	1)
{
	this->parent_window = parent_window;
	this->asset = asset;
}

VorbisConfigAudio::~VorbisConfigAudio()
{
}

int VorbisConfigAudio::create_objects()
{
	int x = 10, y = 10;
	int x1 = 150;
	VorbisLayer *layer;

	add_tool(new BC_Title(x, y, "Layer:"));
	add_tool(layer = new VorbisLayer(x1, y, this));
	layer->create_objects();

	y += 30;
	add_tool(new BC_Title(x, y, "Kbits per second:"));
	add_tool(bitrate = new VorbisABitrate(x1, y, this));
	bitrate->create_objects();
	
	
	add_subwindow(new BC_OKButton(this));
	show_window();
	flush();
	return 0;
}

int VorbisConfigAudio::close_event()
{
	set_done(0);
	return 1;
}







VorbisLayer::VorbisLayer(int x, int y, VorbisConfigAudio *gui)
 : BC_PopupMenu(x, y, 150, layer_to_string(gui->asset->audio_derivative))
{
	this->gui = gui;
}

void VorbisLayer::create_objects()
{
	add_item(new BC_MenuItem(layer_to_string(2)));
	add_item(new BC_MenuItem(layer_to_string(3)));
}

int VorbisLayer::handle_event()
{
	gui->asset->audio_derivative = string_to_layer(get_text());
	gui->bitrate->set_layer(gui->asset->audio_derivative);
	return 1;
};

int VorbisLayer::string_to_layer(char *string)
{
	if(!strcasecmp(layer_to_string(2), string))
		return 2;
	if(!strcasecmp(layer_to_string(3), string))
		return 3;

	return 1;
}

char* VorbisLayer::layer_to_string(int layer)
{
	switch(layer)
	{
		case 2:
			return "II";
			break;
		
		case 3:
			return "III";
			break;
			
		default:
			return "II";
			break;
	}
}







VorbisABitrate::VorbisABitrate(int x, int y, VorbisConfigAudio *gui)
 : BC_PopupMenu(x, y, 150, bitrate_to_string(gui->string, gui->asset->audio_bitrate))
{
	this->gui = gui;
}

void VorbisABitrate::create_objects()
{
	set_layer(gui->asset->audio_derivative);
}

void VorbisABitrate::set_layer(int layer)
{
	while(total_items())
	{
		remove_item(0);
	}

	if(layer == 2)
	{
		add_item(new BC_MenuItem("160"));
		add_item(new BC_MenuItem("192"));
		add_item(new BC_MenuItem("224"));
		add_item(new BC_MenuItem("256"));
		add_item(new BC_MenuItem("320"));
		add_item(new BC_MenuItem("384"));
	}
	else
	{
		add_item(new BC_MenuItem("8"));
		add_item(new BC_MenuItem("16"));
		add_item(new BC_MenuItem("24"));
		add_item(new BC_MenuItem("32"));
		add_item(new BC_MenuItem("40"));
		add_item(new BC_MenuItem("48"));
		add_item(new BC_MenuItem("56"));
		add_item(new BC_MenuItem("64"));
		add_item(new BC_MenuItem("80"));
		add_item(new BC_MenuItem("96"));
		add_item(new BC_MenuItem("112"));
		add_item(new BC_MenuItem("128"));
		add_item(new BC_MenuItem("144"));
		add_item(new BC_MenuItem("160"));
		add_item(new BC_MenuItem("192"));
		add_item(new BC_MenuItem("224"));
		add_item(new BC_MenuItem("256"));
		add_item(new BC_MenuItem("320"));
	}
}

int VorbisABitrate::handle_event()
{
	gui->asset->audio_bitrate = string_to_bitrate(get_text());
	return 1;
};

int VorbisABitrate::string_to_bitrate(char *string)
{
	return atol(string);
}


char* VorbisABitrate::bitrate_to_string(char *string, int bitrate)
{
	sprintf(string, "%d", bitrate);
	return string;
}
















FileVorbisBuffer::FileVorbisBuffer()
{
	len = 0;
	allocated = 0;
	sample = 0;
	channel = 0;
	stream = 0;
	data = 0;
	age = 0;
}

FileVorbisBuffer::~FileVorbisBuffer()
{
//printf("FileVorbisBuffer::~FileVorbisBuffer %p\n", data);
	if(data) delete [] data;
}

void FileVorbisBuffer::reallocate(long len)
{
	if(data && len > allocated)
	{
		delete [] data;
		allocated = 0;
		data = 0;
	}

	if(!data)
	{
		data = new float[len];
		allocated = len;
	}

	age = 0;
}



