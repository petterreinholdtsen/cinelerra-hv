#include "apatchgui.h"
#include "atrack.inc"
#include "automation.h"
#include "floatauto.h"
#include "floatautos.h"
#include "clip.h"
#include "edl.h"
#include "filexml.h"
#include "localsession.h"
#include "mainundo.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "patchbay.h"
#include "patchgui.h"
#include "mainsession.h"
#include "theme.h"
#include "track.h"
#include "tracks.h"
#include "vpatchgui.h"
#include "vtrack.inc"



PatchBay::PatchBay(MWindow *mwindow, MWindowGUI *gui)
 : BC_SubWindow(mwindow->theme->patchbay_x,
 	mwindow->theme->patchbay_y,
	mwindow->theme->patchbay_w,
	mwindow->theme->patchbay_h)
{
	button_down = 0;
	reconfigure_trigger = 0;
	this->mwindow = mwindow;
	this->gui = gui;
}

PatchBay::~PatchBay() 
{
	delete_all();
}

int PatchBay::create_objects()
{
	draw_top_background(get_parent(), 0, 0, get_w(), get_h());
	flash();
	return 0;
}

void PatchBay::resize_event()
{
	reposition_window(mwindow->theme->patchbay_x,
		mwindow->theme->patchbay_y,
		mwindow->theme->patchbay_w,
		mwindow->theme->patchbay_h);
	draw_top_background(get_parent(), 0, 0, get_w(), get_h());
	update();
	flash();
}

void PatchBay::change_meter_format(int mode, float min)
{
	for(int i = 0; i < patches.total; i++)
	{
		PatchGUI *patchgui = patches.values[i];
		if(patchgui->data_type == TRACK_AUDIO)
		{
			APatchGUI *apatchgui = (APatchGUI*)patchgui;
			if(apatchgui->meter)
			{
				apatchgui->meter->change_format(mode, min);
			}
		}
	}
}

void PatchBay::update_meters(ArrayList<double> *module_levels)
{
	for(int level_number = 0, patch_number = 0;
		patch_number < patches.total && level_number < module_levels->total;
		patch_number++)
	{
		APatchGUI *patchgui = (APatchGUI*)patches.values[patch_number];

//printf("PatchBay::update_meters %d %d\n", patch_number, patchgui->data_type);
		if(patchgui->data_type == TRACK_AUDIO)
		{
//printf("PatchBay::update_meters %p\n",  patchgui->meter);
			if(patchgui->meter)
			{
				double level = module_levels->values[level_number];
//printf("PatchBay::update_meters %d %f\n", level_number, level);
				patchgui->meter->update(level, level > 1);
			}

			level_number++;
		}
	}
}

void PatchBay::reset_meters()
{
	for(int patch_number = 0;
		patch_number < patches.total;
		patch_number++)
	{
		APatchGUI *patchgui = (APatchGUI*)patches.values[patch_number];
		if(patchgui->data_type == TRACK_AUDIO && patchgui->meter)
		{
			patchgui->meter->reset_over();
		}
	}
}

void PatchBay::stop_meters()
{
	for(int patch_number = 0;
		patch_number < patches.total;
		patch_number++)
	{
		APatchGUI *patchgui = (APatchGUI*)patches.values[patch_number];
//printf("PatchBay::update_meters %d %d\n", patch_number, patchgui->data_type);
		if(patchgui->data_type == TRACK_AUDIO && patchgui->meter)
		{
			patchgui->meter->reset();
		}
	}
}


#define PATCH_X 3

int PatchBay::update()
{
	int patch_count = 0;
//printf("PatchBay::update 1\n");

// Every patch has a GUI regardless of whether or not it is visible.
// Make sure GUI's are allocated for every patch and deleted for non-existant
// patches.
	for(Track *current = mwindow->edl->tracks->first;
		current;
		current = NEXT, patch_count++)
	{
		PatchGUI *patchgui;
		int y = current->y_pixel;

//printf("PatchBay::update 2\n");
		if(patches.total > patch_count)
		{
//printf("PatchBay::update 2.1\n");
			if(patches.values[patch_count]->track->id != current->id)
			{
//printf("PatchBay::update 2.2\n");
				delete patches.values[patch_count];
//printf("PatchBay::update 2.3\n");

				switch(current->data_type)
				{
					case TRACK_AUDIO:
						patchgui = patches.values[patch_count] = new APatchGUI(mwindow, this, (ATrack*)current, PATCH_X, y);
						break;
					case TRACK_VIDEO:
						patchgui = patches.values[patch_count] = new VPatchGUI(mwindow, this, (VTrack*)current, PATCH_X, y);
						break;
				}
//printf("PatchBay::update 2.4\n");
				patchgui->create_objects();
//printf("PatchBay::update 2.5\n");
			}
			else
			{
//printf("PatchBay::update 2.6\n");
				patches.values[patch_count]->update(PATCH_X, y);
//printf("PatchBay::update 2.7\n");
			}
		}
		else
		{
			switch(current->data_type)
			{
				case TRACK_AUDIO:
					patchgui = new APatchGUI(mwindow, this, (ATrack*)current, PATCH_X, y);
					break;
				case TRACK_VIDEO:
					patchgui = new VPatchGUI(mwindow, this, (VTrack*)current, PATCH_X, y);
					break;
			}
			patches.append(patchgui);
			patchgui->create_objects();
		}
	}
//printf("PatchBay::update 3\n");

	while(patches.total > patch_count)
	{
		delete patches.values[patches.total - 1];
		patches.remove_number(patches.total - 1);
	}
//printf("PatchBay::update 4\n");

	return 0;
}

void PatchBay::synchronize_faders(float change, int data_type, Track *skip)
{
//printf("PatchBay::synchronize_faders 1\n");
	for(Track *current = mwindow->edl->tracks->first;
		current;
		current = NEXT)
	{
//printf("PatchBay::synchronize_faders %p %p\n", current, skip);
		if(current->data_type == data_type &&
			current->gang && 
			current != skip)
		{
			FloatAutos *fade_autos = current->automation->fade_autos;
			double position = mwindow->edl->local_session->selectionstart;
			int update_undo = !fade_autos->auto_exists_for_editing(position);

			if(update_undo)
				mwindow->undo->update_undo_before("fade", LOAD_AUTOMATION);

			FloatAuto *keyframe = (FloatAuto*)fade_autos->get_auto_for_editing(position);

			keyframe->value += change;
			if(update_undo)
				mwindow->undo->update_undo_after();


			for(int i = 0; i < patches.total; i++)
			{
				if(patches.values[i]->track == current)
					patches.values[i]->update(patches.values[i]->x,
						patches.values[i]->y);
			}
		}
	}

//printf("PatchBay::synchronize_faders 2\n");
}


int PatchBay::resize_event(int top, int bottom)
{
	reposition_window(mwindow->theme->patchbay_x,
 		mwindow->theme->patchbay_y,
		mwindow->theme->patchbay_w,
		mwindow->theme->patchbay_h);
	return 0;
}

// REMOVE
int PatchBay::load_patches(FileXML *xml, Patch *current_patch)
{
	return 0;
}


int PatchBay::add_track(int start_pixel, char *default_title, int data_type)
{
	return 0;
}

int PatchBay::delete_track(int start_pixel)
{
	return 0;
}

int PatchBay::delete_track(Patch *patch, int start_pixel)
{
	return 0;
}

int PatchBay::delete_all()
{
	return 0;
}

int PatchBay::expand_t(int start_pixel)
{
	return 0;
}

int PatchBay::zoom_in_t(int start_pixel)
{
	return 0;
}

int PatchBay::trackmovement(int distance)
{
	return 0;
}

int PatchBay::redo_pixels(int start_pixel)
{
	return 0;
}

int PatchBay::number_of(Patch *patch)
{
	return 0;
}

Patch* PatchBay::number(int number)
{
	return 0;
}

int PatchBay::copy_length()
{
	return 0;
}

BC_TextBox* PatchBay::atrack_title_number(int number)    // return textbox of atrack #
{
	return 0;
}

int PatchBay::total_playable_atracks()
{
	return 0;
}

int PatchBay::total_playable_vtracks()
{
	return 0;
}

int PatchBay::total_recordable_atracks()
{
	return 0;
}

int PatchBay::total_recordable_vtracks()
{
	return 0;
}

int PatchBay::deselect_all_play()
{
	return 0;
}

int PatchBay::select_all_play()
{
	return 0;
}

int PatchBay::deselect_all_record()
{
	return 0;
}

int PatchBay::select_all_record()
{
	return 0;
}

int PatchBay::deselect_all_auto()
{
	return 0;
}

int PatchBay::deselect_all_draw()
{
	return 0;
}

int PatchBay::select_all_auto()
{
	return 0;
}

int PatchBay::select_all_draw()
{
	return 0;
}

int PatchBay::plays_selected()
{
	return 0;
}

int PatchBay::records_selected()
{
	return 0;
}

int PatchBay::autos_selected()
{
	return 0;
}

int PatchBay::draws_selected()
{
	return 0;
}

int PatchBay::total_audio()
{
	return 0;
}

int PatchBay::total_video()
{
	return 0;
}


int PatchBay::cursor_motion()
{
	return 0;
}

int PatchBay::button_release()
{
	return 0;
}
