
/*
 * CINELERRA
 * Copyright (C) 2009 Adam Williams <broadcast at earthling dot net>
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

#include "bcdisplayinfo.h"
#include "bchash.h"
#include "filexml.h"
#include "language.h"
#include "resamplert.h"
#include "transportque.h"

#include <string.h>







REGISTER_PLUGIN(ResampleRT);



ResampleRTConfig::ResampleRTConfig()
{
	scale = 1;
}


int ResampleRTConfig::equivalent(ResampleRTConfig &src)
{
	return fabs(scale - src.scale) < 0.0001;
}

void ResampleRTConfig::copy_from(ResampleRTConfig &src)
{
	this->scale = src.scale;
}

void ResampleRTConfig::interpolate(ResampleRTConfig &prev, 
	ResampleRTConfig &next, 
	int64_t prev_frame, 
	int64_t next_frame, 
	int64_t current_frame)
{
	this->scale = prev.scale;
}

void ResampleRTConfig::boundaries()
{
	if(fabs(scale) < 0.0001) scale = 0.0001;
}




ResampleRTWindow::ResampleRTWindow(ResampleRT *plugin)
 : PluginClientWindow(plugin, 
	210, 
	160, 
	200, 
	160, 
	0)
{
	this->plugin = plugin;
}

ResampleRTWindow::~ResampleRTWindow()
{
}

void ResampleRTWindow::create_objects()
{
	int x = 10, y = 10;

	scale = new ResampleRTScale(this,
		plugin, 
		x, 
		y);
	scale->create_objects();
	show_window();
}






ResampleRTScale::ResampleRTScale(ResampleRTWindow *window,
	ResampleRT *plugin, 
	int x, 
	int y)
 : BC_TumbleTextBox(window,
 	plugin->config.scale,
	0.0001,
	1000,
 	x, 
	y, 
	100)
{
	this->plugin = plugin;
}

int ResampleRTScale::handle_event()
{
	plugin->config.scale = atof(get_text());
	plugin->send_configure_change();
	return 1;
}






ResampleRTResample::ResampleRTResample(ResampleRT *plugin)
 : Resample()
{
	this->plugin = plugin;
}

// To get the keyframes to work, resampling is always done in the forward
// direction with the plugin converting to reverse.
int ResampleRTResample::read_samples(double *buffer, int64_t start, int64_t len)
{
	int64_t position = plugin->source_start;

	if(plugin->get_direction() == PLAY_FORWARD)
		position += start;
	else
		position -= start;

	return plugin->read_samples(buffer,
		0,
		plugin->get_samplerate(),
		position,
		len);
}






ResampleRT::ResampleRT(PluginServer *server)
 : PluginAClient(server)
{
	resample = 0;
	need_reconfigure = 1;
	prev_scale = 0;
	dest_end = 0;
}


ResampleRT::~ResampleRT()
{
	delete resample;
}

const char* ResampleRT::plugin_title() { return N_("Reverse audio"); }
int ResampleRT::is_realtime() { return 1; }

#include "picon_png.h"
NEW_PICON_MACRO(ResampleRT)

NEW_WINDOW_MACRO(ResampleRT, ResampleRTWindow)

LOAD_CONFIGURATION_MACRO(ResampleRT, ResampleRTConfig)


int ResampleRT::process_buffer(int64_t size, 
	double *buffer,
	int64_t start_position,
	int sample_rate)
{
	if(!resample) resample = new ResampleRTResample(this);

	load_configuration();
	
	int64_t new_dest_end = start_position;
	if(get_direction() == PLAY_FORWARD)
		new_dest_end += size;
	else
		new_dest_end -= size;

	if(prev_scale != config.scale ||
		new_dest_end != dest_end) need_reconfigure = 1;

// Get start position of current samplerate segment in source 
// by accounting for all previous keyframes
	if(need_reconfigure)
	{
		int64_t total_samples = 0;
		int64_t current_position = get_source_start();
		KeyFrame *keyframe = get_prev_keyframe(current_position, 1);
		int64_t keyframe_position = 0;
		int64_t segment_samples = 0;
		do
		{
			read_data(keyframe);
			int64_t new_keyframe_position = edl_to_local(keyframe->position);
			if(new_keyframe_position == 0) new_keyframe_position = get_source_start();
			if(keyframe_position == new_keyframe_position ||
				new_keyframe_position > start_position)
				break;

			keyframe_position = new_keyframe_position;
			segment_samples = keyframe_position - current_position;
			total_samples += (int64_t)(segment_samples * config.scale);
			current_position = keyframe_position;
			keyframe = get_next_keyframe(current_position, 1);
		}while(keyframe && current_position < start_position);

		load_configuration();
		segment_samples = start_position - keyframe_position;
		total_samples += (int64_t)(segment_samples * config.scale);
		source_start = total_samples;
		dest_start = start_position;
		need_reconfigure = 0;
	}

	resample->resample(buffer,
		size,
		1000000.0,
		(int)(1000000 * config.scale),
		start_position - dest_start,
		PLAY_FORWARD);	

	dest_end = new_dest_end;
	return 0;
}

void ResampleRT::render_stop()
{
	need_reconfigure = 1;
}



int ResampleRT::load_defaults()
{
	char directory[BCTEXTLEN];
// set the default directory
	sprintf(directory, "%sresamplert.rc", BCASTDIR);

// load the defaults
	defaults = new BC_Hash(directory);
	defaults->load();

	config.scale = defaults->get("SCALE", config.scale);
	return 0;
}

int ResampleRT::save_defaults()
{
	defaults->update("SCALE", config.scale);
	defaults->save();
	return 0;
}

void ResampleRT::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause data to be stored directly in text
	output.set_shared_string(keyframe->get_data(), MESSAGESIZE);
	output.tag.set_title("RESAMPLERT");
	output.tag.set_property("SCALE", config.scale);
	output.append_tag();
	output.terminate_string();
}

void ResampleRT::read_data(KeyFrame *keyframe)
{
	FileXML input;

	input.set_shared_string(keyframe->get_data(), strlen(keyframe->get_data()));

	int result = 0;

	while(!input.read_tag())
	{
		if(input.tag.title_is("RESAMPLERT"))
		{
			config.scale = input.tag.get_property("SCALE", config.scale);
		}
	}
}

void ResampleRT::update_gui()
{
	if(thread)
	{
		load_configuration();
		thread->window->lock_window("ResampleRT::update_gui");
		((ResampleRTWindow*)thread->window)->scale->update((float)config.scale);
		thread->window->unlock_window();
	}
}





