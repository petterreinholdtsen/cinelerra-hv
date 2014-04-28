#ifndef FILEVORBIS_H
#define FILEVORBIS_H

#include "file.inc"
#include "filebase.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"
#include "mutex.h"
#include "resample.inc"






class FileVorbis : public FileBase
{
public:
	FileVorbis(Asset *asset, File *file, ArrayList<PluginServer*> *plugindb);
	~FileVorbis();

	static void get_parameters(BC_WindowBase *parent_window, 
		Asset *asset, 
		BC_WindowBase* &format_window,
		int audio_options,
		int video_options);

	static int check_sig(Asset *asset);
	int open_file(int rd, int wr);
	int close_file();
	int set_video_position(long x);
	int set_audio_position(long x);
	int write_samples(double **buffer, 
			long len);
	int write_frames(VFrame ***frames, int len);

	int read_frame(VFrame *frame);
	int read_samples(double *buffer, long len);

// Direct copy routines
	int get_best_colormodel(int driver, int colormodel);
// This file can copy frames directly from the asset
	int can_copy_from(Edit *edit, long position); 
	static char *strtocompression(char *string);
	static char *compressiontostr(char *string);

private:
	FileVorbisBuffer* oldest_buffer();
	void to_streamchannel(int channel, int &stream_out, int &channel_out);
	void new_audio_temp(long len);
	int reset_parameters_derived();
// File descriptor for decoder
	mpeg3_t *fd;

// Fork for video encoder
	ThreadFork *video_out;

// Fork for audio encoder
	ThreadFork *audio_out;
	
	ArrayList<PluginServer*> *plugindb;

// Temporary for color conversion
	VFrame *temp_frame;
	long last_sample;

	FileVorbisBuffer history[HISTORY_SIZE];
	
	unsigned char *audio_temp;
	long audio_allocation;
};


class VorbisConfigAudioPopup;
class VorbisABitrate;


class VorbisConfigAudio : public BC_Window
{
public:
	VorbisConfigAudio(BC_WindowBase *parent_window, Asset *asset);
	~VorbisConfigAudio();

	int create_objects();
	int close_event();

	BC_WindowBase *parent_window;
	VorbisABitrate *bitrate;
	char string[BCTEXTLEN];
	Asset *asset;
};


class VorbisLayer : public BC_PopupMenu
{
public:
	VorbisLayer(int x, int y, VorbisConfigAudio *gui);
	void create_objects();
	int handle_event();
	static int string_to_layer(char *string);
	static char* layer_to_string(int derivative);
	
	VorbisConfigAudio *gui;
};

class VorbisABitrate : public BC_PopupMenu
{
public:
	VorbisABitrate(int x, int y, VorbisConfigAudio *gui);

	void create_objects();
	void set_layer(int layer);

	int handle_event();
	static int string_to_bitrate(char *string);
	static char* bitrate_to_string(char *string, int bitrate);
	
	VorbisConfigAudio *gui;
};


#endif
