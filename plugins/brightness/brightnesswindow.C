#include "bcdisplayinfo.h"
#include "brightnesswindow.h"


BrightnessThread::BrightnessThread(BrightnessMain *client)
 : Thread()
{
	this->client = client;
	set_synchronous(0);
	gui_started.lock();
	completion.lock();
}

BrightnessThread::~BrightnessThread()
{
// Window always deleted here
	delete window;
}

void BrightnessThread::run()
{
	BC_DisplayInfo info;
	window = new BrightnessWindow(client, 
		info.get_abs_cursor_x() - 165, 
		info.get_abs_cursor_y() - 80);
	window->create_objects();
	int result = window->run_window();
	completion.unlock();

// Last command executed in thread
	if(result) client->client_side_close();
}






BrightnessWindow::BrightnessWindow(BrightnessMain *client, int x, int y)
 : BC_Window(client->gui_string, x,
 	y,
	330, 
	160, 
	330, 
	160, 
	0, 
	0)
{ 
	this->client = client; 
}

BrightnessWindow::~BrightnessWindow()
{
}

int BrightnessWindow::create_objects()
{
	int x = 10, y = 10;
	add_tool(new BC_Title(x, y, "Brightness/Contrast"));
	y += 25;
	add_tool(new BC_Title(x, y, "Brightness:"));
	add_tool(brightness = new BrightnessSlider(client, 
		&(client->config.brightness), 
		x + 80, 
		y));
	y += 25;
	add_tool(new BC_Title(x, y, "Contrast:"));
	add_tool(contrast = new BrightnessSlider(client, 
		&(client->config.contrast), 
		x + 80, 
		y));
	y += 30;
	add_tool(luma = new BrightnessLuma(client, 
		x, 
		y));
	show_window();
	flush();
	return 0;
}

int BrightnessWindow::close_event()
{
// Set result to 1 to indicate a client side close
	set_done(1);
	return 1;
}

BrightnessSlider::BrightnessSlider(BrightnessMain *client, 
	float *output, 
	int x, 
	int y)
 : BC_FSlider(x, 
 	y, 
	0, 
	200, 
	200,
	-100, 
	100, 
	(int)*output)
{
	this->client = client;
	this->output = output;
}
BrightnessSlider::~BrightnessSlider()
{
}
int BrightnessSlider::handle_event()
{
	*output = get_value();
	client->send_configure_change();
	return 1;
}

BrightnessLuma::BrightnessLuma(BrightnessMain *client, 
	int x, 
	int y)
 : BC_CheckBox(x, 
 	y, 
	client->config.luma,
	"Boost luminance only")
{
	this->client = client;
}
BrightnessLuma::~BrightnessLuma()
{
}
int BrightnessLuma::handle_event()
{
	client->config.luma = get_value();
	client->send_configure_change();
	return 1;
}
