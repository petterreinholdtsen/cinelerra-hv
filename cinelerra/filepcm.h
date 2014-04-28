#ifndef FILEPCM_H
#define FILEPCM_H

#include "bitspopup.inc"
#include "byteorderpopup.inc"
#include "file.inc"
#include "filebase.h"
#include "guicast.h"

class FilePCM : public FileBase
{
public:
	FilePCM(Asset *asset, File *file);
	~FilePCM();

// basic commands for every file interpreter
	int open_file(int rd, int wr);
	int close_file_derived();
	int set_channel(int channel);
	long get_audio_length();
	long get_audio_position();
	long get_memory_usage();
	int set_audio_position(long x);
	int seek_end();
	int seek_start();
	int write_samples(float **buffer, 
			PluginBuffer *audio_ram, 
			long byte_offset, 
			long allocated_samples, 
			long len);
	int read_samples(PluginBuffer *shared_buffer, long offset, long len, int feather, 
		long lfeather_len, float lfeather_gain, float lfeather_slope);


	static void get_parameters(BC_WindowBase *parent_window, 
		Asset *asset, 
		BC_WindowBase* &format_window,
		int audio_options,
		int video_options);

// PCM file 
	int read_header();     // get format information

	int write_header();
	int read_raw(char *buffer, long len);

	FILE *stream;
private:
	int reset_parameters_derived();
};

class FileWAV : public FilePCM
{
public:
	FileWAV(Asset *asset, File *file);
	~FileWAV();

	static void get_parameters(BC_WindowBase *parent_window, 
		Asset *asset, 
		BC_WindowBase* &format_window,
		int audio_options,
		int video_options);
};


class PCMConfig : public BC_Window
{
public:
	PCMConfig(BC_WindowBase *parent_window, Asset *asset);
	~PCMConfig();

	int create_objects();
	int close_event();

	BC_WindowBase *parent_window;
	BitsPopup *bits_popup;
	ByteOrderPopup *byteorder_popup;
	Asset *asset;
};

class WAVConfig : public BC_Window
{
public:
	WAVConfig(BC_WindowBase *parent_window, Asset *asset);
	~WAVConfig();
	
	int create_objects();
	int close_event();

	BitsPopup *bits_popup;
	BC_WindowBase *parent_window;
	Asset *asset;
};

#endif
