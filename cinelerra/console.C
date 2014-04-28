#include "console.h"
#include "consolescroll.h"
#include "defaults.h"
#include "mainmenu.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "modules.h"
#include "mainsession.h"

Console::Console(MWindow *mwindow) : Thread()
{
	this->mwindow = mwindow;
	pixel_start = 0;
	button_down = 0;
	gui = 0;
//	modules = 0;
	reconfigure_trigger = 0;
}

Console::~Console()
{
	if(gui) 
	{
		gui->set_done(0);
		join();
	}
	
//	delete modules;
	if(gui) delete gui;
	gui = 0;
// only pointers are removed here
}

int Console::create_objects(int w, int h, int console_hidden, int vertical)
{
	set_synchronous(1);
	this->vertical = vertical;

	if(mwindow->gui)	
	{
		if(vertical) 
		h = CONSOLEH;
		else 
		w = CONSOLEW;

		gui = new ConsoleWindow(mwindow, w, h, console_hidden);
		gui->create_objects();
	}

//	modules = new Modules(mwindow, this);
}

int Console::flip_vertical()
{
	if(gui)
	{
		int pixel = 0;
//		modules->flip_vertical(pixel, pixel_start);
	}
}

int Console::redo_pixels()
{
	if(gui)
	{
		int pixel = 0;
//		modules->redo_pixels(pixel);
		gui->scroll->update();
	}
}

void Console::run()
{
	if(gui) gui->run_window();
}

int Console::update_defaults(Defaults *defaults)
{
	if(gui)
	{
		defaults->update("CONSOLEW", gui->get_w());
		defaults->update("CONSOLEH", gui->get_h());
		defaults->update("CONSOLEVERTICAL", vertical);
	}
}

int Console::delete_project()
{
//	modules->delete_all();
}

int Console::change_channels(int oldchannels, int newchannels)
{
//	modules->change_channels(oldchannels, newchannels);
}

int Console::pixelmovement(int distance)
{
	if(gui)
	{
		pixel_start += distance;
//		modules->pixelmovement(-distance);
	}
}

int Console::add_audio_track()
{
//	modules->add_audio_track();
	if(gui) gui->scroll->update();
}

int Console::add_video_track()
{
//	modules->add_video_track();
	if(gui) gui->scroll->update();
}

int Console::delete_track()
{
//	modules->delete_track();
	if(gui) gui->scroll->update();
}


int Console::start_reconfigure(int unlock_window)
{
// 	if(mwindow->session->is_playing_back)
// 	{
// // call stop_playback here because calling from console screws up window locking
// 		if(unlock_window) gui->unlock_window();
// 		mwindow->start_reconfigure(0);
// 		//sleep(1);
// 	}
}

int Console::stop_reconfigure(int unlock_window)
{
// 	if(mwindow->session->is_playing_back)
// 	{
// 		mwindow->stop_reconfigure(0);
// 		if(unlock_window) gui->lock_window();
// 	}
}

int Console::toggles_selected(int on, int show, int mute)
{
//	return modules->toggles_selected(on, show, mute);
}

int Console::select_all_toggles(int on, int show, int mute)
{
//	return modules->select_all_toggles(on, show, mute);
}

int Console::deselect_all_toggles(int on, int show, int mute)
{
//	return modules->deselect_all_toggles(on, show, mute);
}








ConsoleWindow::ConsoleWindow(MWindow *mwindow, int w, int h, int console_hidden)
 : BC_Window(PROGRAM_NAME ": Console", 
 		(int)INFINITY, 
		(int)INFINITY, 
		640, 
		480,
		-1, 
		-1,
		1,
		0,
		1)
{
	this->mwindow = mwindow;
	console = mwindow->console;
}

ConsoleWindow::~ConsoleWindow()
{
	delete scroll;
}

int ConsoleWindow::create_objects()
{
	scroll = new ConsoleMainScroll(this);
	scroll->create_objects(get_w(), get_h());
}


int ConsoleWindow::resize_event(int w, int h)
{
	if(console->vertical && h < CONSOLEH - CONSOLEH / 3) { flip_vertical(w, h); }
	else
	if(!console->vertical && w < CONSOLEW - CONSOLEW / 3) { flip_vertical(w, h); }
	else
	{
		int need_resize = 0;

		if(console->vertical)
		{
			if(h != CONSOLEH) { h = CONSOLEH; need_resize = 1; }
		}
		else 
		{
			if(w != CONSOLEW) { w = CONSOLEW; need_resize = 1; }
		}
		
		if(need_resize)
		{
			resize_window(w, h);
		}
		scroll->resize_event(w, h);
	}
}

int ConsoleWindow::flip_vertical(int w, int h)
{
	console->vertical ^= 1;

	if(console->vertical) 
	{ resize_window(w, CONSOLEH); }
	else 
	{ resize_window(CONSOLEW, h); }

	scroll->flip_vertical(this->get_w(), this->get_h());
	console->flip_vertical();
}

int ConsoleWindow::close_event()
{
	hide_window();
	mwindow->gui->mainmenu->set_show_console(0);
}

int ConsoleWindow::keypress_event()
{
// locks up for some reason
	if(get_keypress() == 'w')
	{
		mwindow->gui->mainmenu->set_show_console(0);
		hide_window();
		return 1;
	}
	return 0;
}

int ConsoleWindow::button_release()
{
	if(console->button_down && console->reconfigure_trigger)
	{
		console->button_down = 0;
		console->reconfigure_trigger = 0;
// restart the playback
		console->start_reconfigure(1);
		console->stop_reconfigure(1);
		return 1;
	}
	console->button_down = 0;
	return 0;
}
