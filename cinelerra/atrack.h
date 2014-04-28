#ifndef ATRACK_H
#define ATRACK_H

#include "arraylist.h"
#include "autoconf.inc"
#include "edl.inc"
#include "filexml.inc"
#include "floatautos.inc"
#include "linklist.h"
#include "maxchannels.h"
#include "panautos.inc"
#include "pluginbuffer.inc"
#include "track.h"




class ATrack : public Track
{
public:
	ATrack(EDL *edl, Tracks *tracks);
	ATrack() { };
	ATrack(MWindow *mwindow, Tracks *tracks);
	~ATrack();

	int create_objects();
	int load_defaults(Defaults *defaults);
	void set_default_title();
	PluginSet* new_plugins();
	int vertical_span(Theme *theme);
	int save_header(FileXML *file);
	int save_derived(FileXML *file);
	int load_header(FileXML *file, unsigned long load_flags);
	int load_derived(FileXML *file, unsigned long load_flags);
	int copy_settings(Track *track);
	int identical(long sample1, long sample2);
	void synchronize_params(Track *track);
	long to_units(double position, int round);
	double to_doubleunits(double position);
	double from_units(long position);








// ====================================== initialization
	int create_derived_objs(int flash);
	int set_index_files(int flash, Asset *asset);


	int change_channels(int oldchannels, int newchannels);

// ==================================== rendering
// Offset: offset in floats from the start of the buffer.
// Input_len: length in floats of the segment to read.
	int render(PluginBuffer *shared_output, 
		long offset, long input_len, long input_position);

// ==================================== drawing
	int draw_derived(int x, int w, int y, int h, int flash = 1);
	int draw_autos_derived(float view_start, float zoom_units, AutoConf *auto_conf);
	int draw_floating_autos_derived(float view_start, float zoom_units, AutoConf *auto_conf, int flash);

// ===================================== editing
	int copy_derived(long start, long end, FileXML *xml);
	int paste_derived(long start, long end, long total_length, FileXML *xml, int &current_channel);
	int paste_output(long startproject, long endproject, long startsource, long endsource, int channel, Asset *asset);
	int clear_derived(long start, long end);

	int copy_automation_derived(AutoConf *auto_conf, long selectionstart, long selectionend, FileXML *xml);
	int paste_automation_derived(long selectionstart, long selectionend, long total_length, FileXML *xml, int shift_autos, int &current_pan);
	int clear_automation_derived(AutoConf *auto_conf, long selectionstart, long selectionend, int shift_autos = 1);
	int paste_auto_silence_derived(long start, long end);

	int select_auto_derived(float zoom_units, float view_start, AutoConf *auto_conf, int cursor_x, int cursor_y);
	int move_auto_derived(float zoom_units, float view_start, AutoConf *auto_conf, int cursor_x, int cursor_y, int shift_down);
	int release_auto_derived();
	int modify_handles(long oldposition, long newposition, int currentend);
	int scale_time_derived(float rate_scale, int scale_edits, int scale_autos, long start, long end);

	long length();
	int get_dimensions(double &view_start, 
		double &view_units, 
		double &zoom_units);
};

#endif
