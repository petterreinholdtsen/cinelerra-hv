#include "clip.h"
#include "defaults.h"
#include "deinterlace.h"
#include "deinterwindow.h"
#include "filexml.h"
#include "keyframe.h"
#include "picon_png.h"
#include "vframe.h"








#include <stdint.h>
#include <string.h>

PluginClient* new_plugin(PluginServer *server)
{
	return new DeInterlaceMain(server);
}




DeInterlaceConfig::DeInterlaceConfig()
{
	mode = DEINTERLACE_EVEN;
}


DeInterlaceMain::DeInterlaceMain(PluginServer *server)
 : PluginVClient(server)
{
	thread = 0;
	load_defaults();
}

DeInterlaceMain::~DeInterlaceMain()
{
	if(thread)
	{
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}
	
	save_defaults();
	delete defaults;
}

char* DeInterlaceMain::plugin_title() { return "Deinterlace"; }
int DeInterlaceMain::is_realtime() { return 1; }

VFrame* DeInterlaceMain::new_picon()
{
	return new VFrame(picon_png);
}




#define DEINTERLACE_EVEN_MACRO(type, components, dominance) \
{ \
	int w = input->get_w(); \
	int h = input->get_h(); \
 \
	for(int i = 0; i < h - 1; i += 2) \
	{ \
		type *input_row = (type*)input->get_rows()[dominance ? i + 1 : i]; \
		type *output_row1 = (type*)output->get_rows()[i]; \
		type *output_row2 = (type*)output->get_rows()[i + 1]; \
		memcpy(output_row1, input_row, w * components); \
		memcpy(output_row2, input_row, w * components); \
	} \
}

#define DEINTERLACE_AVG_MACRO(type, components) \
{ \
	int w = input->get_w(); \
	int h = input->get_h(); \
 \
	for(int i = 0; i < h - 1; i += 2) \
	{ \
		type *input_row1 = (type*)input->get_rows()[i]; \
		type *input_row2 = (type*)input->get_rows()[i + 1]; \
		type *output_row1 = (type*)output->get_rows()[i]; \
		type *output_row2 = (type*)output->get_rows()[i + 1]; \
		type result; \
 \
		for(int j = 0; j < w * components; j++) \
		{ \
			result = ((uint64_t)input_row1[j] + input_row2[j]) >> 1; \
			output_row1[j] = result; \
			output_row2[j] = result; \
		} \
	} \
}

#define DEINTERLACE_SWAP_MACRO(type, components) \
{ \
	int w = input->get_w(); \
	int h = input->get_h(); \
 \
	for(int i = 0; i < h - 1; i++) \
	{ \
		type *input_row1 = (type*)input->get_rows()[i]; \
		type *input_row2 = (type*)input->get_rows()[i + 1]; \
		type *output_row1 = (type*)output->get_rows()[i]; \
		type *output_row2 = (type*)output->get_rows()[i + 1]; \
		type temp; \
 \
		for(int j = 0; j < w * components; j++) \
		{ \
			temp = input_row1[j]; \
			output_row1[j] = input_row2[j]; \
			output_row2[j] = temp; \
		} \
	} \
}


void DeInterlaceMain::deinterlace_even(VFrame *input, VFrame *output, int dominance)
{
	switch(input->get_color_model())
	{
		case BC_RGB888:
		case BC_YUV888:
			DEINTERLACE_EVEN_MACRO(unsigned char, 3, dominance);
			break;
		case BC_RGBA8888:
		case BC_YUVA8888:
			DEINTERLACE_EVEN_MACRO(unsigned char, 4, dominance);
			break;
		case BC_RGB161616:
		case BC_YUV161616:
			DEINTERLACE_EVEN_MACRO(uint16_t, 3, dominance);
			break;
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			DEINTERLACE_EVEN_MACRO(uint16_t, 4, dominance);
			break;
	}
}

void DeInterlaceMain::deinterlace_avg(VFrame *input, VFrame *output)
{
	switch(input->get_color_model())
	{
		case BC_RGB888:
		case BC_YUV888:
			DEINTERLACE_AVG_MACRO(unsigned char, 3);
			break;
		case BC_RGBA8888:
		case BC_YUVA8888:
			DEINTERLACE_AVG_MACRO(unsigned char, 4);
			break;
		case BC_RGB161616:
		case BC_YUV161616:
			DEINTERLACE_AVG_MACRO(uint16_t, 3);
			break;
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			DEINTERLACE_AVG_MACRO(uint16_t, 4);
			break;
	}
}

void DeInterlaceMain::deinterlace_swap(VFrame *input, VFrame *output)
{
	switch(input->get_color_model())
	{
		case BC_RGB888:
		case BC_YUV888:
			DEINTERLACE_SWAP_MACRO(unsigned char, 3);
			break;
		case BC_RGBA8888:
		case BC_YUVA8888:
			DEINTERLACE_SWAP_MACRO(unsigned char, 4);
			break;
		case BC_RGB161616:
		case BC_YUV161616:
			DEINTERLACE_SWAP_MACRO(uint16_t, 3);
			break;
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			DEINTERLACE_SWAP_MACRO(uint16_t, 4);
			break;
	}
}


int DeInterlaceMain::process_realtime(VFrame *input, VFrame *output)
{
	load_configuration();

//printf("DeInterlaceMain::process_realtime %d\n", config.mode);
	switch(config.mode)
	{
		case DEINTERLACE_EVEN:
			deinterlace_even(input, output, 0);
			break;
		case DEINTERLACE_ODD:
			deinterlace_even(input, output, 1);
			break;
		case DEINTERLACE_AVG:
			deinterlace_avg(input, output);
			break;
		case DEINTERLACE_SWAP:
			deinterlace_swap(input, output);
			break;
	}

	return 0;
}

int DeInterlaceMain::show_gui()
{
	load_configuration();
	thread = new DeInterlaceThread(this);
	thread->start();
	return 0;
}

int DeInterlaceMain::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void DeInterlaceMain::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}

int DeInterlaceMain::load_defaults()
{
	char directory[BCTEXTLEN], string[BCTEXTLEN];
	sprintf(directory, "%sdeinterlace.rc", BCASTDIR);
	
	defaults = new Defaults(directory);
	defaults->load();
	config.mode = defaults->get("MODE", config.mode);
	return 0;
}


int DeInterlaceMain::save_defaults()
{
	defaults->update("MODE", config.mode);
	defaults->save();
	return 0;
}

void DeInterlaceMain::load_configuration()
{
	KeyFrame *prev_keyframe = get_prev_keyframe(get_source_position());
	read_data(prev_keyframe);
}

void DeInterlaceMain::save_data(KeyFrame *keyframe)
{
	FileXML output;
	output.set_shared_string(keyframe->data, MESSAGESIZE);
	output.tag.set_title("DEINTERLACE");
	output.tag.set_property("MODE", config.mode);
	output.append_tag();
	output.terminate_string();
}

void DeInterlaceMain::read_data(KeyFrame *keyframe)
{
	FileXML input;
	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	while(!input.read_tag())
	{
		if(input.tag.title_is("DEINTERLACE"))
		{
			config.mode = input.tag.get_property("MODE", config.mode);
		}
	}

}

void DeInterlaceMain::update_gui()
{
	if(thread) 
	{
		load_configuration();
		thread->window->lock_window();
		thread->window->set_mode(config.mode, 0);
		thread->window->unlock_window();
	}
}

