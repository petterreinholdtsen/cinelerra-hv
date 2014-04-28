#ifndef HISTOGRAMCONFIG_H
#define HISTOGRAMCONFIG_H



class HistogramPoint : public ListItem<HistogramPoint*>
{
public:
	HistogramPoint();
	~HistogramPoint();

	float x, y;
};


class HistogramPoints : public List<HistogramPoint*>
{
public:
	HistogramPoints();
	~HistogramPoints();

// Insert new point
	void insert(float x, float y);
};

class HistogramConfig
{
public:
	HistogramConfig();

	int equivalent(HistogramConfig &that);
	void copy_from(HistogramConfig &that);
	void interpolate(HistogramConfig &prev, 
		HistogramConfig &next, 
		int64_t prev_frame, 
		int64_t next_frame, 
		int64_t current_frame);
// Used by constructor and reset button
	void reset(int do_mode);
	void boundaries();

// Range 0 - 1.0
// Input points
	HistogramPoints points[HISTOGRAM_MODES];
// Output points
	float output_min[HISTOGRAM_MODES];
	float output_max[HISTOGRAM_MODES];
	int automatic;
	int mode;
	float threshold;
};


#endif



