#include "clip.h"
#include "defaults.h"
#include "denoisevideo.h"
#include "filexml.h"
#include "guicast.h"
#include "keyframe.h"
#include "picon_png.h"
#include "vframe.h"





#include <stdint.h>
#include <string.h>



REGISTER_PLUGIN(DenoiseVideo)






DenoiseVideoConfig::DenoiseVideoConfig()
{
	frames = 2;
	threshold = 0.1;
}

int DenoiseVideoConfig::equivalent(DenoiseVideoConfig &that)
{
	return frames == that.frames && 
		EQUIV(threshold, that.threshold);
}

void DenoiseVideoConfig::copy_from(DenoiseVideoConfig &that)
{
	frames = that.frames;
	threshold = that.threshold;
}

void DenoiseVideoConfig::interpolate(DenoiseVideoConfig &prev, 
	DenoiseVideoConfig &next, 
	long prev_frame, 
	long next_frame, 
	long current_frame)
{
	double next_scale = (double)(current_frame - prev_frame) / (next_frame - prev_frame);
	double prev_scale = (double)(next_frame - current_frame) / (next_frame - prev_frame);

	this->frames = (int)(prev.frames * prev_scale + next.frames * next_scale);
	this->threshold = prev.threshold * prev_scale + next.threshold * next_scale;
}






DenoiseVideoFrames::DenoiseVideoFrames(DenoiseVideo *plugin, int x, int y)
 : BC_ISlider(x, 
 	y, 
	0,
	190, 
	200, 
	1, 
	256, 
	plugin->config.frames)
{
	this->plugin = plugin;
}

int DenoiseVideoFrames::handle_event()
{
	int result = get_value();
	if(result < 1 || result > 256) result = 256;
	plugin->config.frames = result;
	plugin->send_configure_change();
	return 1;
}







DenoiseVideoThreshold::DenoiseVideoThreshold(DenoiseVideo *plugin, int x, int y)
 : BC_TextBox(x, y, 100, 1, plugin->config.threshold)
{
	this->plugin = plugin;
}

int DenoiseVideoThreshold::handle_event()
{
	plugin->config.threshold = atof(get_text());
	plugin->send_configure_change();
	return 1;
}






DenoiseVideoWindow::DenoiseVideoWindow(DenoiseVideo *plugin, int x, int y)
 : BC_Window(plugin->gui_string, 
 	x, 
	y, 
	210, 
	160, 
	200, 
	160, 
	0, 
	0,
	1)
{
	this->plugin = plugin;
}


void DenoiseVideoWindow::create_objects()
{
	int x = 10, y = 10;
	add_subwindow(new BC_Title(x, y, "Frames to accumulate:"));
	y += 20;
	add_subwindow(frames = new DenoiseVideoFrames(plugin, x, y));
	y += 40;
	add_subwindow(new BC_Title(x, y, "Threshold:"));
	y += 20;
	add_subwindow(threshold = new DenoiseVideoThreshold(plugin, x, y));
	show_window();
	flush();
}

int DenoiseVideoWindow::close_event()
{
	set_done(1);
	return 1;
}






PLUGIN_THREAD_OBJECT(DenoiseVideo, DenoiseVideoThread, DenoiseVideoWindow)











DenoiseVideo::DenoiseVideo(PluginServer *server)
 : PluginVClient(server)
{
	accumulation = 0;
	thread = 0;
	load_defaults();
}


DenoiseVideo::~DenoiseVideo()
{
	if(thread)
	{
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}

	save_defaults();
	delete defaults;

	if(accumulation) delete [] accumulation;
}

int DenoiseVideo::process_realtime(VFrame *input, VFrame *output)
{
	load_configuration();

	int h = input->get_h();
	int w = input->get_w();
	int color_model = input->get_color_model();

	if(!accumulation)
	{
		accumulation = new float[w * h * cmodel_components(color_model)];
		bzero(accumulation, sizeof(float) * w * h * cmodel_components(color_model));
	}

	float *accumulation_ptr = accumulation;
	float opacity = (float)1.0 / config.frames;
	float transparency = 1 - opacity;
	float threshold = (float)config.threshold * 
		cmodel_calculate_max(color_model);

//printf("DenoiseVideo::process_realtime 1\n");
//printf("DenoiseVideo::process_realtime 1\n");

#define DENOISE_MACRO(type, components, max) \
{ \
	for(int i = 0; i < h; i++) \
	{ \
		type *output_row = (type*)output->get_rows()[i]; \
		type *input_row = (type*)input->get_rows()[i]; \
		for(int k = 0; k < w * components; k++) \
		{ \
			float input_pixel = input_row[k]; \
			(*accumulation_ptr) = \
				transparency * (*accumulation_ptr) + \
				opacity * input_pixel; \
 \
 			if(fabs((*accumulation_ptr) - input_pixel) > threshold) \
			{ \
				(*accumulation_ptr) = input_pixel; \
				output_row[k] = (type)(*accumulation_ptr); \
			} \
			else \
				output_row[k] = (type)CLIP((*accumulation_ptr), 0, max); \
 \
			accumulation_ptr++; \
		} \
	} \
\
}






	switch(color_model)
	{
		case BC_RGB888:
		case BC_YUV888:
			DENOISE_MACRO(unsigned char, 3, 0xff);
			break;

		case BC_RGBA8888:
		case BC_YUVA8888:
			DENOISE_MACRO(unsigned char, 4, 0xff);
			break;

		case BC_RGB161616:
		case BC_YUV161616:
			DENOISE_MACRO(uint16_t, 3, 0xffff);
			break;

		case BC_RGBA16161616:
		case BC_YUVA16161616:
			DENOISE_MACRO(uint16_t, 4, 0xffff);
			break;
	}
//printf("DenoiseVideo::process_realtime 2\n");
}

int DenoiseVideo::is_realtime()
{
	return 1;
}

char* DenoiseVideo::plugin_title()
{
	return "Denoise video";
}

VFrame* DenoiseVideo::new_picon()
{
	return new VFrame(picon_png);
}

SHOW_GUI_MACRO(DenoiseVideo, DenoiseVideoThread)

RAISE_WINDOW_MACRO(DenoiseVideo)

void DenoiseVideo::update_gui()
{
	if(thread) 
	{
		load_configuration();
		thread->window->lock_window();
		thread->window->frames->update(config.frames);
		thread->window->threshold->update(config.threshold);
		thread->window->unlock_window();
	}
}



SET_STRING_MACRO(DenoiseVideo);

LOAD_CONFIGURATION_MACRO(DenoiseVideo, DenoiseVideoConfig)

int DenoiseVideo::load_defaults()
{
	char directory[BCTEXTLEN];
// set the default directory
	sprintf(directory, "%sdenoisevideo.rc", BCASTDIR);

// load the defaults
	defaults = new Defaults(directory);
	defaults->load();

	config.frames = defaults->get("FRAMES", config.frames);
	config.threshold = defaults->get("THRESHOLD", config.threshold);
	return 0;
}

int DenoiseVideo::save_defaults()
{
	defaults->update("THRESHOLD", config.threshold);
	defaults->update("FRAMES", config.frames);
	defaults->save();
	return 0;
}

void DenoiseVideo::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause data to be stored directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);
	output.tag.set_title("DENOISE_VIDEO");
	output.tag.set_property("FRAMES", config.frames);
	output.tag.set_property("THRESHOLD", config.threshold);
	output.append_tag();
	output.terminate_string();
}

void DenoiseVideo::read_data(KeyFrame *keyframe)
{
	FileXML input;

	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;

	while(!input.read_tag())
	{
		if(input.tag.title_is("DENOISE_VIDEO"))
		{
			config.frames = input.tag.get_property("FRAMES", config.frames);
			config.threshold = input.tag.get_property("THRESHOLD", config.threshold);
		}
	}
}




