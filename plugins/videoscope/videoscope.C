#include "bcdisplayinfo.h"
#include "clip.h"
#include "defaults.h"
#include "filexml.h"
#include "guicast.h"
#include "loadbalance.h"
#include "picon_png.h"
#include "../colors/plugincolors.h"
#include "pluginvclient.h"
#include "fonts.h"
#include "vframe.h"

#include <math.h>
#include <stdint.h>
#include <string.h>





class VideoScopeEffect;
class VideoScopeEngine;


class VideoScopeConfig
{
public:
	VideoScopeConfig();
};

class VideoScopeWaveform : public BC_SubWindow
{
public:
	VideoScopeWaveform(VideoScopeEffect *plugin, 
		int x, 
		int y,
		int w,
		int h);
	VideoScopeEffect *plugin;
};


class VideoScopeVectorscope : public BC_SubWindow
{
public:
	VideoScopeVectorscope(VideoScopeEffect *plugin, 
		int x, 
		int y,
		int w,
		int h);
	VideoScopeEffect *plugin;
};

class VideoScopeWindow : public BC_Window
{
public:
	VideoScopeWindow(VideoScopeEffect *plugin, int x, int y, int w, int h);
	~VideoScopeWindow();

	void calculate_sizes(int w, int h);
	void create_objects();
	int close_event();
	int resize_event(int w, int h);
	void allocate_bitmaps();
	void draw_overlays();

	VideoScopeEffect *plugin;
	VideoScopeWaveform *waveform;
	VideoScopeVectorscope *vectorscope;
	BC_Bitmap *waveform_bitmap;
	BC_Bitmap *vector_bitmap;

	int vector_x, vector_y, vector_w, vector_h;
	int wave_x, wave_y, wave_w, wave_h;
};

PLUGIN_THREAD_HEADER(VideoScopeEffect, VideoScopeThread, VideoScopeWindow)




class VideoScopePackage : public LoadPackage
{
public:
	VideoScopePackage();
	int row1, row2;
};


class VideoScopeUnit : public LoadClient
{
public:
	VideoScopeUnit(VideoScopeEffect *plugin, VideoScopeEngine *server);
	void process_package(LoadPackage *package);
	VideoScopeEffect *plugin;
};

class VideoScopeEngine : public LoadServer
{
public:
	VideoScopeEngine(VideoScopeEffect *plugin, int cpus);
	~VideoScopeEngine();
	void init_packages();
	LoadClient* new_client();
	LoadPackage* new_package();
	VideoScopeEffect *plugin;
};

class VideoScopeEffect : public PluginVClient
{
public:
	VideoScopeEffect(PluginServer *server);
	~VideoScopeEffect();

	int process_realtime(VFrame *input, VFrame *output);
	int is_realtime();
	char* plugin_title();
	VFrame* new_picon();
	int load_defaults();
	int save_defaults();
	int show_gui();
	int set_string();
	void raise_window();

	int w, h;
	static VideoScopeWindow *window_static;
	static Mutex window_lock;
	VFrame *input;
	VideoScopeConfig config;
	VideoScopeEngine *engine;
	Defaults *defaults;
	VideoScopeThread *thread;
};













VideoScopeConfig::VideoScopeConfig()
{
}










VideoScopeWaveform::VideoScopeWaveform(VideoScopeEffect *plugin, 
		int x, 
		int y,
		int w,
		int h)
 : BC_SubWindow(x, y, w, h, BLACK)
{
	this->plugin = plugin;
}


VideoScopeVectorscope::VideoScopeVectorscope(VideoScopeEffect *plugin, 
		int x, 
		int y,
		int w,
		int h)
 : BC_SubWindow(x, y, w, h, BLACK)
{
	this->plugin = plugin;
}







VideoScopeWindow::VideoScopeWindow(VideoScopeEffect *plugin, 
	int x, 
	int y, 
	int w, 
	int h)
 : BC_Window(plugin->gui_string, 
 	x, 
	y, 
	w, 
	h, 
	50, 
	50, 
	1, 
	0,
	1,
	BLACK)
{
	this->plugin = plugin;
	waveform_bitmap = 0;
	vector_bitmap = 0;
}

VideoScopeWindow::~VideoScopeWindow()
{
	VideoScopeEffect::window_lock.lock();
	VideoScopeEffect::window_static = 0;
	VideoScopeEffect::window_lock.unlock();

	if(waveform_bitmap) delete waveform_bitmap;
	if(vector_bitmap) delete vector_bitmap;
}

void VideoScopeWindow::calculate_sizes(int w, int h)
{
	wave_x = 30;
	wave_y = 10;
	wave_w = w / 2 - 5 - wave_x;
	wave_h = h - 20 - wave_y;
	vector_x = w / 2 + 30;
	vector_y = 10;
	vector_w = w - 10 - vector_x;
	vector_h = h - 10 - vector_y;
}

void VideoScopeWindow::create_objects()
{
	calculate_sizes(get_w(), get_h());

	add_subwindow(waveform = new VideoScopeWaveform(plugin, wave_x, wave_y, wave_w, wave_h));
	add_subwindow(vectorscope = new VideoScopeVectorscope(plugin, vector_x, vector_y, vector_w, vector_h));
	allocate_bitmaps();
	draw_overlays();

	show_window();
	flush();
	
	VideoScopeEffect::window_lock.lock();
	VideoScopeEffect::window_static = this;
	VideoScopeEffect::window_lock.unlock();
}

int VideoScopeWindow::close_event()
{
	set_done(1);
	return 1;
}

int VideoScopeWindow::resize_event(int w, int h)
{
	VideoScopeEffect::window_lock.lock();

	clear_box(0, 0, w, h);
	plugin->w = w;
	plugin->h = h;
	calculate_sizes(w, h);
	waveform->reposition_window(wave_x, wave_y, wave_w, wave_h);
	waveform->clear_box(0, 0, wave_w, wave_h);
	vectorscope->reposition_window(vector_x, vector_y, vector_w, vector_h);
	vectorscope->clear_box(0, 0, wave_w, wave_h);
	allocate_bitmaps();
	draw_overlays();
	flash();

	VideoScopeEffect::window_lock.unlock();
	return 1;
}

void VideoScopeWindow::allocate_bitmaps()
{
	if(waveform_bitmap) delete waveform_bitmap;
	if(vector_bitmap) delete vector_bitmap;

	waveform_bitmap = new_bitmap(wave_w, wave_h);
	vector_bitmap = new_bitmap(vector_w, vector_h);
}

void VideoScopeWindow::draw_overlays()
{
	set_color(GREEN);
	set_font(SMALLFONT);

	for(int i = 0; i <= 10; i++)
	{
		int y = wave_h * i / 10;
		int text_y = y + wave_y + 10;
		int x = wave_x - 20;
		char string[BCTEXTLEN];
		sprintf(string, "%d", 100 - i * 10);
		draw_text(x, text_y, string);
		
		if(i > 0) waveform->draw_line(0, y, wave_w, y);
	}

#define DIVISIONS 5
	int radius = MIN(vector_w / 2, vector_h / 2);
	for(int i = 0; i <= DIVISIONS; i++)
	{
		int x = vector_w / 2 - radius * i / DIVISIONS;
		int y = vector_h / 2 - radius * i / DIVISIONS;
		int text_y = y + 20;
		int w = radius * i / DIVISIONS * 2;
		int h = radius * i / DIVISIONS * 2;
		char string[BCTEXTLEN];
		
		sprintf(string, "%d", i * 100 / DIVISIONS);
		draw_text(vector_x - 20, text_y, string);
		if(i > 0) vectorscope->draw_circle(x, y, w, h);
	}
	set_font(MEDIUMFONT);

	waveform->flash();
	vectorscope->flash();
	flush();
}











VideoScopeThread::VideoScopeThread(VideoScopeEffect *plugin)
 : Thread()
{
	this->plugin = plugin;
	set_synchronous(0);
	completion.lock();
}

VideoScopeThread::~VideoScopeThread()
{
	delete window;
}
	
void VideoScopeThread::run()
{
	BC_DisplayInfo info;
	window = new VideoScopeWindow(plugin, 
		info.get_abs_cursor_x() - 75, 
		info.get_abs_cursor_y() - 65,
		plugin->w,
		plugin->h);
	window->create_objects();

	int result = window->run_window();
	completion.unlock();
	if(result) plugin->client_side_close();
}




REGISTER_PLUGIN(VideoScopeEffect)





VideoScopeWindow* VideoScopeEffect::window_static = 0;
Mutex VideoScopeEffect::window_lock;

VideoScopeEffect::VideoScopeEffect(PluginServer *server)
 : PluginVClient(server)
{
	engine = 0;
	w = 640;
	h = 260;
	PLUGIN_CONSTRUCTOR_MACRO
}

VideoScopeEffect::~VideoScopeEffect()
{
	PLUGIN_DESTRUCTOR_MACRO

	if(engine) delete engine;
}


int VideoScopeEffect::is_realtime()
{
	return 1;
}

char* VideoScopeEffect::plugin_title()
{
	return "VideoScope";
}

NEW_PICON_MACRO(VideoScopeEffect)

int VideoScopeEffect::show_gui()
{
	thread = new VideoScopeThread(this);
	thread->start();
	return 0;
}

RAISE_WINDOW_MACRO(VideoScopeEffect)

SET_STRING_MACRO(VideoScopeEffect)

int VideoScopeEffect::load_defaults()
{
	char directory[BCTEXTLEN];
// set the default directory
	sprintf(directory, "%svideoscope.rc", BCASTDIR);

// load the defaults
	defaults = new Defaults(directory);
	defaults->load();

	w = defaults->get("W", w);
	h = defaults->get("H", h);
	return 0;
}

int VideoScopeEffect::save_defaults()
{
	defaults->update("W", w);
	defaults->update("H", h);
	defaults->save();
	return 0;
}

int VideoScopeEffect::process_realtime(VFrame *input, VFrame *output)
{
	window_lock.lock();

	if(window_static)
	{
//printf("VideoScopeEffect::process_realtime 1\n");
		this->input = input;
//printf("VideoScopeEffect::process_realtime 1\n");

		if(input->get_rows()[0] != output->get_rows()[0])
			output->copy_from(input);
//printf("VideoScopeEffect::process_realtime 1\n");

		if(!engine)
		{
			engine = new VideoScopeEngine(this, 
				(PluginClient::smp + 1));
		}
// printf("VideoScopeEffect::process_realtime 1 %p\n", window_static);
// printf("VideoScopeEffect::process_realtime 1 %p\n", window_static->waveform_bitmap);
// printf("VideoScopeEffect::process_realtime 1 %p %p %p\n", window_static->waveform_bitmap->get_data(), window_static->waveform_bitmap->get_h(), window_static->waveform_bitmap->get_bytes_per_line());

// Clear bitmaps
		bzero(window_static->waveform_bitmap->get_data(), 
			window_static->waveform_bitmap->get_h() * 
			window_static->waveform_bitmap->get_bytes_per_line());
		bzero(window_static->vector_bitmap->get_data(), 
			window_static->vector_bitmap->get_h() * 
			window_static->vector_bitmap->get_bytes_per_line());
//printf("VideoScopeEffect::process_realtime 1\n");

		engine->process_packages();
//printf("VideoScopeEffect::process_realtime 1\n");

		window_static->lock_window();
		window_static->waveform->draw_bitmap(window_static->waveform_bitmap, 
			1,
			0,
			0);
//printf("VideoScopeEffect::process_realtime 1\n");
		window_static->vectorscope->draw_bitmap(window_static->vector_bitmap, 
			1,
			0,
			0);
//printf("VideoScopeEffect::process_realtime 1\n");


		window_static->draw_overlays();








		window_static->unlock_window();
//printf("VideoScopeEffect::process_realtime 2\n");
	}

	window_lock.unlock();


	return 0;
}






VideoScopePackage::VideoScopePackage()
 : LoadPackage()
{
}






VideoScopeUnit::VideoScopeUnit(VideoScopeEffect *plugin, 
	VideoScopeEngine *server)
 : LoadClient(server)
{
	this->plugin = plugin;
}


#define INTENSITY(p) ((unsigned int)(((p)[0]) * 77+ \
									((p)[1] * 150) + \
									((p)[2] * 29)) >> 8)


static void draw_point(unsigned char **rows, int color_model, int x, int y)
{
	switch(color_model)
	{
		case BC_BGR8888:
		{
			unsigned char *pixel = rows[y] + x * 4;
			pixel[0] = 0xff;
			pixel[1] = 0xff;
			pixel[2] = 0xff;
			break;
		}
		case BC_BGR888:
			break;
		case BC_RGB565:
		{
			unsigned char *pixel = rows[y] + x * 2;
			pixel[0] = pixel[1] = 0xff;
			break;
		}
		case BC_BGR565:
			break;
		case BC_RGB8:
			break;
	}
}



#define VIDEOSCOPE(type, max, components, use_yuv) \
{ \
	for(int i = pkg->row1; i < pkg->row2; i++) \
	{ \
		type *in_row = (type*)plugin->input->get_rows()[i]; \
		for(int j = 0; j < w; j++) \
		{ \
			type *in_pixel = in_row + j * components; \
			float intensity; \
 \
/* Analyze pixel */ \
			if(use_yuv) intensity = (float)*in_pixel / max; \
 \
			float r, g, b, h, s, v; \
			if(use_yuv) \
			{ \
				HSV::yuv_to_hsv(in_pixel[0], in_pixel[1], in_pixel[2], h, s, v, max); \
			} \
			else \
			{ \
				r = (float)in_pixel[0] / max; \
				g = (float)in_pixel[1] / max; \
				b = (float)in_pixel[2] / max; \
				HSV::rgb_to_hsv(r, g, b, h, s, v); \
			} \
 \
/* Calculate waveform */ \
			if(!use_yuv) intensity = v; \
			intensity = intensity * \
				waveform_h; \
			int y = waveform_h - (int)intensity; \
			int x = j * waveform_w / w; \
			if(x >= 0 && x < waveform_w && y >= 0 && y < waveform_h) \
				draw_point(waveform_rows, waveform_cmodel, x, y); \
 \
/* Calculate vectorscope */ \
			float adjacent = cos(h / 360 * 2 * M_PI); \
			float opposite = sin(h / 360 * 2 * M_PI); \
			x = (int)(vector_w / 2 +  \
				adjacent * s * radius); \
 \
			y = (int)(vector_h / 2 -  \
				opposite * s * radius); \
 \
 \
			CLAMP(x, 0, vector_w - 1); \
			CLAMP(y, 0, vector_h - 1); \
			draw_point(vector_rows, vector_cmodel, x, y); \
 \
		} \
	} \
}

void VideoScopeUnit::process_package(LoadPackage *package)
{
	VideoScopePackage *pkg = (VideoScopePackage*)package;
	int w = plugin->input->get_w();
	int h = plugin->input->get_h();
	int waveform_h = VideoScopeEffect::window_static->waveform_bitmap->get_h();
	int waveform_w = VideoScopeEffect::window_static->waveform_bitmap->get_w();
	int waveform_cmodel = VideoScopeEffect::window_static->waveform_bitmap->get_color_model();
	unsigned char **waveform_rows = VideoScopeEffect::window_static->waveform_bitmap->get_row_pointers();
	int vector_h = VideoScopeEffect::window_static->vector_bitmap->get_h();
	int vector_w = VideoScopeEffect::window_static->vector_bitmap->get_w();
	int vector_cmodel = VideoScopeEffect::window_static->vector_bitmap->get_color_model();
	unsigned char **vector_rows = VideoScopeEffect::window_static->vector_bitmap->get_row_pointers();
	float radius = MIN(vector_w / 2, vector_h / 2);

	switch(plugin->input->get_color_model())
	{
		case BC_RGB888:
			VIDEOSCOPE(unsigned char, 0xff, 3, 0)
			break;

		case BC_YUV888:
			VIDEOSCOPE(unsigned char, 0xff, 3, 1)
			break;

		case BC_RGB161616:
			VIDEOSCOPE(uint16_t, 0xffff, 3, 0)
			break;

		case BC_YUV161616:
			VIDEOSCOPE(uint16_t, 0xffff, 3, 1)
			break;

		case BC_RGBA8888:
			VIDEOSCOPE(unsigned char, 0xff, 4, 0)
			break;

		case BC_YUVA8888:
			VIDEOSCOPE(unsigned char, 0xff, 4, 1)
			break;

		case BC_RGBA16161616:
			VIDEOSCOPE(uint16_t, 0xffff, 4, 0)
			break;

		case BC_YUVA16161616:
			VIDEOSCOPE(uint16_t, 0xffff, 4, 1)
			break;
	}
}






VideoScopeEngine::VideoScopeEngine(VideoScopeEffect *plugin, int cpus)
 : LoadServer(cpus, cpus)
{
	this->plugin = plugin;
}

VideoScopeEngine::~VideoScopeEngine()
{
}

void VideoScopeEngine::init_packages()
{
	int increment = plugin->input->get_h() / LoadServer::total_packages + 1;
	int y = 0;
	for(int i = 0; i < LoadServer::total_packages; i++)
	{
		VideoScopePackage *pkg = (VideoScopePackage*)packages[i];
		pkg->row1 = y;
		pkg->row2 = y + increment;
		y += increment;
		if(pkg->row2 > plugin->input->get_h())
		{
			y = pkg->row2 = plugin->input->get_h();
		}
	}
}


LoadClient* VideoScopeEngine::new_client()
{
	return new VideoScopeUnit(plugin, this);
}

LoadPackage* VideoScopeEngine::new_package()
{
	return new VideoScopePackage;
}

