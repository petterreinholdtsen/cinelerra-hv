#include "bcdisplayinfo.h"
#include "timeavgwindow.h"


TimeAvgThread::TimeAvgThread(TimeAvgMain *client)
 : Thread()
{
	this->client = client;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

TimeAvgThread::~TimeAvgThread()
{
	delete window;
}
	
void TimeAvgThread::run()
{
	BC_DisplayInfo info;
	window = new TimeAvgWindow(client, 
		info.get_abs_cursor_x() - 75, 
		info.get_abs_cursor_y() - 65);
	window->create_objects();

	gui_started.unlock();
	int result = window->run_window();
	completion.unlock();
	if(result) client->client_side_close();
}






TimeAvgWindow::TimeAvgWindow(TimeAvgMain *client, int x, int y)
 : BC_Window(client->gui_string, 
 	x, 
	y, 
	210, 
	80, 
	200, 
	80, 
	0, 
	0,
	1)
{ 
	this->client = client; 
}

TimeAvgWindow::~TimeAvgWindow()
{
}

int TimeAvgWindow::create_objects()
{
	int x = 10, y = 10;
	add_tool(new BC_Title(x, y, "Frames to average"));
	y += 20;
	add_tool(total_frames = new TimeAvgSlider(client, x, y));
	show_window();
	flush();
	return 0;
}

int TimeAvgWindow::close_event()
{
	set_done(1);
	return 1;
}

TimeAvgSlider::TimeAvgSlider(TimeAvgMain *client, int x, int y)
 : BC_ISlider(x, 
 	y, 
	0,
	190, 
	200, 
	1, 
	256, 
	client->config.frames)
{
	this->client = client;
}
TimeAvgSlider::~TimeAvgSlider()
{
}
int TimeAvgSlider::handle_event()
{
	int result = get_value();
	if(result < 1) result = 1;
	client->config.frames = result;
	client->send_configure_change();
	return 1;
}
