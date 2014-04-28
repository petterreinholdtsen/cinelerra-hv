#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "arender.inc"
#include "mutex.inc"
#include "mwindow.inc"
#include "playbackengine.inc"
#include "timer.inc"
#include "transportque.inc"
#include "vrender.inc"

class RenderEngine : public Thread
{
public:
	RenderEngine(MWindow *mwindow, PlaybackEngine *playback_engine);
	~RenderEngine();

	void run_command();
	void reset();
	void interrupt_playback(int wait_engine);
	void run();
	long sync_sample();

	ARender *arender;
	VRender *vrender;
	TransportCommand *command;
	MWindow *mwindow;
	PlaybackEngine *playback_engine;
	Mutex *input_lock;
	Mutex *interrupt_lock;
	Mutex *completion_lock;
	Timer *sync_timer;
// Kernel side decryption is not threaded during seeking
	Mutex *seek_lock;
};

#endif
