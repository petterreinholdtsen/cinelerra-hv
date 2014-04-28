
/*
 * CINELERRA
 * Copyright (C) 2010 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef VOCODER_H
#define VOCODER_H


#include "bchash.inc"
#include "guicast.h"
#include "mutex.h"
#include "../parametric/fourier.h"
#include "pluginaclient.h"
#include "vframe.inc"


// This vocoder multiplies the FFT of all the tracks & writes the output
// to a user specified output track.


#define WINDOW_SIZE 16384

class Vocoder;
class VocoderWindow;






class VocoderConfig
{
public:
	VocoderConfig();

	int equivalent(VocoderConfig &that);
	void copy_from(VocoderConfig &that);
	void interpolate(VocoderConfig &prev, 
		VocoderConfig &next, 
		int64_t prev_frame, 
		int64_t next_frame, 
		int64_t current_frame);

	float wetness;
	int carrier_track;
	float bandwidth;
};






class VocoderWetness : public BC_FPot
{
public:
	VocoderWetness(Vocoder *plugin, int x, int y);
	int handle_event();
	Vocoder *plugin;
};




class VocoderCarrier : public BC_TumbleTextBox
{
public:
	VocoderCarrier(Vocoder *plugin, 
		VocoderWindow *window, 
		int x, 
		int y);
	int handle_event();
	Vocoder *plugin;
};





class VocoderWindow : public PluginClientWindow
{
public:
	VocoderWindow(Vocoder *plugin);
	~VocoderWindow();

	void create_objects();
	void update_gui();

	VocoderCarrier  *output;
	VocoderWetness *wetness;
	Vocoder *plugin;
};

class VocoderFFT : public CrossfadeFFT
{
public:
	VocoderFFT(Vocoder *plugin);
	~VocoderFFT();
	
	int signal_process();
	int read_samples(int64_t output_sample, 
		int samples, 
		Samples *buffer);

	Vocoder *plugin;
	int is_carrier;
	int channel;
};


class Vocoder : public PluginAClient
{
public:
	Vocoder(PluginServer *server);
	~Vocoder();

	int is_realtime();
	int is_multichannel();
	void read_data(KeyFrame *keyframe);
	void save_data(KeyFrame *keyframe);
	int process_buffer(int64_t size, 
		Samples **buffer, 
		int64_t start_position,
		int sample_rate);

	int load_defaults();
	int save_defaults();
	void reset();
	void reconfigure();
	void update_gui();


	int need_reconfigure;
	PLUGIN_CLASS_MEMBERS(VocoderConfig)
// 1 FFT for each track
	VocoderFFT **fft;

// Carrier windows
// Need 1 buffer for each FFT window
	double **carrier_real;
    double **carrier_imag;
	int carrier_windows;
	int current_window;
};



#endif
