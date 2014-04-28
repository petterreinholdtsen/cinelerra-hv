#ifndef FLIP_H
#define FLIP_H


class FlipMain;

#include "filexml.h"
#include "flipwindow.h"
#include "guicast.h"
#include "pluginvclient.h"

class FlipConfig
{
public:
	FlipConfig();
	void copy_from(FlipConfig &that);
	int equivalent(FlipConfig &that);
	void interpolate(FlipConfig &prev, 
		FlipConfig &next, 
		long prev_frame, 
		long next_frame, 
		long current_frame);
	int flip_horizontal;
	int flip_vertical;
};

class FlipMain : public PluginVClient
{
public:
	FlipMain(PluginServer *server);
	~FlipMain();

// required for all realtime plugins
	int process_realtime(VFrame *input_ptr, VFrame *output_ptr);
	int is_realtime();
	char* plugin_title();
	int show_gui();
	void raise_window();
	int set_string();
	void update_gui();
	int load_configuration();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	int load_defaults();
	int save_defaults();
	VFrame* new_picon();

// a thread for the GUI
	FlipThread *thread;

// Utilities used by flip.
	int swap_pixels(VPixel *in, VPixel *out);
	Defaults *defaults;
	FlipConfig config;
};


#endif
