#ifndef PLUGINSET_H
#define PLUGINSET_H

#include "edits.h"
#include "edl.inc"
#include "keyframe.inc"
#include "module.inc"
#include "plugin.inc"
#include "pluginautos.inc"
#include "sharedlocation.inc"
#include "track.inc"

class PluginSet : public Edits
{
public:
	PluginSet(EDL *edl, Track *track);
	virtual ~PluginSet();

	virtual void synchronize_params(PluginSet *plugin_set);
	virtual PluginSet& operator=(PluginSet& plugins);
	virtual Plugin* create_plugin() { return 0; };
	void clear_keyframes(long start, long end);
// Clear edits only for a handle modification
	void clear_recursive(long start, long end);
	void shift_keyframes_recursive(long position, long length);
	void shift_effects_recursive(long position, long length);
	int clear(long start, long end);
	void copy(long start, long end, FileXML *file);
	void copy_keyframes(long start, long end, FileXML *file, int default_only);
	void paste_keyframes(long start, long length, FileXML *file, int default_only);
	void shift_effects(long start, long length);
	Edit* insert_edit_after(Edit *previous_edit);
	Edit* create_edit();
// The plugin set number in the track
	int get_number();
	void save(FileXML *file);
	void load(FileXML *file, unsigned long load_flags);
	void dump();
	int optimize();

// Insert a new plugin
	Plugin* insert_plugin(char *title, 
		long unit_position, 
		long unit_length,
		int plugin_type,
		SharedLocation *shared_location,
		KeyFrame *default_keyframe,
		int do_optimize);

	PluginAutos *automation;
};


#endif
