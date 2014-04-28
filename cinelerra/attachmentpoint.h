#ifndef ATTACHMENTPOINT_H
#define ATTACHMENTPOINT_H

#include "arraylist.h"
#include "filexml.inc"
#include "floatauto.inc"
#include "floatautos.inc"
#include "mwindow.inc"
#include "messages.inc"
#include "plugin.inc"
#include "pluginserver.inc"
#include "renderengine.inc"
#include "sharedlocation.h"
#include "vframe.inc"
#include "virtualnode.inc"

// Attachment points for Modules to attach plugins
class AttachmentPoint
{
public:
	AttachmentPoint(RenderEngine *renderengine, Plugin *plugin);
	virtual ~AttachmentPoint();

	virtual int reset_parameters();

// Attach a virtual plugin for realtime playback.
	int attach_virtual_plugin(VirtualNode *virtual_plugin);
	virtual void new_buffer_vectors() {};
	virtual void delete_buffer_vectors() {};

// return 0 if ready to render
// check all the virtual plugins for waiting status
// all virtual plugins attached to this must be waiting for a render
	int sort(VirtualNode *virtual_plugin);

// Move new_virtual_plugins to virtual_plugins if duplicate == 0.
	int render_init();
	void render(long current_position, 
		long fragment_size);
	virtual void dispatch_plugin_server(int buffer_number, 
		long current_position, 
		long fragment_size) {};

// For unshared plugins, virtual plugins to send configuration events to.
// For shared plugins, virtual plugins to allocate buffers for.
	ArrayList<VirtualNode*> virtual_plugins;
// List for a new virtual console which is later transferred to virtual_plugins.
	ArrayList<VirtualNode*> new_virtual_plugins;
// Plugin servers to do signal processing
	ArrayList<PluginServer*> plugin_servers;
// For returning to the virtual module and determining if a new plugin must be started.
	int total_input_buffers, new_total_input_buffers;

// renderengine Plugindb entry
	PluginServer *plugin_server;
// Pointer to the plugin configuration in EDL
	Plugin *plugin;
	RenderEngine *renderengine;
// Current input buffer being loaded.  Determines when the plugin server is run
	int current_buffer;








	AttachmentPoint(MWindow *mwindow);
	AttachmentPoint(AttachmentPoint *that);

















// For multichannel plugins store the fragment positions for each plugin
// and render the plugin when the last fragment position is stored
	int render(int double_buffer_in, int double_buffer_out, 
				long fragment_position_in, long fragment_position_out,
				long size, int node_number, 
				long source_position, long source_len, 
				FloatAutos *autos = 0,
				FloatAuto **start_auto = 0,
				FloatAuto **end_auto = 0,
				int reverse = 0);

	int multichannel_shared(int search_new);
	int singlechannel();

// Simply deletes the virtual plugin 
	int render_stop(int duplicate);

// Set up a plugin before attaching
	int update(int plugin_type, int in, int out, char* title, SharedPluginLocation *shared_plugin_location, SharedModuleLocation *shared_module_location);
// Plugin updates its title
	virtual int update_derived() {};
// Attach a new plugin
	int attach(int is_loading = 0);
// Control the gui for the plugin.
	int show_gui();
	int hide_gui();
// Called by the plugin when it is hidden.
	int set_show(int value);
	virtual int set_show_derived(int value) {};
// Default title when nothing is attached;
	virtual char* default_title() {};


// The track offset sets the first track number when importing a shared module or plugin.
	int load(FileXML *file, int track_offset, char *terminator);
	int save(FileXML *file, char *block_title);

	AttachmentPoint& operator=(const AttachmentPoint &that);
	int operator==(const AttachmentPoint &that);

// Get the track or module title.
	virtual char* get_module_title();

// Update the widgets after loading.
	virtual int update_display() {};
// Draw edit after attaching
// Set is_loading during a load operation so overlays aren't drawn
	virtual int update_edit(int is_loading) {};
	
	int dump();

	int get_configuration_change(char *data);

// location of plugin if shared
	SharedPluginLocation shared_plugin_location;
	SharedModuleLocation shared_module_location;

//	int in, out, show, on;
//	int plugin_type;       // 0: none  1: executable   2: shared plugin   3: module
	char plugin_data[MESSAGESIZE];     // configuration data for the plugin
	char plugin_title[1024];    // title of plugin
	MWindow *mwindow;

// Only used by operator= and copy constructor
	int copy_from(const AttachmentPoint &that);
	int identical(const AttachmentPoint &that);
};

#endif
