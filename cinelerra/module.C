#include "attachmentpoint.h"
#include "commonrender.h"
#include "console.h"
#include "edl.h"
#include "edlsession.h"
#include "filexml.h"
#include "mwindow.h"
#include "module.h"
#include "modules.h"
#include "patch.h"
#include "patchbay.h"
#include "plugin.h"
#include "pluginserver.h"
#include "renderengine.h"
#include "sharedlocation.h"
#include "track.h"
#include "tracks.h"
#include "transportque.h"


Module::Module(RenderEngine *renderengine, 
	CommonRender *commonrender, 
	Track *track)
{
	this->renderengine = renderengine;
	this->commonrender = commonrender;
	this->track = track;
	transition = 0;
	transition_server = 0;
	attachments = 0;
	total_attachments = 0;
	new_total_attachments = 0;
	new_attachments = 0;
}

Module::~Module()
{
	if(attachments)
	{
		for(int i = 0; i < track->plugin_set.total; i++)
		{
			if(attachments[i])
				delete attachments[i];
		}
		delete [] attachments;
	}
	if(transition_server)
	{
		transition_server->realtime_stop();
		transition_server->close_plugin();
		delete transition_server;
	}
}

void Module::create_objects()
{
	create_new_attachments();
	swap_attachments();
}

EDL* Module::get_edl()
{
	if(renderengine) 
		return renderengine->edl;
	else
		return edl;
}

void Module::create_new_attachments()
{
// Not needed in pluginarray
	if(commonrender)
	{
		new_total_attachments = track->plugin_set.total;
		new_attachments = new AttachmentPoint*[total_attachments];

// Create plugin servers later when nodes attach
		for(int i = 0; i < new_total_attachments; i++)
		{
			Plugin *plugin = 
				track->get_current_plugin(commonrender->current_position, 
					i, 
					renderengine->command->get_direction(),
					0);

			if(plugin && plugin->plugin_type != PLUGIN_NONE && plugin->on)
				new_attachments[i] = new_attachment(plugin);
			else
				new_attachments[i] = 0;
//printf("Module::create_new_attachments %p %p\n", plugin, new_attachments[i]);
		}
	}
}

void Module::swap_attachments()
{
// None of this is used in a pluginarray
	for(int new_attachment = 0, old_attachment = 0; 
		new_attachment < new_total_attachments &&
		old_attachment < total_attachments; 
		new_attachment++, old_attachment++)
	{
// Copy any old attachment which is identical to a new one
		if(new_attachments[new_attachment] &&
			attachments[old_attachment] &&
			new_attachments[new_attachment]->plugin ==
			attachments[old_attachment]->plugin)
		{
			delete new_attachments[new_attachment];
			new_attachments[new_attachment] = attachments[old_attachment];
			attachments[old_attachment] = 0;
		}
	}

// Delete old attachments which weren't identical to new ones
	for(int i = 0; i < total_attachments; i++)
	{
		if(attachments[i]) delete attachments[i];
	}
	if(attachments) delete attachments;
	
	if(new_total_attachments)
	{
		attachments = new_attachments;
		total_attachments = new_total_attachments;
	}
}

int Module::render_init()
{
	for(int i = 0; i < total_attachments; i++)
	{
		if(attachments[i])
			attachments[i]->render_init();
	}
	return 0;
}

AttachmentPoint* Module::attachment_of(Plugin *plugin)
{
//printf("Module::attachment_of 1 %d\n", total_attachments);
	for(int i = 0; i < total_attachments; i++)
	{
//printf("Module::attachment_of 2 %p\n", attachments[i]);
		if(attachments[i] && 
			attachments[i]->plugin == plugin) return attachments[i];
	}
	return 0;
}


// Test plugins for reconfiguration
int Module::test_plugins()
{
//printf("Module::test_plugins 1\n");
	if(total_attachments != track->plugin_set.total) return 1;
//printf("Module::test_plugins 2\n");

	for(int i = 0; i < total_attachments; i++)
	{
		AttachmentPoint *attachment = attachments[i];
		Plugin *plugin = track->get_current_plugin(commonrender->current_position, 
			i, 
			renderengine->command->get_direction(),
			0);
//printf("Module::test_plugins 3 %p %p\n", attachment, plugin);
// One exists and one doesn't
		int use_plugin = plugin &&
			plugin->plugin_type != PLUGIN_NONE &&
			plugin->on;

		if((attachment && !use_plugin) || 
			(!attachment && use_plugin)) return 1;
//printf("Module::test_plugins 4 %p %p\n", attachment, plugin);

// Plugin not the same
		if(plugin && 
			attachment &&
			attachment->plugin && 
			!plugin->identical(attachment->plugin)) return 1;
	}
//printf("Module::test_plugins 5\n");

	return 0;
}

void Module::update_transition(long current_position, int direction)
{
//printf("Module::update_transition 1\n");
	Plugin *transition = track->get_current_transition(current_position, 
		direction,
		0);
//printf("Module::update_transition 2 %p\n", transition);

	if((!transition && this->transition) || 
		(transition && this->transition && strcmp(transition->title, this->transition->title)))
	{
		this->transition = 0;

//printf("Module::update_transition 2\n");
		transition_server->realtime_stop();
//printf("Module::update_transition 2\n");
		transition_server->close_plugin();
//printf("Module::update_transition 2\n");
		delete transition_server;
//printf("Module::update_transition 2\n");
		transition_server = 0;
	}
//printf("Module::update_transition 3\n");

	if(transition && !this->transition)
	{
		this->transition = transition;
		PluginServer *plugin_server = renderengine->scan_plugindb(transition->title);
		transition_server = new PluginServer(*plugin_server);
		transition_server->open_plugin(0, renderengine->edl, transition);
		transition_server->init_realtime(renderengine->edl->session->real_time_playback &&
						renderengine->command->realtime,
						1);
	}

//printf("Module::update_transition 4\n");
// Test transition
//	
//	if((transition && !this->transition) || (!transition && this->transition)) return 1;
//	if(transition && transition != this->transition->plugin) return 1;
}


void Module::dump()
{
	printf("  Module\n");
	printf("   Plugins\n");
	for(int i = 0; i < total_attachments; i++)
	{
		if(attachments[i])
			attachments[i]->dump();
		else
			printf("    No Plugin\n");
	}
}




















Module::Module(MWindow *mwindow)
{
	this->mwindow = mwindow;
	console = mwindow->console;
//	modules = console->modules;
	mute = 0;
}

Patch* Module::get_patch_of()
{
//	return mwindow->patches->number(modules->number_of(this));
}

Track* Module::get_track_of()
{
//	return mwindow->tracks->number(modules->number_of(this));
}

int Module::render_stop(int duplicate)
{
	int i;
	for(i = 0; i < PLUGINS; i++)
	{
//		plugins[i]->render_stop(duplicate);
	}
}

int Module::swap_plugins(int number1, int number2)
{
//printf("Module::swap_plugins %d %d\n", number1, number2);
	for(int i = 0; i < PLUGINS; i++)
	{
		if(plugins[i]->plugin_type == 2)
		{
			if(plugins[i]->shared_location.module == number1)
				plugins[i]->shared_location.module = number2;
			else
			if(plugins[i]->shared_location.module == number2)
				plugins[i]->shared_location.module = number1;
		}

		if(plugins[i]->plugin_type == 3)
		{
//printf("Module::swap_plugins 1 %d\n", plugins[i]->shared_module_location.module);
			if(plugins[i]->shared_location.module == number1)
				plugins[i]->shared_location.module = number2;
			else
			if(plugins[i]->shared_location.module == number2)
				plugins[i]->shared_location.module = number1;
//printf("Module::swap_plugins 2 %d\n", plugins[i]->shared_module_location.module);
		}
	}
	return 0;
}


int Module::shift_module_pointers(int deleted_track)
{
	for(int i = 0; i < PLUGINS; i++)
	{
		if(plugins[i]->plugin_type == 2)
		{
			if(plugins[i]->shared_location.module > deleted_track)
				plugins[i]->shared_location.module--;
			else
			if(plugins[i]->shared_location.module == deleted_track)
//				plugins[i]->detach();
				;
		}

		if(plugins[i]->plugin_type == 3)
		{
			if(plugins[i]->shared_location.module > deleted_track)
				plugins[i]->shared_location.module--;
			else
			if(plugins[i]->shared_location.module == deleted_track)
//				plugins[i]->detach();
				;
		}
	}
	return 0;
}

int Module::shared_plugins(ArrayList<BC_ListBoxItem*> *shared_data, ArrayList<SharedPluginLocation*> *plugin_locations)
{
	int this_number = /* modules->number_of(this) */ 0;
	char string[1024];

	for(int i = 0; i < PLUGINS; i++)
	{
//		if(plugins[i]->plugin_server)
		{
// add all plugins
// put its title in the string
			sprintf(string, "%s: #%d", title, plugins[i]->get_plugin_number());

			shared_data->append(new BC_ListBoxItem(string));
			plugin_locations->append(new SharedPluginLocation(this_number, i));
		}
	}
	return 0;
}

int Module::toggles_selected(int on, int show, int mute)
{
	int total = 0;
	for(int i = 0; i < PLUGINS; i++)
	{
		if(on && plugins[i]->on) total++;
		if(show && plugins[i]->show) total++;
		if(mute && this->mute) total++;
	}
	return total;
}

int Module::select_all_toggles(int on, int show, int mute)
{
	for(int i = 0; i < PLUGINS; i++)
	{
		if(on && !plugins[i]->on)
		{
			plugins[i]->on = 1;
			if(console->gui) plugins[i]->on_toggle->update(plugins[i]->on);
		}
		if(show && !plugins[i]->show)
		{
			plugins[i]->show = 1;
			if(console->gui) plugins[i]->show_toggle->update(plugins[i]->show);
//			if(plugins[i]->plugin_server)
			{
//				plugins[i]->show_gui();
			}
		}
		if(mute && !this->mute)
		{
			this->mute = 1;
			if(console->gui) set_mute(this->mute);
		}
	}
	return 0;
}

int Module::deselect_all_toggles(int on, int show, int mute)
{
	for(int i = 0; i < PLUGINS; i++)
	{
		if(on && plugins[i]->on) 
		{
			plugins[i]->on = 0;
			if(console->gui) plugins[i]->on_toggle->update(plugins[i]->on);
		}

		if(show && plugins[i]->show) 
		{
			plugins[i]->show = 0;
			if(console->gui) plugins[i]->show_toggle->update(plugins[i]->show);
//			if(plugins[i]->plugin_server)
			{
//				plugins[i]->hide_gui();
			}
		}

		if(mute && this->mute) 
		{
			this->mute = 0;
			if(console->gui) set_mute(this->mute);
		}
	}
	return 0;
}

int Module::console_routing_used()
{
	int i;
	for(i = 0; i < PLUGINS; i++)
	{
		if(plugins[i]->on && plugins[i]->plugin_type) return 1;
	}
	return 0;
}

int Module::console_adjusting_used()
{
	if(fade != 100 || mute) 
		return 1;
	else
		return 0;
}


