#include "histogramconfig.h"







HistogramPoint::HistogramPoint()
 : ListItem<HistogramPoint*>
{
}






HistogramPoints::HistogramPoints()
 : List<HistogramPoint*>
{
}

HistogramPoints::~HistogramPoints()
{
}

void HistogramPoints::insert(float x, float y)
{
	HistogramPoint *current = last;

// Get existing point after new point
	while(current)
	{
		if(current->x > x) 
			current = PREVIOUS;
		else
			break;
	}

// Insert new point before current point
	HistogramPoint *new_point = new HistogramPoint;
	if(current)
	{
		insert_before(current, new_point);
	}
	else
// Append new point to list
	{
		append(new_point);
	}

	new_point->x = x;
	new_point->y = y;
}

void HistogramPoints::boundaries()
{
	HistogramPoint *current = first;
	while(current)
	{
		CLAMP(current->x, FLOAT_MIN, FLOAT_MAX);
		CLAMP(current->y, 0.0, 1.0);
		current = NEXT;
	}
}









HistogramConfig::HistogramConfig()
{
	reset(1);
}

void HistogramConfig::reset(int do_mode)
{
	for(int i = 0; i < HISTOGRAM_MODES; i++)
	{
		while(points[i].last) delete points[i].last;
		points[i].insert(0.0, 0.0);
		points[i].insert(1.0, 1.0);
		output_min[i] = 0.0;
		output_max[i] = 1.0;
	}

	

	if(do_mode) 
	{
		mode = HISTOGRAM_VALUE;
		automatic = 0;
		threshold = 0.1;
	}
}

void HistogramConfig::boundaries()
{
	for(int i = 0; i < HISTOGRAM_MODES; i++)
	{
		points[i].boundaries();
		CLAMP(output_min[i], FLOAT_MIN, FLOAT_MAX);
		CLAMP(output_max[i], FLOAT_MIN, FLOAT_MAX);
		output_min[i] = Units::quantize(output_min[i], PRECISION);
		output_max[i] = Units::quantize(output_max[i], PRECISION);
	}
	CLAMP(threshold, 0, 1);
}

int HistogramConfig::equivalent(HistogramConfig &that)
{
	for(int i = 0; i < HISTOGRAM_MODES; i++)
	{
		if(!EQUIV(input_min[i], that.input_min[i]) ||
			!EQUIV(input_mid[i], that.input_mid[i]) ||
			!EQUIV(input_max[i], that.input_max[i]) ||
			!EQUIV(output_min[i], that.output_min[i]) ||
			!EQUIV(output_max[i], that.output_max[i])) return 0;
	}

	if(automatic != that.automatic ||
		mode != that.mode ||
		!EQUIV(threshold, that.threshold)) return 0;

	return 1;
}

void HistogramConfig::copy_from(HistogramConfig &that)
{
	for(int i = 0; i < HISTOGRAM_MODES; i++)
	{
		input_min[i] = that.input_min[i];
		input_mid[i] = that.input_mid[i];
		input_max[i] = that.input_max[i];
		output_min[i] = that.output_min[i];
		output_max[i] = that.output_max[i];
	}

	automatic = that.automatic;
	mode = that.mode;
	threshold = that.threshold;
}

void HistogramConfig::interpolate(HistogramConfig &prev, 
	HistogramConfig &next, 
	int64_t prev_frame, 
	int64_t next_frame, 
	int64_t current_frame)
{
	double next_scale = (double)(current_frame - prev_frame) / (next_frame - prev_frame);
	double prev_scale = (double)(next_frame - current_frame) / (next_frame - prev_frame);

	for(int i = 0; i < HISTOGRAM_MODES; i++)
	{
		input_min[i] = prev.input_min[i] * prev_scale + next.input_min[i] * next_scale;
		input_mid[i] = prev.input_mid[i] * prev_scale + next.input_mid[i] * next_scale;
		input_max[i] = prev.input_max[i] * prev_scale + next.input_max[i] * next_scale;
		output_min[i] = prev.output_min[i] * prev_scale + next.output_min[i] * next_scale;
		output_max[i] = prev.output_max[i] * prev_scale + next.output_max[i] * next_scale;
	}
	threshold = prev.threshold * prev_scale + next.threshold * next_scale;
	automatic = prev.automatic;
	mode = prev.mode;
}

