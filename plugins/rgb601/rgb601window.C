#include "bcdisplayinfo.h"
#include "rgb601window.h"


RGB601Thread::RGB601Thread(RGB601Main *client)
 : Thread()
{
	this->client = client;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

RGB601Thread::~RGB601Thread()
{
// Window always deleted here
	delete window;
}
	
void RGB601Thread::run()
{
	BC_DisplayInfo info;
	window = new RGB601Window(client, 
		info.get_abs_cursor_x() - 105, 
		info.get_abs_cursor_y() - 100);
	window->create_objects();
	gui_started.unlock();
	int result = window->run_window();
	completion.unlock();
// Last command executed in thread
	if(result) client->client_side_close();
}






RGB601Window::RGB601Window(RGB601Main *client, int x, int y)
 : BC_Window(client->gui_string, 
	x,
	y,
	210, 
	200, 
	210, 
	200, 
	0, 
	0,
	1)
{ 
	this->client = client; 
}

RGB601Window::~RGB601Window()
{
}

int RGB601Window::create_objects()
{
	int x = 10, y = 10;
	
	add_tool(forward = new RGB601Direction(this, x, y, &client->config.direction, 1, "RGB -> 601"));
	y += 30;
	add_tool(reverse = new RGB601Direction(this, x, y, &client->config.direction, 2, "601 -> RGB"));

	show_window();
	flush();
	return 0;
}

void RGB601Window::update()
{
	forward->update(client->config.direction == 1);
	reverse->update(client->config.direction == 2);
}

int RGB601Window::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}

RGB601Direction::RGB601Direction(RGB601Window *window, int x, int y, int *output, int true_value, char *text)
 : BC_CheckBox(x, y, *output == true_value, text)
{
	this->output = output;
	this->true_value = true_value;
	this->window = window;
}
RGB601Direction::~RGB601Direction()
{
}
	
int RGB601Direction::handle_event()
{
	*output = get_value() ? true_value : 0;
	window->update();
	window->client->send_configure_change();
	return 1;
}

