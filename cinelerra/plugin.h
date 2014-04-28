#ifndef PLUGIN_H
#define PLUGIN_H

#include "guicast.h"
#include "console.inc"
#include "edit.h"
#include "edl.inc"
#include "filexml.inc"
#include "keyframe.inc"
#include "keyframes.inc"
#include "messages.inc"
#include "mwindow.inc"
#include "module.inc"
#include "plugin.inc"
#include "pluginbuffer.inc"
#include "pluginset.inc"
#include "pluginpopup.inc"
#include "pluginserver.inc"
#include "sharedlocation.h"
#include "virtualnode.inc"

class PluginOnToggle;



// Plugin is inherited by Transition, Plugins
class Plugin : public Edit
{
public:
// Plugin which belongs to a transition.
	Plugin(EDL *edl, 
		Track *track, 
		char *title);
// Plugins that belongs to a plugin set.
// Plugin can't take a track because it would get edits from the track instead
// of the plugin set.
	Plugin(EDL *edl, 
		PluginSet *plugin_set, 
		char *title);
	virtual ~Plugin();

	virtual Plugin& operator=(Plugin& edit);
	virtual Edit& operator=(Edit& edit);

	virtual int operator==(Plugin& that);
	virtual int operator==(Edit& that);

	virtual void copy_from(Edit *edit);
	virtual int identical(Plugin *that);
	virtual void synchronize_params(Edit *edit);
// For synchronizing parameters
	void copy_keyframes(Plugin *plugin);
// For copying to clipboard
	void copy_keyframes(long start, long end, FileXML *file, int default_only);
// For editing automation
	void clear_keyframes(long start, long end);
	void copy(long start, long end, FileXML *file);
	void paste(FileXML *file);
	void load(FileXML *file);
// Shift in time
	void shift(long difference);
// get the number of this plugin in the module
	int get_plugin_number();
	void dump();
// Get keyframes for configuring plugin
	KeyFrame* get_prev_keyframe(long position);
	KeyFrame* get_next_keyframe(long position);
// If this is a standalone plugin fill its location in the result.
// If it's shared copy the shared location into the result
	void get_shared_location(SharedLocation *result);
// Get keyframes for editing with automatic creation if enabled
	virtual KeyFrame* get_keyframe();
	int silence();
// Calculate title given plugin type
	void calculate_title(char *string);
// Resolve objects pointed to by shared_location
	Track* get_shared_track();
	Plugin* get_shared_plugin();

// Need to resample keyframes
	void resample(double old_rate, double new_rate);

// The title of the plugin is stored and not the plugindb entry in case it doesn't exist in the db
// Title of the plugin currently attached
	char title[BCTEXTLEN];           
	int plugin_type;
	int in, out, show, on;
	PluginSet *plugin_set;

// Data for the plugin is stored here.  Default keyframe always exists.
// As for storing in PluginSet instead of Plugin:

// Each plugin needs a default keyframe of its own.
// The keyframes are meaningless except for the plugin they're stored in.
// Default keyframe has position = 0.
// Other keyframes have absolute position.
	KeyFrames *keyframes;

// location of plugin if shared
	SharedLocation shared_location;















// REMOVE
//	SharedPluginLocation shared_plugin_location;
//	SharedModuleLocation shared_module_location;






// REMOVE replaced by keyframes
	char data[MESSAGESIZE];     // Data for the plugin currently attached
// REMOVE
	Plugin(MWindow *mwindow, Module *module, int plugin_number);




	int update_derived();


// swap module numbers when moving tracks
	int swap_modules(int number1, int number2);
	int set_show_derived(int value);
	int set_string();     // set the string that appears on the plugin
	virtual int use_gui() {};       // whether or not the module has a gui
	char* get_module_title();
	int resize_plugin(int x, int y);
// Update the widgets after loading
	int update_display();
	char* default_title();

// settings for plugin
	int plugin_number;          // number of this plugin in the module starting with 1

	MWindow *mwindow;
	PluginPopup *plugin_popup;
	PluginShowToggle *show_toggle;
	PluginOnToggle *on_toggle;
	BC_Title *show_title;
	BC_Title *on_title;

	Module *module;
};



class PluginShowToggle : public BC_Radial
{
public:
	PluginShowToggle(Plugin *plugin, Console *console, int x, int y);
	int handle_event();
	int cursor_moved_over();
	int button_release();

	Console *console;
	Plugin *plugin;
};

class PluginOnToggle : public BC_Radial
{
public:
	PluginOnToggle(Plugin *plugin, Console *console, int x, int y);
	int handle_event();
	int cursor_moved_over();
	int button_release();

	Console *console;
	Plugin *plugin;
};

/* class PluginSoloToggle : public BC_Radial
{
public:
	PluginSoloToggle();
	~PluginSoloToggle();

	int handle_event();
};
 */




#endif
