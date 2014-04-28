#ifndef RENDERENGINE_H
#define RENDERENGINE_H


class RenderEngine;

#include "arender.inc"
#include "audiodevice.inc"
#include "cache.inc"
#include "canvas.inc"
#include "channel.inc"
#include "mwindow.inc"
#include "playbackengine.inc"
#include "pluginserver.inc"
#include "preferences.inc"
#include "thread.h"
#include "transportque.inc"
#include "videodevice.inc"
#include "vrender.inc"

class RenderEngine : public Thread
{
public:
	RenderEngine(PlaybackEngine *playback_engine,
		Preferences *preferences, 
		TransportCommand *command, 
		Canvas *output,
		ArrayList<PluginServer*> *plugindb,
		ArrayList<Channel*> *channeldb,
		int head_number);
	~RenderEngine();

	int total_playable_channels();
// Translate sequential to subscript for EDL array
	int playable_channel_number(int number);
	void get_duty();
	void create_render_threads();
	void arm_render_threads();
	void start_render_threads();
	void wait_render_threads();
	void interrupt_playback();
	int get_output_w();
	int get_output_h();
// Get current channel for the BUZ output
	Channel* get_current_channel();
	double get_tracking_position();
// Find the plugin whose title matches title and return it
	PluginServer* scan_plugindb(char *title);
	CICache* get_acache();
	CICache* get_vcache();
	void set_acache(CICache *cache);
	void set_vcache(CICache *cache);
// Get levels for tracking
	void get_output_levels(double *levels, long position);
	void get_module_levels(ArrayList<double> *module_levels, long position);

	void run();
// Sends the command sequence, compensating for network latency
	int arm_command(TransportCommand *command,
		int &current_vchannel, 
		int &current_achannel);
// Start the command
	int start_command();

	int open_output();
	int close_output();
// return position to synchronize video against
	long sync_position();
// return samples since start of playback
	long session_position();

// Update preferences window
	void update_framerate(float framerate);

	int head_number;
// Copy of command
	TransportCommand *command;
// EDL to be used by renderengine since not all commands involve an EDL change
	EDL *edl;
// Pointer to playback config for one head
	PlaybackConfig *config;
// Defined only for the master render engine
	PlaybackEngine *playback_engine;
// Copy of preferences
	Preferences *preferences;
// Canvas if being used for CWindow
	Canvas *output;
// Lock out new commands until completion
	Mutex input_lock;
// Lock out interrupts until started
	Mutex start_lock;
	Mutex output_lock;
	int done;
	AudioDevice *audio;
	VideoDevice *video;
	ARender *arender;
	VRender *vrender;
	int do_audio;
	int do_video;
// Timer for synchronization without audio
	Timer timer;
	float actual_frame_rate;
// If the termination came from interrupt or end of selection
	int interrupted;

	ArrayList<PluginServer*> *plugindb;
// Channels for the BUZ output
	ArrayList<Channel*> *channeldb;

// length to send to audio device after speed adjustment
	long adjusted_fragment_len;              
// Lock out interrupts before and after renderengine is active
	Mutex interrupt_lock;
// CICaches for use if no playbackengine exists
	CICache *audio_cache, *video_cache;
















// constructing with an audio device forces output buffer allocation
// constructing without an audio device puts in one buffer at a time mode
	RenderEngine(MWindow *mwindow, 
		AudioDevice *audio = 0, 
		VideoDevice *video = 0,
		PlaybackEngine *playbackengine = 0);

// buffersize is in samples
	int reset_parameters();

	int arm_playback_common(long start_sample, 
			long end_sample,
			long current_sample,
			int reverse, 
			float speed, 
			int follow_loop,
			int infinite);

	int arm_playback_audio(long input_length, 
			long amodule_render_fragment, 
			long playback_buffer, 
			long output_length, 
			int audio_channels);

	int arm_playback_video(int every_frame, 
			long read_length, 
			long output_length,
			int track_w,
			int track_h,
			int output_w,
			int output_h);

	long get_correction_factor(int reset);     // calling it resets the correction factor

// start video since vrender is the master
	int start_video();

	int wait_for_startup();
	int wait_for_completion();

// information for playback
	int reverse;
	int follow_loop;       // loop if mwindow is looped
	int infinite;          // don't stop rendering at the end of the range or loops

	long start_position;      // lowest numbered sample in playback range
	long end_position;        // highest numbered sample in playback range
	long current_sample;
	int audio_channels;
	int every_frame;

	MWindow *mwindow;
};








#endif
