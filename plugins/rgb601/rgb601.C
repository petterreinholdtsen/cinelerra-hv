#include "clip.h"
#include "colormodels.h"
#include "filexml.h"
#include "picon_png.h"
#include "rgb601.h"
#include "rgb601window.h"

#include <stdio.h>
#include <string.h>

PluginClient* new_plugin(PluginServer *server)
{
	return new RGB601Main(server);
}


RGB601Config::RGB601Config()
{
	direction = 0;
}

RGB601Main::RGB601Main(PluginServer *server)
 : PluginVClient(server)
{
	thread = 0;
	load_defaults();
}

RGB601Main::~RGB601Main()
{
	if(thread)
	{
// Set result to 0 to indicate a server side close
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}

	save_defaults();
	delete defaults;
}

char* RGB601Main::plugin_title() { return "RGB - 601"; }
int RGB601Main::is_realtime() { return 1; }

VFrame* RGB601Main::new_picon()
{
	return new VFrame(picon_png);
}

int RGB601Main::load_defaults()
{
	char directory[1024], string[1024];
// set the default directory
	sprintf(directory, "%srgb601.rc", BCASTDIR);

// load the defaults
	defaults = new Defaults(directory);
	defaults->load();

	config.direction = defaults->get("DIRECTION", config.direction);
	return 0;
}

int RGB601Main::save_defaults()
{
	defaults->update("DIRECTION", config.direction);
	defaults->save();
	return 0;
}

void RGB601Main::load_configuration()
{
	KeyFrame *prev_keyframe;

	prev_keyframe = get_prev_keyframe(-1);
// Must also switch between interpolation between keyframes and using first keyframe
	read_data(prev_keyframe);
}


void RGB601Main::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause data to be stored directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);
	output.tag.set_title("RGB601");
	output.tag.set_property("DIRECTION", config.direction);
	output.append_tag();
	output.terminate_string();
}

void RGB601Main::read_data(KeyFrame *keyframe)
{
	FileXML input;

	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;
	float new_threshold;

	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("RGB601"))
			{
				config.direction = input.tag.get_property("DIRECTION", config.direction);
			}
		}
	}

	if(thread) 
	{
		thread->window->update();
	}
}


int RGB601Main::start_realtime()
{
	return 0;
}

int RGB601Main::stop_realtime()
{
	return 0;
}

#define CREATE_TABLE(max) \
{ \
	for(int i = 0; i < max; i++) \
	{ \
		forward_table[i] = (int)((double)0.8588 * i + max * 0.0627 + 0.5); \
		reverse_table[i] = (int)((double)1.1644 * i - max * 0.0627 + 0.5); \
		CLAMP(forward_table[i], 0, max); \
		CLAMP(reverse_table[i], 0, max); \
	} \
}

void RGB601Main::create_table(VFrame *input_ptr)
{
	switch(input_ptr->get_color_model())
	{
		case BC_RGB888:
		case BC_YUV888:
		case BC_RGBA8888:
		case BC_YUVA8888:
			CREATE_TABLE(0xff);
			break;

		case BC_RGB161616:
		case BC_YUV161616:
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			CREATE_TABLE(0xffff);
			break;
	}
}

#define PROCESS(table, type, components, yuv) \
{ \
	int bytes = w * components; \
	for(int i = 0; i < h; i++) \
	{ \
		type *in_row = (type*)input_ptr->get_rows()[i]; \
		type *out_row = (type*)output_ptr->get_rows()[i]; \
 \
		if(yuv) \
		{ \
/* Just do Y */ \
			for(int j = 0; j < bytes; j++) \
			{ \
				int component = j % components; \
/* Y */ \
				if(component == 0) \
					out_row[j] = table[in_row[j]]; \
/* UV */ \
				else \
					out_row[j] = in_row[j]; \
			} \
		} \
		else \
		{ \
			for(int j = 0; j < bytes; j++) \
			{ \
				int component = j % components; \
/* A */ \
				if(component == 3) \
					out_row[j] = in_row[j]; \
				else \
/* RGB */ \
					out_row[j] = table[in_row[j]]; \
			} \
		} \
	} \
}

void RGB601Main::process(int *table, VFrame *input_ptr, VFrame *output_ptr)
{
	int w = input_ptr->get_w();
	int h = input_ptr->get_h();
	
	if(config.direction == 1)
		switch(input_ptr->get_color_model())
		{
			case BC_YUV888:
				PROCESS(forward_table, unsigned char, 3, 1);
				break;
			case BC_YUVA8888:
				PROCESS(forward_table, unsigned char, 4, 1);
				break;
			case BC_YUV161616:
				PROCESS(forward_table, u_int16_t, 3, 1);
				break;
			case BC_YUVA16161616:
				PROCESS(forward_table, u_int16_t, 4, 1);
				break;
			case BC_RGB888:
				PROCESS(forward_table, unsigned char, 3, 0);
				break;
			case BC_RGBA8888:
				PROCESS(forward_table, unsigned char, 4, 0);
				break;
			case BC_RGB161616:
				PROCESS(forward_table, u_int16_t, 3, 0);
				break;
			case BC_RGBA16161616:
				PROCESS(forward_table, u_int16_t, 4, 0);
				break;
		}
	else
	if(config.direction == 2)
		switch(input_ptr->get_color_model())
		{
			case BC_YUV888:
				PROCESS(reverse_table, unsigned char, 3, 1);
				break;
			case BC_YUVA8888:
				PROCESS(reverse_table, unsigned char, 4, 1);
				break;
			case BC_YUV161616:
				PROCESS(reverse_table, u_int16_t, 3, 1);
				break;
			case BC_YUVA16161616:
				PROCESS(reverse_table, u_int16_t, 4, 1);
				break;
			case BC_RGB888:
				PROCESS(reverse_table, unsigned char, 3, 0);
				break;
			case BC_RGBA8888:
				PROCESS(reverse_table, unsigned char, 4, 0);
				break;
			case BC_RGB161616:
				PROCESS(reverse_table, u_int16_t, 3, 0);
				break;
			case BC_RGBA16161616:
				PROCESS(reverse_table, u_int16_t, 4, 0);
				break;
		}
	else
	if(input_ptr->get_rows()[0] != output_ptr->get_rows()[0])
		output_ptr->copy_from(input_ptr);
}

int RGB601Main::process_realtime(VFrame *input_ptr, VFrame *output_ptr)
{
	load_configuration();

	create_table(input_ptr);
	if(config.direction == 1)
		process(forward_table, input_ptr, output_ptr);
	else
	if(config.direction == 2)
		process(reverse_table, input_ptr, output_ptr);
	else
	if(input_ptr->get_rows()[0] != output_ptr->get_rows()[0])
		output_ptr->copy_from(input_ptr);

	return 0;
}

int RGB601Main::show_gui()
{
	load_configuration();
	thread = new RGB601Thread(this);
	thread->start();
	return 0;
}

int RGB601Main::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void RGB601Main::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}



