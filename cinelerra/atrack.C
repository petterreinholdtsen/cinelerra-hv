#include "aedit.h"
#include "aedits.h"
#include "amodule.h"
#include "apluginset.h"
#include "assets.h"
#include "atrack.h"
#include "autoconf.h"
#include "aautomation.h"
#include "edit.h"
#include "edl.h"
#include "edlsession.h"
#include "cache.h"
#include "clip.h"
#include "datatype.h"
#include "file.h"
#include "filexml.h"
#include "floatautos.h"
#include "localsession.h"
#include "mainsession.h"
#include "mwindow.h"
#include "panautos.h"
#include "pluginbuffer.h"
#include "theme.h"
#include "trackcanvas.h"
#include "tracks.h"

#include <string.h>

ATrack::ATrack(EDL *edl, Tracks *tracks)
 : Track(edl, tracks)
{
	data_type = TRACK_AUDIO;
}

ATrack::~ATrack()
{
}

// Used by PlaybackEngine
void ATrack::synchronize_params(Track *track)
{
	Track::synchronize_params(track);

	ATrack *atrack = (ATrack*)track;
}

int ATrack::copy_settings(Track *track)
{
	Track::copy_settings(track);

	ATrack *atrack = (ATrack*)track;
	return 0;
}


int ATrack::save_header(FileXML *file)
{
	file->tag.set_property("TYPE", "AUDIO");
	return 0;
}

int ATrack::save_derived(FileXML *file)
{
	char string[BCTEXTLEN];
	file->append_newline();
	return 0;
}

int ATrack::load_header(FileXML *file, unsigned long load_flags)
{
	return 0;
}


int ATrack::load_derived(FileXML *file, unsigned long load_flags)
{
	return 0;
}

int ATrack::create_objects()
{
	Track::create_objects();
	automation = new AAutomation(edl, this);
	automation->create_objects();
	edits = new AEdits(edl, this);
	return 0;
}

int ATrack::vertical_span(Theme *theme)
{
	int track_h = Track::vertical_span(theme);
	int patch_h = 0;
	if(expand_view)
	{
		patch_h += theme->title_h + theme->play_h + theme->fade_h + theme->meter_h + theme->pan_h;
	}
	return MAX(track_h, patch_h);
}

PluginSet* ATrack::new_plugins()
{
	return new APluginSet(edl, this);
}

int ATrack::load_defaults(Defaults *defaults)
{
	Track::load_defaults(defaults);
	return 0;
}

void ATrack::set_default_title()
{
	Track *current = ListItem<Track>::owner->first;
	int i;
	for(i = 0; current; current = NEXT)
	{
		if(current->data_type == TRACK_AUDIO) i++;
	}
	sprintf(title, "Audio %d", i);
}

long ATrack::to_units(double position, int round)
{
	if(round)
		return Units::round(position * edl->session->sample_rate);
	else
		return Units::to_long(position * edl->session->sample_rate);
}

double ATrack::to_doubleunits(double position)
{
	return position * edl->session->sample_rate;
}

double ATrack::from_units(long position)
{
	return (double)position / edl->session->sample_rate;
}


int ATrack::identical(long sample1, long sample2)
{
// Units of samples
	if(labs(sample1 - sample2) <= 1) return 1; else return 0;
}






















ATrack::ATrack(MWindow *mwindow, Tracks *tracks) : Track(mwindow, tracks)
{
	data_type = TRACK_AUDIO;
}

int ATrack::set_index_files(int flash, Asset *asset)
{
	int result = 0;
	AEdit* current;

	if(mwindow->gui)
	{
		result = 1;
		for(current = (AEdit*)edits->first; current; current = (AEdit*)NEXT)
		{
// check for matching asset
			if(current->asset && current->asset == asset)
			{
				if(mwindow->session->tracks_vertical)
					current->set_index_file(flash, pixel + mwindow->session->zoom_track / 2, 0, 0, tracks->canvas->get_w(), tracks->canvas->get_h());
				else
					current->set_index_file(flash, pixel + mwindow->session->zoom_track / 2, 0, 0, tracks->canvas->get_w(), tracks->canvas->get_h());

				result = 0;
			}
		}
	}

	return result;
}


int ATrack::change_channels(int oldchannels, int newchannels)
{
	int i;
	return 0;
}

int ATrack::render(PluginBuffer *shared_output, 
			   long offset, 
			   long input_len, 
               long input_position) // always start of range
{
	Edit *current_edit;

	bzero((char*)shared_output->get_data() + offset * sizeof(float), input_len * sizeof(float));

	for(current_edit = edits->first; 
		current_edit;
		current_edit = current_edit->next)
	{
		long edit_start = current_edit->startproject;
		long edit_end = current_edit->startproject + current_edit->length + current_edit->feather_right;

		if((edit_start >= input_position && edit_start < input_position + input_len)
			 ||
			(edit_end > input_position && edit_end <= input_position + input_len)
			 ||
			(edit_start <= input_position && edit_end >= input_position + input_len))
		{
			((AEdit*)current_edit)->render(shared_output, offset, input_len, input_position);
		}
	}

	return 0;
}

// =========================== drawing commands ==================

int ATrack::draw_derived(int x, int w, int y, int h, int flash)
{       // make sure this track is visible
	int center_pixel;    // pixel of center line in canvas
	center_pixel = pixel + mwindow->session->zoom_track / 2;

	tracks->canvas->set_color(RED);        // draw zero line
	if(mwindow->session->tracks_vertical)
	tracks->canvas->draw_line(center_pixel, y, center_pixel, y + h);
	else
	tracks->canvas->draw_line(x, center_pixel, x + w, center_pixel);
}


int ATrack::draw_autos_derived(float view_start, float zoom_units, AutoConf *auto_conf)
{
	int i;
	
	for(i = 0; i < mwindow->session->audio_channels; i++)
	{
// 		if(auto_conf->pan) 
// 			pan_autos->draw(tracks->canvas, 
// 							pixel, 
// 							mwindow->session->zoom_track, 
// 							zoom_units, 
// 							view_start, 
// 							mwindow->session->tracks_vertical);
	}
}

int ATrack::draw_floating_autos_derived(float view_start, float zoom_units, AutoConf *auto_conf, int flash)
{
	int i;

	for(i = 0; i < mwindow->session->audio_channels; i++)
	{
// 		if(auto_conf->pan[i]) 
// 			pan_autos[i]->draw_floating_autos(tracks->canvas, 
// 									pixel, 
// 									mwindow->session->zoom_track, 
// 									zoom_units, 
// 									view_start, 
// 									mwindow->session->tracks_vertical, flash);
	}
}

long ATrack::length()
{
	return edits->length();
}

int ATrack::get_dimensions(double &view_start, 
	double &view_units, 
	double &zoom_units)
{
	view_start = (double)edl->local_session->view_start * edl->session->sample_rate;
	view_units = (double)0;
//	view_units = (double)tracks->view_samples();
	zoom_units = (double)edl->local_session->zoom_sample;
}

int ATrack::copy_derived(long start, long end, FileXML *file)
{
// automation is taken care of by Track::copy_automation
}

int ATrack::copy_automation_derived(AutoConf *auto_conf, 
	long selectionstart, 
	long selectionend, 
	FileXML *file)
{
	int i, result = 0;

	for(i = 0, result = 0; i < edl->session->audio_channels; i++)
	{
// 		if(auto_conf->pan[i])
// 		{
// 			file->tag.set_title("PANAUTOS");
// 			file->tag.set_property("CHANNEL", i);
// 			file->append_tag();
// 
// 			automation->pan_autos[i]->copy(selectionstart, selectionend, file, 1);
// 
// 			file->tag.set_title("/PANAUTOS");
// 			file->append_tag();
// 			file->append_newline();
// 			result = 1;
//		}
	}

	return result;
}


int ATrack::paste_automation_derived(long selectionstart, long selectionend, long total_length, FileXML *xml, int shift_autos, int &current_pan)
{
// only used for automation editing routines
	if(xml->tag.title_is("PANAUTOS") && current_pan < MAXCHANNELS)
	{
//		current_pan = xml->tag.get_property("CHANNEL", current_pan);
//		pan_autos[current_pan++]->paste(selectionstart, selectionend, total_length, xml, "/PANAUTOS", 1, shift_autos);
	}
	return 1;
}

int ATrack::clear_automation_derived(AutoConf *auto_conf, long selectionstart, long selectionend, int shift_autos)
{
// used when clearing just automation
	int i, result;

//	for(i = 0, result = 0; i < mwindow->session->audio_channels && !result; i++)
//		if(auto_conf->pan[i]) result = 1;

	for(i = 0; i < mwindow->session->audio_channels && result; i++)
	{
//		pan_autos[i]->clear(selectionstart, selectionend, 1, shift_autos);
	}
	
	return result;
}

int ATrack::paste_derived(long start, long end, long total_length, FileXML *xml, int &current_channel)
{
	if(!strcmp(xml->tag.get_title(), "PANAUTOS"))
	{
		current_channel = xml->tag.get_property("CHANNEL", current_channel);
//		pan_autos->paste(start, end, total_length, xml, "/PANAUTOS", mwindow->session->autos_follow_edits);
		return 1;
	}
	return 0;
}

int ATrack::paste_output(long startproject, long endproject, long startsource, long endsource, int channel, Asset *asset)
{
	int result = 0;

	result = ((AEdits*)edits)->paste_edit(startproject, endproject, startsource, endsource - startsource, channel, asset);
	//if(!result && mwindow->autos_follow_edits)
	//{
	//	paste_auto_silence(startproject, endproject);
	//}
	return result;
}

int ATrack::clear_derived(long start, long end)
{
}

int ATrack::paste_auto_silence_derived(long start, long end)
{
	for(int i = 0; i < mwindow->session->audio_channels; i++)
	{
//		pan_autos[i]->paste_silence(start, end);
	}
}

int ATrack::select_auto_derived(float zoom_units, float view_start, AutoConf *auto_conf, int cursor_x, int cursor_y)
{
	int result = 0;
	int i, temp;

	for(i = mwindow->session->audio_channels - 1, result = 0; i >= 0 && !result; i--)
	{
// 		if(auto_conf->pan[i]) 
// 			result = pan_autos[i]->select_auto(tracks->canvas, 
// 											pixel, 
// 											mwindow->session->zoom_track, 
// 											zoom_units, 
// 											view_start, 
// 											cursor_x, 
// 											cursor_y, 
// 											mwindow->session->tracks_vertical);
	}
	
	return result;
}

int ATrack::move_auto_derived(float zoom_units, float view_start, AutoConf *auto_conf, int cursor_x, int cursor_y, int shift_down)
{
	int result, i;
	result = 0;
	
	for(i = 0; i < mwindow->session->audio_channels && !result; i++)
	{
// 		if(auto_conf->pan[i]) 
// 			result = pan_autos[i]->move_auto(tracks->canvas, 
// 										pixel, 
// 										mwindow->session->zoom_track, 
// 										zoom_units, 
// 										view_start, 
// 										cursor_x, 
// 										cursor_y, 
// 										shift_down, 
// 										mwindow->session->tracks_vertical);
	}
		
	return result;
}

int ATrack::release_auto_derived()
{
	int result, i;
	result = 0;

	for(i = 0; i < mwindow->session->audio_channels && !result; i++)
	{
//		result = pan_autos[i]->release_auto();
	}
		
	return result;
}

int ATrack::scale_time_derived(float rate_scale, int scale_edits, int scale_autos, long start, long end)
{
	for(int i = 0; i < mwindow->session->audio_channels; i++)
	{
//		pan_autos[i]->scale_time(rate_scale, scale_edits, scale_autos, start, end);
	}
}
