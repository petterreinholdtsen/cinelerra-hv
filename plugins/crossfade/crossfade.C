#include "crossfade.h"
#include "edl.inc"
#include "overlayframe.h"
#include "picon_png.h"
#include "vframe.h"

PluginClient* new_plugin(PluginServer *server)
{
	return new CrossfadeMain(server);
}





CrossfadeMain::CrossfadeMain(PluginServer *server)
 : PluginAClient(server)
{
}

CrossfadeMain::~CrossfadeMain()
{
}

char* CrossfadeMain::plugin_title() { return "Crossfade"; }
int CrossfadeMain::is_transition() { return 1; }
int CrossfadeMain::uses_gui() { return 0; }

VFrame* CrossfadeMain::new_picon()
{
	return new VFrame(picon_png);
}


int CrossfadeMain::start_realtime()
{
	return 0;
}

int CrossfadeMain::stop_realtime()
{
	return 0;
}

int CrossfadeMain::process_realtime(long size, 
	double *outgoing, 
	double *incoming)
{
	double intercept = (double)PluginClient::get_source_position() / 
		PluginClient::get_total_len();
	double slope = (double)1 / PluginClient::get_total_len();

	for(int i = 0; i < size; i++)
	{
		incoming[i] = outgoing[i] * ((double)1 - (slope * i + intercept)) + 
			incoming[i] * (slope * i + intercept);
	}

	return 0;
}
