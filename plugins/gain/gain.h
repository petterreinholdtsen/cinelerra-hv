#ifndef GAIN_H
#define GAIN_H

class Gain;
class GainEngine;

#include "gainwindow.h"
#include "pluginaclient.h"

class GainConfig
{
public:
	GainConfig();
	int operator==(GainConfig& that);
	GainConfig& operator=(GainConfig& that);

	double level;
};

class Gain : public PluginAClient
{
public:
	Gain(PluginServer *server);
	~Gain();

	void update_gui();
	void load_configuration();
	
// data for gain
	GainConfig config;
	
	DB db;

	char* plugin_title();
	VFrame* new_picon();
	int is_realtime();
	int process_realtime(long size, double *input_ptr, double *output_ptr);
	int start_realtime();
	int stop_realtime();
	int show_gui();
	int set_string();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	void raise_window();

// non realtime support
	int load_defaults();
	int save_defaults();
	Defaults *defaults;
	
	GainThread *thread;
};

class GainEngine : public Thread
{
public:
	GainEngine(Gain *plugin);
	~GainEngine();

	int process_overlay(double *in, double *out, double &out1, double &out2, double level, long lowpass, long samplerate, long size);
	int process_overlays(int output_buffer, long size);
	int wait_process_overlays();
	void run();

	Mutex input_lock, output_lock;
	int completed;
	int output_buffer;
	long size;
	Gain *plugin;
};

#endif
