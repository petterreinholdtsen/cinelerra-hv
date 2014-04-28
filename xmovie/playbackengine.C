#include "arender.h"
#include "asset.h"
#include "audiodevice.h"
#include "file.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "playbackengine.h"
#include "renderengine.h"
#include "transportque.h"
#include "vrender.h"

PlaybackEngine::PlaybackEngine(MWindow *mwindow)
 : Thread()
{
	reset_parameters();
	this->mwindow = mwindow;
	que = new TransportQue;
	command = new TransportCommand;
	tracking_lock = new Mutex;
	tracking_timer = new Timer;
	interrupt_lock = new Mutex;
}

PlaybackEngine::~PlaybackEngine()
{
	delete que;
	delete command;
	delete tracking_lock;
	delete tracking_timer;
	delete interrupt_lock;
}

int PlaybackEngine::reset_parameters()
{
	tracking_position = 0;
	tracking_active = 0;
	done = 0;
	playing_back = 0;
	scroll = 0;
	render_engine = 0;
	return 0;
}

// Get position for scrollbar and time box
int PlaybackEngine::current_position(double &percentage, double &seconds)
{
	tracking_lock->lock();
	percentage = tracking_position;
	seconds = tracking_time;

	if(tracking_active)
	{
		seconds += (double)tracking_timer->get_difference() / 1000.0;
	}
	tracking_lock->unlock();

	return 0;
}

void PlaybackEngine::update_tracking(double tracking_position, double tracking_time)
{
	tracking_lock->lock();
	this->tracking_position = tracking_position;
	this->tracking_time = tracking_time;
	tracking_timer->update();
	tracking_lock->unlock();
}

void PlaybackEngine::interrupt_playback(int wait_engine)
{
//printf("PlaybackEngine::interrupt_playback 1\n");
	interrupt_lock->lock();
//printf("PlaybackEngine::interrupt_playback 2\n");
	if(render_engine) render_engine->interrupt_playback(wait_engine);
//printf("PlaybackEngine::interrupt_playback 3\n");
	interrupt_lock->unlock();
//printf("PlaybackEngine::interrupt_playback 4\n");
}

void PlaybackEngine::run()
{
	startup_lock.unlock();
	
	do
	{
//printf("PlaybackEngine::run 1\n");
		que->output_lock->lock();

		if(!mwindow->asset) continue;
// Wait for last command to finish

		if(render_engine)
		{
			render_engine->join();
			interrupt_lock->lock();
			delete render_engine;
			render_engine = 0;
			interrupt_lock->unlock();
		}

// Load new command
		que->input_lock->lock();
		*command = que->command;
		que->command.reset();
		que->input_lock->unlock();

// Execute command
		switch(command->command)
		{
			case STOP_PLAYBACK:
				tracking_position = command->start_position;
				tracking_timer->update();
// Remember end position
				mwindow->current_position = tracking_position;
				break;

			case CURRENT_FRAME:
			case PLAY_FORWARD:
			case FRAME_FORWARD:
			case FRAME_REVERSE:
// Override queued position
				if(command->command == FRAME_FORWARD ||
					command->command == FRAME_REVERSE)
				{
					command->start_position = mwindow->current_position;
				}

//printf("PlaybackEngine::run 4 %f %f\n", tracking_position, command->start_position);
				tracking_position = command->start_position;
				tracking_timer->update();
				interrupt_lock->lock();
				render_engine = new RenderEngine(mwindow, this);
				render_engine->run_command();
				interrupt_lock->unlock();
				break;
		}
//printf("PlaybackEngine::run 5\n");
	}while(!done);

	complete.unlock();
}
