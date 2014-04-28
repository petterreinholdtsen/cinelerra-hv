#ifndef VMODULE_H
#define VMODULE_H

class VModuleGUI;
class VModuleTitle;
class VModuleFade;
class VModuleMute;
class VModuleMode;

#define VMODULEHEIGHT 91
#define VMODULEWIDTH 106


#include "guicast.h"
#include "datatype.h"
#include "edl.inc"
#include "filexml.inc"
#include "floatautos.inc"
#include "maxchannels.h"
#include "module.h"
#include "overlayframe.inc"
#include "pluginbuffer.inc"
#include "sharedlocation.inc"
#include "track.inc"
#include "vedit.inc"
#include "vframe.inc"

class VModule : public Module
{
public:
	VModule(RenderEngine *renderengine, CommonRender *commonrender, Track *track);
	VModule() {};
	VModule(MWindow *mwindow);
	virtual ~VModule();

	void create_objects();
	AttachmentPoint* new_attachment(Plugin *plugin);

	CICache* get_cache();
	int import_frame(VFrame *output,
		VEdit *current_edit,
		long input_position,
		int direction);
	int render(VFrame *output,
		long input_position,
		int direction);

	int save(FileXML *xml);
	int load(FileXML *xml, int track_offset);

	int set_pixel(int pixel);
	int create_plugins(int &x, int &y);
	int flip_plugins(int &x, int &y);

	int flip_vertical(int pixel);
	int change_x(int new_pixel);
	int change_y(int new_pixel);
	int set_mute(int value);

// synchronization with tracks
	FloatAutos* get_fade_automation();       // get the fade automation for this module
	int set_title(char *text);

// Method of alpha channel calculation
	int mode;
	int pixel;

// Temp frames for loading from file handlers
	VFrame *input_temp;
	PluginBuffer *temp_shm;
// Temp frame for transition
	VFrame *transition_temp;
	PluginBuffer *transition_shm;
// Engine for transferring from file to buffer_in
	OverlayFrame *overlayer;

	VModuleGUI *gui;
};

class VModuleGUI : public BC_SubWindow
{
public:
	VModuleGUI(MWindow *mwindow, VModule *module, int x, int y, int w, int h);
	~VModuleGUI();
	
	int create_objects();     // don't want subwindow to call create_objects until pans are created
	int flip_vertical(int pixel);

	VModuleMute* mute_toggle;
	VModuleFade* fade_slider;
	VModuleTitle* title;
	MWindow *mwindow;
	Console *console;
	VModule *module;
	BC_Title *fade_title;
	BC_Title *mute_title;
	VModuleMode *mode_popup;
	BC_Title *mode_title;
};

class VModuleTitle : public BC_TextBox
{
public:
	VModuleTitle(VModule *module, Patch *patch, int x, int y);
	int handle_event();
	VModule *module;
	Patch *patch;
};

class VModuleFade : public BC_ISlider
{
public:
	VModuleFade(VModule *module, int x, int y, int w, int h);
	~VModuleFade();
	
	int handle_event();
	VModule *module;
};

class VModuleMute : public BC_CheckBox
{
public:
	VModuleMute(Console *console, VModule *module, int x, int y);
	int handle_event();
	int cursor_moved_over();
	int button_release();

	Console *console;
	VModule *module;
};

class VModuleMode : public BC_PopupMenu
{
public:
	VModuleMode(Console *console, VModule *module, int x, int y);
	~VModuleMode();

	int handle_event();
	int add_items();         // add initial items
	char* mode_to_text(int mode);

	Console *console;
	VModule *module;
};

class VModuleModeItem : public BC_MenuItem
{
public:
	VModuleModeItem(VModuleMode *popup, char *text, int mode);
	~VModuleModeItem();

	int handle_event();
	VModuleMode *popup;
	int mode;
};

#endif
