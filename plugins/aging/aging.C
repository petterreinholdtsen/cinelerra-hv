#include "clip.h"
#include "colormodels.h"
#include "filexml.h"
#include "picon_png.h"
#include "aging.h"
#include "agingwindow.h"
#include "effecttv.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

PluginClient* new_plugin(PluginServer *server)
{
	return new AgingMain(server);
}







int AgingConfig::dx[] = { 1, 1, 0, -1, -1, -1,  0, 1};
int AgingConfig::dy[] = { 0, -1, -1, -1, 0, 1, 1, 1};

AgingConfig::AgingConfig()
{
	dust_interval = 0;
	pits_interval = 0;
	aging_mode = 0;
	area_scale = 10;
	scratch_lines = 7;
	colorage = 1;
	scratch = 1;
	pits = 1;
	dust = 1;
}

AgingMain::AgingMain(PluginServer *server)
 : PluginVClient(server)
{
	thread = 0;
	defaults = 0;
	load_defaults();
}

AgingMain::~AgingMain()
{
	if(thread)
	{
// Set result to 0 to indicate a server side close
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}

	save_defaults();
	if(defaults) delete defaults;
}

char* AgingMain::plugin_title() { return "AgingTV"; }
int AgingMain::is_realtime() { return 1; }

VFrame* AgingMain::new_picon()
{
	return new VFrame(picon_png);
}

int AgingMain::load_defaults()
{
	return 0;
}

int AgingMain::save_defaults()
{
	return 0;
}

void AgingMain::load_configuration()
{
}


void AgingMain::save_data(KeyFrame *keyframe)
{
}

void AgingMain::read_data(KeyFrame *keyframe)
{
}


int AgingMain::start_realtime()
{
	aging_server = new AgingServer(this, 1, 1);
	return 0;
}

int AgingMain::stop_realtime()
{
	delete aging_server;
	return 0;
}

int AgingMain::process_realtime(VFrame *input_ptr, VFrame *output_ptr)
{
	load_configuration();
	this->input_ptr = input_ptr;
	this->output_ptr = output_ptr;


	aging_server->process_packages();

	return 0;
}

int AgingMain::show_gui()
{
	load_configuration();
	thread = new AgingThread(this);
	thread->start();
	return 0;
}

int AgingMain::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void AgingMain::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}




AgingServer::AgingServer(AgingMain *plugin, int total_clients, int total_packages)
 : LoadServer(total_clients, total_packages)
{
	this->plugin = plugin;
}


LoadClient* AgingServer::new_client() 
{
	return new AgingClient(this);
}




LoadPackage* AgingServer::new_package() 
{ 
	return new AgingPackage; 
}



void AgingServer::init_packages()
{
	for(int i = 0; i < total_packages; i++)
	{
		AgingPackage *package = (AgingPackage*)packages[i];
		package->row1 = plugin->input_ptr->get_h() / total_packages * i;
		package->row2 = package->row1 + plugin->input_ptr->get_h() / total_packages;
		if(i >= total_packages - 1)
			package->row2 = plugin->input_ptr->get_h();
	}
}








AgingClient::AgingClient(AgingServer *server)
 : LoadClient(server)
{
	this->plugin = server->plugin;
}









#define COLORAGE(type, components) \
{ \
	type a, b; \
	int i, j, k; \
 \
	for(i = 0; i < h; i++) \
	{ \
		for(j = 0; j < w; j++) \
		{ \
			for(k = 0; k < components; k++) \
			{ \
				a = ((type**)input_rows)[i][j * components + k]; \
 \
				if(k < 3) \
				{ \
					if(sizeof(type) == 2) \
					{ \
						b = (a & 0xffff) >> 2; \
						((type**)output_rows)[i][j * components + k] = \
							a - b + 0x1800 + (EffectTV::fastrand() & 0x1000); \
					} \
					else \
					{ \
						b = (a & 0xff) >> 2; \
						((type**)output_rows)[i][j * components + k] =  \
							a - b + 0x18 + ((EffectTV::fastrand() >> 8) & 0x10); \
					} \
				} \
				else \
				{ \
					((type**)output_rows)[i][j * components + k] = a; \
				} \
			} \
		} \
	} \
}

void AgingClient::coloraging(unsigned char **output_rows, 
	unsigned char **input_rows,
	int color_model,
	int w,
	int h)
{
	switch(color_model)
	{
		case BC_RGB888:
		case BC_YUV888:
			COLORAGE(uint8_t, 3);
			break;
		
		case BC_RGBA8888:
		case BC_YUVA8888:
			COLORAGE(uint8_t, 4);
			break;
		
		case BC_RGB161616:
		case BC_YUV161616:
			COLORAGE(uint16_t, 3);
			break;
		
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			COLORAGE(uint16_t, 4);
			break;
	}
}






#define SCRATCHES(type, components) \
{ \
	int i, j, y, y1, y2; \
	type *p, a, b; \
 \
	for(i = 0; i < plugin->config.scratch_lines; i++) \
	{ \
		if(plugin->config.scratches[i].life)  \
		{ \
			plugin->config.scratches[i].x = plugin->config.scratches[i].x + plugin->config.scratches[i].dx; \
			if(plugin->config.scratches[i].x < 0 || plugin->config.scratches[i].x > w * 256)  \
			{ \
				plugin->config.scratches[i].life = 0; \
				break; \
			} \
\
			p = (type*)output_rows[0] + \
				(plugin->config.scratches[i].x >> 8) * \
				components + j; \
\
			if(plugin->config.scratches[i].init)  \
			{ \
				y1 = plugin->config.scratches[i].init; \
				plugin->config.scratches[i].init = 0; \
			}  \
			else  \
			{ \
				y1 = 0; \
			} \
\
			plugin->config.scratches[i].life--; \
			if(plugin->config.scratches[i].life)  \
			{ \
				y2 = h; \
			}  \
			else  \
			{ \
				y2 = EffectTV::fastrand() % h; \
			} \
 \
			for(y = y1; y < y2; y++)  \
			{ \
				for(j = 0; j < 3; j++) \
				{ \
					if(sizeof(type) == 2) \
					{ \
						a = p[j] & 0xfeff; \
						a += 0x2000; \
						b = a & 0x10000; \
						p[j] = a | (b - (b >> 8)); \
					} \
					else \
					{ \
						a = p[j] & 0xfe; \
						a += 0x20; \
						b = a & 0x100; \
						p[j] = a | (b - (b >> 8)); \
					} \
				} \
 \
				p += w * components; \
			} \
		}  \
		else  \
		{ \
			if((EffectTV::fastrand() & 0xf0000000) == 0)  \
			{ \
				plugin->config.scratches[i].life = 2 + (EffectTV::fastrand() >> 27); \
				plugin->config.scratches[i].x = EffectTV::fastrand() % (w * 256); \
				plugin->config.scratches[i].dx = ((int)EffectTV::fastrand()) >> 23; \
				plugin->config.scratches[i].init = (EffectTV::fastrand() % (h - 1)) + 1; \
			} \
		} \
	} \
}



void AgingClient::scratching(unsigned char **output_rows,
	int color_model,
	int w,
	int h)
{
	switch(color_model)
	{
		case BC_RGB888:
		case BC_YUV888:
			SCRATCHES(uint8_t, 3);
			break;
		
		case BC_RGBA8888:
		case BC_YUVA8888:
			SCRATCHES(uint8_t, 4);
			break;
		
		case BC_RGB161616:
		case BC_YUV161616:
			SCRATCHES(uint16_t, 3);
			break;
		
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			SCRATCHES(uint16_t, 4);
			break;
	}
}



#define PITS(type, components) \
{ \
	int i, j, k; \
	int pnum, size, pnumscale; \
	int x, y; \
 \
	pnumscale = plugin->config.area_scale * 2; \
 \
	if(plugin->config.pits_interval)  \
	{ \
		pnum = pnumscale + (EffectTV::fastrand() % pnumscale); \
		plugin->config.pits_interval--; \
	}  \
	else \
	{ \
		pnum = EffectTV::fastrand() % pnumscale; \
		if((EffectTV::fastrand() & 0xf8000000) == 0)  \
		{ \
			plugin->config.pits_interval = (EffectTV::fastrand() >> 28) + 20; \
		} \
	} \
 \
	for(i = 0; i < pnum; i++)  \
	{ \
		x = EffectTV::fastrand() % (w - 1); \
		y = EffectTV::fastrand() % (h - 1); \
 \
		size = EffectTV::fastrand() >> 28; \
 \
		for(j = 0; j < size; j++)  \
		{ \
			x = x + EffectTV::fastrand() % 3 - 1; \
			y = y + EffectTV::fastrand() % 3 - 1; \
 \
			CLAMP(x, 0, w - 1); \
			CLAMP(y, 0, h - 1); \
			for(k = 0; k < 3; k++) \
				((type**)output_rows)[y][x * components + k] = 0xc0; \
		} \
	} \
}






void AgingClient::pits(unsigned char **output_rows,
	int color_model,
	int w,
	int h)
{
	switch(color_model)
	{
		case BC_RGB888:
		case BC_YUV888:
			PITS(uint8_t, 3);
			break;
		
		case BC_RGBA8888:
		case BC_YUVA8888:
			PITS(uint8_t, 4);
			break;
		
		case BC_RGB161616:
		case BC_YUV161616:
			PITS(uint16_t, 3);
			break;
		
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			PITS(uint16_t, 4);
			break;
	}
}


#define DUSTS(type, components) \
{ \
	int i, j, k; \
	int dnum; \
	int d, len; \
	int x, y; \
 \
	if(plugin->config.dust_interval == 0)  \
	{ \
		if((EffectTV::fastrand() & 0xf0000000) == 0)  \
		{ \
			plugin->config.dust_interval = EffectTV::fastrand() >> 29; \
		} \
		return; \
	} \
 \
	dnum = plugin->config.area_scale * 4 + (EffectTV::fastrand() >> 27); \
 \
	for(i = 0; i < dnum; i++)  \
	{ \
		x = EffectTV::fastrand() % w; \
		y = EffectTV::fastrand() % h; \
		d = EffectTV::fastrand() >> 29; \
		len = EffectTV::fastrand() % plugin->config.area_scale + 5; \
 \
		for(j = 0; j < len; j++)  \
		{ \
			CLAMP(x, 0, w - 1); \
			CLAMP(y, 0, h - 1); \
			for(k = 0; k < 3; k++) \
				((type**)output_rows)[y][x * components + k] = 0x10; \
 \
			y += AgingConfig::dy[d]; \
			x += AgingConfig::dx[d]; \
 \
			if(x < 0 || x >= w) break; \
			if(y < 0 || y >= h) break; \
 \
 \
			d = (d + EffectTV::fastrand() % 3 - 1) & 7; \
		} \
	} \
	plugin->config.dust_interval--; \
}




void AgingClient::dusts(unsigned char **output_rows,
	int color_model,
	int w,
	int h)
{
	switch(color_model)
	{
		case BC_RGB888:
		case BC_YUV888:
			DUSTS(uint8_t, 3);
			break;
		
		case BC_RGBA8888:
		case BC_YUVA8888:
			DUSTS(uint8_t, 4);
			break;
		
		case BC_RGB161616:
		case BC_YUV161616:
			DUSTS(uint16_t, 3);
			break;
		
		case BC_RGBA16161616:
		case BC_YUVA16161616:
			DUSTS(uint16_t, 4);
			break;
	}
}



void AgingClient::process_package(LoadPackage *package)
{
	AgingPackage *local_package = (AgingPackage*)package;
	unsigned char **input_rows = plugin->input_ptr->get_rows() + local_package->row1;
	unsigned char **output_rows = plugin->output_ptr->get_rows() + local_package->row1;


//printf("AgingClient::process_package 1\n");
	if(plugin->config.colorage)
		coloraging(output_rows, 
			input_rows, 
			plugin->input_ptr->get_color_model(), 
			plugin->input_ptr->get_w(), 
			local_package->row2 - local_package->row1);
//printf("AgingClient::process_package 2\n");
	if(plugin->config.scratch)
		scratching(output_rows, 
			plugin->input_ptr->get_color_model(), 
			plugin->input_ptr->get_w(), 
			local_package->row2 - local_package->row1);
//printf("AgingClient::process_package 3\n");
	if(plugin->config.pits)
		pits(output_rows, 
			plugin->input_ptr->get_color_model(), 
			plugin->input_ptr->get_w(), 
			local_package->row2 - local_package->row1);
//printf("AgingClient::process_package 4 %d\n", plugin->config.dust);
	if(plugin->config.dust)
		dusts(output_rows, 
			plugin->input_ptr->get_color_model(), 
			plugin->input_ptr->get_w(), 
			local_package->row2 - local_package->row1);
//printf("AgingClient::process_package 5\n");
}



AgingPackage::AgingPackage()
{
}



