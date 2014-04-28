
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

#include "bcdisplayinfo.h"
#include "bcsignals.h"
#include "clip.h"
#include "cursors.h"
#include "bchash.h"
#include "filexml.h"
#include "graphic.h"
#include "keys.h"
#include "language.h"
#include "picon_png.h"
#include "units.h"
#include "vframe.h"

#include <math.h>
#include <string.h>


// Canvas parameters
#define MAJOR_DIVISIONS 7
#define MINOR_DIVISIONS 5
#define LINE_W4 12
#define LINE_W3 10
#define LINE_W2 5
#define LINE_W1 2








REGISTER_PLUGIN(GraphicEQ)







GraphicPoint::GraphicPoint()
{
	freq = 0;
	value = 0.0;
}








GraphicConfig::GraphicConfig()
{
}

GraphicConfig::~GraphicConfig()
{
	points.remove_all_objects();
}


int GraphicConfig::equivalent(GraphicConfig &that)
{
	if(that.points.total != points.total) return 0;

	for(int i = 0; i < points.total; i++)
	{
		if(that.points.values[i]->freq != points.values[i]->freq ||
			!EQUIV(that.points.values[i]->value, points.values[i]->value))
			return 0;
	}


	return 1;
}

void GraphicConfig::copy_from(GraphicConfig &that)
{
	points.remove_all_objects();
	for(int i = 0; i < that.points.total; i++)
	{
		GraphicPoint *point;
		points.append(point = new GraphicPoint);
		point->freq = that.points.values[i]->freq;
		point->value = that.points.values[i]->value;
	}
}

void GraphicConfig::interpolate(GraphicConfig &prev, 
	GraphicConfig &next, 
	int64_t prev_frame, 
	int64_t next_frame, 
	int64_t current_frame)
{
	double next_scale = (double)(current_frame - prev_frame) / (next_frame - prev_frame);
	double prev_scale = (double)(next_frame - current_frame) / (next_frame - prev_frame);

// Get current set of points from previous configuration
	copy_from(prev);
	

// Interpolate between current set of points and next set
	for(int i = 0; i < MIN(next.points.total, points.total); i++)
	{
		points.values[i]->freq = (int)(prev.points.values[i]->freq *
			prev_scale +
			next.points.values[i]->freq *
			next_scale);
		points.values[i]->value = prev.points.values[i]->value *
			prev_scale +
			next.points.values[i]->value *
			next_scale;
	}
}


void GraphicConfig::delete_point(int number)
{
	points.remove_object_number(number);
}

void GraphicConfig::insert_point(GraphicPoint *point)
{
	int done = 0;

	points.append(point);
	while(!done)
	{
		done = 1;
		for(int i = 0; i < points.total - 1; i++)
		{
			if(points.values[i]->freq > points.values[i + 1]->freq)
			{
				GraphicPoint *point = points.values[i];
				points.values[i] = points.values[i + 1];
				points.values[i + 1] = point;
				done = 0;
			}
		}
	}
}







GraphicCanvas::GraphicCanvas(GraphicEQ *plugin, 
	GraphicGUI *gui,
	int x, 
	int y, 
	int w, 
	int h)
 : BC_SubWindow(x,
 	y,
	w,
	h,
	WHITE)
{
	this->plugin = plugin;
	this->gui = gui;
	state = GraphicCanvas::NONE;
}

int GraphicCanvas::button_press_event()
{
	process(1, 0, 0);
	if(state == GraphicCanvas::DRAG_POINT)
		return 1;
	else 
		return 0;
}

int GraphicCanvas::cursor_motion_event()
{
	process(0, 1, 0);

	if(state == GraphicCanvas::DRAG_POINT)
		return 1;
	else
		return 0;
}

int GraphicCanvas::button_release_event()
{
	if(state == GraphicCanvas::DRAG_POINT && plugin->active_point >= 0)
	{
// Delete point if out of order
		int point_number = plugin->active_point;
		GraphicPoint *active_point = plugin->config.points.values[point_number];


		for(int i = 0; i < plugin->config.points.total; i++)
		{
			GraphicPoint *point = plugin->config.points.values[i];
			if((point->freq <= active_point->freq && i > point_number) ||
				(point->freq >= active_point->freq && i < point_number))
			{
				plugin->config.delete_point(point_number);
				plugin->active_point = -1;
				process(0, 0, 1);
				break;
			}
		}

		plugin->send_configure_change();
	}

	state = GraphicCanvas::NONE;
	return 0;
}

#define BOX_SIZE 10

int GraphicCanvas::freq_to_y(int freq)
{
	int center_y = get_h() / 2;
	double magnitude = plugin->freq_to_magnitude(freq);
	double magnitude_db = DB::todb(magnitude);
	if(magnitude_db < -MAXMAGNITUDE) magnitude_db = -MAXMAGNITUDE;
	int y = (int)(center_y - magnitude_db * center_y / MAXMAGNITUDE);
//printf("GraphicCanvas::freq_to_y magnitude=%f magnitude_db=%f y=%d\n", 
//magnitude, magnitude_db, y);
	return y;
}

void GraphicCanvas::process(int buttonpress, int motion, int draw)
{


	int got_button = 0;
	int center_y = get_h() / 2;
	int out_of_order = 0;

	plugin->calculate_envelope();


// Determine if active point is out of order
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		for(int i = 0; i < plugin->config.points.total; i++)
		{
			if(i == plugin->active_point)
			{
				if(i < plugin->config.points.total - 1 &&
					active_point->freq >= plugin->config.points.values[i + 1]->freq ||
					i > 0 &&
					active_point->freq <= plugin->config.points.values[i - 1]->freq)
				{
					out_of_order = 1;
				}
				break;
			}
		}
	}


	if(motion)
	{
		if(state == GraphicCanvas::DRAG_POINT)
		{
			int point_x = get_cursor_x() + x_diff;
			int point_y = get_cursor_y() + y_diff;
			
			int frequency = Freq::tofreq(point_x * TOTALFREQS / get_w());
			double magnitude_db = (double)(center_y - point_y) * MAXMAGNITUDE / center_y;
			int minfreq = Freq::tofreq(0);
			int maxfreq = Freq::tofreq(TOTALFREQS - 1);

			CLAMP(frequency, minfreq, maxfreq);
			CLAMP(magnitude_db, -MAXMAGNITUDE, MAXMAGNITUDE);
			if(plugin->active_point >= 0)
			{
				GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
				active_point->freq = frequency;
				active_point->value = magnitude_db;
			}

// Redraw with new value
			process(0, 0, 1);
			gui->update_textboxes();
			return;
		}
	}

// Magnitude bars
	if(draw)
	{
		clear_box(0, 0, get_w(), get_h());
		set_color(GREEN);
		for(int i = 1; i < MAJOR_DIVISIONS; i++)
		{
			int y = i * get_h() / (MAJOR_DIVISIONS - 1);
			draw_line(0, y, get_w(), y);
		}
	}

	int y1 = 0;
	if(draw) set_color(BLACK);

// Control points, cursor change and control point selection
	int new_cursor = ARROW_CURSOR;
	for(int i = 0; i < plugin->config.points.total; i++)
	{
		GraphicPoint *point = plugin->config.points.values[i];
		int x = Freq::fromfreq(point->freq) * get_w() / TOTALFREQS;
		int y = freq_to_y(point->freq);

		if(draw)
		{
			y1 = y;
// Draw point under cursor if out of order
			if(i == plugin->active_point && out_of_order) 
				y1 = get_cursor_y() + y_diff;

			if(i == plugin->active_point)
				draw_box(x - BOX_SIZE / 2, y1 - BOX_SIZE / 2, BOX_SIZE, BOX_SIZE);
			else
				draw_rectangle(x - BOX_SIZE / 2, y1 - BOX_SIZE / 2, BOX_SIZE, BOX_SIZE);
		}

		if(motion && 
			state == GraphicCanvas::NONE &&
			is_event_win() && 
			cursor_inside())
		{
			if(get_cursor_x() >= x - BOX_SIZE / 2 &&
				get_cursor_y() >= y - BOX_SIZE / 2 &&
				get_cursor_x() < x + BOX_SIZE / 2 &&
				get_cursor_y() < y + BOX_SIZE / 2)
			{
				new_cursor = UPRIGHT_ARROW_CURSOR;
			}
		}
		
		if(buttonpress &&
			state == GraphicCanvas::NONE &&
			is_event_win() && 
			cursor_inside() &&
			!got_button)
		{
			if(get_cursor_x() >= x - BOX_SIZE / 2 &&
				get_cursor_y() >= y - BOX_SIZE / 2 &&
				get_cursor_x() < x + BOX_SIZE / 2 &&
				get_cursor_y() < y + BOX_SIZE / 2)
			{
				plugin->active_point = i;
				state = GraphicCanvas::DRAG_POINT;
				x_diff = x - get_cursor_x();
				y_diff = y - get_cursor_y();
				got_button = 1;
				process(0, 0, 1);
				gui->update_textboxes();
			}
		}
	}

	if(motion && new_cursor != get_cursor())
	{
		set_cursor(new_cursor);
	}

// Envelope line
	y1 = 0;
	for(int i = 0; i < get_w(); i++)
	{
		int y = freq_to_y(Freq::tofreq(i * TOTALFREQS / get_w()));

		if(draw)
		{
			if(i > 0) draw_line(i - 1, y1, i, y);
		}


		y1 = y;
	}

	if(buttonpress && !got_button)
	{
		if(is_event_win() && cursor_inside())
		{
			GraphicPoint *new_point = new GraphicPoint;
			new_point->freq = Freq::tofreq(get_cursor_x() * 
				TOTALFREQS / 
				get_w());
			new_point->value = (double)(center_y - get_cursor_y()) * 
				MAXMAGNITUDE / 
				center_y;
			state = GraphicCanvas::DRAG_POINT;
			plugin->config.insert_point(new_point);
			plugin->active_point = plugin->config.points.number_of(new_point);
			x_diff = 0;
			y_diff = 0;

// Redraw with new point
			process(0, 0, 1);
			gui->update_textboxes();
		}
	}

	if(draw) 
	{
		flash();
	}
}









FreqTextBox::FreqTextBox(GraphicEQ *plugin,
	GraphicGUI *gui,
	int x,
	int y,
	int w)
 : BC_TextBox(x, y, w, 1, "")
{
	this->plugin = plugin;
	this->gui = gui;
}

int FreqTextBox::handle_event()
{
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		if(atoi(get_text()) != active_point->freq)
		{
			active_point->freq = atoi(get_text());
			gui->update_canvas();
			plugin->send_configure_change();
			return 1;
		}
	}

	return 0;
}

void FreqTextBox::update(int freq)
{
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		if(atoi(get_text()) != active_point->freq)
		{
			char string[BCTEXTLEN];
			sprintf(string, "%d", active_point->freq);
			BC_TextBox::update(string);
		}
	}
}




ValueTextBox::ValueTextBox(GraphicEQ *plugin,
	GraphicGUI *gui,
	int x,
	int y,
	int w)
 : BC_TextBox(x, y, w, 1, "")
{
	this->plugin = plugin;
	this->gui = gui;
}

int ValueTextBox::handle_event()
{
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		if(!EQUIV(atof(get_text()), active_point->value))
		{
			active_point->value = atof(get_text());
			gui->update_canvas();
			plugin->send_configure_change();
			return 1;
		}
	}

	return 0;
}

void ValueTextBox::update(float value)
{
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		if(!EQUIV(atof(get_text()), active_point->value))
		{
			char string[BCTEXTLEN];
			sprintf(string, "%.04f", active_point->value);
			BC_TextBox::update(string);
		}
	}
}

GraphicReset::GraphicReset(GraphicEQ *plugin,
	GraphicGUI *gui,
	int x,
	int y)
 : BC_GenericButton(x, y, _("Reset"))
{
	this->plugin = plugin;
	this->gui = gui;
}

int GraphicReset::handle_event()
{
	plugin->config.points.remove_all_objects();
	plugin->active_point = -1;
	gui->update_canvas();
	gui->update_textboxes();
	plugin->send_configure_change();
	return 1;
}










GraphicGUI::GraphicGUI(GraphicEQ *plugin)
 : PluginClientWindow(plugin, 
	480, 
	400, 
	480, 
	400,
	0)
{
	this->plugin = plugin;
}

GraphicGUI::~GraphicGUI()
{
}


void GraphicGUI::create_objects()
{
	int x = get_text_width(SMALLFONT, "-00") + LINE_W4 + 5;
	int y = 10;
	add_subwindow(canvas = new GraphicCanvas(plugin,
		this,
		x, 
		y, 
		get_w() - x - 10, 
		get_h() - BC_Pot::calculate_h() - 60));
	y += canvas->get_h() + 40;
	BC_Title *title;
	add_subwindow(title = new BC_Title(x, y, "Frequency:"));
	x += title->get_w() + 5;
	add_subwindow(freq_text = new FreqTextBox(plugin, this, x, y, 50));
	x += freq_text->get_w() + 5;

	add_subwindow(title = new BC_Title(x, y, "Level:"));
	x += title->get_w() + 5;
	add_subwindow(value_text = new ValueTextBox(plugin, this, x, y, 50));

	x += value_text->get_w() + 5;
	add_subwindow(reset = new GraphicReset(plugin, this, x, y));

	draw_ticks();
	update_canvas();
	show_window();
}






int GraphicGUI::keypress_event()
{
	if(get_keypress() == BACKSPACE ||
		get_keypress() == DELETE)
	{
		if(plugin->active_point >= 0)
		{
			int point_number = -1;
			for(int i = 0; i < plugin->config.points.total; i++)
			{
				if(i == plugin->active_point)
				{
					point_number = i;
					break;
				}
			}
			
			if(point_number >= 0)
			{
				plugin->config.delete_point(point_number);
				canvas->process(0, 0, 1);
				plugin->send_configure_change();
				return 1;
			}
		}
	}
	return 0;
}


void GraphicGUI::draw_ticks()
{
	int x = canvas->get_x() - 5 - get_text_width(SMALLFONT, "-00");
	int y = canvas->get_y() - 1;
	int x1 = canvas->get_x() - LINE_W3;
	int x2 = canvas->get_x() - LINE_W2;
	int x3 = canvas->get_x() - LINE_W1;
	char string[BCTEXTLEN];

// Amplitude
	set_font(SMALLFONT);
	int major_division = canvas->get_h() / (MAJOR_DIVISIONS - 1);
	for(int i = 0; i < MAJOR_DIVISIONS; i++)
	{
		int current_db = (MAJOR_DIVISIONS - 1 - i) * (MAX_DB - MIN_DB) / (MAJOR_DIVISIONS - 1) + MIN_DB;
		if(current_db == MIN_DB)
			sprintf(string, "oo");
		else
		if(current_db <= 0.0)
			sprintf(string, "%d", current_db);
		else
			sprintf(string, "+%d", current_db);

		set_color(BLACK);
		int y1 = y + 1 + i * canvas->get_h() / (MAJOR_DIVISIONS - 1);
		int x4 = canvas->get_x() - LINE_W4 - get_text_width(SMALLFONT, string);
		draw_text(x4 + 1, y1 + get_text_ascent(SMALLFONT) / 2 + 1, string);
		draw_line(x1 + 1, y1 + 1, x3 + 1, y1 + 1);
		set_color(RED);
		draw_text(x4, y1 + get_text_ascent(SMALLFONT) / 2, string);
		draw_line(x1, y1, x3, y1);


		if(i < MAJOR_DIVISIONS - 1)
		{
			for(int j = 0; j < MINOR_DIVISIONS; j++)
			{
				int y2 = y1 + j * major_division / MINOR_DIVISIONS;
				set_color(BLACK);
				draw_line(x2 + 1, y2 + 1, x3 + 1, y2 + 1);
				set_color(RED);
				draw_line(x2, y2, x3, y2);
			}
		}
	}


// Frequency
	for(int i = 0; i <= MAJOR_DIVISIONS; i++)
	{
		int freq = Freq::tofreq(i * TOTALFREQS / MAJOR_DIVISIONS);
		char string[BCTEXTLEN];
		x1 = canvas->get_x() + i * canvas->get_w() / MAJOR_DIVISIONS;
		int y1 = canvas->get_y() + canvas->get_h() + LINE_W1 - 1;
		sprintf(string, "%d", freq);
		int x2 = x1 - get_text_width(SMALLFONT, string);
		int y2 = canvas->get_y() + canvas->get_h() + LINE_W2 - 1;
		int y3 = canvas->get_y() + canvas->get_h() + LINE_W3 - 1;
		int y4 = canvas->get_y() + canvas->get_h() + get_text_height(SMALLFONT) + LINE_W4 - 1;
		
		set_color(BLACK);
		draw_text(x2 + 1, y4 + 1, string);
		draw_line(x1 + 1, y3 + 1, x1 + 1, y1 + 1);
		
		set_color(RED);
		draw_text(x2, y4, string);
		draw_line(x1, y3, x1, y1);
		
		if(i < MAJOR_DIVISIONS)
		{
			for(int j = 0; j < MINOR_DIVISIONS; j++)
			{
				int x3 = (int)(x1 +
					(canvas->get_w() / MAJOR_DIVISIONS) -
					exp(-(double)j * 0.7) *
					(canvas->get_w() / MAJOR_DIVISIONS));
				set_color(BLACK);
				draw_line(x3 + 1, y2 + 1, x3 + 1, y1 + 1);
				set_color(RED);
				draw_line(x3, y2, x3, y1);
			}
		}
	}
}

void GraphicGUI::update_canvas()
{
	canvas->process(0, 0, 1);
}

void GraphicGUI::update_textboxes()
{
	if(plugin->active_point >= 0)
	{
		GraphicPoint *active_point = plugin->config.points.values[plugin->active_point];
		freq_text->update(active_point->freq);
		value_text->update(active_point->value);
	}
}

int GraphicGUI::resize_event(int w, int h)
{
	return 1;
}


















GraphicEQ::GraphicEQ(PluginServer *server)
 : PluginAClient(server)
{
	
	fft = 0;
	need_reconfigure = 1;
	active_point = -1;
}

GraphicEQ::~GraphicEQ()
{
	

	if(fft) delete fft;
}


int GraphicEQ::is_realtime() { return 1; }

const char* GraphicEQ::plugin_title() { return N_("EQ Graphic"); }

NEW_PICON_MACRO(GraphicEQ)
NEW_WINDOW_MACRO(GraphicEQ, GraphicGUI)

LOAD_CONFIGURATION_MACRO(GraphicEQ, GraphicConfig)

int GraphicEQ::active_point_exists()
{
	if(active_point >= 0 && active_point < config.points.total) return 1;
	return 0;
}

int GraphicEQ::load_defaults()
{
	char directory[BCTEXTLEN], string[BCTEXTLEN];
	sprintf(directory, "%sgraphiceq.rc", BCASTDIR);
	defaults = new BC_Hash(directory);
	defaults->load();
	
	config.points.remove_all_objects();
	int total_points = defaults->get("TOTAL", 0);
	for(int i = 0; i < total_points; i++)
	{
		sprintf(string, "X%d", i);
		int x = defaults->get(string, 0);
		sprintf(string, "Y%d", i);
		double y = defaults->get(string, 0.0);
		GraphicPoint *point;
		config.points.append(point = new GraphicPoint);
		point->freq = x;
		point->value = y;
	}

	if(!active_point_exists()) active_point = -1;

	return 0;
}

int GraphicEQ::save_defaults()
{
	char string[BCTEXTLEN];

	defaults->update("TOTAL", config.points.total);

	for(int i = 0; i < config.points.total; i++)
	{
		sprintf(string, "X%d", i);
		defaults->update(string, config.points.values[i]->freq);
		sprintf(string, "Y%d", i);
		defaults->update(string, config.points.values[i]->value);
	}


	defaults->save();

	return 0;
}

void GraphicEQ::read_data(KeyFrame *keyframe)
{
	FileXML input;
	int result = 0;

	input.set_shared_string(keyframe->get_data(), strlen(keyframe->get_data()));
	config.points.remove_all_objects();

	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("GRAPHICEQ"))
			{
			}
			else
			if(input.tag.title_is("POINT"))
			{
				GraphicPoint *point;
				config.points.append(point = new GraphicPoint);
				point->freq = input.tag.get_property("X", 0);
				point->value = input.tag.get_property("Y", 0.0);
			}
		}
	}

	if(!active_point_exists()) active_point = -1;
}




void GraphicEQ::save_data(KeyFrame *keyframe)
{
	FileXML output;
	output.set_shared_string(keyframe->get_data(), MESSAGESIZE);


	for(int i = 0; i < config.points.total; i++)
	{
		output.tag.set_title("POINT");
		output.tag.set_property("X", config.points.values[i]->freq);
		output.tag.set_property("Y", config.points.values[i]->value);
		output.append_tag();
	}
	if(config.points.total) output.append_newline();
	
	output.terminate_string();
}


void GraphicEQ::update_gui()
{
	if(thread && ((GraphicGUI*)thread->window)->canvas->state != GraphicCanvas::DRAG_POINT)
	{
		if(load_configuration())
		{
			((GraphicGUI*)thread->window)->lock_window("GraphicEQ::update_gui");
			((GraphicGUI*)thread->window)->update_canvas();
			((GraphicGUI*)thread->window)->update_textboxes();
			((GraphicGUI*)thread->window)->unlock_window();
		}
	}
}

void GraphicEQ::reconfigure()
{
	if(!fft)
	{
		fft = new GraphicFFT(this);
		fft->initialize(WINDOW_SIZE);
	}

	calculate_envelope();
	
	for(int i = 0; i < WINDOW_SIZE / 2; i++)
	{
		if(envelope[i] < 0) envelope[i] = 0;
	}

	need_reconfigure = 0;
}

int GraphicEQ::process_buffer(int64_t size, 
	double *buffer, 
	int64_t start_position,
	int sample_rate)
{
	need_reconfigure |= load_configuration();
	if(need_reconfigure) reconfigure();
	
	fft->process_buffer(start_position, size, buffer, get_direction());


	return 0;
}


double GraphicEQ::freq_to_magnitude(double frequency)
{
// Calculate using control points
	for(int i = 0; i < config.points.total; i++)
	{
		GraphicPoint *point = config.points.values[i];
		if(point->freq == (int)frequency)
		{
			return DB::fromdb(point->value);
		}
	}


	int nyquist = PluginAClient::project_sample_rate / 2;
	int slot = (int)(frequency * WINDOW_SIZE / 2 / nyquist);
	if(slot >= WINDOW_SIZE / 2) slot = WINDOW_SIZE / 2 - 1;
//printf("GraphicEQ::freq_to_db %d %f\n", slot, envelope[slot]);
	return envelope[slot];
}


void GraphicEQ::calculate_envelope()
{
	GraphicPoint *point1 = 0;
	GraphicPoint *point2 = 0;
	int niquist = PluginAClient::project_sample_rate / 2;



// Make temporary list of just points in order
	ArrayList<GraphicPoint*> temp_points;
	for(int i = 0; i < config.points.total; i++)
	{
		temp_points.append(config.points.values[i]);
	}

	for(int i = 0; i < temp_points.total; i++)
	{
		GraphicPoint *point = temp_points.values[i];
		if(i == active_point) 
		{
			GraphicPoint *prev_point = 0;
			GraphicPoint *next_point = 0;
			if(i >= 1) prev_point = temp_points.values[i - 1];
			if(i < temp_points.total - 1) next_point = temp_points.values[i + 1];
			if(prev_point && prev_point->freq >= point->freq ||
				next_point && next_point->freq <= point->freq)
				temp_points.remove_number(i);
			break;
		}
	}


// Join each point
	if(temp_points.total)
	{
		GraphicPoint *first_point = temp_points.values[0];
		GraphicPoint *last_point = temp_points.values[temp_points.total - 1];
		for(int i = 0; i < WINDOW_SIZE / 2; i++)
		{
			int freq = i * niquist / (WINDOW_SIZE / 2);
			if(freq <= first_point->freq)
				envelope[i] = first_point->value;
			else
			if(freq >= last_point->freq)
				envelope[i] = last_point->value;
			else
			{
				GraphicPoint *point1 = first_point;
				GraphicPoint *point2 = last_point;
				for(int j = 0; j < temp_points.total; j++)
				{
					if(temp_points.values[j]->freq <= freq)
						point1 = temp_points.values[j];
					else
					{
						point2 = temp_points.values[j];
						break;
					}
				}
				
				if(point2->freq != point1->freq)
				{
					int freqslot1 = Freq::fromfreq(point1->freq);
					int freqslot2 = Freq::fromfreq(point2->freq);
					int freqslot = Freq::fromfreq(freq);
				
					envelope[i] = (double)(freqslot - freqslot1) *
						(point2->value - point1->value) / 
						(freqslot2 - freqslot1) +
						point1->value;
				}
				else
					envelope[i] = point1->value;
			}
			envelope[i] = DB::fromdb(envelope[i]);
		}
	}
	else
	{
		for(int i = 0; i < WINDOW_SIZE / 2; i++)
		{
			envelope[i] = 1.0;
		}
	}
}







GraphicFFT::GraphicFFT(GraphicEQ *plugin)
 : CrossfadeFFT()
{
	this->plugin = plugin;
}

GraphicFFT::~GraphicFFT()
{
}


int GraphicFFT::signal_process()
{
	for(int i = 0; i < window_size / 2; i++)
	{
		double result = plugin->envelope[i] * sqrt(freq_real[i] * freq_real[i] + freq_imag[i] * freq_imag[i]);
		double angle = atan2(freq_imag[i], freq_real[i]);
		freq_real[i] = result * cos(angle);
		freq_imag[i] = result * sin(angle);
	}

	symmetry(window_size, freq_real, freq_imag);

	return 0;
}

int GraphicFFT::read_samples(int64_t output_sample, 
	int samples, 
	double *buffer)
{
	return plugin->read_samples(buffer,
		0,
		plugin->get_samplerate(),
		output_sample,
		samples);
}


