#ifndef MWINDOW_H
#define MWINDOW_H

#include "assets.inc"
#include "audiodevice.inc"
#include "awindow.inc"
#include "edit.inc"
#include "guicast.h"
#include "cache.inc"
#include "channel.inc"
#include "cwindow.inc"
#include "defaults.inc"
#include "edl.inc"
#include "filexml.inc"
#include "levelwindow.inc"
#include "loadmode.inc"
#include "mainindexes.inc"
#include "mainprogress.inc"
#include "mainundo.inc"
#include "mwindow.inc"
#include "mwindowgui.inc"
#include "maxchannels.h"
#include "mutex.inc"
#include "new.inc"
#include "patchbay.inc"
#include "tracking.inc"
#include "playbackengine.inc"
#include "plugin.inc"
#include "pluginset.inc"
#include "pluginserver.inc"
#include "preferences.inc"
#include "preferencesthread.inc"
#include "recordlabel.inc"
#include "render.inc"
#include "mainsession.inc"
#include "sharedlocation.inc"
#include "theme.inc"
#include "threadloader.inc"
#include "timebar.inc"
#include "timebomb.h"
#include "track.inc"
#include "tracks.inc"
#include "transition.inc"
#include "transportque.inc"
#include "videowindow.inc"
#include "vwindow.inc"


// All entry points for commands except for window locking should be here.
// This allows scriptability.

class MWindow
{
public:
	MWindow();
	~MWindow();

// ======================================== initialization commands
	void create_objects(int want_gui, int want_new);
	static void init_tuner(ArrayList<Channel*> &channeldb, char *path);

	int run_script(FileXML *script);
	int new_project();
	int delete_project(int flash = 1);

	int load_defaults();
	int save_defaults();
	int set_filename(char *filename);
// Total vertical pixels in timeline
	int get_tracks_height();
// Total horizontal pixels in timeline
	int get_tracks_width();
// Show windows
	void show_vwindow();
	void show_awindow();
	void show_lwindow();
	void show_cwindow();
	void tile_windows();
	void set_titles(int value);
	int asset_to_edl(EDL *new_edl, Asset *new_asset, RecordLabels *labels = 0);

// Entry point to insert assets and insert edls.
// Takes the drag vectors from MainSession and
// pastes either assets or clips depending on which is full.
// Returns 1 if the vectors were full
	int paste_assets(double position, Track *dest_track);
	
// Insert the assets at a point in the EDL
	void load_assets(ArrayList<Asset*> *new_assets, 
		double position, 
		int load_mode,
		Track *first_track /* = 0 */,
		RecordLabels *labels /* = 0 */,
		int edit_labels,
		int edit_plugins);
	int paste_edls(ArrayList<EDL*> *new_edls, 
		int load_mode, 
		Track *first_track /* = 0 */,
		double current_position /* = -1 */,
		int edit_labels,
		int edit_plugins);
// Reset everything for a load
	void update_project(int load_mode);
	void fit_selection();
// move the window to include the cursor
	void find_cursor();
// Append a plugindb with pointers to the master plugindb
	void create_plugindb(int do_audio, 
		int do_video, 
		int is_realtime, 
		int is_transition,
		int is_theme,
		ArrayList<PluginServer*> &plugindb);
// Find the plugin whose title matches title and return it
	PluginServer* scan_plugindb(char *title);
	void dump_plugins();



	
	int load_filenames(ArrayList<char*> *filenames, 
		int load_mode = LOAD_REPLACE);
	
	int interrupt_indexes();  // Stop index building


	int redraw_time_dependancies();     // after reconfiguring the time format, sample rate, frame rate

// =========================================== movement

	void next_time_format();
	int reposition_timebar(int new_pixel, int new_height);
	int expand_sample();
	int zoom_in_sample();
	int zoom_sample(long zoom_sample);
	void zoom_amp(long zoom_amp);
	void zoom_track(long zoom_track);
	int fit_sample();
	int move_left(long distance = 0);
	int move_right(long distance = 0);
	void move_up(long distance = 0);
	void move_down(long distance = 0);
	int next_label();   // seek to labels
	int prev_label();
	void trackmovement(int track_start);
	int samplemovement(long view_start);     // view_start is pixels
	void select_all();
	int goto_start();
	int goto_end();
	int expand_y();
	int zoom_in_y();
	int expand_t();
	int zoom_in_t();
	void crop_video();
	void update_plugins();
// Call after every edit operation
	void save_backup();
	void show_plugin(Plugin *plugin);
	void hide_plugin(Plugin *plugin, int lock);
	void hide_plugins();
// Update plugins with configuration changes
	void update_plugin_guis();
	void update_plugin_states();
	void update_plugin_titles();


// ============================= editing commands ========================

	void add_audio_track_entry();
	int add_audio_track();
	void add_clip_to_edl(EDL *edl);
	void add_video_track_entry();
	int add_video_track();

	void asset_to_size();
	void clear();
	void clear_labels();
	int clear_labels(double start, double end);
	void concatenate_tracks();
	void copy();
	int copy(double start, double end);
	static int create_aspect_ratio(float &w, float &h, int width, int height);
	void cut();

	void delete_folder(char *folder);
	void delete_inpoint();
	void delete_outpoint();    

	void delete_track();
	void delete_track(Track *track);
	void delete_tracks();
//	void extract_selection();
	int feather_edits(long feather_samples, int audio, int video);
	long get_feather(int audio, int video);
	float get_aspect_ratio();
	void insert(double position, 
		FileXML *file,
		int edit_labels,
		int edit_plugins,
		EDL *parent_edl = 0);
	void insert_effect(char *title, 
		SharedLocation *shared_location, 
		Track *track,
		PluginSet *plugin_set,
		double start,
		double length,
		int plugin_type);

	void match_output_size(Track *track);
// Move edit to new position
	void move_edits(ArrayList<Edit*> *edits,
		Track *track,
		double position);
// Move effect to position
	void move_effect(Plugin *plugin,
		PluginSet *plugin_set,
		Track *track,
		long position);
	void move_plugins_up(PluginSet *plugin_set);
	void move_plugins_down(PluginSet *plugin_set);
	void move_track_down(Track *track);
	void move_tracks_down();
	void move_track_up(Track *track);
	void move_tracks_up();
	void mute_selection();
	void new_folder(char *new_folder);
	void overwrite(EDL *source);
// For clipboard commands
	void paste();
// For splice and overwrite
	int paste(double start, 
		double end, 
		FileXML *file,
		int edit_labels,
		int edit_plugins);
	int paste_output(long startproject, 
				long endproject, 
				long startsource_sample, 
				long endsource_sample, 
				long startsource_frame,
				long endsource_frame,
				Asset *asset, 
				RecordLabels *new_labels);
	void paste_silence();

	void paste_audio_transition();
	void paste_video_transition();
	void rebuild_indices();
	void redo_entry();
// Asset removal
	void remove_assets_from_project(int push_undo = 0);
	void remove_assets_from_disk();
	void render_single();
	void render_list();
	void resize_track(Track *track, int w, int h);
	void set_auto_keyframes(int value);
// Update the editing mode
	int set_editing_mode(int new_editing_mode);
	void set_inpoint(int is_mwindow);
	void set_outpoint(int is_mwindow);
	void splice(EDL *source);
	void toggle_loop_playback();
	void trim_selection();
// Synchronize EDL settings with all playback engines depending on current 
// operation.  Doesn't redraw anything.
	void sync_parameters(int change_type = CHANGE_PARAMS);
	void to_clip();
	int toggle_label(int is_mwindow);
	void undo_entry();

	int cut_automation();
	int copy_automation();
	int paste_automation();
	int clear_automation();
	int cut_default_keyframe();
	int copy_default_keyframe();
// Use paste_automation to paste the default keyframe in other position.
// Use paste_default_keyframe to replace the default keyframe with whatever is
// in the clipboard.
	int paste_default_keyframe();
	int clear_default_keyframe();

	int modify_edithandles();
	int modify_pluginhandles();
	void finish_modify_handles();

	
	
	
	
	

// Send new EDL to caches
	void update_caches();
	int optimize_assets();            // delete unused assets from the cache and assets

// ================================= cursor selection ======================

// start a cursor selection
	int init_selection(long cursor_position, 
					int cursor_x, 
					int cursor_y, 
					int &current_end, 
					long &selection_midpoint1,
					long &selection_midpoint2,
					int &selection_type);
	int update_selection(long cursor_position,
					int cursor_x, 
					int cursor_y, 
					int &current_end, 
					long selection_midpoint1,
					long selection_midpoint2,
					int selection_type);
	int end_selection();
	void select_point(double position);
	int set_loop_boundaries();         // toggle loop playback and set boundaries for loop playback

// ================================ handle selection =======================

	int init_handle_selection(long cursor_position, int handle_pixel, int which_handle);     // handle selection


	LevelWindow *level_window;
	Tracks *tracks;
	PatchBay *patches;
	MainUndo *undo;
	TimeBar *timebar;
	Defaults *defaults;
	Assets *assets;
// CICaches for drawing timeline only
	CICache *audio_cache, *video_cache;
	ThreadLoader *threadloader;
	VideoWindow *video_window;
	Preferences *preferences;
	PreferencesThread *preferences_thread;
	MainSession *session;
	Theme *theme;
	MainIndexes *mainindexes;
	MainProgress *mainprogress;

// Menu items
	ArrayList<ColormodelItem*> colormodels;

	int reset_meters();



// ====================================== plugins ==============================

// Contain file descriptors for all the dlopens
	ArrayList<PluginServer*> *plugindb;
// Currently visible plugins
	ArrayList<PluginServer*> *plugin_guis;


// TV stations to record from
	ArrayList<Channel*> channeldb_v4l;
	ArrayList<Channel*> channeldb_buz;
// Adjust sample position to line up with frames.
	int fix_timing(long &samples_out, long &frames_out, long samples_in);     



	Render *render;
	Render *renderlist;
// Master edl
	EDL *edl;
// Main Window GUI
	MWindowGUI *gui;
// Compositor
	CWindow *cwindow;
// Viewer
	VWindow *vwindow;
// Asset manager
	AWindow *awindow;
// Levels
	LevelWindow *lwindow;
// Lock during creation and destruction of GUI
	Mutex *plugin_gui_lock;

	void init_render();
	static void init_defaults(Defaults* &defaults);
	void init_edl();
	void init_awindow();
// Used by MWindow and RenderFarmClient
	static void init_plugins(Preferences *preferences, ArrayList<PluginServer*>* &plugindb);
	void init_preferences();
	void init_theme();
	void init_compositor();
	void init_levelwindow();
	void init_viewer();
	void init_cache();
	void init_menus();
	void init_indexes();
	void init_gui();
	void init_playbackcursor();
	void delete_plugins();
	void clean_indexes();
	void save_tuner(ArrayList<Channel*> &channeldb, char *path);
//	TimeBomb timebomb;
};

#endif