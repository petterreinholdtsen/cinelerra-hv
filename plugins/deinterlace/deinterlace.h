#ifndef DEINTERLACE_H
#define DEINTERLACE_H

// the simplest plugin possible

class DeInterlaceMain;

#include "defaults.inc"
#include "deinterwindow.h"
#include "pluginvclient.h"
#include "vframe.inc"





enum
{
	DEINTERLACE_EVEN,
	DEINTERLACE_ODD,
	DEINTERLACE_AVG,
	DEINTERLACE_SWAP
};

class DeInterlaceConfig
{
public:
	DeInterlaceConfig();
	
	int mode;
};

class DeInterlaceMain : public PluginVClient
{
public:
	DeInterlaceMain(PluginServer *server);
	~DeInterlaceMain();

// required for all realtime plugins
	int process_realtime(VFrame *input, VFrame *output);
	int is_realtime();
	char* plugin_title();
	VFrame* new_picon();
	int show_gui();
	int hide_gui();
	void update_gui();
	void load_configuration();
	int set_string();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	void raise_window();
	int load_defaults();
	int save_defaults();

	void deinterlace_even(VFrame *input, VFrame *output, int dominance);
	void deinterlace_avg(VFrame *input, VFrame *output);
	void deinterlace_swap(VFrame *input, VFrame *output);

// a thread for the GUI
	DeInterlaceThread *thread;
	DeInterlaceConfig config;
	Defaults *defaults;
};


#endif
