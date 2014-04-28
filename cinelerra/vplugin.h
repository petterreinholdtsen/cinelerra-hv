#ifndef VPLUGIN_H
#define VPLUGIN_H

#include "mwindow.inc"
#include "plugin.h"
#include "pluginset.h"
#include "vmodule.inc"

class VPlugin : public Plugin
{
public:
	VPlugin(EDL *edl, PluginSet *plugin_set);
	~VPlugin();

	int create_objects(int x, int y);
	int use_gui();       // whether or not the module has a gui

	VModule *vmodule;
};




#endif
