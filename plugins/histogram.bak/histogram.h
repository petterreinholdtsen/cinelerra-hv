#ifndef HISTOGRAM_H
#define HISTOGRAM_H


#include "histogram.inc"
#include "histogramconfig.inc"
#include "histogramwindow.inc"
#include "pluginvclient.h"


class HistogramMain : public PluginVClient
{
public:
	HistogramMain(PluginServer *server);
	~HistogramMain();

	int process_realtime(VFrame *input_ptr, VFrame *output_ptr);
	int is_realtime();
	int load_defaults();
	int save_defaults();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	void update_gui();
	void render_gui(void *data);
	PLUGIN_CLASS_MEMBERS(HistogramConfig, HistogramThread)

// Convert input to input curve
	float calculate_transfer(float input, int mode, int do_value);
// Calculate automatic settings
	void calculate_automatic(VFrame *data);
// Calculate histogram
	void calculate_histogram(VFrame *data);



	YUV yuv;
	VFrame *input, *output;
	HistogramEngine *engine;
	int *lookup[4];
	int64_t *accum[5];
};

class HistogramPackage : public LoadPackage
{
public:
	HistogramPackage();
	int start, end;
};

class HistogramUnit : public LoadClient
{
public:
	HistogramUnit(HistogramEngine *server, HistogramMain *plugin);
	~HistogramUnit();
	void process_package(LoadPackage *package);
	HistogramEngine *server;
	HistogramMain *plugin;
	int64_t *accum[5];
};

class HistogramEngine : public LoadServer
{
public:
	HistogramEngine(HistogramMain *plugin, 
		int total_clients, 
		int total_packages);
	void process_packages(int operation, VFrame *data);
	void init_packages();
	LoadClient* new_client();
	LoadPackage* new_package();
	HistogramMain *plugin;
	int total_size;


	int operation;
	enum
	{
		HISTOGRAM,
		TABULATE,
		APPLY
	};
	VFrame *data;
};











#endif
