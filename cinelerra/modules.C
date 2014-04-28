#include "amodule.h"
#include "console.h"
#include "filexml.h"
#include "mwindow.h"
#include "module.h"           // for module list item
#include "modules.h"
#include "patch.h"
#include "patchbay.h"
#include "mainsession.h"
#include "sharedlocation.h"       // for plugin location
#include "vmodule.h"
#include <string.h>

Modules::Modules(MWindow *mwindow, Console *console)
 : List<Module>()
{
	this->mwindow = mwindow;
	this->console = console;
}

Modules::~Modules()
{
	delete_all();    // delete objects
}

int Modules::load_console(FileXML *xml, Module* module, int track_offset)
{
	int result = 0;
	
	do{
		result = xml->read_tag();
		if(!result)
		{
			if(!strcmp(xml->tag.get_title(), "/TRACK"))
			{
				result = 1;
			}
			else
			if(!strcmp(xml->tag.get_title(), "MODULE"))
			{
				module->load(xml, track_offset);
			}
		}
	}while(!result);
}



int Modules::flip_vertical(int &pixel, int pixel_start)
{
	if(mwindow->gui)
	{
		Module *current;
		int offset;
		
		if(first)
		{
			for(current = first; current; current = NEXT, pixel += offset)
			{
				offset = (console->vertical ? current->module_width : current->module_height);
				current->flip_vertical(pixel - console->pixel_start);
			}
		}
	}
}

int Modules::redo_pixels(int &pixel)
{
	if(mwindow->gui)
	{
		Module *current;
		int offset = 0;

		if(first)
		{
			for(current = first; current; current = NEXT, pixel += offset)
			{
				offset = console->vertical ? current->module_width : current->module_height;
				current->set_pixel(pixel - console->pixel_start);
			}
		}
	}
}

int Modules::pixelmovement(int distance)
{
	if(mwindow->gui)
	{
		for(Module* current = first; current; current = NEXT)
		{
			if(console->vertical)
			current->change_x(distance);
			else
			current->change_y(distance);
		}
	}
}


int Modules::add_audio_track()
{
	AModule *new_module;
	int pixel = total_pixels() - console->pixel_start;      // append increases this

	append(new_module = new AModule(mwindow));
	new_module->create_objects();
}

int Modules::add_video_track()
{
	VModule *new_module;
	int pixel = total_pixels() - console->pixel_start;      // append increases this

	append(new_module = new VModule(mwindow));
	new_module->create_objects();
}

int Modules::delete_track()
{
// must delete according to derived class
	if(last)
	{
		delete last;
	}
}

int Modules::delete_track(Module *module)
{
	if(module->data_type == TRACK_AUDIO) delete (AModule*)module;
	else
	if(module->data_type == TRACK_VIDEO) delete (VModule*)module;
}

int Modules::delete_all()
{
	while(last)
	{
		delete_track();
	}
}

int Modules::change_channels(int old_channels, int new_channels)
{
	if(mwindow->gui)
	{
		for(Module* current = first; current; current = NEXT)
		{
			current->change_channels(new_channels, mwindow->session->achannel_positions);
		}
	}
}

int Modules::total_pixels()
{
	Module *current;
	int result = 0;
	
	for(current = first; current; current = NEXT)
	{
		result += console->vertical ? current->module_width : current->module_height;
	}

	return result;
}

int Modules::number_of(Module *module)
{
	int i;
	Module *current;

	for(i = 0, current = first; current && current != module; i++, current = NEXT)
		;

	return i;
}

int Modules::init_meters(int total_peaks)
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// allocate buffers
		if(current->data_type == TRACK_AUDIO) ((AModule*)current)->init_meters(total_peaks);
	}
}

int Modules::stop_meters()
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// allocate buffers
		if(current->data_type == TRACK_AUDIO) ((AModule*)current)->stop_meters();
	}
}

int Modules::reset_meters()
{
	if(mwindow->gui)
	{
		for(Module* current = first; current; current = NEXT)
		{
			if(current->data_type == TRACK_AUDIO) ((AModule*)current)->reset_meter();
		}
	}
}

int Modules::arender_init(int realtime_sched, int duplicate)
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// allocate buffers
		if(current->data_type == TRACK_AUDIO) current->render_init(realtime_sched, duplicate);
	}
}

int Modules::arender_stop(int duplicate)
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// stop plugins
		if(current->data_type == TRACK_AUDIO) current->render_stop(duplicate);
	}
}

int Modules::vrender_init(int duplicate)
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// allocate buffers
		if(current->data_type == TRACK_VIDEO) current->render_init(0, duplicate);
	}
}

int Modules::vrender_stop(int duplicate)
{
	Module* current;

	for(current = first; current; current = NEXT)
	{
// stop plugins
		if(current->data_type == TRACK_VIDEO) current->render_stop(duplicate);
	}
}

int Modules::update_meters(int peak_number, int last_peak, int total_peaks)
{
	if(mwindow->gui && !console->gui->get_hidden())
	{
		console->gui->lock_window();
		for(Module* current = first; current; current = NEXT)
		{
			if(current->data_type == TRACK_AUDIO) ((AModule*)current)->update_meter(peak_number, last_peak, total_peaks);
		}
		console->gui->unlock_window();
	}
}



int Modules::shared_aplugins(ArrayList<BC_ListBoxItem*> *shared_data, ArrayList<SharedPluginLocation*> *plugin_locations, int exclude_module)
{
	Module* current;
	int module_number;
	for(current = first, module_number = 0; current; current = NEXT, module_number++)
		if(current->data_type == TRACK_AUDIO && module_number != exclude_module) 
			current->shared_plugins(shared_data, plugin_locations);
}

int Modules::shared_amodules(ArrayList<BC_ListBoxItem*> *module_data, ArrayList<SharedModuleLocation*> *module_locations, int exclude_module)
{
	Module* current;
	int module_number;
	for(current = first, module_number = 0; current; current = NEXT, module_number++)
	{
		if(current->data_type == TRACK_AUDIO && module_number != exclude_module) 
		{
			module_data->append(new BC_ListBoxItem(current->title));
			module_locations->append(new SharedModuleLocation(module_number));
		}
	}
}

int Modules::shared_vplugins(ArrayList<BC_ListBoxItem*> *shared_data, ArrayList<SharedPluginLocation*> *plugin_locations, int exclude_module)
{
	Module* current;
	int module_number;
	for(current = first, module_number = 0; current; current = NEXT, module_number++)
		if(current->data_type == TRACK_VIDEO && module_number != exclude_module) 
			current->shared_plugins(shared_data, plugin_locations);
}

int Modules::shared_vmodules(ArrayList<BC_ListBoxItem*> *module_data, ArrayList<SharedModuleLocation*> *module_locations, int exclude_module)
{
	Module* current;
	int module_number;
	for(current = first, module_number = 0; current; current = NEXT, module_number++)
	{
		if(current->data_type == TRACK_VIDEO && module_number != exclude_module) 
		{
			module_data->append(new BC_ListBoxItem(current->title));
			module_locations->append(new SharedModuleLocation(module_number));
		}
	}
}

int Modules::change_format()
{
	for(Module* current = first; current; current = NEXT)
	{
		if(current->data_type == TRACK_AUDIO) current->change_format();
	}
}

int Modules::swap_plugins(Module* module1, Module* module2)
{
	int number1 = number_of(module1);
	int number2 = number_of(module2);
	Module* current = first;

	for( ; current; current = current->next)
	{
		current->swap_plugins(number1, number2);
	}
}


int Modules::playable_amodules(ArrayList<Module *> *playable_modules)
{
	Module* current = first;
	Patch* current_patch = mwindow->patches->first;

	for(; current && current_patch; 
		current = NEXT, current_patch = current_patch->next)
	{
		if(current_patch->play && current->data_type == TRACK_AUDIO)
		{
			playable_modules->append(current);
		}
	}
}

Module* Modules::module_number(int number)
{
	int i;
	Module* current;
	
	for(current = first, i = 0; i < number && current; i++, current = NEXT)
		;

	return current;
}

int Modules::number_of_audio(Module *module)
{
	int i;
	Module *current;

	for(i = 0, current = first; current && current != module; current = NEXT)
		if(current->data_type == TRACK_AUDIO) i++;

	return i;
}

int Modules::toggles_selected(int on, int show, int mute)
{
	int total = 0;
	Module *current;
	for(current = first; current; current = NEXT)
		total += current->toggles_selected(on, show, mute);

	return total;
}

int Modules::select_all_toggles(int on, int show, int mute)
{
	Module *current;
	for(current = first; current; current = NEXT)
		current->select_all_toggles(on, show, mute);
	
	return 0;
}

int Modules::deselect_all_toggles(int on, int show, int mute)
{
	Module *current;
	for(current = first; current; current = NEXT)
		current->deselect_all_toggles(on, show, mute);
	
	return 0;
}

