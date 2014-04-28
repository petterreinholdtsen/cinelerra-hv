#ifndef MAINSESSION_H
#define MAINSESSION_H

#include "assets.inc"
#include "auto.inc"
#include "defaults.inc"
#include "edit.inc"
#include "guicast.h"
#include "mainsession.inc"
#include "maxchannels.h"
#include "mwindow.inc"
#include "plugin.inc"
#include "pluginset.inc"
#include "pluginserver.inc"
#include "track.inc"

// Options not in EDL but not changed in preferences
class MainSession
{
public:
	MainSession(MWindow *mwindow);
	~MainSession();

	int load_defaults(Defaults *defaults);
	int save_defaults(Defaults *defaults);
	void default_window_positions();
	void boundaries();

// REMOVE AND REPLACE WITH EDL.H
// Editing mode
//	int editing_mode;
// for vertical tracks
	int tracks_vertical;
// sample start of track view
	long view_start;
// pixel start of track view
	long track_start;
// zooming of the timeline
	long zoom_sample, zoom_y, zoom_track;
// labels follow edits during editing
	int labels_follow_edits;
// automation follows edits during editing
 	int autos_follow_edits;
// align cursor on frame boundaries
	int cursor_on_frames;




// For drag and drop events

// The entire track where the dropped asset is going to go
	Track *track_highlighted;
// The edit after the point where the media is going to be dropped.
	Edit *edit_highlighted;
// The plugin set where the plugin is going to be dropped.
	PluginSet *pluginset_highlighted;
// The plugin after the point where the plugin is going to be dropped.
	Plugin *plugin_highlighted;
// Viewer canvas highlighted
	int vcanvas_highlighted;
// Compositor canvas highlighted
	int ccanvas_highlighted;
// Current drag operation
	int current_operation;
// Item being dragged
	ArrayList <PluginServer*> *drag_pluginservers;
	Plugin *drag_plugin;
	ArrayList<Asset*> *drag_assets;
	ArrayList<EDL*> *drag_clips;
	Auto *drag_auto;

// Edit whose handle is being dragged
	Edit *drag_edit;
// Edits who are being dragged
	ArrayList<Edit*> *drag_edits;
// Button pressed during drag
	int drag_button;
// Handle being dragged
	int drag_handle;
// Current position of drag cursor
	double drag_position;
// Starting position of drag cursor
	double drag_start;
// Cursor position when button was pressed
	int drag_origin_x, drag_origin_y;
// Value of keyframe when button was pressed
	float drag_start_percentage;
	long drag_start_position;

// Audio session
	int changes_made;
	int audio_channels;
	int achannel_positions[MAXCHANNELS];
// sample rate used by audio
	long sample_rate;

// Video session
	float cwindow_zoom;
	float vwindow_zoom;
// frame rate used by video
	float frame_rate;
// size of the compositing surface
	int track_w, track_h;              
// size of the video output
	int output_w, output_h;            
// Aspect ratio
	float aspect_w, aspect_h;
// filename of the current project for window titling and saving
	char filename[BCTEXTLEN];

// Maintain playback cursor on GUI
//	int playback_cursor_visible;
// Position of playback cursor and position for pauses
//	long playback_position;

// Window positions
	int lwindow_x, lwindow_y, lwindow_w, lwindow_h;
	int mwindow_x, mwindow_y, mwindow_w, mwindow_h;
	int vwindow_x, vwindow_y, vwindow_w, vwindow_h;
	int cwindow_x, cwindow_y, cwindow_w, cwindow_h;
	int ctool_x, ctool_y;
	int awindow_x, awindow_y, awindow_w, awindow_h;
	int rmonitor_x, rmonitor_y, rmonitor_w, rmonitor_h;
	int rwindow_x, rwindow_y, rwindow_w, rwindow_h;
	int afolders_w;
	int show_vwindow, show_awindow, show_cwindow, show_lwindow;

	MWindow *mwindow;
};

#endif
