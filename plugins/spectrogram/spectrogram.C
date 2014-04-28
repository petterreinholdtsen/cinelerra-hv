#include "bcdisplayinfo.h"
#include "defaults.h"
#include "filexml.h"
#include "picon_png.h"
#include "spectrogram.h"
#include "units.h"
#include "vframe.h"


#include <string.h>




PluginClient* new_plugin(PluginServer *server)
{
	return new Spectrogram(server);
}





SpectrogramConfig::SpectrogramConfig()
{
	level = 0.0;
}







SpectrogramLevel::SpectrogramLevel(Spectrogram *plugin, int x, int y)
 : BC_FPot(x, y, plugin->config.level, INFINITYGAIN, 0)
{
	this->plugin = plugin;
}

int SpectrogramLevel::handle_event()
{
	plugin->config.level = get_value();
	plugin->send_configure_change();
	return 1;
}











SpectrogramWindow::SpectrogramWindow(Spectrogram *plugin, int x, int y)
 : BC_Window(plugin->gui_string, 
 	x, 
	y, 
	640, 
	480, 
	640, 
	480,
	0, 
	0,
	1)
{
	this->plugin = plugin;
}

SpectrogramWindow::~SpectrogramWindow()
{
	Spectrogram::graph_lock.lock();
	Spectrogram::window = 0;
	Spectrogram::graph_lock.unlock();
}

void SpectrogramWindow::create_objects()
{
	int x = 60, y = 10;
	int divisions = 5;
	char string[BCTEXTLEN];

	add_subwindow(canvas = new BC_SubWindow(x, 
		y, 
		get_w() - x - 10, 
		get_h() - 50 - y,
		BLACK));
	x = 10;

	for(int i = 0; i <= divisions; i++)
	{
		y = (int)((float)(canvas->get_h() - 10) / divisions * i) + 10;
		sprintf(string, "%d", 
			Freq::tofreq((int)((float)TOTALFREQS / divisions * (divisions - i))));
		add_subwindow(new BC_Title(x, y, string));
	}

	x = canvas->get_x();
	y = canvas->get_y() + canvas->get_h() + 5;

	add_subwindow(new BC_Title(x, y + 10, "Level:"));
	add_subwindow(level = new SpectrogramLevel(plugin, x + 50, y));

	show_window();
	flush();
}


int SpectrogramWindow::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}

void SpectrogramWindow::update_gui()
{
	level->update(plugin->config.level);
}











SpectrogramThread::SpectrogramThread(Spectrogram *plugin)
 : Thread()
{
	this->plugin = plugin;
	set_synchronous(0);
	completion.lock();
}

SpectrogramThread::~SpectrogramThread()
{
	delete window;
}

void SpectrogramThread::run()
{
	BC_DisplayInfo info;
	window = new SpectrogramWindow(plugin,
		info.get_abs_cursor_x() - 125, 
		info.get_abs_cursor_y() - 115);
	window->create_objects();

	Spectrogram::graph_lock.lock();
	Spectrogram::window = window->canvas;
	Spectrogram::graph_lock.unlock();

	int result = window->run_window();
	completion.unlock();
// Last command in thread
	if(result) plugin->client_side_close();
}









SpectrogramFFT::SpectrogramFFT(Spectrogram *plugin)
 : CrossfadeFFT()
{
	this->plugin = plugin;
}

SpectrogramFFT::~SpectrogramFFT()
{
}


int SpectrogramFFT::signal_process()
{
	Spectrogram::graph_lock.lock();

//printf("SpectrogramFFT::signal_process %d\n", window_size);
	if(Spectrogram::window)
	{
		Spectrogram::window->lock_window();
		int h = Spectrogram::window->get_h();
		double *temp = new double[h];
		int niquist = plugin->PluginAClient::project_sample_rate / 2;
		int input1 = window_size / 2;
		double level = DB::fromdb(plugin->config.level);

		for(int i = 0; i < h; i++)
		{
			int input2 = (int)((float)(h - 1 - i) / h * TOTALFREQS);
			input2 = (int)((float)Freq::tofreq(input2) / 
				niquist * 
				window_size / 
				2);
			if(input2 > window_size / 2 - 1) input2 = window_size / 2 - 1;

			double sum = 0;
			if(input1 > input2)
			{
				for(int j = input1 - 1; j >= input2; j--)
					sum += sqrt(freq_real[j] * freq_real[j] + 
						freq_imag[j] * freq_imag[j]);

				sum /= (double)(input1 - input2);
			}
			else
				sum = sqrt(freq_real[input2] * freq_real[input2] + 
						freq_imag[input2] * freq_imag[input2]);


			temp[i] = sum * level;
			input1 = input2;
		}

		Spectrogram::window->copy_area(1, 
			0, 
			0, 
			0, 
			Spectrogram::window->get_w() - 1,
			Spectrogram::window->get_h());
		int x = Spectrogram::window->get_w() - 1;

		double scale = (double)0xffffff;
		for(int i = 0; i < h; i++)
		{
			int64_t color;
			color = (int)(scale * temp[i]);

			if(color < 0) color = 0;
			if(color > 0xffffff) color = 0xffffff;
			Spectrogram::window->set_color(color);
			Spectrogram::window->draw_pixel(x, i);
		}

		Spectrogram::window->flash();
		Spectrogram::window->flush();
		Spectrogram::window->unlock_window();
		delete [] temp;
	}

	Spectrogram::graph_lock.unlock();

	return 0;
}






BC_SubWindow* Spectrogram::window = 0;
Mutex Spectrogram::graph_lock;




Spectrogram::Spectrogram(PluginServer *server)
 : PluginAClient(server)
{
	reset();
	load_defaults();
}

Spectrogram::~Spectrogram()
{
	if(thread)
	{
		thread->window->set_done(0);
		thread->completion.lock();
		delete thread;
	}

	save_defaults();
	delete defaults;

	if(fft) delete fft;
}


void Spectrogram::reset()
{
	thread = 0;
	fft = 0;
	done = 0;
}


VFrame* Spectrogram::new_picon()
{
	return new VFrame(picon_png);
}

char* Spectrogram::plugin_title()
{
	return "Spectrogram";
}

int Spectrogram::is_realtime()
{
	return 1;
}

int Spectrogram::process_realtime(long size, double *input_ptr, double *output_ptr)
{
	load_configuration();

	if(!fft) 
	{
		fft = new SpectrogramFFT(this);
		fft->initialize(WINDOW_SIZE);
	}

	double *temp = new double[size];
	fft->process_fifo(size, input_ptr, temp);

	memcpy(output_ptr, input_ptr, sizeof(double) * size);
	delete [] temp;
	return 0;
}

int Spectrogram::show_gui()
{
	load_configuration();
	thread = new SpectrogramThread(this);
	thread->start();
	return 0;
}

void Spectrogram::raise_window()
{
	if(thread)
	{
		thread->window->raise_window();
		thread->window->flush();
	}
}

void Spectrogram::update_gui()
{
	if(thread)
	{
		load_configuration();
		thread->window->lock_window();
		thread->window->update_gui();
		thread->window->unlock_window();
	}
}

void Spectrogram::load_configuration()
{
	KeyFrame *prev_keyframe;
	prev_keyframe = get_prev_keyframe(-1);
	
 	read_data(prev_keyframe);
}

void Spectrogram::read_data(KeyFrame *keyframe)
{
	FileXML input;
	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;
	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("SPECTROGRAM"))
			{
				config.level = input.tag.get_property("LEVEL", config.level);
			}
		}
	}
}

void Spectrogram::save_data(KeyFrame *keyframe)
{
	FileXML output;
	output.set_shared_string(keyframe->data, MESSAGESIZE);

	output.tag.set_title("SPECTROGRAM");
	output.tag.set_property("LEVEL", (double)config.level);
	output.append_tag();
	output.append_newline();
	output.terminate_string();
}

int Spectrogram::load_defaults()
{
	char directory[BCTEXTLEN];

	sprintf(directory, "%sspectrogram.rc", BCASTDIR);
	defaults = new Defaults(directory);
	defaults->load();
	config.level = defaults->get("LEVEL", config.level);
	return 0;
}

int Spectrogram::save_defaults()
{
	defaults->update("LEVEL", config.level);
	defaults->save();
	return 0;
}


int Spectrogram::set_string()
{
	if(thread) thread->window->set_title(gui_string);
	return 0;
}




