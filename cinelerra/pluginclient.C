#include "edl.h"
#include "edlsession.h"
#include "pluginclient.h"
#include "pluginserver.h"

#include <string.h>

PluginClient::PluginClient(PluginServer *server)
{
	reset();
	this->server = server;
}

PluginClient::~PluginClient()
{
}

int PluginClient::reset()
{
	interactive = 0;
	gui_messages = 0;
	show_initially = 0;
	message_lock = 0;
	wr = rd = 0;
	master_gui_on = 0;
	client_gui_on = 0;
	sample_rate = 0;
	frame_rate = 0;
	realtime_priority = 0;
	gui_string[0] = 0;
	total_in_buffers = 0;
	total_out_buffers = 0;
	source_len = 0;
	source_position = 0;
	total_len = 0;
}

int PluginClient::plugin_init(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("This is a plugin for Broadcast 2000a.  Grow a mane.\n");
		success = 0;
		return 1;
	}

// get the message pipes
	messages = new Messages(MESSAGE_TO_PLUGIN, MESSAGE_FROM_PLUGIN, atol(argv[1]));

	if(atol(argv[2]) >= 0)
	{
		gui_messages = new Messages(MESSAGE_TO_PLUGIN, MESSAGE_FROM_PLUGIN, atol(argv[2]));
		gui_messages->write_message(GET_STRING);
		gui_messages->read_message(gui_string);

		show_initially = 1;       // flag for plugin
		client_gui_on = 1;
	}
	success = 1;

	return 0;
}

int PluginClient::plugin_get_range()
{
	messages->read_message(&start, &end);
}

// For non realtime plugins, allocate all the buffers.
int PluginClient::plugin_negotiate_buffers()
{
	long recommended_size;
	long total_channels;

	messages->read_message(&total_channels, &recommended_size);
	total_in_buffers = total_out_buffers = total_channels;

// get desired sizes from user
	in_buffer_size = get_in_buffers(recommended_size);
	out_buffer_size = get_out_buffers(recommended_size);
// notify server of desired sizes
	messages->write_message(out_buffer_size, in_buffer_size);

// get shared buffers from server
	data_in = new PluginBuffer*[total_in_buffers];
	data_out = new PluginBuffer*[total_out_buffers];

	long mem_id, size;
// Sizes recieved from server are now byte counts.
	for(int i = 0; i < total_in_buffers; i++)
	{
		messages->read_message(&mem_id, &size);
		data_in[i] = new PluginBuffer(mem_id, size, 1);
	}

	for(int i = 0; i < total_out_buffers; i++)
	{
		messages->read_message(&mem_id, &size);
		data_out[i] = new PluginBuffer(mem_id, size, 1);
	}
	return 0;
}


// For realtime plugins initialize buffers
int PluginClient::plugin_init_realtime(int realtime_priority, 
	int total_in_buffers)
{
//printf("PluginClient::plugin_init_realtime 1\n");
// Get parameters for all
	master_gui_on = get_gui_status();

//printf("PluginClient::plugin_init_realtime 1\n");
// get parameters depending on video or audio
	init_realtime_parameters();
//printf("PluginClient::plugin_init_realtime 1\n");
	smp = server->edl->session->smp;
	this->realtime_priority = realtime_priority;
	this->total_in_buffers = total_in_buffers;
//printf("PluginClient::plugin_init_realtime 1\n");

// derived initialization
	start_realtime();
//printf("PluginClient::plugin_init_realtime 2\n");
	return 0;
}

int PluginClient::plugin_start_loop(long start, long end, long buffer_size, int total_buffers)
{
	this->start = start;
	this->end = end;
	this->in_buffer_size = this->out_buffer_size = buffer_size;
	this->total_in_buffers = this->total_out_buffers = total_buffers;
	sample_rate = get_project_samplerate();
	frame_rate = get_project_framerate();
	start_loop();
	return 0;
}

int PluginClient::plugin_process_loop()
{
	return process_loop();
}

int PluginClient::plugin_stop_loop()
{
	return stop_loop();
}

MainProgressBar* PluginClient::start_progress(char *string, long length)
{
	return server->start_progress(string, length);
}



int PluginClient::plugin_stop_realtime()
{
// give to user
	stop_realtime();
	return 0;
}

int PluginClient::plugin_delete_buffers()
{
	int i, j;
// used in plugin that also runs the GUI
	delete_buffer_ptrs();
	if(total_in_buffers)
	{
		for(i = 0; i < total_in_buffers; i++)
		{
			for(j = 0; j < double_buffers_in.values[i]; j++)
			{
				delete data_in_realtime.values[i][j];
			}
			delete data_in_realtime.values[i];
		}
		offset_in_render.remove_all();
		double_buffer_in_render.remove_all();
		data_in_realtime.remove_all();
		double_buffers_in.remove_all();
		realtime_in_size.remove_all();
	}
	
	if(total_out_buffers)
	{
		for(i = 0; i < total_out_buffers; i++)
		{
			for(j = 0; j < double_buffers_out.values[i]; j++)
			{
				delete data_out_realtime.values[i][j];
			}
			delete data_out_realtime.values[i];
		}
		offset_out_render.remove_all();
		double_buffer_out_render.remove_all();
		data_out_realtime.remove_all();
		double_buffers_out.remove_all();
		realtime_out_size.remove_all();
	}

	total_in_buffers = 0;
	total_out_buffers = 0;
	return 0;
}

int PluginClient::plugin_get_parameters()
{
	sample_rate = get_project_samplerate();
	frame_rate = get_project_framerate();
	int result = get_parameters();
	return result;
}

// ========================= main loop
int PluginClient::plugin_run()
{
	if(!success) return 1;

	int command;
	int done = 0;

//	if(client_gui_on)
//		start_gui();         // Start the GUI

	while(!done)
	{
		command = messages->read_message();

		switch(command)
		{
			case -1:                    done = 1;                                                 break;
//			case START_REALTIME:        plugin_init_realtime();                                   break;
			case STOP_REALTIME:         plugin_stop_realtime();                                   break;
//			case GET_REALTIME:          messages->write_message(plugin_is_realtime());            break;
//			case GET_MULTICHANNEL:      messages->write_message(plugin_is_multi_channel());       break;
//			case GET_FILEIO:            messages->write_message(plugin_is_fileio());              break;
//			case GET_TITLE:             messages->write_message(plugin_title());                  break;
//			case EXIT_PLUGIN:           plugin_exit(); done = 1;                                  break;
			case LOAD_DEFAULTS:         load_defaults();                                          break;
			case SAVE_DEFAULTS:         save_defaults();                                          break;
			case STOP_GUI:              stop_gui_client();                                        break;
//			case HIDE_GUI:              hide_gui(); 	                                          break;
			case GET_PARAMETERS:        plugin_get_parameters();                                  break;
//			case GET_SAMPLERATE:        messages->write_message(get_plugin_samplerate());         break;
//			case GET_FRAMERATE:         messages->write_message((long)(get_plugin_framerate() * 1000));break;
			case SET_INTERACTIVE:       interactive = 1;                                          break;
			case SET_RANGE:             plugin_get_range();                                       break;
			case GET_BUFFERS:           plugin_negotiate_buffers();                               break;
			case START_PLUGIN:          plugin_start_plugin();                                    break;
			case SAVE_DATA:             save_data_client();                                       break;
			case LOAD_DATA:             load_data_client();                                       break;
			case CONFIGURE_CHANGE:      get_configure_change();                                   break;
			case GUI_ON:                master_gui_on = 1;                                        break;
			case GUI_OFF:               master_gui_on = 0;                                        break;
			default:                    plugin_command_derived(command);                          break;
		}
	}
	return 0;
}

int PluginClient::start_realtime() { return 0; }
int PluginClient::stop_realtime() { return 0; }
int PluginClient::is_multichannel() { return 0; }
int PluginClient::is_synthesis() { return 0; }
int PluginClient::is_realtime() { return 0; }
int PluginClient::is_fileio() { return 0; }
int PluginClient::delete_buffer_ptrs() { return 0; }
char* PluginClient::plugin_title() { return "Untitled"; }
VFrame* PluginClient::new_picon() { return 0; }
Theme* PluginClient::new_theme() { return 0; }
int PluginClient::is_audio() { return 0; }
int PluginClient::is_video() { return 0; }
int PluginClient::is_theme() { return 0; }
int PluginClient::uses_gui() { return 1; }
int PluginClient::is_transition() { return 0; }
int PluginClient::load_defaults() { return 0; }
int PluginClient::save_defaults() { return 0; }
//int PluginClient::start_gui() { return 0; }
//int PluginClient::stop_gui() { return 0; }
int PluginClient::show_gui() { return 0; }
//int PluginClient::hide_gui() { return 0; }
int PluginClient::set_string() { return 0; }
int PluginClient::get_parameters() { return 0; }
int PluginClient::get_samplerate() { return get_project_samplerate(); }
double PluginClient::get_framerate() { return get_project_framerate(); }
int PluginClient::init_realtime_parameters() { return 0; }
int PluginClient::delete_nonrealtime_parameters() { return 0; }
int PluginClient::start_loop() { return 0; };
int PluginClient::process_loop() { return 0; };
int PluginClient::stop_loop() { return 0; };

void PluginClient::set_interactive()
{
	interactive = 1;
}

long PluginClient::get_in_buffers(long recommended_size)
{
	return recommended_size;
}

long PluginClient::get_out_buffers(long recommended_size)
{
	return recommended_size;
}

int PluginClient::get_gui_status()
{
	return server->get_gui_status();
}

int PluginClient::plugin_start_plugin()
{
// Create any necessary pointers and download any parameters for the derived plugin.
	smp = get_project_smp();

	start_plugin();

	delete_nonrealtime_parameters();
	send_completed();
	return 0;
}

int PluginClient::start_plugin()
{
	printf("No processing defined for this plugin.\n");
	return 0;
}

// close event from client side
void PluginClient::client_side_close()
{
// Last command executed
	server->client_side_close();
}

int PluginClient::stop_gui_client()
{
	if(!client_gui_on) return 0;
	client_gui_on = 0;
//	stop_gui();                      // give to plugin
	return 0;
}

int PluginClient::get_project_samplerate()
{
	return server->get_project_samplerate();
}

float PluginClient::get_project_framerate()
{
	return server->get_project_framerate();
}


void PluginClient::update_display_title()
{
	server->generate_display_title(gui_string);
	set_string();
}


int PluginClient::set_string_client(char *string)
{
	strcpy(gui_string, string);
	set_string();
	return 0;
}

int PluginClient::save_data_client()
{
//	save_data(messages->get_message_buffer());         // user puts data directly into buffer
//	messages->write_message_raw();
	return 0;
}

int PluginClient::load_data_client()
{
//	read_data(messages->read_message_raw());         // user reads data directly from buffer
	return 0;
}

KeyFrame* PluginClient::get_prev_keyframe(long position)
{
	return server->get_prev_keyframe(position);
}

KeyFrame* PluginClient::get_next_keyframe(long position)
{
	return server->get_next_keyframe(position);
}

int PluginClient::get_interpolation_type()
{
	return server->get_interpolation_type();
}


int PluginClient::automation_used()    // If automation is used
{
	return 0;
}

float PluginClient::get_automation_value(long position)     // Get the automation value for the position in the current fragment
{
	int i;
	for(i = automation.total - 1; i >= 0; i--)
	{
		if(automation.values[i].position <= position)
		{
			return automation.values[i].intercept + automation.values[i].slope * (position - automation.values[i].position);
		}
	}
	return 0;
}

long PluginClient::get_source_len()
{
	return source_len;
}

long PluginClient::get_source_position()
{
	return source_position;
}

long PluginClient::get_total_len()
{
	return total_len;
}


int PluginClient::get_configure_change()
{
//	read_data(messages->read_message_raw());
	return 0;
}


int PluginClient::get_project_smp()
{
	return smp;
}

int PluginClient::get_gui_visible()
{
	int result;
	messages->write_message(GET_GUI_STATUS);
	result = messages->read_message();
	return result;
}

char* PluginClient::get_defaultdir()
{
	return BCASTDIR;
}

int PluginClient::get_use_float()
{
	int result;
	messages->write_message(GET_USE_FLOAT);
	result = messages->read_message();
	return result;
}

int PluginClient::get_use_alpha()
{
	int result;
	messages->write_message(GET_USE_ALPHA);
	result = messages->read_message();
	return result;
}

int PluginClient::get_aspect_ratio(float &aspect_w, float &aspect_h)
{
	long result1, result2;
	messages->write_message(GET_ASPECT_RATIO);
	messages->read_message(&result1, &result2);
	aspect_w = (float)result1;
	aspect_h = (float)result2;
	return 0;
}

int PluginClient::send_completed()
{
	messages->write_message(COMPLETED);
}

int PluginClient::send_cancelled()
{
	messages->write_message(CANCEL);
}

int PluginClient::send_hide_gui()
{
// Stop the GUI server and delete GUI messages
	client_gui_on = 0;
	return 0;
}

int PluginClient::send_configure_change()
{
// handle everything using the gui messages
//printf("PluginClient::send_configure_change 1 %p\n", server);
	KeyFrame* keyframe = server->get_keyframe();
//printf("PluginClient::send_configure_change 1\n");
	save_data(keyframe);
//printf("PluginClient::send_configure_change 1\n");
	server->sync_parameters();
//printf("PluginClient::send_configure_change 2\n");
	return 0;
}

int PluginClient::write_frames(long total_frames)
{
// buffers are preloaded by client
	messages->write_message(WRITE_FRAMES);

	messages->write_message(total_frames);
	int result = messages->read_message();
	if(result == OK) return 0;
	else return 1;
}

int PluginClient::write_samples(long total_samples)
{
// buffers are preloaded by client
	messages->write_message(WRITE_SAMPLES);
	
	messages->write_message(total_samples);
	int result = messages->read_message();
	if(result == OK) return 0;
	else return 1;
}
