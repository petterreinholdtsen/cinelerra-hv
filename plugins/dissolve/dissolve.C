#include "dissolve.h"
#include "edl.inc"
#include "overlayframe.h"
#include "picon_png.h"
#include "vframe.h"

PluginClient* new_plugin(PluginServer *server)
{
	return new DissolveMain(server);
}





DissolveMain::DissolveMain(PluginServer *server)
 : PluginVClient(server)
{
}

DissolveMain::~DissolveMain()
{
}

char* DissolveMain::plugin_title() { return "Dissolve"; }
int DissolveMain::is_video() { return 1; }
int DissolveMain::is_transition() { return 1; }
int DissolveMain::uses_gui() { return 0; }

VFrame* DissolveMain::new_picon()
{
	return new VFrame(picon_png);
}


int DissolveMain::start_realtime()
{
	overlayer = new OverlayFrame(get_project_smp() + 1);
	return 0;
}

int DissolveMain::stop_realtime()
{
	delete overlayer;
	return 0;
}

int DissolveMain::process_realtime(VFrame *incoming, VFrame *outgoing)
{
	float fade = (float)PluginClient::get_source_position() / 
			PluginClient::get_total_len();

//printf("DissolveMain::process_realtime %f\n", fade);
	overlayer->overlay(outgoing, 
		incoming, 
		0, 
		0, 
		incoming->get_w(),
		incoming->get_h(),
		0,
		0,
		incoming->get_w(),
		incoming->get_h(),
		fade,
		TRANSFER_NORMAL,
		NEAREST_NEIGHBOR);

	return 0;
}
