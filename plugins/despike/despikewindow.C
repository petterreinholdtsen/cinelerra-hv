#include "bcdisplayinfo.h"
#include "despikewindow.h"

#include <string.h>

DespikeThread::DespikeThread(Despike *despike)
 : Thread()
{
	this->despike = despike;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

DespikeThread::~DespikeThread()
{
// Window always deleted here
	delete window;
}
	
void DespikeThread::run()
{
	BC_DisplayInfo info;
	window = new DespikeWindow(despike, 
		info.get_abs_cursor_x() - 125, 
		info.get_abs_cursor_y() - 115);
	window->create_objects();
	gui_started.unlock();
	int result = window->run_window();
	completion.unlock();
// Last command executed in thread
	if(result) despike->client_side_close();
}






DespikeWindow::DespikeWindow(Despike *despike, int x, int y)
 : BC_Window(despike->gui_string, 
 	x, 
	y, 
	230, 
	110, 
	230, 
	110, 
	0, 
	0,
	1)
{ 
	this->despike = despike; 
}

DespikeWindow::~DespikeWindow()
{
}

int DespikeWindow::create_objects()
{
	int x = 10, y = 10;
	add_tool(new BC_Title(5, y, "Maximum level:"));
	y += 20;
	add_tool(level = new DespikeLevel(despike, x, y));
	y += 30;
	add_tool(new BC_Title(5, y, "Maximum rate of change:"));
	y += 20;
	add_tool(slope = new DespikeSlope(despike, x, y));
	show_window();
	flush();
	return 0;
}

int DespikeWindow::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}





DespikeLevel::DespikeLevel(Despike *despike, int x, int y)
 : BC_FSlider(x, 
 	y, 
	0,
	200,
	200,
	INFINITYGAIN, 
	0,
	despike->config.level)
{
	this->despike = despike;
}
int DespikeLevel::handle_event()
{
	despike->config.level = get_value();
	despike->send_configure_change();
	return 1;
}

DespikeSlope::DespikeSlope(Despike *despike, int x, int y)
 : BC_FSlider(x, 
 	y, 
	0,
	200,
	200,
	INFINITYGAIN, 
	0,
	despike->config.slope)
{
	this->despike = despike;
}
int DespikeSlope::handle_event()
{
	despike->config.slope = get_value();
	despike->send_configure_change();
	return 1;
}
