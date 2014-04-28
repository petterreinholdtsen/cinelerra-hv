#include "arender.h"
#include "asset.h"
#include "mutex.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "renderengine.h"
#include "playbackengine.h"
#include "playbackscroll.h"
#include "timer.h"
#include "transportque.h"
#include "vrender.h"


RenderEngine::RenderEngine(MWindow *mwindow, PlaybackEngine *playback_engine)
{
	reset();
	this->mwindow = mwindow;
	this->playback_engine = playback_engine;
	input_lock = new Mutex;
	command = new TransportCommand;
	*command = *playback_engine->command;
	set_synchronous(1);
	sync_timer = new Timer;
	interrupt_lock = new Mutex;
	completion_lock = new Mutex;
	seek_lock = new Mutex;
}


RenderEngine::~RenderEngine()
{
	delete command;
	delete input_lock;
	delete sync_timer;
	delete interrupt_lock;
	delete completion_lock;
	delete seek_lock;
}

void RenderEngine::reset()
{
	arender = 0;
	vrender = 0;
}

void RenderEngine::run_command()
{
	sync_timer->update();
	completion_lock->lock();
	Thread::start();
}

long RenderEngine::sync_sample()
{
	if(mwindow->software_sync || !arender)
	{
		return sync_timer->get_scaled_difference(mwindow->asset->rate);
	}
	else
		return arender->sync_sample();
}

void RenderEngine::interrupt_playback(int wait_engine)
{
//printf("RenderEngine::interrupt_playback 1\n");
	interrupt_lock->lock();
//printf("RenderEngine::interrupt_playback 1.1\n");
	if(arender)
	{
		arender->interrupt_playback();
	}
//printf("RenderEngine::interrupt_playback 2\n");
	if(vrender)
	{
		vrender->interrupt_playback();
	}
//printf("RenderEngine::interrupt_playback 3\n");
	interrupt_lock->unlock();
//printf("RenderEngine::interrupt_playback 4\n");

	if(wait_engine)
	{
		completion_lock->lock();
		completion_lock->unlock();
	}
//printf("RenderEngine::interrupt_playback 5\n");
}

void RenderEngine::run()
{
//printf("RenderEngine::run 1\n");
	interrupt_lock->lock();
	if(mwindow->asset->video_data)
	{
		vrender = new VRender(mwindow, this);
		vrender->arm_playback();
	}
//printf("RenderEngine::run 2\n");

	if(mwindow->asset->audio_data)
	{
		arender = new ARender(mwindow, this);
		arender->arm_playback();
	}
//printf("RenderEngine::run 3\n");

	playback_engine->tracking_active = 1;
	if(!command->single_frame()) mwindow->playback_scroll->start_playback();
//printf("RenderEngine::run 4\n");

	if(arender) arender->start_playback();
	if(vrender) vrender->start_playback();
//printf("RenderEngine::run 5\n");

	interrupt_lock->unlock();
	if(arender) arender->join();
	if(vrender) vrender->join();
//printf("RenderEngine::run 6\n");

	interrupt_lock->lock();
	if(arender) arender->close_playback();
	if(vrender) vrender->close_playback();
//printf("RenderEngine::run 7\n");

	if(arender) delete arender;
	if(vrender) delete vrender;
	reset();
	interrupt_lock->unlock();
//printf("RenderEngine::run 8\n");

	playback_engine->tracking_active = 0;

// Overridden by a STOP_PLAYBACK command.
	double percentage, seconds;
	playback_engine->current_position(percentage, seconds);

// Only store resulting position if the command caused a change
	if(command->change_position())
	{
		mwindow->current_position = percentage;
	}
//printf("RenderEngine::run 9 %f %f\n", percentage, seconds);

	if(!command->single_frame())
	{
		mwindow->playback_scroll->stop_playback();
	}
	else
	{
//printf("RenderEngine::run 10 %f %f %f\n", mwindow->current_position, percentage, seconds);
		mwindow->gui->lock_window();
		mwindow->gui->update_position(percentage, 
			seconds, 
			command->change_position());
		mwindow->gui->unlock_window();
//printf("RenderEngine::run 9.2 %f %f\n", percentage, seconds);
	}

	mwindow->gui->lock_window();
	mwindow->gui->playbutton->set_mode(0);
	mwindow->gui->unlock_window();
	completion_lock->unlock();
//printf("RenderEngine::run 10\n");
}
