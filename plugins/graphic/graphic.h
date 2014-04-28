
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "guicast.h"
#include "../parametric/fourier.h"
#include "pluginaclient.h"


#define WINDOW_SIZE 16384
#define MAXMAGNITUDE 15
#define MAXFREQ 20000
#define MIN_DB -15
#define MAX_DB 15


class GraphicGUI;
class GraphicEQ;



class GraphicPoint
{
public:
	GraphicPoint();
// Frequency in Hz
	int freq;
// Amplitude in DB
	double value;
};



class GraphicConfig
{
public:
	GraphicConfig();
	~GraphicConfig();
	
	int equivalent(GraphicConfig &that);
	void copy_from(GraphicConfig &that);
	void interpolate(GraphicConfig &prev, 
		GraphicConfig &next, 
		int64_t prev_frame, 
		int64_t next_frame, 
		int64_t current_frame);
	void insert_point(GraphicPoint *point);
	void delete_point(int number);

	ArrayList<GraphicPoint*> points;
};



class GraphicCanvas : public BC_SubWindow
{
public:
	GraphicCanvas(GraphicEQ *plugin, GraphicGUI *gui, int x, int y, int w, int h);
	int button_press_event();
	int cursor_motion_event();
	int button_release_event();
	void process(int buttonpress, int motion, int draw);
	int freq_to_y(int freq);
	GraphicEQ *plugin;
	GraphicGUI *gui;
	int state;
	enum
	{
		NONE,
		DRAG_POINT
	};
	int x_diff, y_diff;
};

class FreqTextBox : public BC_TextBox
{
public:
	FreqTextBox(GraphicEQ *plugin,
		GraphicGUI *gui,
		int x,
		int y,
		int w);
	int handle_event();
	void update(int freq);
	GraphicEQ *plugin;
	GraphicGUI *gui;
};

class ValueTextBox : public BC_TextBox
{
public:
	ValueTextBox(GraphicEQ *plugin,
		GraphicGUI *gui,
		int x,
		int y,
		int w);
	int handle_event();
	void update(float value);
	GraphicEQ *plugin;
	GraphicGUI *gui;
};

class GraphicReset : public BC_GenericButton
{
public:
	GraphicReset(GraphicEQ *plugin,
		GraphicGUI *gui,
		int x,
		int y);
	int handle_event();
	GraphicEQ *plugin;
	GraphicGUI *gui;
};

class GraphicGUI : public PluginClientWindow
{
public:
	GraphicGUI(GraphicEQ *plugin);
	~GraphicGUI();

	void create_objects();
	int keypress_event();
	void update_canvas();
	int resize_event(int w, int h);
	void draw_ticks();
	void update_textboxes();

	FreqTextBox *freq_text;
	ValueTextBox *value_text;
	GraphicEQ *plugin;
	GraphicCanvas *canvas;
	GraphicReset *reset;
};





class GraphicFFT : public CrossfadeFFT
{
public:
	GraphicFFT(GraphicEQ *plugin);
	~GraphicFFT();
	
	int signal_process();
	int read_samples(int64_t output_sample, 
		int samples, 
		double *buffer);

	GraphicEQ *plugin;
};



class GraphicEQ : public PluginAClient
{
public:
	GraphicEQ(PluginServer *server);
	~GraphicEQ();

	int is_realtime();
	int load_defaults();
	int save_defaults();
	void read_data(KeyFrame *keyframe);
	void save_data(KeyFrame *keyframe);
	int process_buffer(int64_t size, 
		double *buffer, 
		int64_t start_position,
		int sample_rate);
	void update_gui();
	double freq_to_magnitude(double frequency);
	void calculate_envelope();
	int active_point_exists();
	void reconfigure();


	PLUGIN_CLASS_MEMBERS(GraphicConfig)



	double envelope[WINDOW_SIZE / 2];
	int active_point;
	GraphicFFT *fft;
	int need_reconfigure;
};



#endif // GRAPHIC_H
