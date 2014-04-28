#include "amodule.h"
#include "aplugin.h"
#include "modules.h"
#include "pluginpopup.h"
#include <string.h>


// plugin
APlugin::APlugin(EDL *edl, PluginSet *plugin_set)
 : Plugin(edl, plugin_set, "")
{
}


APlugin::~APlugin()
{
}

int APlugin::create_objects(int x, int y)
{
	if(amodule->gui)
	{
//		amodule->gui->add_subwindow(plugin_popup = new PluginPopup(this, x, y));
		amodule->gui->add_subwindow(show_toggle = new PluginShowToggle(this, amodule->modules->console, x + 10, y + 23));
		amodule->gui->add_subwindow(show_title = new BC_Title(x + 30, y + 25, "Show", SMALLFONT));
		amodule->gui->add_subwindow(on_toggle = new PluginOnToggle(this, amodule->modules->console, x + 60, y + 23));
		amodule->gui->add_subwindow(on_title = new BC_Title(x + 80, y + 25, "On", SMALLFONT));
	}
	return 0;
}

int APlugin::use_gui()
{
	return (amodule->gui != 0);
}

