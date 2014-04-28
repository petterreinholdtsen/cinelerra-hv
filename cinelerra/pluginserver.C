#include "amodule.h"
#include "atrack.h"
#include "autoconf.h"
#include "cplayback.h"
#include "cwindow.h"
#include "edl.h"
#include "edlsession.h"
#include "floatautos.h"
#include "localsession.h"
#include "mainprogress.h"
#include "menueffects.h"
#include "messages.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "neworappend.h"
#include "playbackengine.h"
#include "plugin.h"
#include "pluginbuffer.h"
#include "pluginclient.h"
#include "plugincommands.h"
#include "pluginserver.h"
#include "preferences.h"
#include "sema.h"
#include "mainsession.h"
#include "trackcanvas.h"
#include "transportque.h"
#include "vframe.h"
#include "vmodule.h"
#include "vtrack.h"


#include <sys/types.h>
#include <sys/wait.h>
#include <dlfcn.h>


PluginGUIServer::PluginGUIServer()
 : Thread()
{
// PluginGUIServer must be able to exit by itself for a plugin close event
	set_synchronous(0);
}

PluginGUIServer::~PluginGUIServer()
{
}

int PluginGUIServer::start_gui_server(PluginServer *plugin_server, char *string)
{
	this->plugin_server = plugin_server;
	messages = new Messages(MESSAGE_FROM_PLUGIN, MESSAGE_TO_PLUGIN);
	strcpy(this->string, string);
	completion_lock.lock();
	start();
	return 0;
}

void PluginGUIServer::run()
{
	int done = 0;
	int command;
	
	while(!done)
	{
		command = messages->read_message();

		switch(command)
		{
			case COMPLETED:          
			{
				MenuEffectPrompt *prompt = plugin_server->prompt;
				done = 1;  						  
				plugin_server->gui_on = 0;
                if(plugin_server->plugin_open)
                {
			        if(prompt)
			        {
// Cancel a non realtime effect
				        prompt->set_done(1);
			        }
			        else
			        {
// Update the attachment point.
//				        plugin_server->attachment->set_show(0);
			        }
                }
				done = 1;

                plugin_server->close_plugin();
			}
				break;

			case CONFIGURE_CHANGE:
// Propagate the configuration to the attachment point.
// non realtime context doesn't have an attachment
//				if(plugin_server->attachment)
//					plugin_server->attachment->get_configuration_change(messages->read_message_raw());
//printf("PluginGUIServer::run 2\n");
				break;

			case GET_STRING:
				messages->write_message(string);
				break;
		}
	}
	delete messages;
	completion_lock.unlock();
}




PluginForkThread::PluginForkThread() : Thread()
{
	set_synchronous(0);    // detach the thread
}

PluginForkThread::~PluginForkThread()
{
}

int PluginForkThread::fork_plugin(char *path, char **args)
{
	this->path = path;
	this->args = args;
//	fork_lock.lock();
	completion_lock.lock();
	start();
// wait for process to start
// 	fork_lock.lock();
// 	fork_lock.unlock();
	return 0;
}

int PluginForkThread::wait_completion()
{
	completion_lock.lock();
	completion_lock.unlock();
}


void PluginForkThread::run()
{
// fork it
	plugin_pid = fork();

	if(plugin_pid == 0)
	{
		execv(path, args);     // turn into the plugin
	}
	else
	{
		int plugin_status;
//		sleep(1);
// Signal fork finished
//		fork_lock.unlock();
// Wait for plugin to finish
		if(waitpid(plugin_pid, &plugin_status, WUNTRACED) == -1)
		{
			perror("PluginForkThread::waitpid:");
		}
		completion_lock.unlock();
		return;            // wait for the plugin to finish and clean up
	}
}







PluginServer::PluginServer()
{
	reset_parameters();
	modules = new ArrayList<Module*>;
	fork_thread = new PluginForkThread;
	gui_server = new PluginGUIServer;
}

PluginServer::PluginServer(char *path)
{
	reset_parameters();
	set_path(path);
	modules = new ArrayList<Module*>;
	fork_thread = new PluginForkThread;
	gui_server = new PluginGUIServer;
//if(path) printf("PluginServer::PluginServer %s\n", path);
}

PluginServer::PluginServer(PluginServer &that)
{
	reset_parameters();

	if(that.title)
	{
		title = new char[strlen(that.title) + 1];
		strcpy(title, that.title);
	}

	if(that.path)
	{
		path = new char[strlen(that.path) + 1];
		strcpy(path, that.path);
	}

	modules = new ArrayList<Module*>;
	fork_thread = new PluginForkThread;
	gui_server = new PluginGUIServer;

	attachment = that.attachment;	
	realtime = that.realtime;
	multichannel = that.multichannel;
	synthesis = that.synthesis;
	audio = that.audio;
	video = that.video;
	theme = that.theme;
	fileio = that.fileio;
	uses_gui = that.uses_gui;
	mwindow = that.mwindow;
	keyframe = that.keyframe;
	plugin_fd = that.plugin_fd;
}

PluginServer::~PluginServer()
{
//printf("PluginServer::~PluginServer 1\n");
	close_plugin();
//printf("PluginServer::~PluginServer 1\n");
	if(path) delete path;
//printf("PluginServer::~PluginServer 1\n");
	if(title) delete title;
//printf("PluginServer::~PluginServer 1\n");
	if(modules) delete modules;
//printf("PluginServer::~PluginServer 1\n");
	if(message_lock) delete message_lock;
//printf("PluginServer::~PluginServer 1\n");
	if(fork_thread) delete fork_thread;
//printf("PluginServer::~PluginServer 1\n");
	if(gui_server) delete gui_server;
//printf("PluginServer::~PluginServer 1\n");
	if(picon) delete picon;
//printf("PluginServer::~PluginServer 2\n");
}

// Done only once at creation
int PluginServer::reset_parameters()
{
	mwindow = 0;
	keyframe = 0;
	prompt = 0;
	cleanup_plugin();
	plugin_fd = 0;
	autos = 0;
	plugin = 0;
	edl = 0;
	title = 0;
	path = 0;
	audio = video = theme = 0;
	uses_gui = 0;
	realtime = multichannel = fileio = 0;
	synthesis = 0;
	start_auto = end_auto = 0;
	picon = 0;
	transition = 0;
}


// Done every time the plugin is opened or closed
int PluginServer::cleanup_plugin()
{
	in_buffer_size = out_buffer_size = 0;
	total_in_buffers = total_out_buffers = 0;
	messages = 0;
	message_lock = 0;
	error_flag = 0;
	written_samples = 0;
	shared_buffers = 0;
	new_buffers = 0;
	written_samples = written_frames = 0;
	gui_on = 0;
	temp_frame_buffer = 0;
	temp_frame = 0;
	plugin = 0;
	plugin_open = 0;
}

void PluginServer::set_mwindow(MWindow *mwindow)
{
	this->mwindow = mwindow;
}

void PluginServer::set_keyframe(KeyFrame *keyframe)
{
	this->keyframe = keyframe;
}

void PluginServer::set_prompt(MenuEffectPrompt *prompt)
{
	this->prompt = prompt;
}


int PluginServer::set_path(char *path)
{
	if(this->path) delete this->path;
	this->path = new char[strlen(path) + 1];
	strcpy(this->path, path);
}

void PluginServer::set_title(char *string)
{
	if(title) delete title;
	title = new char[strlen(string) + 1];
	strcpy(title, string);
}

void PluginServer::generate_display_title(char *string)
{
//printf("PluginServer::generate_display_title %s %s\n", plugin->track->title, title);
	if(plugin) 
		sprintf(string, "%s: %s", plugin->track->title, title);
	else
		strcpy(string, title);
}

// Open plugin for signal processing
int PluginServer::open_plugin(int master, EDL *edl, Plugin *plugin)
{
	if(plugin_open) return 0;

	if(!plugin_fd) plugin_fd = dlopen(path, RTLD_NOW);
	this->plugin = plugin;
	this->edl = edl;
//printf("PluginServer::open_plugin %s %p %p\n", path, this->plugin, plugin_fd);

	if(!plugin_fd)
	{
// If the dlopen failed it may still be an executable tool for a specific
// file format, in which case we just store the path.
		set_title(path);
		char string[BCTEXTLEN];
		strcpy(string, dlerror());

		if(!strstr(string, "executable"))
			printf("PluginServer::open_plugin: %s\n", string);
		
		return 0;
	}

	new_plugin = (PluginClient* (*)(PluginServer*))dlsym(plugin_fd, "new_plugin");
	if(!new_plugin)
	{
		fprintf(stderr, "PluginServer::open_plugin: new_plugin undefined in %s\n", path);
		return 1;
	}



//printf("PluginServer::open_plugin 2\n");
	client = new_plugin(this);
//printf("PluginServer::open_plugin 3\n");

	realtime = client->is_realtime();
	audio = client->is_audio();
	video = client->is_video();
	theme = client->is_theme();
	fileio = client->is_fileio();
	uses_gui = client->uses_gui();
	multichannel = client->is_multichannel();
	synthesis = client->is_synthesis();
	transition = client->is_transition();
	set_title(client->plugin_title());
//printf("PluginServer::open_plugin 4\n");

	if(master)
	{
		picon = client->new_picon();
	}
//printf("PluginServer::open_plugin 3\n");

	plugin_open = 1;
	return 0;
}

int PluginServer::close_plugin()
{
	if(!plugin_open) return 0;

	int plugin_status, result;
//printf("PluginServer::close_plugin 1\n");
	if(client) delete client;
//printf("PluginServer::close_plugin 1 %p\n", plugin_fd);

// shared object is persistent since plugin deletion would unlink its own object
//	dlclose(plugin_fd);
//printf("PluginServer::close_plugin 1\n");
	plugin_open = 0;

	cleanup_plugin();
//printf("PluginServer::close_plugin 2\n");

return 0;
	messages->write_message(EXIT_PLUGIN);
	fork_thread->wait_completion();
	delete messages;

// delete pointers to shared buffers but not the shared buffers themselves
	if(data_in.total)
	{
		if(!shared_buffers)
			for(int i = 0; i < data_in.total; i++) delete data_in.values[i];
		data_in.remove_all();
	}

	if(data_out.total)
	{
		if(!shared_buffers)
			for(int i = 0; i < data_out.total; i++) delete data_out.values[i];
		data_out.remove_all();
	}

	if(temp_frame)
	{
		delete temp_frame;
		delete temp_frame_buffer;
	}

// these are always shared
	data_in_realtime.remove_all();
	data_out_realtime.remove_all();
	offset_in_render.remove_all();
	offset_out_render.remove_all();
	double_buffer_in_render.remove_all();
	double_buffer_out_render.remove_all();
	realtime_in_size.remove_all();
	realtime_out_size.remove_all();
	delete args[0];
	delete args[1];
	delete args[3];
	total_args = 0;

}

void PluginServer::client_side_close()
{
// Last command executed in client thread
	if(plugin) mwindow->hide_plugin(plugin);
	else
	if(prompt)
	{
		prompt->lock_window();
		prompt->set_done(1);
		prompt->unlock_window();
	}
}

int PluginServer::init_realtime(int realtime_sched,
		int total_in_buffers)
{
	if(!plugin_open) return 0;
// set for realtime priority
// initialize plugin
	client->plugin_init_realtime(realtime_sched, total_in_buffers);
}


void PluginServer::process_realtime(VFrame **input, 
		VFrame **output, 
		long current_position,
		long total_len)
{
//printf("PluginServer::process_realtime 1 %d\n", plugin_open);
	if(!plugin_open) return;

	client->plugin_process_realtime(input, 
		output, 
		current_position,
		total_len);
//printf("PluginServer::process_realtime 2 %d\n", plugin_open);
}

void PluginServer::process_realtime(double **input, 
		double **output,
		long current_position, 
		long fragment_size,
		long total_len)
{
	if(!plugin_open) return;

	client->plugin_process_realtime(input, 
		output, 
		current_position, 
		fragment_size,
		total_len);
}


int PluginServer::plugin_server_loop()
{
	int done = 0;
	int command;
	if(!plugin_open) return 0;

	while(!done)
	{
		done = handle_plugin_command();
	}
	return 0;
}

MainProgressBar* PluginServer::start_progress(char *string, long length)
{
	mwindow->gui->lock_window();
	MainProgressBar *result = mwindow->mainprogress->start_progress(string, length);
	mwindow->gui->unlock_window();
	return result;
}

// return values
// 0 ok
// 1 finished
// 2 buffer written
// 3 cancel
int PluginServer::handle_plugin_command()
{
	int result;
	int command;
	if(!plugin_open) return 0;

	command = messages->read_message();
	result = 0;

	switch(command)
	{
		case COMPLETED:
			result = 1;
			break;

		case GET_SAMPLERATE:
			messages->write_message(mwindow->session->sample_rate);
			break;

		case GET_FRAMERATE:
			messages->write_message((int)(mwindow->session->frame_rate * 1000));
			break;

		case GET_FRAMESIZE:
			messages->write_message(mwindow->session->track_w, mwindow->session->track_h);
			break;

		case GET_SMP:
			messages->write_message(mwindow->edl->session->smp + 1);
			break;

		case GET_USE_FLOAT:
//			messages->write_message(mwindow->preferences->video_floatingpoint);
			break;

		case GET_USE_ALPHA:
//			messages->write_message(mwindow->preferences->video_use_alpha);
			break;

		case GET_USE_INTERPOLATION:
//			messages->write_message(mwindow->preferences->video_interpolate);
			break;

		case GET_ASPECT_RATIO:
			messages->write_message((long)mwindow->edl->session->aspect_w, (long)mwindow->edl->session->aspect_h);
			break;
		
		case GET_GUI_STATUS:
			send_gui_status(gui_on);
			break;

		case GET_REALTIME_BUFFERS:
			send_buffer_info();
			break;

		case READ_SAMPLES:
			break;

		case WRITE_SAMPLES:
			write_samples();
			result = 2;
			break;
		
		case READ_FRAMES:
			break;

		case WRITE_FRAMES:
			write_frames();
			result = 2;
			break;
		
		case CANCEL:
			result = 3;
// Need to get the COMPLETED that is returned by default when a plugin exits.
			messages->read_message_raw();
			break;
	}

	return result;
}

int PluginServer::write_samples()
{
	if(!plugin_open) return 0;
	if(error_flag)
	{
		messages->write_message(CANCEL);
		return 1;
	}
	else
	{
		written_samples = messages->read_message();
		return 0;
	}
}

long PluginServer::get_written_samples()
{
	if(!plugin_open) return 0;
	return written_samples;
}

int PluginServer::write_frames()
{
	if(!plugin_open) return 0;
	if(error_flag)
	{
		messages->write_message(CANCEL);
		return 1;
	}
	else
	{
		written_frames = messages->read_message();
		return 0;
	}
}

long PluginServer::get_written_frames()
{
	if(!plugin_open) return 0;
	return written_frames;
}






int PluginServer::load_defaults()             // loads defaults from disk file
{
	if(!plugin_open) return 0;
	messages->write_message(LOAD_DEFAULTS);
}

int PluginServer::save_defaults()             // save defaults from disk file
{
	if(!plugin_open) return 0;
	messages->write_message(SAVE_DEFAULTS);
}

int PluginServer::get_parameters()      // waits for plugin to finish and returns a result
{
	if(!plugin_open) return 0;
	return client->plugin_get_parameters();
}





// ======================= Non-realtime plugin

int PluginServer::set_interactive()
{
	if(!plugin_open) return 0;
	client->set_interactive();
	return 0;
}

int PluginServer::set_range(long start, long end)
{
	if(!plugin_open) return 0;
	messages->write_message(SET_RANGE);
	messages->write_message(start, end);
	return 0;
}

int PluginServer::set_module(Module *module)
{
	modules->append(module);
	return 0;
}

int PluginServer::set_error()
{
	error_flag = 1;
	return 0;
}

int PluginServer::set_realtime_sched()
{
	struct sched_param params;
	params.sched_priority = 1;
	if(sched_setscheduler(fork_thread->plugin_pid, SCHED_RR, &params)) perror("sched_setscheduler");
	return 0;
}

int PluginServer::send_cancel()
{
	if(!plugin_open) return 0;
	messages->write_message(CANCEL);
	return 0;
}

int PluginServer::send_write_result(int result)
{
	if(!plugin_open) return 0;
	if(!result) messages->write_message(OK);
	else messages->write_message(CANCEL);
	return 0;
}


int PluginServer::process_loop(VFrame **buffers, long &write_length)
{
	if(!plugin_open) return 1;
	return client->plugin_process_loop(buffers, write_length);
}

int PluginServer::process_loop(double **buffers, long &write_length)
{
	if(!plugin_open) return 1;
	return client->plugin_process_loop(buffers, write_length);
}

int PluginServer::start_loop(long start, long end, long buffer_size, int total_buffers)
{
	if(!plugin_open) return 0;
	client->plugin_start_loop(start, end, buffer_size, total_buffers);
	return 0;
}

int PluginServer::stop_loop()
{
	if(!plugin_open) return 0;
	return client->plugin_stop_loop();
}

int PluginServer::read_frame(VFrame *buffer, int channel, long start_position)
{
//printf("PluginServer::read_frame 1 %p\n", modules);
//printf("PluginServer::read_frame 1 %p\n", modules->values[channel]);
//printf("PluginServer::read_frame 1 %d %d\n", buffer->get_w(), buffer->get_h());
	((VModule*)modules->values[channel])->render(buffer,
		start_position,
		PLAY_FORWARD);
//printf("PluginServer::read_frame 2\n");
	return 0;
}

int PluginServer::read_samples(double *buffer, int channel, long start_position, long total_samples)
{
//printf("PluginServer::read_samples 1\n");
	((AModule*)modules->values[channel])->render(buffer, 
		total_samples, 
		start_position,
		PLAY_FORWARD);
//printf("PluginServer::read_samples 2\n");
	return 0;
}

int PluginServer::read_samples(double *buffer, long start_position, long total_samples)
{
	((AModule*)modules->values[0])->render(buffer, 
		total_samples, 
		start_position,
		PLAY_FORWARD);
	return 0;
}



// Called by client
int PluginServer::get_gui_status()
{
//printf("PluginServer::get_gui_status %p %p\n", this, plugin);
	if(plugin)
		return plugin->show ? GUI_ON : GUI_OFF;
	else
		return GUI_OFF;
}

int PluginServer::send_gui_status(int visible)
{
	if(!plugin_open) return 0;
	messages->write_message(visible ? GUI_ON : GUI_OFF);
	return 0;
}

void PluginServer::raise_window()
{
	if(!plugin_open) return;
	client->raise_window();
}

void PluginServer::show_gui()
{
//printf("PluginServer::show_gui 1\n");
	if(!plugin_open) return;
//printf("PluginServer::show_gui 1\n");
	client->update_display_title();
//printf("PluginServer::show_gui 1\n");
	client->show_gui();
//printf("PluginServer::show_gui 2\n");
}

void PluginServer::update_gui()
{
//printf("PluginServer::update_gui 1\n");
	if(!plugin_open) return;
//printf("PluginServer::update_gui 2\n");

	if(video)
		client->source_position = Units::to_long(
			mwindow->edl->local_session->selectionstart * mwindow->edl->session->frame_rate);
	else
	if(audio)
		client->source_position = Units::to_long(
			mwindow->edl->local_session->selectionstart * mwindow->edl->session->sample_rate);
	client->update_gui();
}

void PluginServer::update_title()
{
	if(!plugin_open) return;
	
	client->update_display_title();
}


int PluginServer::set_string(char *string)
{
	if(!plugin_open) return 0;

	client->set_string_client(string);
	return 0;
}

int PluginServer::process_realtime(long source_len, long source_position, long fragment_len)
{
	process_realtime_start(source_len, source_position, fragment_len);
	process_realtime_end();
	return 0;
}

int PluginServer::process_realtime_start(long source_len, long source_position, long fragment_len)
{
	if(!plugin_open) return 0;
	if(message_lock) message_lock->lock();
	messages->write_message(PROCESS_REALTIME);

// send information on the buffers
	messages->write_message(fragment_len, source_len, source_position);
	int i;
	for(i = 0; i < total_in_buffers; i++)
	{
		messages->write_message(offset_in_render.values[i], double_buffer_in_render.values[i]);
	}
	for(i = 0; i < total_out_buffers; i++)
	{
		messages->write_message(offset_out_render.values[i], double_buffer_out_render.values[i]);
	}
// Send information on the automation
	send_automation(source_len, source_position, fragment_len);
	return 0;
}

int PluginServer::send_automation(long source_len, long source_position, long buffer_len)
{
	long position;
	int i, done = 0;
	FloatAuto *current;
	int automate = 1;
	double constant = 0;
	long buffer_position;
	long input_start;
	long input_end;
	double slope_value;
	double slope_start;
	double slope_end;
	double slope_position;
	double slope;

	if(autos)
	{
		autos->init_automation(buffer_position, 
						input_start, 
						input_end, 
						automate, 
						constant, 
						source_position,
						buffer_len,
						(Auto**)start_auto, 
						(Auto**)end_auto,
						reverse);

		if(automate)
		{
			autos->init_slope((Auto**)&current, 
					slope_start,
					slope_value,
					slope_position, 
					input_start,
					input_end, 
					(Auto**)start_auto, 
					(Auto**)end_auto,
					reverse);

			while(buffer_position < buffer_len)
			{
				autos->get_slope((Auto**)&current, 
						slope_start, 
						slope_end, 
						slope_value, 
						slope, 
						buffer_len, 
						buffer_position,
						reverse);

				messages->write_message_f(buffer_position, slope_value + slope_position * slope, slope);
				buffer_position += (long)(slope_end - slope_position);
				slope_position = slope_end;
				autos->advance_slope((Auto**)&current, 
								slope_start, 
								slope_value,
								slope_position, 
								reverse);
			}
		}
		else
		{
// Send constant if no automation but constant is changed
			if(constant != 0)
				messages->write_message_f(0, constant, (double)0);
		}
	}

	messages->write_message_f(-1, (double)0, (double)0);
	return 0;
}

int PluginServer::process_realtime_end()
{
	if(!plugin_open) return 0;
	messages->read_message();       // wait for completed
	if(message_lock) message_lock->unlock();
	start_auto = end_auto = 0;
	autos = 0;
	return 0;
}


// ============================= queries

int PluginServer::get_samplerate()
{
	if(!plugin_open) return 0;
	if(audio)
	{
		return client->get_samplerate();
	}
	else
	if(mwindow)
		return mwindow->edl->session->sample_rate;
	else
	{
		printf("PluginServer::get_samplerate audio and mwindow == NULL\n");
		return 1;
	}
}


double PluginServer::get_framerate()
{
	if(!plugin_open) return 0;
	if(video)
	{
		return client->get_framerate();
	}
	else
	if(mwindow)
		return mwindow->edl->session->frame_rate;
	else 
	{
		printf("PluginServer::get_framerate video and mwindow == NULL\n");
		return 1;
	}
}

int PluginServer::get_project_samplerate()
{
	return mwindow->edl->session->sample_rate;
}

double PluginServer::get_project_framerate()
{
	return  mwindow->edl->session->frame_rate;
}


int PluginServer::negotiate_buffers(long recommended_size)
{
	if(!plugin_open) return 0;
// prepare to negotiate buffers
	messages->write_message(GET_BUFFERS);
// send number of tracks and recommended size for input and output buffers
//	messages->write_message(tracks->total, recommended_size);

// get actual sizes	
	messages->read_message(&in_buffer_size, &out_buffer_size);  // get desired sizes
// init buffers
//	total_out_buffers = total_in_buffers = tracks->total;

// init buffers
	int word_size;
//	if(video) 
//		word_size = mwindow->session->track_w * mwindow->session->track_h * sizeof(VPixel);
//	else
//	if(audio)
//		word_size = sizeof(double);

// Sizes sent back to the client are byte counts.
// 	for(int i = 0; i < tracks->total; i++)
// 	{
// 		data_out.append(new PluginBuffer(out_buffer_size, word_size));
// 
// 		messages->write_message(data_out.values[i]->get_id(), data_out.values[i]->get_size());
// 	}

// 	for(int i = 0; i < tracks->total; i++)
// 	{
// 		data_in.append(new PluginBuffer(in_buffer_size, word_size));
// 		messages->write_message(data_in.values[i]->get_id(), data_in.values[i]->get_size());
// 	}
}

int PluginServer::attach_input_buffer(PluginBuffer *input, long size)
{
	shared_buffers = 1;     // all buffers are shared

	data_in.append(input);

	in_buffer_size = size;
	total_in_buffers++;
	return total_in_buffers - 1;
}

int PluginServer::attach_input_buffer(PluginBuffer **input, long ring_buffers, long buffer_size, long fragment_size)
{
	shared_buffers = 1;     // all buffers are shared

//printf("PluginServer::attach_input_buffer %p %d %d %d\n", input, ring_buffers, buffer_size, fragment_size);
	data_in_realtime.append(input);
	ring_buffers_in.append(ring_buffers);
	offset_in_render.append(0);
	double_buffer_in_render.append(0);
	realtime_in_size.append(buffer_size);
	in_buffer_size = fragment_size;

	return total_in_buffers++;
}

int PluginServer::attach_output_buffer(PluginBuffer *output, long size)
{
	shared_buffers = 1;     // all buffers are shared

	data_out.append(output);

	total_out_buffers++;
	out_buffer_size = size;
	return total_out_buffers - 1;
}


int PluginServer::attach_output_buffer(PluginBuffer **output, long ring_buffers, long buffer_size, long fragment_size)
{
//printf("PluginServer::attach_output_buffer %p %d %d %d\n", output, ring_buffers, buffer_size, fragment_size);
	shared_buffers = 1;     // all buffers are shared

	data_out_realtime.append(output);
	ring_buffers_out.append(ring_buffers);
	offset_out_render.append(0);
	double_buffer_out_render.append(0);
	realtime_out_size.append(buffer_size);
	out_buffer_size = fragment_size;

	return total_out_buffers++;
}

int PluginServer::detach_buffers()
{
	data_out_realtime.remove_all();
	ring_buffers_out.remove_all();
	offset_out_render.remove_all();
	double_buffer_out_render.remove_all();
	realtime_out_size.remove_all();

	data_in_realtime.remove_all();
	ring_buffers_in.remove_all();
	offset_in_render.remove_all();
	double_buffer_in_render.remove_all();
	realtime_in_size.remove_all();
	
	out_buffer_size = 0;
	shared_buffers = 0;
	total_out_buffers = 0;
	in_buffer_size = 0;
	total_in_buffers = 0;
	return 0;
}

int PluginServer::arm_buffer(int buffer_number, 
		long offset_in, 
		long offset_out,
		int double_buffer_in,
		int double_buffer_out)
{
	offset_in_render.values[buffer_number] = offset_in;
	offset_out_render.values[buffer_number] = offset_out;
	double_buffer_in_render.values[buffer_number] = double_buffer_in;
	double_buffer_out_render.values[buffer_number] = double_buffer_out;
}


int PluginServer::restart_realtime()
{
	messages->write_message(RESTART_REALTIME);
	send_buffer_info();
}

int PluginServer::set_automation(FloatAutos *autos, FloatAuto **start_auto, FloatAuto **end_auto, int reverse)
{
	this->autos = autos;
	this->start_auto = start_auto;
	this->end_auto = end_auto;
	this->reverse = reverse;
}

// REMOVE
int PluginServer::send_buffer_info()
{
	return 0;
}

int PluginServer::realtime_stop()
{
	if(!plugin_open) return 0;
	client->plugin_stop_realtime();
}

void PluginServer::save_data(KeyFrame *keyframe)
{
	if(!plugin_open) return;
	client->save_data(keyframe);
}

int PluginServer::notify_load_data()
{
	if(!plugin_open) return 0;
// send the notification
	messages->write_message(LOAD_DATA);
}

KeyFrame* PluginServer::get_prev_keyframe(long position)
{
	if(plugin)
		return plugin->get_prev_keyframe(position);
	else
		return keyframe;
}

int PluginServer::get_interpolation_type()
{
	return plugin->edl->session->interpolation_type;
}

KeyFrame* PluginServer::get_next_keyframe(long position)
{
	if(plugin)
		return plugin->get_next_keyframe(position);
	else
		return 0;
}

KeyFrame* PluginServer::get_keyframe()
{
//printf("PluginServer::get_keyframe %p\n", plugin);
	if(plugin)
		return plugin->get_keyframe();
	else
		return keyframe;
}

Theme* PluginServer::new_theme()
{
	return client->new_theme();
}


// Called when plugin interface is tweeked
void PluginServer::sync_parameters()
{
	mwindow->sync_parameters();
	if(mwindow->edl->session->auto_conf->plugins)
	{
		mwindow->gui->lock_window();
		mwindow->gui->canvas->draw_overlays();
		mwindow->gui->canvas->flash();
		mwindow->gui->unlock_window();
	}
}


int PluginServer::load_data()
{
	if(!plugin_open) return 0;
// send the text that was previously loaded by char* get_message_buffer()
	messages->write_message_raw();
}

int PluginServer::get_configuration_change(char *data)
{
	if(!plugin_open) return 0;
	if(message_lock) message_lock->lock();
	messages->write_message(CONFIGURE_CHANGE);
	messages->write_message(data);
	if(message_lock) message_lock->unlock();
}


void PluginServer::dump()
{
	printf("    PluginServer %s %s\n", path, title);
}
