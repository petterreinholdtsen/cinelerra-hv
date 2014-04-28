#include "mutex.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "playbackengine.h"
#include "playbackscroll.h"
#include "timer.h"


PlaybackScroll::PlaybackScroll(MWindow *mwindow)
 : Thread()
{
	this->mwindow = mwindow;
	pause_lock = new Mutex;
	loop_lock = new Mutex;
	startup_lock = new Mutex;
	set_synchronous(1);
	done = 0;
}

PlaybackScroll::~PlaybackScroll()
{
	done = 1;
	pause_lock->unlock();
	Thread::join();
	delete pause_lock;
	delete loop_lock;
	delete startup_lock;
}

void PlaybackScroll::create_objects()
{
	pause_lock->lock();
	startup_lock->lock();
	Thread::start();
}


int PlaybackScroll::start_playback()
{
	pause_lock->unlock();
	return 0;
}

int PlaybackScroll::stop_playback()
{
	loop_lock->lock();
	pause_lock->lock();
	loop_lock->unlock();
	return 0;
}

void PlaybackScroll::run()
{
	double percentage, seconds;
	Timer timer;

	startup_lock->unlock();
//printf("PlaybackScroll::run 1\n", percentage);
	while(!done)
	{
		pause_lock->lock();
		pause_lock->unlock();

		loop_lock->lock();
		if(mwindow->engine->tracking_active)   
		{
			mwindow->engine->current_position(percentage, seconds);
			mwindow->gui->lock_window();
			mwindow->gui->update_position(percentage, seconds, 1);
//printf("PlaybackScroll::run 2 %f %f\n", percentage, seconds);
			mwindow->gui->flush();
			mwindow->gui->unlock_window();
		}

		timer.delay(100);
		loop_lock->unlock();
	}
}


