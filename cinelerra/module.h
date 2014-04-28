#ifndef MODULE_H
#define MODULE_H

#include "attachmentpoint.inc"
#include "cache.inc"
#include "commonrender.inc"
#include "console.inc"
#include "datatype.h"
#include "edl.inc"
#include "filexml.inc"
#include "guicast.h"
#include "maxchannels.h"
#include "mwindow.inc"
#include "module.inc"
#include "modules.inc"
#include "patch.inc"
#include "plugin.inc"
#include "pluginserver.inc"
#include "pluginset.inc"
#include "renderengine.inc"
#include "sharedlocation.inc"
#include "track.inc"

class Module
{
public:
	Module(RenderEngine *renderengine, 
		CommonRender *commonrender, 
		Track *track);
	Module() {};
	Module(MWindow *mwindow);
	virtual ~Module();

	virtual void create_objects();
	void create_new_attachments();
	void swap_attachments();
	virtual AttachmentPoint* new_attachment(Plugin *plugin) { return 0; };
	int test_plugins();
	AttachmentPoint* attachment_of(Plugin *plugin);
	void dump();
	int render_init();
	void update_transition(long current_position, int direction);
	EDL* get_edl();

// CICache used during effect
	CICache *cache;
// EDL used during effect
	EDL *edl;
	CommonRender *commonrender;
	RenderEngine *renderengine;
	Track *track;
// TRACK_AUDIO or TRACK_VIDEO
	int data_type;       

// Pointer to transition in EDL
	Plugin *transition;
// PluginServer for transition
	PluginServer *transition_server;
// Currently active plugins.
// Use one AttachmentPoint for every pluginset to allow shared plugins to create
// extra plugin servers.
// AttachmentPoints are 0 if there is no plugin on the pluginset.
	AttachmentPoint **attachments;
	int total_attachments;
// AttachmentPoints are swapped in at render start to keep unchanged modules
// from resetting
	AttachmentPoint **new_attachments;
	int new_total_attachments;


	virtual int save(FileXML *xml) { return 0; };
	virtual int load(FileXML *xml, int track_offset) { return 0; };

	virtual int set_pixel(int pixel) { return 0; };
	virtual int set_title(char *new_title) { return 0; };
	virtual int flip_plugins(int &x, int &y) { return 0; };
	virtual int change_format() { return 0; };    // change between DB and INT for meter
	virtual int flip_vertical(int pixel) { return 0; };
	virtual int change_x(int new_pixel) { return 0; };
	virtual int change_y(int new_pixel) { return 0; };
	virtual int set_mute(int value) { return 0; };   // for setting all mutes on or off
	int render_stop(int duplicate);

	virtual int change_channels(int new_channels, int *value_positions) { return 0; };
	int shared_plugins(ArrayList<BC_ListBoxItem*> *shared_data, ArrayList<SharedPluginLocation*> *plugin_locations);

// swap the shared module numbers
	int swap_plugins(int number1, int number2);
	int shift_module_pointers(int deleted_track);
	int toggles_selected(int on, int show, int mute);
	int select_all_toggles(int on, int show, int mute);
	int deselect_all_toggles(int on, int show, int mute);

// Queries used for direct frame rendering
// Overlay mode isn't checked.
	int console_routing_used();
	int console_adjusting_used();
	
	Patch* get_patch_of();
	Track* get_track_of();








// REMOVE replaced by **attachments
	Plugin* plugins[PLUGINS];

// REMOVE
	float fade;
	int mute;
	int module_width, module_height;          // size of module
	char title[1024];
	MWindow *mwindow;
	Modules *modules;
	Console *console;
};

#endif
