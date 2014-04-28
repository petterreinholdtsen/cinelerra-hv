#ifndef PLAYBACKSCROLL_H
#define PLAYBACKSCROLL_H

#include "mutex.inc"
#include "mwindow.inc"
#include "thread.h"





class PlaybackScroll : public Thread
{
public:
	PlaybackScroll(MWindow *mwindow);
	~PlaybackScroll();
	
	void create_objects();
	int start_playback();
	int stop_playback();
	void run();
	
	Mutex *pause_lock, *loop_lock, *startup_lock;
	MWindow *mwindow;
	int done;
};




#endif
