#include "histogram.h"
#include "histogramwindow.h"



PLUGIN_THREAD_OBJECT(HistogramMain, HistogramThread, HistogramWindow)



HistogramWindow::HistogramWindow(HistogramMain *plugin, int x, int y)
 : BC_Window(plugin->gui_string, 
 	x,
	y,
	440, 
	480, 
	440, 
	480, 
	0, 
	1)
{
	this->plugin = plugin; 
}

HistogramWindow::~HistogramWindow()
{
}

static VFrame max_picon_image(max_picon_png);
static VFrame mid_picon_image(mid_picon_png);
static VFrame min_picon_image(min_picon_png);

int HistogramWindow::create_objects()
{
	int x = 10, y = 10, x1 = 10;
	int subscript = plugin->config.mode;

	max_picon = new BC_Pixmap(this, &max_picon_image);
	mid_picon = new BC_Pixmap(this, &mid_picon_image);
	min_picon = new BC_Pixmap(this, &min_picon_image);
	add_subwindow(mode_v = new HistogramMode(plugin, 
		x, 
		y,
		HISTOGRAM_VALUE,
		_("Value")));
	x += 70;
	add_subwindow(mode_r = new HistogramMode(plugin, 
		x, 
		y,
		HISTOGRAM_RED,
		_("Red")));
	x += 70;
	add_subwindow(mode_g = new HistogramMode(plugin, 
		x, 
		y,
		HISTOGRAM_GREEN,
		_("Green")));
	x += 70;
	add_subwindow(mode_b = new HistogramMode(plugin, 
		x, 
		y,
		HISTOGRAM_BLUE,
		_("Blue")));
// 	x += 70;
// 	add_subwindow(mode_a = new HistogramMode(plugin, 
// 		x, 
// 		y,
// 		HISTOGRAM_ALPHA,
// 		_("Alpha")));

	x = x1;
	y += 30;
	add_subwindow(new BC_Title(x, y, _("Input min:")));
	x += 80;
	input_min = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.input_min[subscript],
		subscript,
		HistogramSlider::DRAG_MIN_INPUT);
	input_min->create_objects();
	x += 90;
	add_subwindow(new BC_Title(x, y, _("Mid:")));
	x += 40;
	input_mid = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.input_mid[subscript],
		subscript,
		HistogramSlider::DRAG_MID_INPUT);
	input_mid->create_objects();
	input_mid->update(plugin->config.input_mid[subscript]);
	x += 90;
	add_subwindow(new BC_Title(x, y, _("Max:")));
	x += 40;
	input_max = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.input_max[subscript],
		subscript,
		HistogramSlider::DRAG_MAX_INPUT);
	input_max->create_objects();

	x = x1;
	y += 30;
	canvas_w = get_w() - x - x;
	canvas_h = get_h() - y - 170;
	title1_x = x;
	title2_x = x + (int)(canvas_w * -FLOAT_MIN / FLOAT_RANGE);
	title3_x = x + (int)(canvas_w * (1.0 - FLOAT_MIN) / FLOAT_RANGE);
	title4_x = x + (int)(canvas_w);
	add_subwindow(canvas = new BC_SubWindow(x, 
		y, 
		canvas_w, 
		canvas_h,
		0xffffff));
	draw_canvas_overlay();
	canvas->flash();

	y += canvas->get_h() + 1;
	add_subwindow(new BC_Title(title1_x, 
		y, 
		"-10%"));
	add_subwindow(new BC_Title(title2_x,
		y,
		"0%"));
	add_subwindow(new BC_Title(title3_x - get_text_width(MEDIUMFONT, "100"),
		y,
		"100%"));
	add_subwindow(new BC_Title(title4_x - get_text_width(MEDIUMFONT, "110"),
		y,
		"110%"));

	y += 20;
	add_subwindow(input = new HistogramSlider(plugin, 
		this,
		x, 
		y, 
		get_w() - 20,
		30,
		1));
	input->update();

	y += input->get_h() + 10;
	add_subwindow(new BC_Title(x, y, _("Output min:")));
	x += 90;
	output_min = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.output_min[subscript],
		subscript,
		HistogramSlider::DRAG_MIN_OUTPUT);
	output_min->create_objects();
	x += 90;
	add_subwindow(new BC_Title(x, y, _("Max:")));
	x += 40;
	output_max = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.output_max[subscript],
		subscript,
		HistogramSlider::DRAG_MAX_OUTPUT);
	output_max->create_objects();

	x = x1;
	y += 30;



	add_subwindow(output = new HistogramSlider(plugin, 
		this,
		x, 
		y, 
		get_w() - 20,
		30,
		0));
	output->update();
	y += 40;


	add_subwindow(automatic = new HistogramAuto(plugin, 
		x, 
		y));

	x += 120;
	add_subwindow(new HistogramReset(plugin, 
		x, 
		y));
	x += 100;
	add_subwindow(new BC_Title(x, y, _("Threshold:")));
	x += 100;
	threshold = new HistogramText(plugin,
		this,
		x,
		y,
		&plugin->config.threshold,
		0,
		0);
	threshold->create_objects();

	show_window();
	flush();
	return 0;
}

WINDOW_CLOSE_EVENT(HistogramWindow)

void HistogramWindow::update(int do_input)
{
	automatic->update(plugin->config.automatic);
	threshold->update(plugin->config.threshold);
	update_mode();

	if(do_input) update_input();
	update_output();
}

void HistogramWindow::update_input()
{
	int subscript = plugin->config.mode;
	input->update();
	input_min->subscript = subscript;
	input_mid->subscript = subscript;
	input_max->subscript = subscript;
	input_min->update(plugin->config.input_min[subscript]);
	input_mid->update(plugin->config.input_mid[subscript]);
	input_max->update(plugin->config.input_max[subscript]);
}

void HistogramWindow::update_output()
{
	int subscript = plugin->config.mode;
	output->update();
	output_min->update(plugin->config.output_min[subscript]);
	output_max->update(plugin->config.output_max[subscript]);
}

void HistogramWindow::update_mode()
{
	mode_v->update(plugin->config.mode == HISTOGRAM_VALUE ? 1 : 0);
	mode_r->update(plugin->config.mode == HISTOGRAM_RED ? 1 : 0);
	mode_g->update(plugin->config.mode == HISTOGRAM_GREEN ? 1 : 0);
	mode_b->update(plugin->config.mode == HISTOGRAM_BLUE ? 1 : 0);
//	mode_a->update(plugin->config.mode == HISTOGRAM_ALPHA ? 1 : 0);
	input_min->output = &plugin->config.input_min[plugin->config.mode];
	input_mid->output = &plugin->config.input_mid[plugin->config.mode];
	input_max->output = &plugin->config.input_max[plugin->config.mode];
	output_min->output = &plugin->config.output_min[plugin->config.mode];
	output_max->output = &plugin->config.output_max[plugin->config.mode];
}

void HistogramWindow::draw_canvas_overlay()
{
	canvas->set_color(0x00ff00);
	int y1;
//printf("HistogramWindow::draw_canvas_overlay 1 %d\n", plugin->config.mode);
	for(int i = 0; i < canvas_w; i++)
	{
		float input = (float)i / 
				canvas_w * 
				FLOAT_RANGE + 
				FLOAT_MIN;
		float output = plugin->calculate_transfer(input, plugin->config.mode, 0);
		int y2 = canvas_h - (int)(output * canvas_h);
		if(i > 0)
		{
			canvas->draw_line(i - 1, y1, i, y2);
		}
		y1 = y2;
	}

	canvas->set_color(0xff0000);
	canvas->draw_line(title2_x - canvas->get_x(), 
		0, 
		title2_x - canvas->get_x(), 
		canvas_h);
	canvas->draw_line(title3_x - canvas->get_x(), 
		0, 
		title3_x - canvas->get_x(), 
		canvas_h);
}

void HistogramWindow::update_canvas()
{
	int64_t *accum = plugin->accum[plugin->config.mode];
	int accum_per_canvas_i = HISTOGRAM_RANGE / canvas_w + 1;
	float accum_per_canvas_f = (float)HISTOGRAM_RANGE / canvas_w;
	int normalize = 0;
	int max = 0;
	for(int i = 0; i < HISTOGRAM_RANGE; i++)
	{
		if(accum[i] > normalize) normalize = accum[i];
	}


	if(normalize)
	{
		for(int i = 0; i < canvas_w; i++)
		{
			int accum_start = (int)(accum_per_canvas_f * i);
			int accum_end = accum_start + accum_per_canvas_i;
			max = 0;
			for(int j = accum_start; j < accum_end; j++)
			{
				max = MAX(accum[j], max);
			}
//printf("HistogramWindow::update_canvas 1 %d %d\n", i, max);

//			max = max * canvas_h / normalize;
			max = (int)(log(max) / log(normalize) * canvas_h);

			canvas->set_color(0xffffff);
			canvas->draw_line(i, 0, i, canvas_h - max);
			canvas->set_color(0x000000);
			canvas->draw_line(i, canvas_h - max, i, canvas_h);
		}
	}
	else
	{
		canvas->set_color(0xffffff);
		canvas->draw_box(0, 0, canvas_w, canvas_h);
	}

	draw_canvas_overlay();
	canvas->flash();
}








HistogramReset::HistogramReset(HistogramMain *plugin, 
	int x,
	int y)
 : BC_GenericButton(x, y, _("Reset"))
{
	this->plugin = plugin;
}
int HistogramReset::handle_event()
{
	plugin->config.reset(0);
	plugin->thread->window->update(1);
	plugin->send_configure_change();
	return 1;
}









HistogramSlider::HistogramSlider(HistogramMain *plugin, 
	HistogramWindow *gui,
	int x, 
	int y, 
	int w,
	int h,
	int is_input)
 : BC_SubWindow(x, y, w, h)
{
	this->plugin = plugin;
	this->gui = gui;
	this->is_input = is_input;
	operation = NONE;
}

int HistogramSlider::input_to_pixel(float input)
{
	return (int)((input - FLOAT_MIN) / FLOAT_RANGE * get_w());
}

int HistogramSlider::button_press_event()
{
	if(is_event_win() && cursor_inside())
	{
		int subscript = plugin->config.mode;
		int min;
		int max;
		int w = get_w();
		int h = get_h();
		int half_h = get_h() / 2;

		if(is_input)
		{
			int x1 = input_to_pixel(plugin->config.input_mid[subscript]) - 
				gui->mid_picon->get_w() / 2;
			int x2 = x1 + gui->mid_picon->get_w();
			if(get_cursor_x() >= x1 && get_cursor_x() < x2 &&
				get_cursor_y() >= half_h && get_cursor_y() < h)
			{
				operation = DRAG_MID_INPUT;
			}
		}

		if(operation == NONE)
		{
			if(is_input)
			{
				int x1 = input_to_pixel(plugin->config.input_min[subscript]) - 
					gui->mid_picon->get_w() / 2;
				int x2 = x1 + gui->mid_picon->get_w();
				if(get_cursor_x() >= x1 && get_cursor_x() < x2 &&
					get_cursor_y() >= half_h && get_cursor_y() < h)
				{
					operation = DRAG_MIN_INPUT;
				}
			}
			else
			{
				int x1 = input_to_pixel(plugin->config.output_min[subscript]) - 
					gui->mid_picon->get_w() / 2;
				int x2 = x1 + gui->mid_picon->get_w();
				if(get_cursor_x() >= x1 && get_cursor_x() < x2 &&
					get_cursor_y() >= half_h && get_cursor_y() < h)
				{
					operation = DRAG_MIN_OUTPUT;
				}
			}
		}

		if(operation == NONE)
		{
			if(is_input)
			{
				int x1 = input_to_pixel(plugin->config.input_max[subscript]) - 
					gui->mid_picon->get_w() / 2;
				int x2 = x1 + gui->mid_picon->get_w();
				if(get_cursor_x() >= x1 && get_cursor_x() < x2 &&
					get_cursor_y() >= half_h && get_cursor_y() < h)
				{
					operation = DRAG_MAX_INPUT;
				}
			}
			else
			{
				int x1 = input_to_pixel(plugin->config.output_max[subscript]) - 
					gui->mid_picon->get_w() / 2;
				int x2 = x1 + gui->mid_picon->get_w();
				if(get_cursor_x() >= x1 && get_cursor_x() < x2 &&
					get_cursor_y() >= half_h && get_cursor_y() < h)
				{
					operation = DRAG_MAX_OUTPUT;
				}
			}
		}
		return 1;
	}
	return 0;
}

int HistogramSlider::button_release_event()
{
	if(operation != NONE)
	{
		operation = NONE;
		return 1;
	}
	return 0;
}

int HistogramSlider::cursor_motion_event()
{
//printf("HistogramSlider::cursor_motion_event 1\n");
	if(operation != NONE)
	{
		float value = (float)get_cursor_x() / get_w() * FLOAT_RANGE + FLOAT_MIN;
		CLAMP(value, FLOAT_MIN, FLOAT_MAX);
		int subscript = plugin->config.mode;
		float input_min = plugin->config.input_min[subscript];
		float input_max = plugin->config.input_max[subscript];
		float input_mid = plugin->config.input_mid[subscript];
		float input_mid_fraction = (input_mid - input_min) / (input_max - input_min);

		switch(operation)
		{
			case DRAG_MIN_INPUT:
				input_min = MIN(input_max, value);
				plugin->config.input_min[subscript] = input_min;
				input_mid = input_min + (input_max - input_min) * input_mid_fraction;
				break;
			case DRAG_MID_INPUT:
				CLAMP(value, input_min, input_max);
// Quantize value here so automatic calculation doesn't get rounding errors.
				value = Units::quantize(value, PRECISION);
				input_mid = value;
				break;
			case DRAG_MAX_INPUT:
				input_max = MAX(input_mid, value);
				input_mid = input_min + (input_max - input_min) * input_mid_fraction;
				break;
			case DRAG_MIN_OUTPUT:
				value = MIN(plugin->config.output_max[subscript], value);
				plugin->config.output_min[subscript] = value;
				break;
			case DRAG_MAX_OUTPUT:
				value = MAX(plugin->config.output_min[subscript], value);
				plugin->config.output_max[subscript] = value;
				break;
		}
	
		if(operation == DRAG_MIN_INPUT ||
			operation == DRAG_MID_INPUT ||
			operation == DRAG_MAX_INPUT)
		{
			plugin->config.input_mid[subscript] = input_mid;
			plugin->config.input_min[subscript] = input_min;
			plugin->config.input_max[subscript] = input_max;
			plugin->config.boundaries();
			gui->update_input();
		}
		else
		{
			plugin->config.boundaries();
			gui->update_output();
		}

		gui->unlock_window();
		plugin->send_configure_change();
		gui->lock_window("HistogramSlider::cursor_motion_event");
		return 1;
	}
	return 0;
}

void HistogramSlider::update()
{
	int w = get_w();
	int h = get_h();
	int half_h = get_h() / 2;
	int quarter_h = get_h() / 4;
	int mode = plugin->config.mode;
	int r = 0xff;
	int g = 0xff;
	int b = 0xff;
	int subscript = plugin->config.mode;

	clear_box(0, 0, w, h);

	switch(mode)
	{
		case HISTOGRAM_RED:
			g = b = 0x00;
			break;
		case HISTOGRAM_GREEN:
			r = b = 0x00;
			break;
		case HISTOGRAM_BLUE:
			r = g = 0x00;
			break;
	}

	for(int i = 0; i < w; i++)
	{
		int color = (int)(i * 0xff / w);
		set_color(((r * color / 0xff) << 16) | 
			((g * color / 0xff) << 8) | 
			(b * color / 0xff));

		if(is_input)
		{
			draw_line(i, quarter_h, i, half_h);
			color = (int)plugin->calculate_transfer(i * 0xffff / w, 
				subscript,
				1);
			set_color(((r * color / 0xffff) << 16) | 
				((g * color / 0xffff) << 8) | 
				(b * color / 0xffff));
			draw_line(i, 0, i, quarter_h);
		}
		else
			draw_line(i, 0, i, half_h);

	}

	float min;
	float max;
	if(is_input)
	{
		
		draw_pixmap(gui->mid_picon,
			input_to_pixel(plugin->config.input_mid[subscript]) - 
				gui->mid_picon->get_w() / 2,
			half_h + 1);
		min = plugin->config.input_min[subscript];
		max = plugin->config.input_max[subscript];
	}
	else
	{
		min = plugin->config.output_min[subscript];
		max = plugin->config.output_max[subscript];
	}

	draw_pixmap(gui->min_picon,
		input_to_pixel(min) - gui->min_picon->get_w() / 2,
		half_h + 1);
	draw_pixmap(gui->max_picon,
		input_to_pixel(max) - gui->max_picon->get_w() / 2,
		half_h + 1);

// printf("HistogramSlider::update %d %d\n", min, max);
	flash();
	flush();
}









HistogramAuto::HistogramAuto(HistogramMain *plugin, 
	int x, 
	int y)
 : BC_CheckBox(x, y, plugin->config.automatic, _("Automatic"))
{
	this->plugin = plugin;
}

int HistogramAuto::handle_event()
{
	plugin->config.automatic = get_value();
	plugin->send_configure_change();
	return 1;
}




HistogramMode::HistogramMode(HistogramMain *plugin, 
	int x, 
	int y,
	int value,
	char *text)
 : BC_Radial(x, y, plugin->config.mode == value, text)
{
	this->plugin = plugin;
	this->value = value;
}
int HistogramMode::handle_event()
{
	plugin->config.mode = value;
	plugin->thread->window->update_mode();
	plugin->thread->window->update_input();
	plugin->thread->window->update_output();
	plugin->thread->window->input->update();
	plugin->thread->window->output->update();
	plugin->send_configure_change();
	return 1;
}









HistogramText::HistogramText(HistogramMain *plugin,
	HistogramWindow *gui,
	int x,
	int y,
	float *output,
	int subscript,
	int operation)
 : BC_TumbleTextBox(gui, 
		(float)*output,
		(float)FLOAT_MIN,
		(float)FLOAT_MAX,
		x, 
		y, 
		60)
{
	this->plugin = plugin;
	this->output = output;
	this->subscript = subscript;
	this->operation = operation;
	set_precision(DIGITS);
	set_increment(PRECISION);
}


int HistogramText::handle_event()
{
	if(output)
	{
// Get parameters for midpoint before new data
		float *input_min = &plugin->config.input_min[subscript];
		float *input_max = &plugin->config.input_max[subscript];
		float *input_mid = &plugin->config.input_mid[subscript];
		float input_mid_fraction = (*input_mid - *input_min) / 
			(*input_max - *input_min);

		*output = atof(get_text());

// Recalculate midpoint
		if(operation != HistogramSlider::NONE &&
			operation != HistogramSlider::DRAG_MID_INPUT &&
			operation != HistogramSlider::DRAG_MIN_OUTPUT &&
			operation != HistogramSlider::DRAG_MAX_OUTPUT)
		{
			*input_mid = *input_min + 
				(*input_max - *input_min) * 
				input_mid_fraction;
			plugin->thread->window->update_input();
		}
	}
	plugin->thread->window->input->update();
	plugin->thread->window->output->update();
	plugin->send_configure_change();
	return 1;
}















