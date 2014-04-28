#ifndef TIMESTRETCH_H
#define TIMESTRETCH_H

#include "defaults.inc"
#include "../parametric/fourier.h"
#include "guicast.h"
#include "mainprogress.inc"
#include "pluginaclient.h"
#include "resample.inc"
#include "vframe.inc"




class TimeStretch;
class TimeStretchWindow;




class TimeStretchFraction : public BC_TextBox
{
public:
	TimeStretchFraction(TimeStretch *plugin, 
		TimeStretchWindow *gui, 
		int x, 
		int y);
	int handle_event();
	TimeStretch *plugin;
	TimeStretchWindow *gui;
};

class TimeStretchWindow : public BC_Window
{
public:
	TimeStretchWindow(TimeStretch *plugin, int x, int y);
	~TimeStretchWindow();

	void create_objects();
	void update_memory_required();

	TimeStretch *plugin;
	BC_Title *memory;
};

class TimeStretch : public PluginAClient
{
public:
	TimeStretch(PluginServer *server);
	~TimeStretch();
	
	
	char* plugin_title();
	int is_multichannel();
	int get_parameters();
	VFrame* new_picon();
	int start_loop();
	int process_loop(double **buffer, int64_t &write_length);
	int stop_loop();
	
	int load_defaults();
	int save_defaults();
	
	int64_t get_memory_required();
	int64_t calculate_window(int64_t size);
	int allocate_buffer_vector(double ***buffer, int size);
	void delete_buffer_vector(double ***buffer);

	
	FFT *fft;
	Resample *resample;
	double **real_in;
	double **imag_in;
	double **real_out;
	double **imag_out;
	int input_size;
	int output_size;
	int window_size;

	Defaults *defaults;
	MainProgressBar *progress;
	double scale;
	int need_process;
	int64_t current_progress;
	int64_t write_position;
	int64_t *resampled_size;
};


#endif
