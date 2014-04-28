#include "console.h"
#include "edl.h"
#include "edlsession.h"
#include "filexml.h"
#include "keyframe.h"
#include "keyframes.h"
#include "localsession.h"
#include "mwindow.h"
#include "messages.h"
#include "module.h"
#include "plugin.h"
#include "pluginbuffer.h"
#include "pluginpopup.h"
#include "pluginset.h"
#include "pluginserver.h"
#include "track.h"
#include "tracks.h"
#include "virtualnode.h"


Plugin::Plugin(EDL *edl, 
		Track *track, 
		char *title)
 : Edit(edl, track)
{
	this->track = track;
	this->plugin_set = 0;
	strcpy(this->title, title);
	plugin_type = PLUGIN_NONE;
	in = 1;
	out = 1;
	show = 0;
	on = 1;
	keyframes = new KeyFrames(edl, track);
	keyframes->create_objects();
}


Plugin::Plugin(EDL *edl, PluginSet *plugin_set, char *title)
 : Edit(edl, plugin_set)
{
	this->track = plugin_set->track;
	this->plugin_set = plugin_set;
	strcpy(this->title, title);
	plugin_type = PLUGIN_NONE;
	in = 1;
	out = 1;
	show = 0;
	on = 1;
	keyframes = new KeyFrames(edl, track);
	keyframes->create_objects();
}

Plugin::~Plugin()
{
//printf("Plugin::~Plugin 1 %p %p\n", keyframes, keyframes->last);
	while(keyframes->last) delete keyframes->last;
//printf("Plugin::~Plugin 2\n");
	delete keyframes;
//printf("Plugin::~Plugin 3\n");
}

Edit& Plugin::operator=(Edit& edit)
{
//printf("Plugin::operator= called\n");
	copy_from(&edit);
//printf("Plugin::operator= 2\n");
	return *this;
}

Plugin& Plugin::operator=(Plugin& edit)
{
	copy_from(&edit);
	return *this;
}

int Plugin::operator==(Plugin& that)
{
	return identical(&that);
}

int Plugin::operator==(Edit& that)
{
	return identical((Plugin*)&that);
}

int Plugin::silence()
{
//printf("Plugin::silence 1 %p %d\n", this, plugin_type);
	if(plugin_type != PLUGIN_NONE) 
		return 0;
	else
		return 1;
}

void Plugin::clear_keyframes(long start, long end)
{
	keyframes->clear(start, end, 0);
}


void Plugin::copy_from(Edit *edit)
{
	Plugin *plugin = (Plugin*)edit;

	this->startsource = edit->startsource;
	this->startproject = edit->startproject;
	this->length = edit->length;


	this->plugin_type = plugin->plugin_type;
	this->in = plugin->in;
	this->out = plugin->out;
	this->show = plugin->show;
	this->on = plugin->on;
	this->shared_location = plugin->shared_location;
	strcpy(this->title, plugin->title);

	copy_keyframes(plugin);
}

void Plugin::copy_keyframes(Plugin *plugin)
{
	keyframes->copy_from(plugin->keyframes);
}

void Plugin::copy_keyframes(long start, long end, FileXML *file, int default_only)
{
	keyframes->copy(start, end, file, default_only);
}

void Plugin::synchronize_params(Edit *edit)
{
	Plugin *plugin = (Plugin*)edit;
	this->in = plugin->in;
	this->out = plugin->out;
	this->show = plugin->show;
	this->on = plugin->on;
	strcpy(this->title, plugin->title);
	copy_keyframes(plugin);
}

int Plugin::identical(Plugin *that)
{
	char title1[BCTEXTLEN], title2[BCTEXTLEN];

	calculate_title(title1);
	that->calculate_title(title2);

// printf("Plugin::identical %s %s %d %d %s %s %d\n",
// 	title1, title2,
// 	plugin_type, that.plugin_type,
// 	keyframes->default_keyframe->data,
// 	that.keyframes->default_keyframe->data,
// 	*keyframes->default_keyframe == *that.keyframes->default_keyframe);

	return (this->plugin_type == that->plugin_type &&
		this->in == that->in &&
		this->out == that->out &&
		this->on == that->on &&
		!strcasecmp(title1, title2) &&
		((KeyFrame*)keyframes->default_auto)->identical(
			((KeyFrame*)that->keyframes->default_auto)));
}

// FIXME
int Plugin::get_plugin_number()
{
	return 0;
}

KeyFrame* Plugin::get_prev_keyframe(long position)
{
	KeyFrame *current = 0;

//printf("Plugin::get_prev_keyframe 1 %p %p\n", this, edl);
// This doesn't work because edl->selectionstart doesn't change during
// playback at the same rate as PluginClient::source_position.
	if(position < 0)
	{
printf("Plugin::get_prev_keyframe position < 0\n");
		position = track->to_units(edl->local_session->selectionstart, 0);
	}
//printf("Plugin::get_prev_keyframe 1 %d\n", position);

	for(current = (KeyFrame*)keyframes->last;
		current;
		current = (KeyFrame*)PREVIOUS)
	{
		if(current->position <= position) break;
	}

//printf("Plugin::get_prev_keyframe %p %p %ld\n", current, keyframes->first, keyframes->first->position);
	if(!current && keyframes->first)
	{
		current = (KeyFrame*)keyframes->first;
	}
	else
	if(!current)
	{
		current = (KeyFrame*)keyframes->default_auto;
	}

//printf("Plugin::get_prev_keyframe 2 %ld %ld\n", 
//	position, current->position);
	return current;
}

KeyFrame* Plugin::get_next_keyframe(long position)
{
	KeyFrame *current;

// This doesn't work because edl->selectionstart doesn't change during
// playback at the same rate as PluginClient::source_position.
	if(position < 0)
	{
printf("Plugin::get_next_keyframe position < 0\n");
		position = track->to_units(edl->local_session->selectionstart, 0);
	}

	for(current = (KeyFrame*)keyframes->first;
		current;
		current = (KeyFrame*)NEXT)
	{
		if(current->position > position) break;
	}

	if(!current && keyframes->last)
	{
		current =  (KeyFrame*)keyframes->last;
	}
	else
	if(!current)
	{
		current = (KeyFrame*)keyframes->default_auto;
	}

//printf("Plugin::get_next_keyframe 2 %ld %ld\n", 
//	position, current->position);
	return current;
}

KeyFrame* Plugin::get_keyframe()
{
// Search for keyframe on or before selection
	KeyFrame *result = 
		get_prev_keyframe(track->to_units(edl->local_session->selectionstart, 0));

//printf("Plugin::get_keyframe %p %p %p\n", result, edl, edl->session);
// Return nearest keyframe if not in automatic keyframe generation
	if(!edl->session->auto_keyframes)
	{
		return result;
	}
	else
// Return new keyframe
	if(result == (KeyFrame*)keyframes->default_auto || 
		result->position != track->to_units(edl->local_session->selectionstart, 0))
	{
		return (KeyFrame*)keyframes->insert_auto(track->to_units(edl->local_session->selectionstart, 0));
	}
	else
// Return existing keyframe
	{
		return result;
	}

	return 0;
}

void Plugin::copy(long start, long end, FileXML *file)
{
	long endproject = startproject + length;

	if((startproject >= start && startproject <= end) ||  // startproject in range
		 (endproject <= end && endproject >= start) ||	   // endproject in range
		 (startproject <= start && endproject >= end))    // range in project
	{
// edit is in range
		long startproject_in_selection = startproject; // start of edit in selection in project
		long startsource_in_selection = startsource; // start of source in selection in source
		long endsource_in_selection = startsource + length; // end of source in selection
		long length_in_selection = length;             // length of edit in selection

		if(startproject < start)
		{         // start is after start of edit in project
			long length_difference = start - startproject;

			startsource_in_selection += length_difference;
			startproject_in_selection += length_difference;
			length_in_selection -= length_difference;
		}

// end is before end of edit in project
		if(endproject > end)
		{         
			length_in_selection = end - startproject_in_selection;
		}

// Plugins don't store silence
		file->tag.set_title("PLUGIN");	
//		file->tag.set_property("STARTPROJECT", startproject_in_selection - start);
		file->tag.set_property("LENGTH", length_in_selection);
		file->tag.set_property("TYPE", plugin_type);
		file->tag.set_property("TITLE", title);
		file->append_tag();
		file->append_newline();


		if(plugin_type == PLUGIN_SHAREDPLUGIN ||
			plugin_type == PLUGIN_SHAREDMODULE)
		{
			shared_location.save(file);
		}



		if(in)
		{
			file->tag.set_title("IN");
			file->append_tag();
		}
		if(out)
		{
			file->tag.set_title("OUT");
			file->append_tag();
		}
		if(show)
		{
			file->tag.set_title("SHOW");
			file->append_tag();
		}
		if(on)
		{
			file->tag.set_title("ON");
			file->append_tag();
		}
		file->append_newline();

// Keyframes
		keyframes->copy(start, end, file, 0);

		file->tag.set_title("/PLUGIN");	
		file->append_tag();
		file->append_newline();
	}
}

void Plugin::load(FileXML *file)
{
	int result = 0;
	int first_keyframe = 1;
 	in = 0;
	out = 0;

// Currently show is ignored when loading
	show = 0;
	on = 0;

//printf("Plugin::load 1\n");
	do{
		result = file->read_tag();

//printf("Plugin::load 1 %s\n", file->tag.get_title());
		if(!result)
		{
			if(file->tag.title_is("/PLUGIN"))
			{
				result = 1;
			}
			else
			if(file->tag.title_is("SHARED_LOCATION"))
			{
				shared_location.load(file);
			}
			else
			if(file->tag.title_is("IN"))
			{
				in = 1;
			}
			else
			if(file->tag.title_is("OUT"))
			{
				out = 1;
			}
			else
			if(file->tag.title_is("SHOW"))
			{
//				show = 1;
			}
			else
			if(file->tag.title_is("ON"))
			{
				on = 1;
			}
			else
			if(file->tag.title_is("KEYFRAME"))
			{
// Default keyframe
				if(first_keyframe)
				{
//printf("Plugin::load 2 %p\n", keyframes);
//printf("Plugin::load 2 %p\n", keyframes->default_auto);
					keyframes->default_auto->load(file);
					first_keyframe = 0;
//printf("Plugin::load 3\n");
				}
				else
// Override default keyframe
				{
//printf("Plugin::load 4\n");
					KeyFrame *keyframe = (KeyFrame*)keyframes->append(new KeyFrame(edl, keyframes));
//printf("Plugin::load 5\n");
					keyframe->position = file->tag.get_property("POSITION", (long)0);
//printf("Plugin::load 6\n");
					keyframe->load(file);
//printf("Plugin::load 7 %d\n", keyframes->total());
				}
			}
		}
	}while(!result);
//dump();
//printf("Plugin::load 8 %s\n", title);
}

void Plugin::get_shared_location(SharedLocation *result)
{
	if(plugin_type == PLUGIN_STANDALONE && plugin_set)
	{
		result->module = edl->tracks->number_of(track);
		result->plugin = track->plugin_set.number_of(plugin_set);
	}
	else
	{
		*result = this->shared_location;
	}
}

Track* Plugin::get_shared_track()
{
	return edl->tracks->get_item_number(shared_location.module);
}

Plugin* Plugin::get_shared_plugin()
{
	Track *track = get_shared_track();

	if(track && 
		shared_location.plugin >= 0)
	{
		return track->get_current_plugin(startproject, 
			shared_location.plugin, 
			PLAY_FORWARD,
			0);
	}
	else
		return 0;
}


void Plugin::calculate_title(char *string)
{
	if(plugin_type == PLUGIN_STANDALONE)
	{
		strcpy(string, title);
	}
	else
	if(plugin_type == PLUGIN_SHAREDPLUGIN || plugin_type == PLUGIN_SHAREDMODULE)
	{
		shared_location.calculate_title(string, edl, startproject, 0, plugin_type);
	}
}


void Plugin::paste(FileXML *file)
{
	length = file->tag.get_property("LENGTH", (long)0);
}

void Plugin::resample(double old_rate, double new_rate)
{
// Resample keyframes in here
	keyframes->resample(old_rate, new_rate);
}

void Plugin::shift(long difference)
{
	Edit::shift(difference);

	for(KeyFrame *keyframe = (KeyFrame*)keyframes->first;
		keyframe; 
		keyframe = (KeyFrame*)keyframe->next)
	{
		keyframe->position += difference;
	}
}

void Plugin::dump()
{
	printf("    PLUGIN: type=%d title=\"%s\" on=%d\n", 
		plugin_type, title, on);
	printf("    startproject %ld length %ld\n", startproject, length);
	printf("    DEFAULT_KEYFRAME\n");
	((KeyFrame*)keyframes->default_auto)->dump();
	printf("    KEYFRAMES total=%d\n", keyframes->total());
	for(KeyFrame *current = (KeyFrame*)keyframes->first;
		current;
		current = (KeyFrame*)NEXT)
	{
		current->dump();
	}
}























Plugin::Plugin(MWindow *mwindow, Module *module, int plugin_number)
 : Edit((EDL*)0, (Edits*)0)
{
printf("Plugin::Plugin(MWindow *mwindow, Module *module, int plugin_number)\n");
	this->mwindow = mwindow;
	this->plugin_number = plugin_number;
	this->module = module;
//	plugin_server = 0;
	in = out = 0;
	on = 1;
	show = 1;
	plugin_type = 0;
//	sprintf(plugin_title, default_title());
	plugin_popup = 0;
	show_title = 0;
	show_toggle = 0;
	on_toggle = 0;
	on_title = 0;
//	total_input_buffers = 0;
//	new_total_input_buffers = 0;
}

char* Plugin::default_title()
{
	return "Plugin";
}

int Plugin::update_derived()
{
//	plugin_popup->update(in, out, plugin_title);
}

int Plugin::update_display()
{
	if(mwindow->gui)
	{
//		plugin_popup->update(in, out, plugin_title);
//		show_toggle->update(show);
//		on_toggle->update(on);
	}
}

int Plugin::swap_modules(int number1, int number2)
{
// 	if(shared_plugin_location.module == number1) shared_plugin_location.module == number2;
// 	else
// 	if(shared_plugin_location.module == number2) shared_plugin_location.module == number1;
// 
// 	if(shared_module_location.module == number1) shared_module_location.module == number2;
// 	else
// 	if(shared_module_location.module == number2) shared_module_location.module == number1;
}

int Plugin::set_show_derived(int value)
{
	if(show_toggle) show_toggle->update(value);
}

int Plugin::set_string()
{
// 	if(plugin_server)
// 	{
// 		if(use_gui())
// 		{
// 			char new_string[1024];
// 			sprintf(new_string, "%s: %s\n", get_module_title(), plugin_title);
// 			plugin_server->set_string(new_string);
// 		}
// 	}
}

char* Plugin::get_module_title()
{
	return module->title;
}

int Plugin::resize_plugin(int x, int y)
{
	if(plugin_popup) plugin_popup->reposition_window(x, y);
	if(show_toggle) show_toggle->reposition_window(x + 10, y + 23);
	if(show_title) show_title->reposition_window(x + 30, y + 25);
	if(on_toggle) on_toggle->reposition_window(x + 60, y + 23);
	if(on_title) on_title->reposition_window(x + 80, y + 25);
}




// ======================================= radial

PluginShowToggle::PluginShowToggle(Plugin *plugin, Console *console, int x, int y)
 : BC_Radial(x, y, plugin->show)
{
	this->console = console;
	this->plugin = plugin;
}

int PluginShowToggle::handle_event()
{
	if(shift_down())
	{
		int total_selected = console->toggles_selected(0, 1, 0);

		if(total_selected == 0)
		{
// nothing previously selected
			console->select_all_toggles(0, 1, 0);
		}
		else
		if(total_selected == 1)
		{
			if(plugin->show)
			{
// this patch was previously the only one on
				console->select_all_toggles(0, 1, 0);
			}
			else
			{
// another patch was previously the only one on
				console->deselect_all_toggles(0, 1, 0);
				plugin->show = 1;
			}
		}
		else
		if(total_selected > 1)
		{
// other patches were previously on
			console->deselect_all_toggles(0, 1, 0);
			plugin->show = 1;
		}
		
		update(plugin->show);
	}
	else
	{
		if(get_value() != plugin->show)
		{
			plugin->show = get_value();
//			if(plugin->plugin_server)
//			{
//				if(get_value()) plugin->show_gui();
//				else plugin->hide_gui();
//			}
		}
	}

	console->button_down = 1;
	console->new_status = get_value();
	return 1;
}

int PluginShowToggle::cursor_moved_over()
{
	if(console->button_down && console->new_status != get_value())
	{
		update(console->new_status);
		plugin->show = get_value();

// 		if(plugin->plugin_server && console->gui)
// 		{
// 			if(get_value()) plugin->show_gui();
// 			else plugin->hide_gui();
// 		}
	}
}

int PluginShowToggle::button_release()
{
	console->button_down = 0;
}

PluginOnToggle::PluginOnToggle(Plugin *plugin, Console *console, int x, int y)
 : BC_Radial(x, y, plugin->show)
{
	this->console = console;
	this->plugin = plugin;
}

int PluginOnToggle::handle_event()
{
	if(shift_down())
	{
		int total_selected = console->toggles_selected(1, 0, 0);

		if(total_selected == 0)
		{
// nothing previously selected
			console->select_all_toggles(1, 0, 0);
		}
		else
		if(total_selected == 1)
		{
			if(plugin->on)
			{
// this patch was previously the only one on
				console->select_all_toggles(1, 0, 0);
			}
			else
			{
// another patch was previously the only one on
				console->deselect_all_toggles(1, 0, 0);
				plugin->on = 1;
			}
		}
		else
		if(total_selected > 1)
		{
// other patches were previously on
			console->deselect_all_toggles(1, 0, 0);
			plugin->on = 1;
		}

		update(plugin->on);
	}
	else
	{
		if(/*plugin->plugin_server && */get_value() != plugin->on)
		{
			plugin->on = get_value();
		}
	}

	console->button_down = 1;
	console->reconfigure_trigger = 1;
	console->new_status = get_value();
	return 0;
}

int PluginOnToggle::cursor_moved_over()
{
	if(console->button_down && console->new_status != get_value())
	{
		update(console->new_status);
		plugin->on = get_value();
	}
}

int PluginOnToggle::button_release()
{
	console->button_down = 0;
}
