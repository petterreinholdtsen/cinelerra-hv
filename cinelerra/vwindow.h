#ifndef VWINDOW_H
#define VWINDOW_H

#include "assets.inc"
#include "clipedit.inc"
#include "edl.inc"
#include "mwindow.inc"
#include "thread.h"
#include "transportque.inc"
#include "vplayback.inc"
#include "vtracking.inc"
#include "vwindowgui.inc"

class VWindow : public Thread
{
public:
	VWindow(MWindow *mwindow);
	~VWindow();

	void load_defaults();
	int create_objects();
	void run();
// Change source to asset, creating a new EDL
	void change_source(Asset *asset);
// Change source to EDL
	void change_source(EDL *edl);
// Change source to folder and item number
	void change_source(char *folder, int item);
// Remove source
	void remove_source();
// Returns private EDL of VWindow
	EDL* get_edl();
// Returns last argument of change_source or 0 if it was an EDL
	Asset* get_asset();
	void update(int do_timebar);
		
	void update_position(int change_type = CHANGE_NONE,
		int use_slider = 1,
		int update_slider = 0);
	void set_inpoint();
	void set_outpoint();
	void clear_inpoint();
	void clear_outpoint();
	void copy();
	void splice_selection();
	void overwrite_selection();	
	void delete_edl();
	void goto_start();
	void goto_end();


	VTracking *playback_cursor;

// Number of source in GUI list
	MWindow *mwindow;
	VWindow *vwindow;
	VWindowGUI *gui;
	VPlayback *playback_engine;
	ClipEdit *clip_edit;
// Object being played back.
// An EDL is created for every asset dropped in but shared for clips dropped in
	EDL *edl;
// Pointer to asset for accounting
	Asset *asset;
	int edl_shared;
};


#endif
