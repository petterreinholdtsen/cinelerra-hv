#include "aedit.h"
#include "amodule.h"
#include "arender.h"
#include "assets.h"
#include "atrack.h"
#include "audiodevice.h"
#include "cache.h"
#include "console.h"
#include "edit.h"
#include "edits.h"
#include "edl.h"
#include "edlsession.h"
#include "file.h"
#include "levelwindow.h"
#include "mwindow.h"
#include "modules.h"
#include "playabletracks.h"
#include "plugin.h"
#include "pluginbuffer.h"
#include "preferences.h"
#include "renderengine.h"
#include "thread.h"
#include "tracks.h"
#include "transportque.h"
#include "virtualaconsole.h"
#include "virtualanode.h"
#include "virtualnode.h"


VirtualAConsole::VirtualAConsole(RenderEngine *renderengine, ARender *arender)
 : VirtualConsole(renderengine, arender, TRACK_AUDIO)
{
//printf("VirtualAConsole::VirtualAConsole\n");
	this->arender = arender;
}

VirtualAConsole::~VirtualAConsole()
{
	for(int i = 0; i < ring_buffers; i++)
	{
		delete_input_buffer(i);
	}
}

int VirtualAConsole::total_ring_buffers()
{
	return renderengine->command->realtime ? 2 : 1;
}

void VirtualAConsole::get_playable_tracks()
{
	if(!playable_tracks)
		playable_tracks = new PlayableTracks(renderengine, 
			renderengine->config->aconfig->do_channel, 
			commonrender->current_position, 
			TRACK_AUDIO);
}

void VirtualAConsole::new_input_buffer(int ring_buffer)
{
	buffer_in[ring_buffer] = new double*[total_tracks];
	for(int i = 0; i < total_tracks; i++)
	{
		buffer_in[ring_buffer][i] = new double[renderengine->edl->session->audio_read_length];
	}
}

void VirtualAConsole::delete_input_buffer(int ring_buffer)
{
	for(int i = 0; i < total_tracks; i++)
	{
		delete [] buffer_in[ring_buffer][i];
	}
	delete [] buffer_in[ring_buffer];
}

VirtualNode* VirtualAConsole::new_toplevel_node(Track *track, 
	Module *module,
	int track_number)
{
	double *track_buffer[RING_BUFFERS];
//printf("VirtualAConsole::new_toplevel_node %p\n", module);
	for(int i = 0; i < total_ring_buffers(); i++)
		track_buffer[i] = buffer_in[i][track_number];
	return new VirtualANode(renderengine,
		this, 
		module,
		0,
		track,
		0,
		track_buffer,
		track_buffer,
		1,
		1,
		1,
		1);
	return 0;
}

int VirtualAConsole::stop_rendering(int duplicate)
{
	if(renderengine->command->realtime)
	{
		Thread::join();
	}
	return 0;
}


int VirtualAConsole::process_buffer(long input_len,
	long input_position,
	int last_buffer,
	long absolute_position)
{
// printf("VirtualAConsole::process_buffer 1 %d\n", current_input_buffer);
	int result = 0;
// wait for an input_buffer to become available
	if(renderengine->command->realtime)
		output_lock[current_input_buffer]->lock();

// printf("VirtualAConsole::process_buffer 2 %p\n", virtual_modules);
	if(!interrupt)
	{
// Load tracks
		double **buffer_in = this->buffer_in[current_input_buffer];

// printf("VirtualAConsole::process_buffer 3 %ld\n", input_position);
		for(int i = 0; i < total_tracks; i++)
		{
			result |= ((AModule*)virtual_modules[i]->real_module)->render(buffer_in[i],
				input_len, 
				input_position,
				renderengine->command->get_direction());
		}

// printf("VirtualAConsole::process_buffer 4\n");
		this->input_len[current_input_buffer] = input_len;
		this->input_position[current_input_buffer] = input_position;
		this->last_playback[current_input_buffer] = last_buffer;
		this->last_reconfigure[current_input_buffer] = 0;
		this->absolute_position[current_input_buffer] = absolute_position;

		if(renderengine->command->realtime)
			input_lock[current_input_buffer]->unlock();
		else
			process_console();

// printf("VirtualAConsole::process_buffer 5\n");

		swap_input_buffer();
// printf("VirtualAConsole::process_buffer 6\n");
	}
	return result;
}

void VirtualAConsole::process_console()
{
	int i, j, k;
// length and lowest numbered sample of fragment in input buffer
	int buffer = current_vconsole_buffer;
	long fragment_len, fragment_position;	
// generic buffer
	double *current_buffer;
// starting sample of fragment in project
	long real_position;
// info for meters
	double min, max, peak;
	long meter_render_end;    // end of current meter fragment for getting levels
	long current_fragment_peak; // first meter peak in fragment
	long input_len = this->input_len[buffer];
	long input_position = this->input_position[buffer];
	long absolute_position = this->absolute_position[buffer];



//printf("VirtualAConsole::process_console 1 %d\n", input_len);
// process entire input buffer by filling one output buffer at a time
	for(fragment_position = 0; 
		fragment_position < input_len && !interrupt; )
	{

// test for end of input buffer
		fragment_len = renderengine->edl->session->audio_module_fragment;
		if(fragment_position + fragment_len > input_len)
			fragment_len = input_len - fragment_position;

//printf("VirtualAConsole::process_console 2 %d %d %p\n", fragment_position, fragment_len, arender);
//printf("VirtualAConsole::process_console 2.1 %p\n", arender->audio_out);

// clear output buffers
		for(i = 0; i < MAX_CHANNELS; i++)
		{
//printf("VirtualAConsole::process_console 2.2 %p\n", arender->audio_out[i]);
			if(arender->audio_out[i])
			{
				bzero(arender->audio_out[i], fragment_len * sizeof(double));
			}
		}

//printf("VirtualAConsole::process_console 3\n");

// get the start of the fragment in the project
		real_position = 
			(renderengine->command->get_direction() == PLAY_REVERSE) ? 
				input_position - fragment_position : 
				input_position + fragment_position;

//printf("VirtualAConsole::process_console 4 %d\n", render_list.total);

// render nodes in sorted list
		for(i = 0; i < render_list.total; i++)
		{
			((VirtualANode*)render_list.values[i])->render(arender->audio_out, 
					0, 
					buffer,
					fragment_position,
					fragment_len, 
					real_position, 
					arender->source_length,
					renderengine->reverse,
					arender);
		}

//printf("VirtualAConsole::process_console 5 %d\n", fragment_len);
// get peaks and limit volume in the fragment
		for(i = 0; i < MAX_CHANNELS; i++)
		{
			current_buffer = arender->audio_out[i];
			if(current_buffer)
			{

//printf("VirtualAConsole::process_console 6 %d %d\n", fragment_len, arender->meter_render_fragment);
				for(j = 0; j < fragment_len; )
				{
// Get length to test for meter
					if(renderengine->command->realtime)
						meter_render_end = j + arender->meter_render_fragment;
					else
						meter_render_end = fragment_len;

					if(meter_render_end > fragment_len) 
						meter_render_end =  fragment_len;

					min = max = 0;

//printf("VirtualAConsole::process_console 7 %d %d\n", j, meter_render_end);
					for( ; j < meter_render_end; j++)
					{
// Level history comes before clipping to get over status
						if(current_buffer[j] > max) max = current_buffer[j];
						else
						if(current_buffer[j] < min) min = current_buffer[j];

						if(current_buffer[j] > 1) current_buffer[j] = 1;
						else
						if(current_buffer[j] < -1) current_buffer[j] = -1;
					}

//printf("VirtualAConsole::process_console 8 %ld %ld\n", real_position, j);

					if(fabs(max) > fabs(min))
						peak = fabs(max);
					else
						peak = fabs(min);

 					if(renderengine->command->realtime)
 					{
						arender->level_history[i][arender->current_level[i]] = peak;
						arender->level_samples[arender->current_level[i]] = 
							(renderengine->command->get_direction() == PLAY_REVERSE) ? 
							real_position - j : 
							real_position + j;
 						arender->current_level[i] = arender->get_next_peak(arender->current_level[i]);
//for(int k = 0; k <= arender->current_level[i]; k++)
//	printf("%ld ", arender->level_samples[k]);
//printf("\n");
 					}
				}

//printf("VirtualAConsole::process_console 9 %d\n", fragment_len);
			}
		}

//printf("VirtualAConsole::process_console 10\n");

// advance fragment
		fragment_position += fragment_len;
//printf("VirtualAConsole::process_console 11\n");

// Pack channels, fix speed and send to device.
		if(renderengine->command->realtime && !interrupt)
		{
// speed parameters
			long real_output_len; // length compensated for speed
			double sample;       // output sample
			int k;
			double *audio_out_packed[MAX_CHANNELS];

//printf("VirtualAConsole::process_console 12\n");
			for(i = 0, j = 0; i < MAX_CHANNELS; i++)
			{
				if(renderengine->config->aconfig->do_channel[i])
				{
					audio_out_packed[j++] = arender->audio_out[i];
				}
			}

//printf("VirtualAConsole::process_console 13 %f\n", renderengine->command->get_speed());
			for(i = 0; 
				i < renderengine->config->aconfig->total_playable_channels(); 
				i++)
			{
				long in, out;
				long fragment_end;

				current_buffer = audio_out_packed[i];

// Time stretch the fragment to the real_output size
				if(renderengine->command->get_speed() > 1)
				{
// Number of samples in real output buffer for each to sample rendered.
					int interpolate_len = (int)renderengine->command->get_speed();
					for(in = 0, out = 0; in < fragment_len; )
					{
						sample = 0;
						for(k = 0; k < interpolate_len; k++)
						{
							sample += current_buffer[in++];
						}
						sample /= renderengine->command->get_speed();
						current_buffer[out++] = sample;
					}
					real_output_len = out;
				}
				else
				if(renderengine->command->get_speed() < 1)
				{
					int interpolate_len = (int)(1 / renderengine->command->get_speed()); // number of samples to skip
					real_output_len = fragment_len * interpolate_len;

					for(in = fragment_len - 1, out = real_output_len - 1; in >= 0; )
					{
						for(k = 0; k < interpolate_len; k++)
						{
							current_buffer[out--] = current_buffer[in];
						}
						in--;
					}
				}
				else
					real_output_len = fragment_len;
			}
//printf("VirtualAConsole::process_console 14\n");

			if(!renderengine->audio->get_interrupted())
			{
//printf("VirtualAConsole::process_console 15 %f %f %f %f %f\n", audio_out_packed[0][0], audio_out_packed[0][1], audio_out_packed[0][2], audio_out_packed[0][3], audio_out_packed[0][4]);
				renderengine->audio->write_buffer(audio_out_packed, 
					real_output_len, 
					renderengine->config->aconfig->total_playable_channels());
//printf("VirtualAConsole::process_console 16\n");
			}

			if(renderengine->audio->get_interrupted()) interrupt = 1;
		}
//printf("VirtualAConsole::process_console 17\n");

	}
//printf("VirtualAConsole::process_console 18\n");
}


void VirtualAConsole::run()
{
	startup_lock->unlock();
//printf("VirtualConsole::run 0\n");

	while(!done && !interrupt)
	{
//printf("VirtualConsole::run 1\n");
// wait for a buffer to render through console
		input_lock[current_vconsole_buffer]->lock();

//printf("VirtualConsole::run 2\n");
		if(!done && !interrupt && !last_reconfigure[current_vconsole_buffer])
		{
// render it if not last buffer
// send to output device or the previously set output buffer
//printf("VirtualConsole::run 3 %d\n", current_vconsole_buffer);
			process_console();
//printf("VirtualConsole::run 4\n");

// test for exit conditions tied to the buffer
			if(last_playback[current_vconsole_buffer]) done = 1;
//printf("VirtualConsole::run 2\n");

// free up buffer for reading from disk
			output_lock[current_vconsole_buffer]->unlock();

//printf("VirtualConsole::run 2\n");
// get next buffer
			if(!done) swap_thread_buffer();
//printf("VirtualConsole::run 2\n");
		}
		else
		if(last_reconfigure[current_vconsole_buffer])
			done = 1;
//printf("VirtualConsole::run 3\n");
	}

//printf("VirtualConsole::run 4\n");
	if(interrupt)
	{
//		commonrender->interrupt = 1;
		for(int i = 0; i < total_ring_buffers(); i++)
		{
			output_lock[i]->unlock();
		}
	}
	else
	if(!last_reconfigure[current_vconsole_buffer])
	{
		if(renderengine->command->realtime)
			send_last_output_buffer();
	}
//printf("VirtualConsole::run 5\n");
}


























VirtualAConsole::VirtualAConsole(MWindow *mwindow, ARender *arender)
 : VirtualConsole(mwindow, arender)
{
	this->arender = arender;
}

int VirtualAConsole::init_rendering(int duplicate)
{
	return 0;
}


int VirtualAConsole::send_last_output_buffer()
{
	renderengine->audio->set_last_buffer();
	return 0;
}

