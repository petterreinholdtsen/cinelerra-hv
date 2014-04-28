#include "bcdisplayinfo.h"
#include "clip.h"
#include "defaults.h"
#include "delayvideo.h"
#include "filexml.h"
#include "picon_png.h"
#include "vframe.h"




#include <string.h>







PluginClient* new_plugin(PluginServer *server)
{
	return new DelayVideo(server);
}







DelayVideoConfig::DelayVideoConfig()
{
	length = 0;
}

int DelayVideoConfig::equivalent(DelayVideoConfig &that)
{
	return EQUIV(length, that.length);
}

void DelayVideoConfig::copy_from(DelayVideoConfig &that)
{
	length = that.length;
}





DelayVideoWindow::DelayVideoWindow(DelayVideo *plugin, int x, int y)
 : BC_Window(plugin->gui_string, 
	x,
	y,
	210, 
	120, 
	210, 
	120, 
	0, 
	0,
	1)
{
	this->plugin = plugin;
}

DelayVideoWindow::~DelayVideoWindow()
{
}

	
void DelayVideoWindow::create_objects()
{
	int x = 10, y = 10;
	
	add_subwindow(new BC_Title(x, y, "Delay seconds:"));
	y += 20;
	add_subwindow(slider = new DelayVideoSlider(plugin, x, y));
	update_gui();

	show_window();
	flush();
}

int DelayVideoWindow::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}

void DelayVideoWindow::update_gui()
{
	char string[BCTEXTLEN];
	sprintf(string, "%.04f", plugin->config.length);
	slider->update(string);
}












DelayVideoSlider::DelayVideoSlider(DelayVideo *plugin, int x, int y)
 : BC_TextBox(x, y, 150, 1, "")
{
	this->plugin = plugin;
}

int DelayVideoSlider::handle_event()
{
	plugin->config.length = atof(get_text());
	plugin->send_configure_change();
	return 1;
}













DelayVideoThread::DelayVideoThread(DelayVideo *plugin)
 : Thread()
{
	this->plugin = plugin;
	set_synchronous(0);
	completion.lock();
}


DelayVideoThread::~DelayVideoThread()
{
// Window only deleted here
	delete window;
}

	
void DelayVideoThread::run()
{
	BC_DisplayInfo info;
	window = new DelayVideoWindow(plugin, 
		info.get_abs_cursor_x() - 105, 
		info.get_abs_cursor_y() - 60);
	window->create_objects();
	int result = window->run_window();
	completion.unlock();
// Last command executed in thread
	if(result) plugin->client_side_close();
}











DelayVideo::DelayVideo(PluginServer *server)
 : PluginVClient(server)
{
	reset();
	load_defaults();
}

DelayVideo::~DelayVideo()
{
//printf("DelayVideo::~DelayVideo 1\n");
	if(thread)
	{
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}
//printf("DelayVideo::~DelayVideo 1\n");

	if(buffer)
	{
//printf("DelayVideo::~DelayVideo 1\n");
		for(int i = 0; i < allocation; i++)
			delete buffer[i];
//printf("DelayVideo::~DelayVideo 1\n");
	
		delete [] buffer;
//printf("DelayVideo::~DelayVideo 1\n");
	}

	save_defaults();
//printf("DelayVideo::~DelayVideo 1\n");
	delete defaults;
//printf("DelayVideo::~DelayVideo 2\n");
}

void DelayVideo::reset()
{
	thread = 0;
	defaults = 0;
	need_reconfigure = 1;
	buffer = 0;
	allocation = 0;
}

void DelayVideo::reconfigure()
{
	int new_allocation = 1 + (int)(config.length * PluginVClient::project_frame_rate);
	VFrame **new_buffer = new VFrame*[new_allocation];
	int reuse = MIN(new_allocation, allocation);

	for(int i = 0; i < reuse; i++)
	{
		new_buffer[i] = buffer[i];
	}

	for(int i = reuse; i < new_allocation; i++)
	{
		new_buffer[i] = new VFrame(0, 
			input->get_w(),
			input->get_h(),
			PluginVClient::project_color_model);
	}

	for(int i = reuse; i < allocation; i++)
	{
		delete buffer[i];
	}

	if(buffer) delete [] buffer;
	
	
	buffer = new_buffer;
	allocation = new_allocation;
	
	need_reconfigure = 0;
}

int DelayVideo::process_realtime(VFrame *input_ptr, VFrame *output_ptr)
{
	this->input = input_ptr;
	this->output = output_ptr;
	load_configuration();
	if(need_reconfigure) reconfigure();
	
	buffer[allocation - 1]->copy_from(input_ptr);
	output_ptr->copy_from(buffer[0]);
	
	VFrame *temp = buffer[0];
	for(int i = 0; i < allocation -1; i++)
	{
		buffer[i] = buffer[i + 1];
	}

	buffer[allocation - 1] = temp;
	
	
	return 0;
}

int DelayVideo::is_realtime()
{
	return 1;
}

char* DelayVideo::plugin_title()
{
	return "Delay Video";
}

int DelayVideo::show_gui()
{
	load_configuration();
	
	thread = new DelayVideoThread(this);
	thread->start();
	return 0;
}

void DelayVideo::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}

int DelayVideo::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}

void DelayVideo::save_data(KeyFrame *keyframe)
{
	FileXML output;
	output.set_shared_string(keyframe->data, MESSAGESIZE);

	output.tag.set_title("DELAYVIDEO");
	output.tag.set_property("LENGTH", (double)config.length);
	output.append_tag();
	output.append_newline();
	output.terminate_string();
}

void DelayVideo::read_data(KeyFrame *keyframe)
{
	FileXML input;
	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;
	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("DELAYVIDEO"))
			{
				config.length = input.tag.get_property("LENGTH", (double)config.length);
			}
		}
	}
}

VFrame* DelayVideo::new_picon()
{
	return new VFrame(picon_png);
}

void DelayVideo::update_gui()
{
	if(thread) 
	{
		load_configuration();
		thread->window->lock_window();
		thread->window->update_gui();
		thread->window->unlock_window();
	}
}



void DelayVideo::load_configuration()
{
	KeyFrame *prev_keyframe;
	prev_keyframe = get_prev_keyframe(get_source_position());
	
	DelayVideoConfig old_config;
	old_config.copy_from(config);
 	read_data(prev_keyframe);

 	if(!old_config.equivalent(config))
 	{
		need_reconfigure = 1;
	}
}

int DelayVideo::load_defaults()
{
	char directory[BCTEXTLEN];
	sprintf(directory, "%sdelayvideo.rc", BCASTDIR);
	defaults = new Defaults(directory);
	defaults->load();
	config.length = defaults->get("LENGTH", (double)1);
	return 0;
}

int DelayVideo::save_defaults()
{
	defaults->update("LENGTH", config.length);
	defaults->save();
	return 0;
}



