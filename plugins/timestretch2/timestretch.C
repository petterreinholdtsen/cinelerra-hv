#include "bcdisplayinfo.h"
#include "clip.h"
#include "defaults.h"
#include "language.h"
#include "mainprogress.h"
#include "picon_png.h"
#include "resample.h"
#include "timestretch.h"
#include "timestretchengine.h"
#include "vframe.h"


#include <string.h>

#define FRAGMENT_SIZE 65536


REGISTER_PLUGIN(TimeStretch)

// This was an attempt to time stretch with a single FFT window.
// Instead of making the sound go faster it just made different parts of the
// audio overlap.



TimeStretchFraction::TimeStretchFraction(TimeStretch *plugin, 
	TimeStretchWindow *gui,
	int x, 
	int y)
 : BC_TextBox(x, y, 100, 1, (float)plugin->scale)
{
	this->plugin = plugin;
	this->gui = gui;
	set_precision(4);
}

int TimeStretchFraction::handle_event()
{
	plugin->scale = atof(get_text());
	gui->update_memory_required();
	return 1;
}








TimeStretchWindow::TimeStretchWindow(TimeStretch *plugin, int x, int y)
 : BC_Window(PROGRAM_NAME ": Time stretch", 
 				x - 160,
				y - 75,
 				320, 
				150, 
				320, 
				150,
				0,
				0,
				1)
{
	this->plugin = plugin;
}


TimeStretchWindow::~TimeStretchWindow()
{
}

void TimeStretchWindow::create_objects()
{
	int x = 10, y = 10;


	add_subwindow(memory = new BC_Title(x, y, ""));
	update_memory_required();
	y += 20;
	add_subwindow(new BC_Title(x, y, _("Fraction of original length:")));
	y += 20;
	add_subwindow(new TimeStretchFraction(plugin, this, x, y));

	add_subwindow(new BC_OKButton(this));
	add_subwindow(new BC_CancelButton(this));
	show_window();



	flush();
}

void TimeStretchWindow::update_memory_required()
{
	char string[BCTEXTLEN];
	sprintf(string, 
		_("Memory required: %dM"), 
		plugin->get_memory_required() /
			1024 /
			1024);
	memory->update(string);
}















TimeStretch::TimeStretch(PluginServer *server)
 : PluginAClient(server)
{
	load_defaults();
	resample = 0;
	fft = 0;
	real_in = 0;
	imag_in = 0;
	real_out = 0;
	imag_out = 0;
	window_size = 0;
	resampled_size = 0;
}


TimeStretch::~TimeStretch()
{
	save_defaults();
	delete defaults;
	if(fft) delete fft;
	if(resample) delete resample;
	delete_buffer_vector(&real_in);
	delete_buffer_vector(&imag_in);
	delete_buffer_vector(&real_out);
	delete_buffer_vector(&imag_out);
	if(resampled_size) delete [] resampled_size;
}

int TimeStretch::allocate_buffer_vector(double ***buffer, int size)
{
	if(!(*buffer))
	{
		(*buffer) = new double*[get_total_buffers()];
		bzero((*buffer), sizeof(double*) * get_total_buffers());
		for(int i = 0; i < get_total_buffers(); i++)
		{
			(*buffer)[i] = new double[size];
			if(!(*buffer)[i])
			{
				fprintf(stderr, "TimeStretch::allocate_buffer_vector: out of memory.\n");
				return 1;
			}
		}
	}
	return 0;
}

void TimeStretch::delete_buffer_vector(double ***buffer)
{
	if((*buffer))
	{
		for(int i = 0; i < get_total_buffers(); i++)
		{
			if((*buffer)[i]) delete [] (*buffer)[i];
		}
		delete [] (*buffer);
		(*buffer) = 0;
	}
}

char* TimeStretch::plugin_title()
{
	return _("Time stretch");
}

int TimeStretch::is_multichannel()
{
	return 1;
}

int TimeStretch::get_parameters()
{
	BC_DisplayInfo info;
	TimeStretchWindow window(this, 
		info.get_abs_cursor_x(), 
		info.get_abs_cursor_y());
	window.create_objects();
	int result = window.run_window();

	return result;
}

VFrame* TimeStretch::new_picon()
{
	return new VFrame(picon_png);
}

int64_t TimeStretch::calculate_window(int64_t size)
{
	int64_t result = 2;
	while(result < size)
		result *= 2;
	return result;
}

int64_t TimeStretch::get_memory_required()
{
// Input buffers
	window_size = calculate_window(
		(int64_t)(get_total_len() * scale));
	input_size = MAX(window_size, get_total_len());
	output_size = window_size;
	int64_t result = input_size * 
		8 *
		2 *
		get_total_buffers() +
		output_size *
		8 *
		2 *
		get_total_buffers();
	return result;
}


int TimeStretch::start_loop()
{
	int64_t scaled_len = (int64_t)(get_total_len() * scale);
	get_memory_required();
	char string[BCTEXTLEN];
	sprintf(string, "%s...", plugin_title());
	progress = start_progress(string, 
// Read samples
		get_total_len() +
// Resample
		get_total_len() +
// FFT conversion
		window_size * get_total_buffers() +
// Frequency shift
		window_size * get_total_buffers() +
// FFT conversion
		window_size * get_total_buffers() +
// Write samples
		scaled_len);

	write_position = 0;

	fft = new FFT;
	resample = new Resample(0, get_total_buffers());
	need_process = 1;
	current_progress = 0;
	write_position = 0;
	resampled_size = new int64_t[get_total_buffers()];
	bzero(resampled_size, sizeof(int64_t) * get_total_buffers());

	return 0;
}

int TimeStretch::stop_loop()
{
	if(PluginClient::interactive)
	{
		progress->stop_progress();
		delete progress;
	}
	return 0;
}

int TimeStretch::process_loop(double **buffer, int64_t &write_length)
{
	int result = 0;

// Allocate window buffers
	if(need_process)
	{
		write_length = 0;
		need_process = 0;

		if(allocate_buffer_vector(&real_in, input_size)) return 1;
		if(allocate_buffer_vector(&imag_in, input_size)) return 1;
		if(allocate_buffer_vector(&real_out, output_size)) return 1;
		if(allocate_buffer_vector(&imag_out, output_size)) return 1;


// Read entire range into imag_in for all channels
		int fragment_size = 0;
		for(int64_t i = 0, j = get_source_start();
			i < get_total_len() && !result;
			i += fragment_size,
			j += fragment_size)
		{
			fragment_size = FRAGMENT_SIZE;
			if(fragment_size + i > get_total_len())
				fragment_size = get_total_len() - i;
			for(int k = 0; k < get_total_buffers(); k++)
			{
				read_samples(imag_in[k] + i, k, j, fragment_size);
			}
			current_progress += fragment_size;
			result = progress->update(current_progress);
		}


// Resample entire range into real_in now to reduce memory usage
		if(!result)
		{
			int fragment_size = 0;
			for(int i = 0; i < get_total_len() && !result; i += fragment_size)
			{
				fragment_size = FRAGMENT_SIZE;
				if(fragment_size + i > get_total_len())
					fragment_size = get_total_len() - i;
				for(int j = 0; j < get_total_buffers(); j++)
				{
					resample->resample_chunk(imag_in[j] + i,
						fragment_size,
						1000000,
						(int)(1000000 * scale),
						j);
					if((output_size = resample->get_output_size(j)))
					{
						resample->read_output(real_in[j] + resampled_size[j],
							j,
							output_size);
						resampled_size[j] += output_size;
					}
				}
				current_progress += fragment_size;
				result = progress->update(current_progress);
			}
			for(int i = 0; i < get_total_buffers(); i++)
			{
				bzero(real_in[i] + resampled_size[i], 
					sizeof(double) * (window_size - resampled_size[i]));
			}
		}

// Do the forward FFT on the entire window at once
		if(!result)
		{
			for(int i = 0; i < get_total_buffers() && !result; i++)
			{
				fft->do_fft(window_size,
					0,
					real_in[i],
					0,
					real_out[i],
					imag_out[i]);
				current_progress += window_size;
				result = progress->update(current_progress);
			}
		}

// Shift frequencies
		if(!result)
		{
			int min_freq = 
				1 + 
				(int)(20.0 / 
					((double)get_project_samplerate() / 
						window_size * 
						2) + 
					0.5);

			for(int j = 0; j < get_total_buffers() && !result; j++)
			{
				double *freq_real = real_out[j];
				double *freq_imag = imag_out[j];

				if(scale < 1)
				{
					for(int i = min_freq; i < window_size / 2; i++)
					{
						double destination = i * scale;
						int dest_i = (int)(destination + 0.5);
						if(dest_i != i)
						{
							if(dest_i <= window_size / 2)
							{
								freq_real[dest_i] = freq_real[i];
								freq_imag[dest_i] = freq_imag[i];
							}
							freq_real[i] = 0;
							freq_imag[i] = 0;
						}
					}
				}
				else
				if(scale > 1)
				{
					for(int i = window_size / 2 - 1; i >= min_freq; i--)
					{
						double destination = i * scale;
						int dest_i = (int)(destination + 0.5);
						if(dest_i != i)
						{
							if(dest_i <= window_size / 2)
							{
								freq_real[dest_i] = freq_real[i];
								freq_imag[dest_i] = freq_imag[i];
							}
							freq_real[i] = 0;
							freq_imag[i] = 0;
						}
					}
				}

				fft->symmetry(window_size, freq_real, freq_imag);
				current_progress += window_size;
				result = progress->update(current_progress);
			}
		}

// Do the reverse FFT on the entire window at once
		if(!result)
		{
			for(int i = 0; i < get_total_buffers() && !result; i++)
			{
				fft->do_fft(window_size,
					1,
					real_out[i],
					imag_out[i],
					real_in[i],
					imag_in[i]);
				current_progress += window_size;
				result = progress->update(current_progress);
			}
		}
	}
	else
	{
// Write samples
		int fragment_size = get_buffer_size();
		if(write_position + fragment_size > resampled_size[0])
			fragment_size = resampled_size[0] - write_position;
		for(int i = 0; i < get_total_buffers(); i++)
			memcpy(buffer[i], 
				real_in[i] + write_position, 
				fragment_size * sizeof(int64_t));
		write_length = fragment_size;
		write_position += fragment_size;
		current_progress += fragment_size;
		result = progress->update(current_progress);


// Finished
		if(write_position >= resampled_size[0]) result = 1;
	}



	return result;
}



int TimeStretch::load_defaults()
{
	char directory[BCTEXTLEN];

// set the default directory
	sprintf(directory, "%stimestretch.rc", BCASTDIR);
// load the defaults
	defaults = new Defaults(directory);
	defaults->load();

	scale = defaults->get("SCALE", (double)1);
	return 0;
}

int TimeStretch::save_defaults()
{
	defaults->update("SCALE", scale);
	defaults->save();
	return 0;
}
