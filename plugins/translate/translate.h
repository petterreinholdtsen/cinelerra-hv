#ifndef TRANSLATE_H
#define TRANSLATE_H

// the simplest plugin possible

class TranslateMain;

#include "defaults.h"
#include "mutex.h"
#include "translatewin.h"
#include "overlayframe.h"
#include "pluginvclient.h"

class TranslateConfig
{
public:
	TranslateConfig();
	int equivalent(TranslateConfig &that);
	void copy_from(TranslateConfig &that);
	void interpolate(TranslateConfig &prev, 
		TranslateConfig &next, 
		long prev_frame, 
		long next_frame, 
		long current_frame);

	float in_x, in_y, in_w, in_h, out_x, out_y, out_w, out_h;
};


class TranslateMain : public PluginVClient
{
public:
	TranslateMain(PluginServer *server);
	~TranslateMain();

// required for all realtime plugins
	int process_realtime(VFrame *input_ptr, VFrame *output_ptr);
	int is_realtime();
	char* plugin_title();
	int start_realtime();
	int stop_realtime();
	int show_gui();
	void raise_window();
	void update_gui();
	int set_string();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	VFrame* new_picon();
	int load_defaults();
	int save_defaults();
	int load_configuration();

// a thread for the GUI
	TranslateThread *thread;

	OverlayFrame *overlayer;   // To translate images
	VFrame *temp_frame;        // Used if buffers are the same
	Defaults *defaults;
	TranslateConfig config;
};


#endif
