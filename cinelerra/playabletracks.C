#include "automation.h"
#include "edl.h"
#include "edlsession.h"
#include "mwindow.h"
#include "patchbay.h"
#include "playabletracks.h"
#include "plugin.h"
#include "preferences.h"
#include "renderengine.h"
#include "intauto.h"
#include "intautos.h"
#include "tracks.h"
#include "transportque.h"



PlayableTracks::PlayableTracks(RenderEngine *renderengine, 
	int *do_channel, 
	long current_position, 
	int data_type)
 : ArrayList<Track*>()
{
	this->renderengine = renderengine;
	this->data_type = data_type;
	this->do_channel = do_channel;

//printf("PlayableTracks::PlayableTracks 1 %d\n", renderengine->edl->tracks->total());
	for(Track *current_track = renderengine->edl->tracks->first; 
		current_track; 
		current_track = current_track->next)
	{
//printf("PlayableTracks::PlayableTracks 1 %d\n", is_playable(current_track, current_position));
		if(is_playable(current_track, current_position))
		{
			append(current_track);
		}
	}
//printf("PlayableTracks::PlayableTracks %d %d\n", data_type, total);
}


int PlayableTracks::is_playable(Track *current_track, long position)
{
	int result = 1;
	int direction = renderengine->command->get_direction();
	Auto *current = 0;
	IntAuto *play_auto = (IntAuto*)current_track->automation->play_autos->get_prev_auto(
		position, 
		direction,
		current);

	if(current_track->data_type != data_type) result = 0;

// Track is off screen and not bounced to other modules
//printf("PlayableTracks::is_playable 1 %d %d\n", 
//	result, 
//	current_track->channel_is_playable(position, direction, do_channel));


	if(result)
	{
		if(!current_track->plugin_used(position, direction) &&
			!current_track->channel_is_playable(position, direction, do_channel))
			result = 0;
	}
//printf("PlayableTracks::is_playable 2 %d\n", result);

// Play patch is off
	if(!play_auto->value)
	{
		result = 0;
	}

//printf("PlayableTracks::is_playable 3 %d %d\n", position, result);
	if(result)
	{
		int plugin_synthesis = 0;

// Test if synthesizing plugin exists
		for(int i = 0; i < current_track->plugin_set.total; i++)
		{
			PluginSet *plugin_set = current_track->plugin_set.values[i];
			Plugin *plugin = current_track->get_current_plugin(position, 
				i, 
				direction,
				0);
			if(plugin)
			{
				PluginServer *plugin_server = renderengine->scan_plugindb(plugin->title);
				if(plugin_server)
				{
					plugin_synthesis |= plugin_server->synthesis;
				}
			}
		}

// Test if edit exists under the current position.
		if(renderengine->edl->session->test_playback_edits)
		{
//printf("PlayableTracks::is_playable 3 %p\n", plugin_server);
			if(!current_track->playable_edit(position) &&
				!plugin_synthesis)
				result = 0;
		}
	}
//printf("PlayableTracks::is_playable 4 %d\n", result);

	return result;
}


int PlayableTracks::is_listed(Track *track)
{
	for(int i = 0; i < total; i++)
	{
		if(values[i] == track) return 1;
	}
	return 0;
}
