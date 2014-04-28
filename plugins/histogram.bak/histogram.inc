#ifndef HISTOGRAM_INC
#define HISTOGRAM_INC





// modes
#define HISTOGRAM_MODES 5
#define HISTOGRAM_RED 0
#define HISTOGRAM_GREEN 1
#define HISTOGRAM_BLUE 2
#define HISTOGRAM_ALPHA 3
#define HISTOGRAM_VALUE 4

// Number of divisions in histogram.  
// 65536 + min and max range to speed up the tabulation
#define HISTOGRAM_RANGE 0x13333
#define FLOAT_RANGE 1.2
// Minimum value in percentage
#define HISTOGRAM_MIN -10
#define FLOAT_MIN -0.1
// Maximum value in percentage
#define HISTOGRAM_MAX 110
#define FLOAT_MAX 1.1

#define PRECISION 0.001
#define DIGITS 3
#define THRESHOLD_SCALE 1000


class HistogramMain;


#endif
