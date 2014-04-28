#include "bcdisplayinfo.h"
#include "language.h"
#include "motion.h"
#include "motionwindow.h"






PLUGIN_THREAD_OBJECT(MotionMain, MotionThread, MotionWindow)



MotionWindow::MotionWindow(MotionMain *plugin, int x, int y)
 : BC_Window(plugin->gui_string, 
 	x,
	y,
	450, 
	630, 
	450,
	630,
	0, 
	1)
{
	this->plugin = plugin; 
}

MotionWindow::~MotionWindow()
{
}

int MotionWindow::create_objects()
{
	int x = 10, y = 10;
	BC_Title *title;

	add_subwindow(title = new BC_Title(x, y + 10, _("Search radius:")));
	add_subwindow(radius = new MotionSearchRadius(plugin, 
		x + title->get_w() + 10, 
		y));
	add_subwindow(radius_title = new BC_Title(x +  + title->get_w() + radius->get_w() + 20, 
		y + 10, 
		get_radius_title()));

	y += 40;
	add_subwindow(title = new BC_Title(x, y + 10, _("Search area:")));
	add_subwindow(block_size = new MotionBlockSize(plugin, 
		x + title->get_w() + 10, 
		y));
	add_subwindow(new BC_Title(x + title->get_w() + block_size->get_w() + 20, 
		y + 10, 
		"Percent of image"));

	y += 40;
	add_subwindow(title = new BC_Title(x, y + 10, _("Block X:")));
	add_subwindow(block_x = new MotionBlockX(plugin, 
		this, 
		x + title->get_w() + 10, 
		y));
	add_subwindow(block_x_text = new MotionBlockXText(plugin, 
		this, 
		x + title->get_w() + 10 + block_x->get_w() + 10, 
		y + 10));
	y += 40;
	add_subwindow(title = new BC_Title(x, y + 10, _("Block Y:")));
	add_subwindow(block_y = new MotionBlockY(plugin, 
		this, 
		x + title->get_w() + 10, 
		y));
	add_subwindow(block_y_text = new MotionBlockYText(plugin, 
		this, 
		x + title->get_w() + 10 + block_y->get_w() + 10, 
		y + 10));

//	y += 40;
//	add_subwindow(title = new BC_Title(x, y, _("Total blocks:")));
// 	add_subwindow(block_count = new MotionBlockCount(plugin, 
// 		x + title->get_w() + 10, 
// 		y, 
// 		75));
// 	block_count->create_objects();

	y += 40;
	add_subwindow(title = new BC_Title(x, y, _("Logarithmic search steps:")));
	add_subwindow(search_positions = new MotionSearchPositions(plugin, 
		x + title->get_w() + 10, 
		y, 
		100));
	search_positions->create_objects();

	y += 40;
	add_subwindow(title = new BC_Title(x, y + 10, _("Maximum absolute offset:")));
	add_subwindow(magnitude = new MotionMagnitude(plugin, 
		x + title->get_w() + 10, 
		y));

	y += 40;
	add_subwindow(title = new BC_Title(x, y + 10, _("Settling speed:")));
	add_subwindow(return_speed = new MotionReturnSpeed(plugin,
		x + title->get_w() + 10, 
		y));



	y += 40;
	add_subwindow(vectors = new MotionDrawVectors(plugin,
		this,
		x,
		y));


	y += 40;
	add_subwindow(track_single = new TrackSingleFrame(plugin, 
		this,
		x, 
		y));
	add_subwindow(new BC_Title(x + 200, y, _("Frame number:")));
	add_subwindow(track_frame_number = new TrackFrameNumber(plugin, 
		this,
		x + 320, 
		y));

	y += 20;
	add_subwindow(track_previous = new TrackPreviousFrame(plugin, 
		this,
		x, 
		y));


	y += 40;
	int y1 = y;
	add_subwindow(new BC_Title(x, y, "Master layer:"));
	y += 20;
	add_subwindow(master_layer_top = new MasterLayerTop(plugin, 
		this,
		x, 
		y));
	y += 20;
	add_subwindow(master_layer_bottom = new MasterLayerBottom(plugin, 
		this,
		x, 
		y));


	x += 150;
	y = y1;
	add_subwindow(new BC_Title(x, y, _("Mode:")));
	y += 20;
	add_subwindow(track = new MotionTrack(plugin, 
		this,
		x, 
		y));

	y += 20;
	add_subwindow(track_pixel = new MotionTrackPixel(plugin, 
		this,
		x, 
		y));

	y += 20;
	add_subwindow(stabilize = new MotionStabilize(plugin, 
		this,
		x, 
		y));


	y += 20;
	add_subwindow(stabilize_pixel = new MotionStabilizePixel(plugin, 
		this,
		x, 
		y));

	y += 20;
	add_subwindow(nothing = new MotionNothing(plugin,
		this,
		x,
		y));



	x += 180;
	y = y1;
	add_subwindow(new BC_Title(x, y, _("Direction:")));
	y += 20;
	add_subwindow(global = new MotionGlobal(plugin,
		this,
		x,
		y));

	y += 20;
	add_subwindow(rotate = new MotionRotate(plugin,
		this,
		x,
		y));




	x = 10;
	y += 90;
	add_subwindow(new BC_Title(x, y, "Optimization:"));
	y += 20;
	add_subwindow(nocalculate = new MotionNocalculate(plugin, 
		this,
		x,
		y));
	y += 20;
	add_subwindow(recalculate = new MotionRecalculate(plugin, 
		this,
		x,
		y));
	y += 20;
	add_subwindow(save = new MotionSave(plugin, 
		this,
		x,
		y));
	y += 20;
	add_subwindow(load = new MotionLoad(plugin, 
		this,
		x,
		y));

	show_window();
	flush();
	return 0;
}

char* MotionWindow::get_radius_title()
{
	if(plugin->config.global) return _("Percent of search area");
	return "Degrees";
}

void MotionWindow::update_mode()
{
	if(plugin->config.global)
	{
	 	radius->update(plugin->config.global_range,
	 		MIN_RADIUS,
	 		MAX_RADIUS);
	}
	else
	{
	 	radius->update(plugin->config.rotation_range,
	 		MIN_ROTATION,
	 		MAX_ROTATION);
	}
	stabilize->update(plugin->config.mode1 == MotionConfig::STABILIZE);
	track->update(plugin->config.mode1 == MotionConfig::TRACK);
	stabilize_pixel->update(plugin->config.mode1 == MotionConfig::STABILIZE_PIXEL);
	track_pixel->update(plugin->config.mode1 == MotionConfig::TRACK_PIXEL);
	nothing->update(plugin->config.mode1 == MotionConfig::NOTHING);
	vectors->update(plugin->config.draw_vectors);
	global->update(plugin->config.global);
	rotate->update(plugin->config.rotate);
	radius_title->update(get_radius_title());
	recalculate->update(plugin->config.mode3 == MotionConfig::RECALCULATE);
	nocalculate->update(plugin->config.mode3 == MotionConfig::NO_CALCULATE);
	save->update(plugin->config.mode3 == MotionConfig::SAVE);
	load->update(plugin->config.mode3 == MotionConfig::LOAD);
}


WINDOW_CLOSE_EVENT(MotionWindow)










MotionSearchRadius::MotionSearchRadius(MotionMain *plugin, 
	int x, 
	int y)
 : BC_IPot(x, 
		y, 
		plugin->config.global ? (int64_t)plugin->config.global_range : (int64_t)plugin->config.rotation_range,
		plugin->config.global ? (int64_t)MIN_RADIUS : (int64_t)MIN_ROTATION,
		plugin->config.global ? (int64_t)MAX_RADIUS : (int64_t)MAX_ROTATION)
{
	this->plugin = plugin;
}


int MotionSearchRadius::handle_event()
{
	if(plugin->config.global)
		plugin->config.global_range = (int)get_value();
	else
		plugin->config.rotation_range = (int)get_value();
	plugin->send_configure_change();
	return 1;
}








MotionBlockSize::MotionBlockSize(MotionMain *plugin, 
	int x, 
	int y)
 : BC_IPot(x, 
		y, 
		(int64_t)plugin->config.block_size,
		(int64_t)MIN_BLOCK,
		(int64_t)MAX_BLOCK)
{
	this->plugin = plugin;
}


int MotionBlockSize::handle_event()
{
	plugin->config.block_size = (int)get_value();
	plugin->send_configure_change();
	return 1;
}









MotionBlockCount::MotionBlockCount(MotionMain *plugin, 
	int x, 
	int y,
	int w)
 : BC_PopupMenu(x, 
		y, 
		w,
		"",
		1)
{
	this->plugin = plugin;
}

void MotionBlockCount::create_objects()
{
	add_item(new BC_MenuItem("1"));
	add_item(new BC_MenuItem("4"));
	add_item(new BC_MenuItem("9"));
	add_item(new BC_MenuItem("16"));
	add_item(new BC_MenuItem("25"));
	add_item(new BC_MenuItem("36"));
	add_item(new BC_MenuItem("49"));
	add_item(new BC_MenuItem("64"));
	add_item(new BC_MenuItem("81"));
	add_item(new BC_MenuItem("100"));
	add_item(new BC_MenuItem("121"));
	add_item(new BC_MenuItem("144"));
	add_item(new BC_MenuItem("169"));

	char string[BCTEXTLEN];
	sprintf(string, "%d", plugin->config.block_count);
	set_text(string);
}

int MotionBlockCount::handle_event()
{
	plugin->config.block_count = atoi(get_text());
	plugin->send_configure_change();
	return 1;
}









MotionSearchPositions::MotionSearchPositions(MotionMain *plugin, 
	int x, 
	int y,
	int w)
 : BC_PopupMenu(x,
 	y,
	w,
	"",
	1)
{
	this->plugin = plugin;
	global = 0;
	rotate = 0;
}
void MotionSearchPositions::create_objects()
{
// No change
	if(total_items() && 
		plugin->config.global == global &&
		plugin->config.rotate == rotate)
		return;

	while(total_items())
		remove_item(get_item(0));
	char string[BCTEXTLEN];
	if(plugin->config.global)
	{
		add_item(new BC_MenuItem("64"));
		add_item(new BC_MenuItem("128"));
		add_item(new BC_MenuItem("256"));
		add_item(new BC_MenuItem("512"));
		add_item(new BC_MenuItem("1024"));
		add_item(new BC_MenuItem("2048"));
		add_item(new BC_MenuItem("4096"));
		add_item(new BC_MenuItem("8192"));
		add_item(new BC_MenuItem("16384"));
		add_item(new BC_MenuItem("32768"));
		add_item(new BC_MenuItem("65536"));
		add_item(new BC_MenuItem("131072"));
		sprintf(string, "%d", plugin->config.global_positions);
	}
	else
	if(plugin->config.rotate)
	{
		add_item(new BC_MenuItem("4"));
		add_item(new BC_MenuItem("8"));
		add_item(new BC_MenuItem("16"));
		add_item(new BC_MenuItem("32"));
		sprintf(string, "%d", plugin->config.rotate_positions);
	}
	set_text(string);
	global = plugin->config.global;
	rotate = plugin->config.rotate;
}

int MotionSearchPositions::handle_event()
{
	if(plugin->config.global)
		plugin->config.global_positions = atoi(get_text());
	else
		plugin->config.rotate_positions = atoi(get_text());
	plugin->send_configure_change();
	return 1;
}








MotionMagnitude::MotionMagnitude(MotionMain *plugin, 
	int x, 
	int y)
 : BC_IPot(x, 
		y, 
		(int64_t)plugin->config.magnitude,
		(int64_t)0,
		(int64_t)100)
{
	this->plugin = plugin;
}

int MotionMagnitude::handle_event()
{
	plugin->config.magnitude = (int)get_value();
	plugin->send_configure_change();
	return 1;
}


MotionReturnSpeed::MotionReturnSpeed(MotionMain *plugin, 
	int x, 
	int y)
 : BC_IPot(x, 
		y, 
		(int64_t)plugin->config.return_speed,
		(int64_t)0,
		(int64_t)100)
{
	this->plugin = plugin;
}

int MotionReturnSpeed::handle_event()
{
	plugin->config.return_speed = (int)get_value();
	plugin->send_configure_change();
	return 1;
}








MotionBlockX::MotionBlockX(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_FPot(x,
 	y,
	plugin->config.block_x,
	(float)0, 
	(float)100)
{
	this->plugin = plugin;
	this->gui = gui;
}

int MotionBlockX::handle_event()
{
	plugin->config.block_x = get_value();
	gui->block_x_text->update(plugin->config.block_x);
	plugin->send_configure_change();
	return 1;
}




MotionBlockY::MotionBlockY(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_FPot(x,
 	y,
	plugin->config.block_x,
	(float)0, 
	(float)100)
{
	this->plugin = plugin;
	this->gui = gui;
}

int MotionBlockY::handle_event()
{
	plugin->config.block_y = get_value();
	gui->block_y_text->update(plugin->config.block_y);
	plugin->send_configure_change();
	return 1;
}

MotionBlockXText::MotionBlockXText(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_TextBox(x,
 	y,
	75,
	1,
	plugin->config.block_x)
{
	this->plugin = plugin;
	this->gui = gui;
	set_precision(4);
}

int MotionBlockXText::handle_event()
{
	plugin->config.block_x = atof(get_text());
	gui->block_x->update(plugin->config.block_x);
	plugin->send_configure_change();
	return 1;
}




MotionBlockYText::MotionBlockYText(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_TextBox(x,
 	y,
	75,
	1,
	plugin->config.block_y)
{
	this->plugin = plugin;
	this->gui = gui;
	set_precision(4);
}

int MotionBlockYText::handle_event()
{
	plugin->config.block_y = atof(get_text());
	gui->block_y->update(plugin->config.block_y);
	plugin->send_configure_change();
	return 1;
}







MotionGlobal::MotionGlobal(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.global,
	_("Global"))
{
	this->plugin = plugin;
	this->gui = gui;
}

int MotionGlobal::handle_event()
{
	plugin->config.global = 1;
	plugin->config.rotate = 0;
	gui->rotate->update(0);
	gui->radius_title->update(gui->get_radius_title());
	gui->search_positions->create_objects();
	plugin->send_configure_change();
	return 1;
}


MotionRotate::MotionRotate(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.rotate,
	_("Rotation"))
{
	this->plugin = plugin;
	this->gui = gui;
}

int MotionRotate::handle_event()
{
	plugin->config.global = 0;
	plugin->config.rotate = 1;
	gui->global->update(0);
	gui->radius_title->update(gui->get_radius_title());
	gui->search_positions->create_objects();
	plugin->send_configure_change();
	return 1;
}










MotionNothing::MotionNothing(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode1 == MotionConfig::NOTHING,
	_("Do Nothing"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionNothing::handle_event()
{
	plugin->config.mode1 = MotionConfig::NOTHING;
	gui->update_mode();
	plugin->send_configure_change();
	return 1;
}




MotionStabilize::MotionStabilize(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode1 == MotionConfig::STABILIZE,
	_("Stabilize Subpixel"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionStabilize::handle_event()
{
	plugin->config.mode1 = MotionConfig::STABILIZE;
	gui->update_mode();
	plugin->send_configure_change();
	return 1;
}

MotionStabilizePixel::MotionStabilizePixel(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode1 == MotionConfig::STABILIZE_PIXEL,
	_("Stabilize Single Pixel"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionStabilizePixel::handle_event()
{
	plugin->config.mode1 = MotionConfig::STABILIZE_PIXEL;
	gui->update_mode();
	plugin->send_configure_change();
	return 1;
}


MotionTrack::MotionTrack(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x,
 	y, 
	plugin->config.mode1 == MotionConfig::TRACK,
	_("Track Subpixel"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionTrack::handle_event()
{
	plugin->config.mode1 = MotionConfig::TRACK;
	gui->update_mode();
	plugin->send_configure_change();
	return 1;
}

MotionTrackPixel::MotionTrackPixel(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x,
 	y, 
	plugin->config.mode1 == MotionConfig::TRACK_PIXEL,
	_("Track Single Pixel"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionTrackPixel::handle_event()
{
	plugin->config.mode1 = MotionConfig::TRACK_PIXEL;
	gui->update_mode();
	plugin->send_configure_change();
	return 1;
}





MotionDrawVectors::MotionDrawVectors(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_CheckBox(x,
 	y, 
	plugin->config.draw_vectors,
	_("Draw vectors"))
{
	this->gui = gui;
	this->plugin = plugin;
}

int MotionDrawVectors::handle_event()
{
	plugin->config.draw_vectors = get_value();
	plugin->send_configure_change();
	return 1;
}








TrackSingleFrame::TrackSingleFrame(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, y, plugin->config.tracksingle == 1, _("Track single frame"))
{
	this->plugin = plugin;
	this->gui = gui;
}

int TrackSingleFrame::handle_event()
{
	plugin->config.tracksingle = 1;
	gui->track_previous->update(0);
	gui->track_frame_number->enable();
	plugin->send_configure_change();
	return 1;
}








TrackFrameNumber::TrackFrameNumber(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_TextBox(x, y, 100, 1, plugin->config.track_frame)
{
	this->plugin = plugin;
	this->gui = gui;
	if(!plugin->config.tracksingle) disable();
}

int TrackFrameNumber::handle_event()
{
	plugin->config.track_frame = atol(get_text());
	plugin->send_configure_change();
	return 1;
}







TrackPreviousFrame::TrackPreviousFrame(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, y, plugin->config.tracksingle == 0, _("Track previous frame"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int TrackPreviousFrame::handle_event()
{
	plugin->config.tracksingle = 0;
	gui->track_single->update(0);
	gui->track_frame_number->disable();
	plugin->send_configure_change();
	return 1;
}









MasterLayerTop::MasterLayerTop(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, y, !plugin->config.bottom_is_master, _("Top layer"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MasterLayerTop::handle_event()
{
	plugin->config.bottom_is_master = 0;
	gui->master_layer_bottom->update(0);
	plugin->send_configure_change();
	return 1;
}








MasterLayerBottom::MasterLayerBottom(MotionMain *plugin, 
	MotionWindow *gui,
	int x, 
	int y)
 : BC_Radial(x, y, plugin->config.bottom_is_master, _("Bottom layer"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MasterLayerBottom::handle_event()
{
	plugin->config.bottom_is_master = 1;
	gui->master_layer_top->update(0);
	plugin->send_configure_change();
	return 1;
}








MotionRecalculate::MotionRecalculate(MotionMain *plugin,
	MotionWindow *gui,
	int x,
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode3 == MotionConfig::RECALCULATE,
	_("Recalculate coords"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MotionRecalculate::handle_event()
{
	plugin->config.mode3 = MotionConfig::RECALCULATE;
	gui->save->update(0);
	gui->nocalculate->update(0);
	gui->load->update(0);
	plugin->send_configure_change();
	return 1;
}



MotionNocalculate::MotionNocalculate(MotionMain *plugin,
	MotionWindow *gui,
	int x,
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode3 == MotionConfig::NO_CALCULATE,
	_("Don't calculate"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MotionNocalculate::handle_event()
{
	plugin->config.mode3 = MotionConfig::NO_CALCULATE;
	gui->recalculate->update(0);
	gui->save->update(0);
	gui->load->update(0);
	plugin->send_configure_change();
	return 1;
}











MotionSave::MotionSave(MotionMain *plugin,
	MotionWindow *gui,
	int x,
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode3 == MotionConfig::SAVE,
	_("Save coords to /tmp"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MotionSave::handle_event()
{
	plugin->config.mode3 = MotionConfig::SAVE;
	gui->recalculate->update(0);
	gui->nocalculate->update(0);
	gui->load->update(0);
	plugin->send_configure_change();
	return 1;
}







MotionLoad::MotionLoad(MotionMain *plugin,
	MotionWindow *gui,
	int x,
	int y)
 : BC_Radial(x, 
 	y, 
	plugin->config.mode3 == MotionConfig::LOAD,
	_("Load coords from /tmp"))
{
	this->plugin = plugin;
	this->gui = gui;
}
int MotionLoad::handle_event()
{
	plugin->config.mode3 = MotionConfig::LOAD;
	gui->recalculate->update(0);
	gui->save->update(0);
	plugin->send_configure_change();
	return 1;
}







