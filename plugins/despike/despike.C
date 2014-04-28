#include "confirmsave.h"
#include "defaults.h"
#include "errorbox.h"
#include "filexml.h"
#include "picon_png.h"
#include "despike.h"
#include "despikewindow.h"

#include "vframe.h"

#include <string.h>


PluginClient* new_plugin(PluginServer *server)
{
	return new Despike(server);
}



Despike::Despike(PluginServer *server)
 : PluginAClient(server)
{
	thread = 0;
	load_defaults();
}

Despike::~Despike()
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

char* Despike::plugin_title() { return "Despike"; }
int Despike::is_realtime() { return 1; }

VFrame* Despike::new_picon()
{
	return new VFrame(picon_png);
}

int Despike::start_realtime()
{
	last_sample = 0;
	return 0;
}

int Despike::stop_realtime()
{
	return 0;
}

int Despike::process_realtime(long size, double *input_ptr, double *output_ptr)
{
	load_configuration();

	double threshold = db.fromdb(config.level);
	double change = db.fromdb(config.slope);

//printf("Despike::process_realtime 1\n");
	for(long i = 0; i < size; i++)
	{
		if(fabs(input_ptr[i]) > threshold || 
			fabs(input_ptr[i]) - fabs(last_sample) > change) 
		{
			output_ptr[i] = last_sample;
		}
		else
		{
			output_ptr[i] = input_ptr[i];
			last_sample = input_ptr[i];
		}
	}
//printf("Despike::process_realtime 2\n");

	return 0;
}



int Despike::show_gui()
{
	load_configuration();
	thread = new DespikeThread(this);
	thread->start();
	return 0;
}

int Despike::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void Despike::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}

int Despike::load_defaults()
{
	char directory[1024];

// set the default directory
	sprintf(directory, "%sdespike.rc", get_defaultdir());
	
// load the defaults

	defaults = new Defaults(directory);

	defaults->load();

	config.level = defaults->get("LEVEL", (double)0);
	config.slope = defaults->get("SLOPE", (double)0);

	return 0;
}

int Despike::save_defaults()
{
	defaults->update("LEVEL", config.level);
	defaults->update("SLOPE", config.slope);
	defaults->save();
	return 0;
}

void Despike::load_configuration()
{
	KeyFrame *prev_keyframe, *next_keyframe;
//printf("BlurMain::load_configuration 1\n");

	prev_keyframe = get_prev_keyframe(-1);
	next_keyframe = get_next_keyframe(-1);
//printf("BlurMain::load_configuration %s\n", prev_keyframe->data);
	read_data(prev_keyframe);
}

void Despike::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause xml file to store data directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);

	output.tag.set_title("DESPIKE");
	output.tag.set_property("LEVEL", config.level);
	output.tag.set_property("SLOPE", config.slope);
	output.append_tag();
	output.append_newline();
	output.terminate_string();
}

void Despike::read_data(KeyFrame *keyframe)
{
	FileXML input;
// cause xml file to read directly from text
	input.set_shared_string(keyframe->data, strlen(keyframe->data));
	int result = 0;
	DespikeConfig new_config;

	result = input.read_tag();

	if(!result)
	{
		if(input.tag.title_is("DESPIKE"))
		{
			new_config.level = input.tag.get_property("LEVEL", new_config.level);
			new_config.slope = input.tag.get_property("SLOPE", new_config.slope);
		}

		if(!(new_config == config))
		{
//printf("Despike::read_data %f\n", new_config.ref_level1);
			config = new_config;
			update_gui();
		}
	}
}

void Despike::update_gui()
{
	if(thread)
	{
		thread->window->lock_window();
		thread->window->level->update(config.level);
		thread->window->slope->update(config.slope);
		thread->window->unlock_window();
	}
}




DespikeConfig::DespikeConfig()
{
}

int DespikeConfig::operator==(DespikeConfig& that)
{
	return(level == that.level && slope == that.slope);
}

DespikeConfig& DespikeConfig::operator=(DespikeConfig& that)
{
	level = that.level;
	slope = that.slope;
	return *this;
}
