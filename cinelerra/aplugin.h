#ifndef APLUGIN_H
#define APLUGIN_H

#include "amodule.inc"
#include "mwindow.inc"
#include "plugin.h"

class APlugin : public Plugin
{
public:
	APlugin(EDL *edl, PluginSet *plugin_set);
	~APlugin();

	int create_objects(int x, int y);
	int use_gui();

	AModule *amodule;
};

#endif
