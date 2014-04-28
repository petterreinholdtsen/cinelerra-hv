#include "bcdisplayinfo.h"
#include "colorbalancewindow.h"


#include <libintl.h>
#define _(String) gettext(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)





PLUGIN_THREAD_OBJECT(ColorBalanceMain, ColorBalanceThread, ColorBalanceWindow)






ColorBalanceWindow::ColorBalanceWindow(ColorBalanceMain *client, int x, int y)
 : BC_Window(client->gui_string, x,
 	y,
	330, 
	190, 
	330, 
	190, 
	0, 
	0)
{ 
	this->client = client; 
}

ColorBalanceWindow::~ColorBalanceWindow()
{
}

int ColorBalanceWindow::create_objects()
{
	int x = 10, y = 10;
	add_tool(new BC_Title(x, y, _("Color Balance")));
	y += 25;
	add_tool(new BC_Title(x, y, _("Cyan")));
	add_tool(cyan = new ColorBalanceSlider(client, &(client->config.cyan), x + 70, y));
	add_tool(new BC_Title(x + 270, y, _("Red")));
	y += 25;
	add_tool(new BC_Title(x, y, _("Magenta")));
	add_tool(magenta = new ColorBalanceSlider(client, &(client->config.magenta), x + 70, y));
	add_tool(new BC_Title(x + 270, y, _("Green")));
	y += 25;
	add_tool(new BC_Title(x, y, _("Yellow")));
	add_tool(yellow = new ColorBalanceSlider(client, &(client->config.yellow), x + 70, y));
	add_tool(new BC_Title(x + 270, y, _("Blue")));
	y += 25;
	add_tool(preserve = new ColorBalancePreserve(client, x + 70, y));
	y += preserve->get_h() + 10;
	add_tool(lock_params = new ColorBalanceLock(client, x + 70, y));
	y += lock_params->get_h() + 10;
	add_tool(new ColorBalanceWhite(client, x, y));

	show_window();
	flush();
	return 0;
}

WINDOW_CLOSE_EVENT(ColorBalanceWindow)

ColorBalanceSlider::ColorBalanceSlider(ColorBalanceMain *client, float *output, int x, int y)
 : BC_ISlider(x, 
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
    old_value = *output;
}
ColorBalanceSlider::~ColorBalanceSlider()
{
}
int ColorBalanceSlider::handle_event()
{
	float difference = get_value() - *output;
	*output = get_value();
    client->synchronize_params(this, difference);
	client->send_configure_change();
	return 1;
}

ColorBalancePreserve::ColorBalancePreserve(ColorBalanceMain *client, int x, int y)
 : BC_CheckBox(x, 
 	y, 
	client->config.preserve, 
	_("Preserve luminosity"))
{
	this->client = client;
}
ColorBalancePreserve::~ColorBalancePreserve()
{
}

int ColorBalancePreserve::handle_event()
{
	client->config.preserve = get_value();
	client->send_configure_change();
	return 1;
}

ColorBalanceLock::ColorBalanceLock(ColorBalanceMain *client, int x, int y)
 : BC_CheckBox(x, 
 	y, 
	client->config.lock_params, 
	_("Lock parameters"))
{
	this->client = client;
}
ColorBalanceLock::~ColorBalanceLock()
{
}

int ColorBalanceLock::handle_event()
{
	client->config.lock_params = get_value();
	client->send_configure_change();
	return 1;
}


ColorBalanceWhite::ColorBalanceWhite(ColorBalanceMain *plugin, int x, int y)
 : BC_GenericButton(x, y, "White balance")
{
	this->plugin = plugin;
}

int ColorBalanceWhite::handle_event()
{
	return 1;
}


