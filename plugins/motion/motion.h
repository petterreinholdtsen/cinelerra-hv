#include <math.h>
#include <stdint.h>
#include <string.h>

#include "bcdisplayinfo.h"
#include "clip.h"
#include "defaults.h"
#include "filexml.h"
#include "keyframe.h"
#include "loadbalance.h"
#include "picon_png.h"
#include "pluginvclient.h"
#include "vframe.h"

class MotionMain;
class MotionWindow;
class MotionEngine;



#define OVERSAMPLE 2

class MotionConfig
{
public:
	MotionConfig();

	int equivalent(MotionConfig &that);
	void copy_from(MotionConfig &that);
	void interpolate(MotionConfig &prev, 
		MotionConfig &next, 
		long prev_frame, 
		long next_frame, 
		long current_frame);

	int source;
	int target;
	enum
	{
		TRACK,
		COMPENSATE
	}
	int mode;
};



class MotionCanvas : public BC_SubWindow
{
public:
	MotionCanvas(MotionMain *plugin, 
		int x, 
		int y, 
		int w,
		int w);
	int button_press_event();
	int button_release_event();
	int cursor_motion_event();
	int state;

	enum
	{
		NONE,
		DRAG,
		DRAG_FULL,
		ZOOM
	};

	int start_cursor_x, start_cursor_y;
	float start_x1, start_y1;
	float start_x2, start_y2;
	float start_x3, start_y3;
	float start_x4, start_y4;
	MotionMain *plugin;
};

class MotionCoord : public BC_TumbleTextBox
{
public:
	MotionCoord(MotionWindow *gui,
		MotionMain *plugin, 
		int x, 
		int y,
		float value,
		int is_x);
	int handle_event();
	MotionMain *plugin;
	int is_x;
};

class MotionReset : public BC_GenericButton
{
public:
	MotionReset(MotionMain *plugin, 
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
};

class MotionMode : public BC_Radial
{
public:
	MotionMode(MotionMain *plugin, 
		int x, 
		int y,
		int value,
		char *text);
	int handle_event();
	MotionMain *plugin;
	int value;
};

class MotionDirection : public BC_Radial
{
public:
	MotionDirection(MotionMain *plugin, 
		int x, 
		int y,
		int value,
		char *text);
	int handle_event();
	MotionMain *plugin;
	int value;
};

class MotionWindow : public BC_Window
{
public:
	MotionWindow(MotionMain *plugin, int x, int y);
	~MotionWindow();

	int create_objects();
	int close_event();
	int resize_event(int x, int y);
	void update_canvas();
	void update_mode();
	void update_coord();
	void calculate_canvas_coords(int &x1, 
		int &y1, 
		int &x2, 
		int &y2, 
		int &x3, 
		int &y3, 
		int &x4, 
		int &y4);

	MotionCanvas *canvas;
	MotionCoord *x, *y;
	MotionReset *reset;
	MotionMode *mode_motion, *mode_sheer, *mode_stretch;
	MotionMain *plugin;
	MotionDirection *forward, *reverse;
};



PLUGIN_THREAD_HEADER(MotionMain, MotionThread, MotionWindow)


class MotionMain : public PluginVClient
{
public:
	MotionMain(PluginServer *server);
	~MotionMain();

	int process_realtime(VFrame *input_ptr, VFrame *output_ptr);
	int is_multichannel();
	int is_realtime();
	char* plugin_title();
	int load_defaults();
	int save_defaults();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	void update_gui();

	PLUGIN_CLASS_MEMBERS(MotionConfig, MotionThread)

	float get_current_x();
	float get_current_y();
	void set_current_x(float value);
	void set_current_y(float value);
	VFrame *input, *output;
	VFrame *temp;
	MotionEngine *engine;
};


class MotionMatrix
{
public:
	MotionMatrix();
	void identity();
	void translate(double x, double y);
	void scale(double x, double y);
// Result is put in dst
	void multiply(MotionMatrix *dst);
	void copy_from(MotionMatrix *src);
	void invert(MotionMatrix *dst);
	void transform_point(float x, float y, float *newx, float *newy);
	double determinant();
	void dump();
	double values[3][3];
};

class MotionPackage : public LoadPackage
{
public:
	MotionPackage();
	int y1, y2;
};

class MotionUnit : public LoadClient
{
public:
	MotionUnit(MotionEngine *server, MotionMain *plugin);
	void process_package(LoadPackage *package);
	void calculate_matrix(
		double in_x1,
		double in_y1,
		double in_x2,
		double in_y2,
		double out_x1,
		double out_y1,
		double out_x2,
		double out_y2,
		double out_x3,
		double out_y3,
		double out_x4,
		double out_y4,
		MotionMatrix *result);
	float MotionUnit::transform_cubic(float dx,
    	float jm1,
    	float j,
    	float jp1,
    	float jp2);
	MotionEngine *server;
	MotionMain *plugin;
};

class MotionEngine : public LoadServer
{
public:
	MotionEngine(MotionMain *plugin, 
		int total_clients, 
		int total_packages);
	void init_packages();
	LoadClient* new_client();
	LoadPackage* new_package();
	MotionMain *plugin;
};













