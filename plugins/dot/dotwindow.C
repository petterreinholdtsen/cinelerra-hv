#include "bcdisplayinfo.h"
#include "dotwindow.h"


DotThread::DotThread(DotMain *client)
 : Thread()
{
	this->client = client;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

DotThread::~DotThread()
{
// Window always deleted here
	delete window;
}
	
void DotThread::run()
{
	BC_DisplayInfo info;
	window = new DotWindow(client, 
		info.get_abs_cursor_x() - 105, 
		info.get_abs_cursor_y() - 100);
	window->create_objects();
	gui_started.unlock();
	int result = window->run_window();
	completion.unlock();
// Last command executed in thread
	if(result) client->client_side_close();
}






DotWindow::DotWindow(DotMain *client, int x, int y)
 : BC_Window(client->gui_string, 
	x,
	y,
	300, 
	170, 
	300, 
	170, 
	0, 
	0,
	1)
{ 
	this->client = client; 
}

DotWindow::~DotWindow()
{
}

int DotWindow::create_objects()
{
	int x = 10, y = 10;
	add_subwindow(new BC_Title(x, y, 
		"DotTV from EffectTV\n"
		"Copyright (C) 2001 FUKUCHI Kentarou"
	));

	show_window();
	flush();
	return 0;
}

int DotWindow::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}






