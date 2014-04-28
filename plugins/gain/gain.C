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
	PLUGIN_CONSTRUCTOR_MACRO
}

Gain::~Gain()
{
	PLUGIN_DESTRUCTOR_MACRO
}

char* Gain::plugin_title() { return "Gain"; }
int Gain::is_realtime() { return 1; }


SHOW_GUI_MACRO(Gain, GainThread)
SET_STRING_MACRO(Gain)
RAISE_WINDOW_MACRO(Gain)
NEW_PICON_MACRO(Gain)

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

	prev_keyframe = get_prev_keyframe(get_source_position());
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
