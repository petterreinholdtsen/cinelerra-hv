#include "clip.h"
#include "filexml.h"
#include "floatauto.h"
#include "floatautos.h"
#include "transportque.inc"

FloatAutos::FloatAutos(EDL *edl,
				Track *track, 
				int color, 
				float min, 
				float max, 
				int virtual_h,
				int use_floats)
 : Autos(edl, track)
{
	this->max = max; this->min = min;
	this->virtual_h = virtual_h;
	this->use_floats = use_floats;
}

FloatAutos::~FloatAutos()
{
}

int FloatAutos::get_track_pixels(int zoom_track, int pixel, int &center_pixel, float &yscale)
{
	center_pixel = pixel + zoom_track / 2;
	yscale = -(float)zoom_track / (max - min);
}

int FloatAutos::draw_joining_line(BC_SubWindow *canvas, int vertical, int center_pixel, int x1, int y1, int x2, int y2)
{
	if(vertical)
	canvas->draw_line(center_pixel - y1, x1, center_pixel - y2, x2);
	else
	canvas->draw_line(x1, center_pixel + y1, x2, center_pixel + y2);
}

Auto* FloatAutos::add_auto(long position, float value)
{
	FloatAuto* current = (FloatAuto*)autoof(position);
	FloatAuto* new_auto;
	
	insert_before(current, new_auto = new FloatAuto(edl, this));

	new_auto->position = position;
	new_auto->value = value;
	
	return new_auto;
}

Auto* FloatAutos::append_auto()
{
	return append(new FloatAuto(edl, this));
}

Auto* FloatAutos::new_auto()
{
	return new FloatAuto(edl, this);
}

float FloatAutos::fix_value(float value)
{
	int value_int;
	
	if(use_floats)
	{
// Fix precision
		value_int = (int)(value * 100);
		value = (float)value_int / 100;
	}
	else
	{
// not really floating point
		value_int = (int)value;
		value = value_int;
	}

	if(value < min) value = min;
	else
	if(value > max) value = max;
	
	return value;	
}

int FloatAutos::get_testy(float slope, int cursor_x, int ax, int ay)
{
	return (int)(slope * (cursor_x - ax)) + ay;
}

int FloatAutos::automation_is_constant(long start, long end)
{
	Auto *current_auto, *before = 0, *after = 0;
	int result;

	result = 1;          // default to constant
	if(!last && !first) return result; // no automation at all

// quickly get autos just outside range	
	get_neighbors(start, end, &before, &after);

// autos before range
	if(before) 
		current_auto = before;   // try first auto
	else 
		current_auto = first;

// test autos in range	
	for( ; result && 
		current_auto && 
		current_auto->next && 
		current_auto->position < end; 
		current_auto = current_auto->next)
	{
// not constant
		if(((FloatAuto*)current_auto->next)->value != ((FloatAuto*)current_auto)->value) result = 0;
	}

	return result;
}

double FloatAutos::get_automation_constant(long start, long end)
{
	Auto *current_auto, *before = 0, *after = 0;
	
// quickly get autos just outside range	
	get_neighbors(start, end, &before, &after);

// no auto before range so use first
	if(before)
		current_auto = before;
	else
		current_auto = first;

// no autos at all so use default value
	if(!current_auto) current_auto = default_auto;

	return ((FloatAuto*)current_auto)->value;
}


float FloatAutos::get_value(long position, 
	int direction, 
	FloatAuto* &previous, 
	FloatAuto* &next)
{
	double slope;
	double intercept;
	long slope_len;

// Calculate bezier equation at position
	float y0, y1, y2, y3;
 	float t;



	







	get_fade_automation(slope,
		intercept,
		position,
		slope_len,
		PLAY_FORWARD);

	return (float)intercept;
}


void FloatAutos::get_fade_automation(double &slope,
	double &intercept,
	long input_position,
	long &slope_len,
	int direction)
{
	Auto *current = 0;
	FloatAuto *prev_keyframe = 
		(FloatAuto*)get_prev_auto(input_position, direction, current);
	FloatAuto *next_keyframe = 
		(FloatAuto*)get_next_auto(input_position, direction, current);
	long new_slope_len;

	if(direction == PLAY_FORWARD)
	{
		new_slope_len = next_keyframe->position - prev_keyframe->position;

//printf("FloatAutos::get_fade_automation %d %d %d\n", 
//	prev_keyframe->position, input_position, next_keyframe->position);

// Two distinct automation points within range
		if(next_keyframe->position > prev_keyframe->position)
		{
			slope = ((double)next_keyframe->value - prev_keyframe->value) / 
				new_slope_len;
			intercept = ((double)input_position - prev_keyframe->position) * slope + prev_keyframe->value;

			if(next_keyframe->position < input_position + new_slope_len)
				new_slope_len = next_keyframe->position - input_position;
			slope_len = MIN(slope_len, new_slope_len);
		}
		else
// One automation point within range
		{
			slope = 0;
			intercept = prev_keyframe->value;
		}
	}
	else
	{
		new_slope_len = prev_keyframe->position - next_keyframe->position;
// Two distinct automation points within range
		if(next_keyframe->position < prev_keyframe->position)
		{
			slope = ((double)next_keyframe->value - prev_keyframe->value) / new_slope_len;
			intercept = ((double)prev_keyframe->position - input_position) * slope + prev_keyframe->value;

			if(prev_keyframe->position > input_position - new_slope_len)
				new_slope_len = input_position - prev_keyframe->position;
			slope_len = MIN(slope_len, new_slope_len);
		}
		else
// One automation point within range
		{
			slope = 0;
			intercept = next_keyframe->value;
		}
	}
}

float FloatAutos::value_to_percentage(float value)
{
	return (value - min) / (max - min);
}


int FloatAutos::dump()
{
	printf("	FloatAutos::dump %p\n", this);
	printf("	Default: position %ld value %f\n", default_auto->position, ((FloatAuto*)default_auto)->value);
	for(Auto* current = first; current; current = NEXT)
	{
		printf("	position %ld value %f\n", current->position, ((FloatAuto*)current)->value);
	}
	return 0;
}
