#ifndef TRACKS_H
#define TRACKS_H


#include "autoconf.h"
#include "cursor.inc"
#include "edl.inc"
#include "file.inc"
#include "filexml.inc"
#include "linklist.h"
#include "mwindow.inc"
#include "pluginserver.inc"
#include "threadindexer.inc"
#include "track.h"
#include "trackcanvas.inc"
#include "transition.inc"



class Tracks : public List<Track>
{
public:
	Tracks();
	Tracks(EDL *edl);
	virtual ~Tracks();

	Tracks& operator=(Tracks &tracks);
	int load(FileXML *xml, int &track_offset, unsigned long load_flags);
	void move_edits(ArrayList<Edit*> *edits, 
		Track *track,
		double position,
		int edit_labels,
		int edit_plugins);
	void move_effect(Plugin *plugin,
		PluginSet *plugin_set,
		Track *track, 
		long position);

// Construct a list of all the recordable edits which start on position
	void get_affected_edits(ArrayList<Edit*> *drag_edits, 
		double position, 
		Track *start_track);

	void equivalent_output(Tracks *tracks, double *result);

	int move_track_up(Track *track);        // move recordable tracks up
	int move_track_down(Track *track);      // move recordable tracks down
	int move_tracks_up();                   // move recordable tracks up
	int move_tracks_down();                 // move recordable tracks down
	void paste_audio_transition(PluginServer *server);
	void paste_video_transition(PluginServer *server, int first_track = 0);

	void paste_transition(PluginServer *server, Edit *dest_edit);
// Return the numbers of tracks with the play patch enabled
	int playable_audio_tracks();
	int playable_video_tracks();
// Return number of tracks with the record patch enabled
	int recordable_audio_tracks();
	int recordable_video_tracks();
	int total_audio_tracks();
	int total_video_tracks();
// return the longest track in all the tracks in seconds
 	double total_length();
 	double total_video_length();
// Update y pixels after a zoom
	void update_y_pixels(Theme *theme);
// Total number of tracks where the following toggles are selected
	void select_all(int play, 
		int record, 
		int automate, 
		int gang, 
		int draw, 
		int mute,
		int expand,
		int value);
	void translate_camera(float offset_x, float offset_y);
	void translate_projector(float offset_x, float offset_y);
	int total_of(int play, 
		int record, 
		int automate, 
		int gang, 
		int draw, 
		int mute,
		int expand);
// add a track
	Track* add_audio_track(int to_end = 1);
	Track* add_video_track(int to_end = 1);
	int delete_track();     // delete last track
	int delete_track(Track* track);        // delete any track

	EDL *edl;
	
	
	
	
	
	
	
	
	
	
	
	Tracks(MWindow *mwindow);

	int create_objects(Defaults *defaults, int w, int h, int top, int bottom);
	int load_defaults(Defaults *defaults);
	int save_defaults(Defaults *defaults);

	int change_channels(int oldchannels, int newchannels);
	int set_index_file(int flash, Asset *asset);
	int dump();

// ======================================= file operations

	int update_old_filename(char *old_filename, char *new_filename);

// ======================================= drawing

	int show_overlays(int flash);
	int hide_overlays(int flash);

	int resize_event(int w, int h, int top, int bottom);
	int flip_vertical(int top, int bottom);

	int draw_cursor(int flash);
	int draw_loop_points(int flash);

	int toggle_handles();
	int toggle_titles();
	int set_draw_output();

	int draw_handles(int flash);
	int draw_floating_handle(int flash);

	int draw_titles(int flash);

	int toggle_auto_fade();
	int toggle_auto_play();
	int toggle_auto_mute();
// set the video autos since only one can be visible at a time
	int set_show_autos(int camera, int project);
	int toggle_auto_project();
	int toggle_auto_project_z();
	int toggle_auto_camera();
	int toggle_auto_camera_z();
	int toggle_auto_pan(int pan);
	int toggle_auto_plugin(int plugin);
	int draw_autos(int flash);
	int show_autos(int flash);
	int hide_autos(int flash);
	int draw_floating_autos(int flash);
	int draw_playback_cursor(int x, int flash = 1);
	int draw_loop_points(long start, long end, int flash);


// ================================== movement

	int zoom_y();
	int expand_y();
	int expand_t();
	int zoom_t();
	int samplemovement(long distance);
	int trackmovement(long distance);
	int move_up(long distance = 0);
	int move_down(long distance = 0);
	int redo_pixels();           // reset all the track pixels after a delete or move

// ================================== track editing
	int import_audio_track(long length, int channel, Asset *asset);
	int import_video_track(long length, int layer, Asset *asset);
	int import_vtransition_track(long length, Asset *asset);
	int import_atransition_track(long length, Asset *asset);
	int add_vtransition_track(int flash = 1);
	int add_atransition_track(int flash = 1);

// Change references to shared modules in all tracks from old to new.
// If do_swap is true values of new are replaced with old.
	void change_modules(int old_location, int new_location, int do_swap);
// Append all the tracks to the end of the recordable tracks
	int concatenate_tracks(int edit_plugins);
	int copyable_tracks(long start, long end);  // return number of tracks to copy
// Change references to shared plugins in all tracks
	void change_plugins(SharedLocation &old_location, SharedLocation &new_location, int do_swap);

	int delete_audio_track();       // delete the last audio track
	int delete_video_track();        // delete the last video track
	int delete_tracks();     // delete all the recordable tracks
	int delete_all(int flash = 1);      // delete just the tracks
	int delete_all_tracks();      // delete just the tracks
// Swap link list positions of tracks including references
	void swap_tracks(Track *first, Track *second);

// ================================== EDL editing
	int copy(double start, 
		double end, 
		int all, 
		FileXML *file, 
		char *output_path = "");



	int copy_assets(FileXML *xml, 
		double start, 
		double end, 
		int all);
	int clear(double start, double end, int clear_plugins);
// Returns the point to restart background rendering at.
// -1 means nothing changed.
	void clear_automation(double selectionstart, 
		double selectionend);
	int clear_default_keyframe();
	int clear_handle(double start, 
		double end,
		double &longest_distance,
		int clear_labels,
		int clear_plugins);
	int copy_automation(double selectionstart, 
		double selectionend, 
		FileXML *file,
		int default_only,
		int autos_only);
	int copy_default_keyframe(FileXML *file);
	void paste_automation(double selectionstart, 
		FileXML *xml,
		int default_only);
	int paste_default_keyframe(FileXML *file);
	int paste(long start, long end);
// all units are samples by default
	int paste_output(long startproject, 
				long endproject, 
				long startsource_sample, 
				long endsource_sample, 
				long startsource_frame, 
				long endsource_frame, 
				Asset *asset);
	int paste_silence(double start, 
		double end, 
		int edit_plugins);
	int purge_asset(Asset *asset);
	int asset_used(Asset *asset);
	int select_translation(int cursor_x, int cursor_y);    // select video coordinates for frame
	int update_translation(int cursor_x, int cursor_y, int shift_down);
// Transition popup
	int popup_transition(int cursor_x, int cursor_y);
	int select_auto(int cursor_x, int cursor_y);
	int move_auto(int cursor_x, int cursor_y, int shift_down);
	int modify_edithandles(double &oldposition, 
		double &newposition, 
		int currentend, 
		int handle_mode,
		int edit_labels,
		int edit_plugins);
	int modify_pluginhandles(double &oldposition, 
		double &newposition, 
		int currentend, 
		int handle_mode,
		int edit_labels);
	int end_translation();
	int select_handles();
	int select_region();
	int select_edit(long cursor_position, int cursor_x, int cursor_y, long &new_start, long &new_end);
	int feather_edits(long start, long end, long samples, int audio, int video);
	long get_feather(long selectionstart, long selectionend, int audio, int video);
	int reset_translation(long start, long end);
// Move edit boundaries and automation during a framerate change
	int scale_time(float rate_scale, int ignore_record, int scale_edits, int scale_autos, long start, long end);

// ================================== accounting

	int handles, titles;               // show handles or titles
	int show_output;          // what type of video to draw
	AutoConf auto_conf;      // which autos are visible
	int overlays_visible;
	int view_start;         // vertical start of track view
	int view_pixels();          // return the view width in pixels from the canvas
	int vertical_pixels();       // return the view height in pixels
	long view_samples();      // return the view width in samples from the canvas
	double total_playable_length();     // Longest track.
// Used by equivalent_output
	int total_playable_vtracks();
	double total_recordable_length();   // Longest track with recording on
	int totalpixels();       // height of all tracks in pixels
	int number_of(Track *track);        // track number of pointer
	Track* number(int number);      // pointer to track number
	int copy_length(long start, long end);

	MWindow *mwindow;
	TrackCanvas *canvas;
	Cursor_ *cursor;

private:
	int paste_assets(FileXML *xml);
};

#endif
