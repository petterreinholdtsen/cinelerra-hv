#ifndef PATCHBAY_H
#define PATCHBAY_H

#include "guicast.h"
#include "filexml.inc"
#include "mwindow.inc"
#include "mwindowgui.inc"
#include "patch.h"
#include "patchbay.inc"
#include "patchgui.inc"



class PatchBay
 : public BC_SubWindow
{
public:
	PatchBay(MWindow *mwindow, MWindowGUI *gui);
	~PatchBay();

	int create_objects();
	void resize_event();
// Synchronize with Master EDL
	int update();
	void update_meters(ArrayList<double> *module_levels);
	void stop_meters();
	void synchronize_faders(float value, int data_type, Track *skip);
	void change_meter_format(int mode, float min);
	void reset_meters();

	ArrayList<PatchGUI*> patches;










// =========================================== drawing

	int button_release();
	int cursor_motion();
	int resize_event(int top, int bottom);

// =========================================== editing

// need the data type to get the proper title
	int add_track(int start_pixel, char *default_title, int data_type);
	int delete_track(int start_pixel);
	int delete_track(Patch *patch, int start_pixel);
	int delete_all();
	int expand_t(int start_pixel);
	int zoom_in_t(int start_pixel);
	int trackmovement(int distance);
	int redo_pixels(int start_pixel);


	int deselect_all_play();
	int select_all_play();
	int deselect_all_record();
	int select_all_record();
	int deselect_all_auto();
	int select_all_auto();
	int deselect_all_draw();
	int select_all_draw();
	int plays_selected();
	int records_selected();
	int autos_selected();
	int draws_selected();

// queries for getting title
	int total_audio();
	int total_video();
	int total_playable_atracks();
	int total_playable_vtracks();
	int total_recordable_atracks();
	int total_recordable_vtracks();
	int number_of(Patch *patch);        // patch number of pointer
	int copy_length();
	Patch* number(int number);      // pointer to patch number

	MWindow *mwindow;
	MWindowGUI *gui;

	BC_TextBox* atrack_title_number(int number);    // return textbox of atrack # to console 
	int button_down, new_status, reconfigure_trigger;
};

#endif
