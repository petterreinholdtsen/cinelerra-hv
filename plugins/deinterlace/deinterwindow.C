#include "bcdisplayinfo.h"
#include "deinterwindow.h"


DeInterlaceThread::DeInterlaceThread(DeInterlaceMain *client)
 : Thread()
{
	this->client = client;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

DeInterlaceThread::~DeInterlaceThread()
{
	delete window;
}

void DeInterlaceThread::run()
{
	BC_DisplayInfo info;
	window = new DeInterlaceWindow(client, 
		info.get_abs_cursor_x() - 30, 
		info.get_abs_cursor_y() - 30);
	window->create_objects();

	gui_started.unlock();
	int result = window->run_window();
	completion.unlock();
	if(result) client->client_side_close();
}






DeInterlaceWindow::DeInterlaceWindow(DeInterlaceMain *client, int x, int y)
 : BC_Window(client->gui_string, 
 	x, 
	y, 
	200, 
	180, 
	200, 
	180, 
	0, 
	1)
{ 
	this->client = client; 
}

DeInterlaceWindow::~DeInterlaceWindow()
{
}

int DeInterlaceWindow::create_objects()
{
	int x = 10, y = 10;
	add_tool(new BC_Title(x, y, "Select lines to keep"));
	y += 25;
	add_tool(odd_fields = new DeInterlaceOption(client, this, DEINTERLACE_EVEN, x, y, "Odd lines"));
	y += 25;
	add_tool(even_fields = new DeInterlaceOption(client, this, DEINTERLACE_ODD, x, y, "Even lines"));
	y += 25;
	add_tool(average_fields = new DeInterlaceOption(client, this, DEINTERLACE_AVG, x, y, "Average lines"));
	y += 25;
	add_tool(swap_fields = new DeInterlaceOption(client, this, DEINTERLACE_SWAP, x, y, "Swap lines"));
	return 0;
}

int DeInterlaceWindow::close_event()
{
	hide_window();
	client->send_hide_gui();
}

int DeInterlaceWindow::set_mode(int mode, int recursive)
{
	odd_fields->update(mode == DEINTERLACE_EVEN);
	even_fields->update(mode == DEINTERLACE_ODD);
	average_fields->update(mode == DEINTERLACE_AVG);
	swap_fields->update(mode == DEINTERLACE_SWAP);

	client->config.mode = mode;
	
	if(!recursive)
		client->send_configure_change();
}


DeInterlaceOption::DeInterlaceOption(DeInterlaceMain *client, 
		DeInterlaceWindow *window, 
		int output, 
		int x, 
		int y, 
		char *text)
 : BC_Radial(x, y, client->config.mode == output, text)
{
	this->client = client;
	this->window = window;
	this->output = output;
}

DeInterlaceOption::~DeInterlaceOption()
{
}
int DeInterlaceOption::handle_event()
{
	window->set_mode(output, 0);
	return 1;
}
