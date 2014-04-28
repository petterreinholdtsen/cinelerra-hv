#include "confirmsave.h"
#include "defaults.h"
#include "errorbox.h"
#include "filexml.h"
#include "picon_png.h"
#include "gain.h"
#include "gainwindow.h"

#include "vframe.h"

#include <string.h>


PluginClient* new_plugin(PluginServer *server)
{
	return new Gain(server);
}



Gain::Gain(PluginServer *server)
 : PluginAClient(server)
{
	thread = 0;
	load_defaults();
}

Gain::~Gain()
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

char* Gain::plugin_title() { return "Gain"; }
int Gain::is_realtime() { return 1; }

VFrame* Gain::new_picon()
{
	return new VFrame(picon_png);
}

int Gain::start_realtime()
{
	return 0;
}

int Gain::stop_realtime()
{
	return 0;
}

int Gain::process_realtime(long size, double *input_ptr, double *output_ptr)
{
	load_configuration();

	double gain = db.fromdb(config.level);

	for(long i = 0; i < size; i++)
	{
		output_ptr[i] = input_ptr[i] * gain;
	}

	return 0;
}



int Gain::show_gui()
{
	load_configuration();
	thread = new GainThread(this);
	thread->start();
	return 0;
}

int Gain::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void Gain::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}

int Gain::load_defaults()
{
	char directory[1024];

// set the default directory
	sprintf(directory, "%sgain.rc", get_defaultdir());
	
// load the defaults

	defaults = new Defaults(directory);

	defaults->load();

	config.level = defaults->get("LEVEL", (double)0);

	return 0;
}

int Gain::save_defaults()
{
	defaults->update("LEVEL", config.level);
	defaults->save();
	return 0;
}

void Gain::load_configuration()
{
	KeyFrame *prev_keyframe, *next_keyframe;
//printf("BlurMain::load_configuration 1\n");

	prev_keyframe = get_prev_keyframe(-1);
	next_keyframe = get_next_keyframe(-1);
//printf("BlurMain::load_configuration %s\n", prev_keyframe->data);
	read_data(prev_keyframe);
}

void Gain::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause xml file to store data directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);

	output.tag.set_title("GAIN");
	output.tag.set_property("LEVEL", config.level);
	output.append_tag();
	output.append_newline();
	output.terminate_string();
}

void Gain::read_data(KeyFrame *keyframe)
{
	FileXML input;
// cause xml file to read directly from text
	input.set_shared_string(keyframe->data, strlen(keyframe->data));
	int result = 0;
	GainConfig new_config;

	result = input.read_tag();

	if(!result)
	{
		if(input.tag.title_is("GAIN"))
		{
			new_config.level = input.tag.get_property("LEVEL", new_config.level);
		}
		
		if(!(new_config == config))
		{
//printf("Gain::read_data %f\n", new_config.ref_level1);
			config = new_config;
			update_gui();
		}
	}
}

void Gain::update_gui()
{
	if(thread)
	{
		thread->window->lock_window();
		thread->window->level->update(config.level);
		thread->window->unlock_window();
	}
}




GainConfig::GainConfig()
{
}

int GainConfig::operator==(GainConfig& that)
{
	return(level == that.level);
}

GainConfig& GainConfig::operator=(GainConfig& that)
{
	level = that.level;
	return *this;
}
