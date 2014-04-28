#include <math.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "bcdisplayinfo.h"
#include "bcsignals.h"
#include "clip.h"
#include "bchash.h"
#include "filexml.h"
#include "histogramconfig.h"
#include "histogramwindow.h"
#include "keyframe.h"
#include "language.h"
#include "loadbalance.h"
#include "max_picon_png.h"
#include "mid_picon_png.h"
#include "min_picon_png.h"
#include "picon_png.h"
#include "../colors/plugincolors.h"
#include "vframe.h"



class HistogramMain;
class HistogramEngine;
class HistogramWindow;





REGISTER_PLUGIN(HistogramMain)















HistogramMain::HistogramMain(PluginServer *server)
 : PluginVClient(server)
{
	PLUGIN_CONSTRUCTOR_MACRO
	engine = 0;
	lookup[0] = lookup[1] = lookup[2] = lookup[3] = 0;
	accum[0] = accum[1] = accum[2] = accum[3] = accum[4] = 0;
}

HistogramMain::~HistogramMain()
{
	PLUGIN_DESTRUCTOR_MACRO
	if(lookup[0]) delete [] lookup[0];
	if(lookup[1]) delete [] lookup[1];
	if(lookup[2]) delete [] lookup[2];
	if(lookup[3]) delete [] lookup[3];
	if(accum[0]) delete [] accum[0];
	if(accum[1]) delete [] accum[1];
	if(accum[2]) delete [] accum[2];
	if(accum[3]) delete [] accum[3];
	if(accum[4]) delete [] accum[4];
	if(engine) delete engine;
}

char* HistogramMain::plugin_title() { return N_("Histogram"); }
int HistogramMain::is_realtime() { return 1; }


NEW_PICON_MACRO(HistogramMain)

SHOW_GUI_MACRO(HistogramMain, HistogramThread)

SET_STRING_MACRO(HistogramMain)

RAISE_WINDOW_MACRO(HistogramMain)

LOAD_CONFIGURATION_MACRO(HistogramMain, HistogramConfig)

void HistogramMain::render_gui(void *data)
{
	if(thread)
	{
		calculate_histogram((VFrame*)data);
		if(config.automatic)
		{
			calculate_automatic((VFrame*)data);
		}

		thread->window->lock_window("HistogramMain::render_gui");
		thread->window->update_canvas();
		if(config.automatic)
		{
			thread->window->update_input();
		}
		thread->window->unlock_window();
	}
}

void HistogramMain::update_gui()
{
	if(thread)
	{
		thread->window->lock_window("HistogramMain::update_gui");
		int reconfigure = load_configuration();
		if(reconfigure) 
		{
			thread->window->update(0);
			if(!config.automatic)
			{
				thread->window->update_input();
			}
		}
		thread->window->unlock_window();
	}
}


int HistogramMain::load_defaults()
{
	char directory[BCTEXTLEN], string[BCTEXTLEN];
// set the default directory
	sprintf(directory, "%shistogram.rc", BCASTDIR);

// load the defaults
	defaults = new BC_Hash(directory);
	defaults->load();

	for(int i = 0; i < 5; i++)
	{
		sprintf(string, "INPUT_MIN_%d", i);
		config.input_min[i] = defaults->get(string, config.input_min[i]);
		sprintf(string, "INPUT_MID_%d", i);
		config.input_mid[i] = defaults->get(string, config.input_mid[i]);
		sprintf(string, "INPUT_MAX_%d", i);
		config.input_max[i] = defaults->get(string, config.input_max[i]);
		sprintf(string, "OUTPUT_MIN_%d", i);
		config.output_min[i] = defaults->get(string, config.output_min[i]);
		sprintf(string, "OUTPUT_MAX_%d", i);
		config.output_max[i] = defaults->get(string, config.output_max[i]);
	}
	config.automatic = defaults->get("AUTOMATIC", config.automatic);
	config.mode = defaults->get("MODE", config.mode);
	config.threshold = defaults->get("THRESHOLD", config.threshold);
	config.boundaries();
	return 0;
}


int HistogramMain::save_defaults()
{
	char string[BCTEXTLEN];
	for(int i = 0; i < 5; i++)
	{
		sprintf(string, "INPUT_MIN_%d", i);
		defaults->update(string, config.input_min[i]);
		sprintf(string, "INPUT_MID_%d", i);
		defaults->update(string, config.input_mid[i]);
		sprintf(string, "INPUT_MAX_%d", i);
		defaults->update(string, config.input_max[i]);
		sprintf(string, "OUTPUT_MIN_%d", i);
		defaults->update(string, config.output_min[i]);
		sprintf(string, "OUTPUT_MAX_%d", i);
	   	defaults->update(string, config.output_max[i]);
	}
	defaults->update("AUTOMATIC", config.automatic);
	defaults->update("MODE", config.mode);
	defaults->update("THRESHOLD", config.threshold);
	defaults->save();
	return 0;
}



void HistogramMain::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause data to be stored directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);
	output.tag.set_title("HISTOGRAM");

	char string[BCTEXTLEN];
	for(int i = 0; i < 5; i++)
	{
		sprintf(string, "INPUT_MIN_%d", i);
		output.tag.set_property(string, config.input_min[i]);
		sprintf(string, "INPUT_MID_%d", i);
		output.tag.set_property(string, config.input_mid[i]);
		sprintf(string, "INPUT_MAX_%d", i);
		output.tag.set_property(string, config.input_max[i]);
		sprintf(string, "OUTPUT_MIN_%d", i);
		output.tag.set_property(string, config.output_min[i]);
		sprintf(string, "OUTPUT_MAX_%d", i);
	   	output.tag.set_property(string, config.output_max[i]);
//printf("HistogramMain::save_data %d %f %d\n", config.input_min[i], config.input_mid[i], config.input_max[i]);
	}
	output.tag.set_property("AUTOMATIC", config.automatic);
	output.tag.set_property("MODE", config.mode);
	output.tag.set_property("THRESHOLD", config.threshold);
	output.append_tag();
	output.terminate_string();
}

void HistogramMain::read_data(KeyFrame *keyframe)
{
	FileXML input;

	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;

	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("HISTOGRAM"))
			{
				char string[BCTEXTLEN];
				for(int i = 0; i < 5; i++)
				{
					sprintf(string, "INPUT_MIN_%d", i);
					config.input_min[i] = input.tag.get_property(string, config.input_min[i]);
					sprintf(string, "INPUT_MID_%d", i);
					config.input_mid[i] = input.tag.get_property(string, config.input_mid[i]);
					sprintf(string, "INPUT_MAX_%d", i);
					config.input_max[i] = input.tag.get_property(string, config.input_max[i]);
					sprintf(string, "OUTPUT_MIN_%d", i);
					config.output_min[i] = input.tag.get_property(string, config.output_min[i]);
					sprintf(string, "OUTPUT_MAX_%d", i);
					config.output_max[i] = input.tag.get_property(string, config.output_max[i]);
//printf("HistogramMain::read_data %d %f %d\n", config.input_min[i], config.input_mid[i], config.input_max[i]);
				}
				config.automatic = input.tag.get_property("AUTOMATIC", config.automatic);
				config.mode = input.tag.get_property("MODE", config.mode);
				config.threshold = input.tag.get_property("THRESHOLD", config.threshold);
			}
		}
	}
	config.boundaries();
}

float HistogramMain::calculate_transfer(float input, 
	int subscript,
	int do_value)
{
	float y1, y2, y3, y4;
	float input_min = config.input_min[subscript];
	float input_max = config.input_max[subscript];
	float input_mid = config.input_mid[subscript];
	float output_min = config.output_min[subscript];
	float output_max = config.output_max[subscript];
	float output;
	float output_left;
	float output_right;
	float output_linear;

// Expand input
// Below minimum
	if(input < input_min) return output_min;

// Above maximum
	if(input >= input_max) return output_max;

// Get slope of line from minimum to midpoint
	float slope1 = 0.5 / (input_mid - input_min);
// Slope of line form midpoint to maximum
	float slope2 = 0.5 / (input_max - input_mid);
// lesser of two slopes
	float min_slope = MIN(slope1, slope2);
// value of 45` diagonal with midpoint
	float output_perfect = 0.5 + min_slope * (input - input_mid);

	if(input < input_mid)
	{
// Fraction of left hand curve to use
		float mid_fraction = (input - input_min) / (input_mid - input_min);
// value of straight line connecting min to mid
		output_linear = mid_fraction * 0.5;
// Blend perfect diagonal with linear
		output = output_linear * 
			(1.0 - mid_fraction) + 
			output_perfect * 
			mid_fraction;
	}
	else
	{
// Fraction of perfect diagonal to use
		float mid_fraction = (input_max - input) / (input_max - input_mid);
// value of line connecting max to mid
		output_linear = 0.5 + (1.0 - mid_fraction) * 0.5;
// Blend perfect diagonal with linear
		output = output_linear * 
			(1.0 - mid_fraction) + 
			output_perfect * 
			mid_fraction;
	}


// Linear
// 	if(input < input_mid)
// 	{
// 		output = slope1 * (input - input_min);
// 	}
// 	else
// 	{
// 		output = 0.5 + slope2 * (input - input_mid);
// 	}



// Expand value
	if(do_value)
	{
		output = calculate_transfer(output, HISTOGRAM_VALUE, 0);
	}

// Compress output for value followed by channel
	output = output_min + 
		output * 
		(output_max - output_min);


// printf("HistogramMain::calculate_transfer 1 %.0f %.0f %.0f %.0f %.0f\n",
// output, 
// input,
// min,
// mid,
// max);
	return output;
}


void HistogramMain::calculate_histogram(VFrame *data)
{

	if(!engine) engine = new HistogramEngine(this,
		get_project_smp() + 1,
		get_project_smp() + 1);

	if(!accum[0])
	{
		for(int i = 0; i < 5; i++)
			accum[i] = new int64_t[HISTOGRAM_RANGE];
	}
	engine->process_packages(HistogramEngine::HISTOGRAM, data);

	for(int i = 0; i < engine->get_total_clients(); i++)
	{
		HistogramUnit *unit = (HistogramUnit*)engine->get_client(i);
		if(i == 0)
		{
			for(int j = 0; j < 5; j++)
				memcpy(accum[j], unit->accum[j], sizeof(int64_t) * HISTOGRAM_RANGE);
		}
		else
		{
			for(int j = 0; j < 5; j++)
			{
				int64_t *out = accum[j];
				int64_t *in = unit->accum[j];
				for(int k = 0; k < HISTOGRAM_RANGE; k++)
					out[k] += in[k];
			}
		}
	}

// Remove top and bottom from calculations.  Doesn't work in high
// precision colormodels.
	for(int i = 0; i < 5; i++)
	{
		accum[i][0] = 0;
		accum[i][HISTOGRAM_RANGE - 1] = 0;
	}
}


void HistogramMain::calculate_automatic(VFrame *data)
{
	calculate_histogram(data);

// Do each channel
	for(int i = 0; i < 3; i++)
	{
		int64_t *accum = this->accum[i];
		int max = 0;

		for(int j = 0; j < HISTOGRAM_RANGE; j++)
		{
			max = MAX(accum[j], max);
		}

		int threshold = (int)(config.threshold * max);


// Minimum input
		config.input_min[i] = 0;
		for(int j = (int)(HISTOGRAM_RANGE * (0 - FLOAT_MIN) / (FLOAT_MAX - FLOAT_MIN)); 
			j < (int)(HISTOGRAM_RANGE * (1 - FLOAT_MIN) / (FLOAT_MAX - FLOAT_MIN)); j++)
		{
			if(accum[j] > threshold)
			{
				config.input_min[i] = (float)j / 
					HISTOGRAM_RANGE * 
					FLOAT_RANGE +
					FLOAT_MIN;
				break;
			}
		}


// Maximums
		config.input_max[i] = 1.0;
		for(int j = (int)(HISTOGRAM_RANGE * (1 - FLOAT_MIN) / (FLOAT_MAX - FLOAT_MIN)) - 1; 
			j >= (int)(HISTOGRAM_RANGE * (0 - FLOAT_MIN) / (FLOAT_MAX - FLOAT_MIN)); 
			j--)
		{
			if(accum[j] > threshold)
			{
				config.input_max[i] = (float)j / 
					HISTOGRAM_RANGE * 
					FLOAT_RANGE +
					FLOAT_MIN;
				break;
			}
		}

		config.input_mid[i] = (config.input_min[i] + config.input_max[i]) / 2;
	}
}






int HistogramMain::process_realtime(VFrame *input_ptr, VFrame *output_ptr)
{
	int need_reconfigure = load_configuration();


	if(!engine) engine = new HistogramEngine(this,
		get_project_smp() + 1,
		get_project_smp() + 1);
	this->input = input_ptr;
	this->output = output_ptr;

	send_render_gui(input_ptr);

	if(input_ptr->get_rows()[0] != output_ptr->get_rows()[0])
	{
		output_ptr->copy_from(input_ptr);
	}

// Generate tables here.  The same table is used by many packages to render
// each horizontal stripe.  Need to cover the entire output range in  each
// table to avoid green borders
	if(need_reconfigure || !lookup[0] || config.automatic)
	{
		if(!lookup[0])
			for(int i = 0; i < 4; i++)
				lookup[i] = new int[HISTOGRAM_RANGE];

// Calculate new curves
		if(config.automatic)
		{
			calculate_automatic(input);
		}

		engine->process_packages(HistogramEngine::TABULATE, input);



// Convert 16 bit lookup table to 8 bits
// 		switch(input->get_color_model())
// 		{
// 			case BC_RGB888:
// 			case BC_RGBA8888:
// 				for(int i = 0; i < 0x100; i++)
// 				{
// 					int subscript = (i << 8) | i;
// 					lookup[0][i] = lookup[0][subscript];
// 					lookup[1][i] = lookup[1][subscript];
// 					lookup[2][i] = lookup[2][subscript];
// 					lookup[3][i] = lookup[3][subscript];
// 				}
// 				break;
// 		}
	}




// Apply histogram
	engine->process_packages(HistogramEngine::APPLY, input);


	return 0;
}






HistogramPackage::HistogramPackage()
 : LoadPackage()
{
}




HistogramUnit::HistogramUnit(HistogramEngine *server, 
	HistogramMain *plugin)
 : LoadClient(server)
{
	this->plugin = plugin;
	this->server = server;
	for(int i = 0; i < 5; i++)
		accum[i] = new int64_t[HISTOGRAM_RANGE];
}

HistogramUnit::~HistogramUnit()
{
	for(int i = 0; i < 5; i++)
		delete [] accum[i];
}

void HistogramUnit::process_package(LoadPackage *package)
{
	HistogramPackage *pkg = (HistogramPackage*)package;

	if(server->operation == HistogramEngine::HISTOGRAM)
	{

#define HISTOGRAM_HEAD(type) \
{ \
	for(int i = pkg->start; i < pkg->end; i++) \
	{ \
		type *row = (type*)data->get_rows()[i]; \
		for(int j = 0; j < w; j++) \
		{

#define HISTOGRAM_TAIL(components) \
/*			v = (r * 76 + g * 150 + b * 29) >> 8; */ \
			v = MAX(r, g); \
			v = MAX(v, b); \
			r += -HISTOGRAM_MIN * 0xffff / 100; \
			g += -HISTOGRAM_MIN * 0xffff / 100; \
			b += -HISTOGRAM_MIN * 0xffff / 100; \
			v += -HISTOGRAM_MIN * 0xffff / 100; \
			CLAMP(r, 0, HISTOGRAM_RANGE); \
			CLAMP(g, 0, HISTOGRAM_RANGE); \
			CLAMP(b, 0, HISTOGRAM_RANGE); \
			CLAMP(v, 0, HISTOGRAM_RANGE); \
			accum_r[r]++; \
			accum_g[g]++; \
			accum_b[b]++; \
/*			if(components == 4) accum_a[row[3]]++; */ \
			accum_v[v]++; \
			row += components; \
		} \
	} \
}




		VFrame *data = server->data;
		int w = data->get_w();
		int h = data->get_h();
		int64_t *accum_r = accum[HISTOGRAM_RED];
		int64_t *accum_g = accum[HISTOGRAM_GREEN];
		int64_t *accum_b = accum[HISTOGRAM_BLUE];
		int64_t *accum_a = accum[HISTOGRAM_ALPHA];
		int64_t *accum_v = accum[HISTOGRAM_VALUE];
		int r, g, b, a, y, u, v;

		switch(data->get_color_model())
		{
			case BC_RGB888:
				HISTOGRAM_HEAD(unsigned char)
				r = (row[0] << 8) | row[0];
				g = (row[1] << 8) | row[1];
				b = (row[2] << 8) | row[2];
				HISTOGRAM_TAIL(3)
				break;
			case BC_RGB_FLOAT:
				HISTOGRAM_HEAD(float)
				r = (int)(row[0] * 0xffff);
				g = (int)(row[1] * 0xffff);
				b = (int)(row[2] * 0xffff);
				HISTOGRAM_TAIL(3)
				break;
			case BC_YUV888:
				HISTOGRAM_HEAD(unsigned char)
				y = (row[0] << 8) | row[0];
				u = (row[1] << 8) | row[1];
				v = (row[2] << 8) | row[2];
				plugin->yuv.yuv_to_rgb_16(r, g, b, y, u, v);
				HISTOGRAM_TAIL(3)
				break;
			case BC_RGBA8888:
				HISTOGRAM_HEAD(unsigned char)
				r = (row[0] << 8) | row[0];
				g = (row[1] << 8) | row[1];
				b = (row[2] << 8) | row[2];
				HISTOGRAM_TAIL(4)
				break;
			case BC_RGBA_FLOAT:
				HISTOGRAM_HEAD(float)
				r = (int)(row[0] * 0xffff);
				g = (int)(row[1] * 0xffff);
				b = (int)(row[2] * 0xffff);
				HISTOGRAM_TAIL(4)
				break;
			case BC_YUVA8888:
				HISTOGRAM_HEAD(unsigned char)
				y = (row[0] << 8) | row[0];
				u = (row[1] << 8) | row[1];
				v = (row[2] << 8) | row[2];
				plugin->yuv.yuv_to_rgb_16(r, g, b, y, u, v);
				HISTOGRAM_TAIL(4)
				break;
			case BC_RGB161616:
				HISTOGRAM_HEAD(uint16_t)
				r = row[0];
				g = row[1];
				b = row[2];
				HISTOGRAM_TAIL(3)
				break;
			case BC_YUV161616:
				HISTOGRAM_HEAD(uint16_t)
				y = row[0];
				u = row[1];
				v = row[2];
				plugin->yuv.yuv_to_rgb_16(r, g, b, y, u, v);
				HISTOGRAM_TAIL(3)
				break;
			case BC_RGBA16161616:
				HISTOGRAM_HEAD(uint16_t)
				r = row[0];
				g = row[1];
				b = row[2];
				HISTOGRAM_TAIL(3)
				break;
			case BC_YUVA16161616:
				HISTOGRAM_HEAD(uint16_t)
				y = row[0];
				u = row[1];
				v = row[2];
				plugin->yuv.yuv_to_rgb_16(r, g, b, y, u, v);
				HISTOGRAM_TAIL(4)
				break;
		}
	}
	else
	if(server->operation == HistogramEngine::APPLY)
	{



#define PROCESS(type, components) \
{ \
	for(int i = pkg->start; i < pkg->end; i++) \
	{ \
		type *row = (type*)input->get_rows()[i]; \
		for(int j = 0; j < w; j++) \
		{ \
			row[0] = lookup_r[row[0]]; \
			row[1] = lookup_g[row[1]]; \
			row[2] = lookup_b[row[2]]; \
			row += components; \
		} \
	} \
}

#define PROCESS_YUV(type, components, max) \
{ \
	for(int i = pkg->start; i < pkg->end; i++) \
	{ \
		type *row = (type*)input->get_rows()[i]; \
		for(int j = 0; j < w; j++) \
		{ \
/* Convert to 16 bit RGB */ \
			if(max == 0xff) \
			{ \
				y = (row[0] << 8) | row[0]; \
				u = (row[1] << 8) | row[1]; \
				v = (row[2] << 8) | row[2]; \
			} \
			else \
			{ \
				y = row[0]; \
				u = row[1]; \
				v = row[2]; \
			} \
 \
			plugin->yuv.yuv_to_rgb_16(r, g, b, y, u, v); \
 \
/* Look up in RGB domain */ \
			r = lookup_r[r]; \
			g = lookup_g[g]; \
			b = lookup_b[b]; \
 \
/* Convert to 16 bit YUV */ \
			plugin->yuv.rgb_to_yuv_16(r, g, b, y, u, v); \
 \
			if(max == 0xff) \
			{ \
				row[0] = y >> 8; \
				row[1] = u >> 8; \
				row[2] = v >> 8; \
			} \
			else \
			{ \
				row[0] = y; \
				row[1] = u; \
				row[2] = v; \
			} \
			row += components; \
		} \
	} \
}

#define PROCESS_FLOAT(components) \
{ \
	for(int i = pkg->start; i < pkg->end; i++) \
	{ \
		float *row = (float*)input->get_rows()[i]; \
		for(int j = 0; j < w; j++) \
		{ \
			float r = row[0]; \
			float g = row[1]; \
			float b = row[2]; \
 \
			r = plugin->calculate_transfer(r, HISTOGRAM_RED, 1); \
			g = plugin->calculate_transfer(g, HISTOGRAM_GREEN, 1); \
			b = plugin->calculate_transfer(b, HISTOGRAM_BLUE, 1); \
 \
 			row[0] = r; \
			row[1] = g; \
			row[2] = b; \
 \
			row += components; \
		} \
	} \
}


		VFrame *input = plugin->input;
		VFrame *output = plugin->output;
		int w = input->get_w();
		int h = input->get_h();
		int *lookup_r = plugin->lookup[0];
		int *lookup_g = plugin->lookup[1];
		int *lookup_b = plugin->lookup[2];
		int *lookup_a = plugin->lookup[3];
		int r, g, b, y, u, v, a;
		switch(input->get_color_model())
		{
			case BC_RGB888:
				PROCESS(unsigned char, 3)
				break;
			case BC_RGB_FLOAT:
				PROCESS_FLOAT(3);
				break;
			case BC_RGBA8888:
				PROCESS(unsigned char, 4)
				break;
			case BC_RGBA_FLOAT:
				PROCESS_FLOAT(4);
				break;
			case BC_RGB161616:
				PROCESS(uint16_t, 3)
				break;
			case BC_RGBA16161616:
				PROCESS(uint16_t, 4)
				break;
			case BC_YUV888:
				PROCESS_YUV(unsigned char, 3, 0xff)
				break;
			case BC_YUVA8888:
				PROCESS_YUV(unsigned char, 4, 0xff)
				break;
			case BC_YUV161616:
				PROCESS_YUV(uint16_t, 3, 0xffff)
				break;
			case BC_YUVA16161616:
				PROCESS_YUV(uint16_t, 4, 0xffff)
				break;
		}
	}
	else
	if(server->operation == HistogramEngine::TABULATE)
	{
		int colormodel = plugin->input->get_color_model();
// Float uses direct calculation
		if(colormodel != BC_RGB_FLOAT &&
			colormodel != BC_RGBA_FLOAT)
		{
			for(int i = pkg->start; i < pkg->end; i++)
			{
// Fix input for legal integer range
				float input = (float)i / server->total_size;

// Expand input
				float r = plugin->calculate_transfer(input, HISTOGRAM_RED, 1);
				float g = plugin->calculate_transfer(input, HISTOGRAM_GREEN, 1);
				float b = plugin->calculate_transfer(input, HISTOGRAM_BLUE, 1);
// Convert to desired colormodel
				switch(colormodel)
				{
					case BC_RGB888:
					case BC_RGBA8888:
						plugin->lookup[0][i] = (int)(r * 0xff);
						plugin->lookup[1][i] = (int)(g * 0xff);
						plugin->lookup[2][i] = (int)(b * 0xff);
						CLAMP(plugin->lookup[0][i], 0, 0xff);
						CLAMP(plugin->lookup[1][i], 0, 0xff);
						CLAMP(plugin->lookup[2][i], 0, 0xff);
						break;
					default:
// Can't look up yuv.
						plugin->lookup[0][i] = (int)(r * 0xffff);
						plugin->lookup[1][i] = (int)(g * 0xffff);
						plugin->lookup[2][i] = (int)(b * 0xffff);
						CLAMP(plugin->lookup[0][i], 0, 0xffff);
						CLAMP(plugin->lookup[1][i], 0, 0xffff);
						CLAMP(plugin->lookup[2][i], 0, 0xffff);
						break;
				}
			}
		}
	}
}






HistogramEngine::HistogramEngine(HistogramMain *plugin, 
	int total_clients, 
	int total_packages)
 : LoadServer(total_clients, total_packages)
{
	this->plugin = plugin;
}

void HistogramEngine::init_packages()
{
	switch(operation)
	{
		case HISTOGRAM:
			total_size = data->get_h();
			break;
		case TABULATE:
		{
			int colormodel = plugin->input->get_color_model();
// Tabulation only works for integer so we only do integer ranges
			if(colormodel == BC_RGB888 |
				colormodel == BC_RGBA8888)
				total_size = 0x100;
			else
				total_size = 0x10000;
			break;
		}
		case APPLY:
			total_size = data->get_h();
			break;
	}


	int package_size = (int)((float)total_size / 
			get_total_packages() + 1);
	int start = 0;

	for(int i = 0; i < get_total_packages(); i++)
	{
		HistogramPackage *package = (HistogramPackage*)get_package(i);
		package->start = start;
		package->end = start + package_size;
		if(package->end > total_size)
			package->end = total_size;
		start = package->end;
	}

// Initialize clients here in case some don't get run.
	for(int i = 0; i < get_total_clients(); i++)
	{
		HistogramUnit *unit = (HistogramUnit*)get_client(i);
		for(int i = 0; i < 5; i++)
			bzero(unit->accum[i], sizeof(int64_t) * HISTOGRAM_RANGE);
	}

}

LoadClient* HistogramEngine::new_client()
{
	return new HistogramUnit(this, plugin);
}

LoadPackage* HistogramEngine::new_package()
{
	return new HistogramPackage;
}

void HistogramEngine::process_packages(int operation, VFrame *data)
{
	this->data = data;
	this->operation = operation;
	LoadServer::process_packages();
}



