#include "guicast.h"
#include "motion.inc"

class TrackSingleFrame : public BC_Radial
{
public:
	TrackSingleFrame(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class TrackFrameNumber : public BC_TextBox
{
public:
	TrackFrameNumber(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class TrackPreviousFrame : public BC_Radial
{
public:
	TrackPreviousFrame(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MasterLayerTop : public BC_Radial
{
public:
	MasterLayerTop(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MasterLayerBottom : public BC_Radial
{
public:
	MasterLayerBottom(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionSearchRadius : public BC_IPot
{
public:
	MotionSearchRadius(MotionMain *plugin, 
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
};

class MotionBlockSize : public BC_IPot
{
public:
	MotionBlockSize(MotionMain *plugin, 
		
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
};

class MotionBlockX : public BC_FPot
{
public:
	MotionBlockX(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionBlockY : public BC_FPot
{
public:
	MotionBlockY(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionBlockXText : public BC_TextBox
{
public:
	MotionBlockXText(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionBlockYText : public BC_TextBox
{
public:
	MotionBlockYText(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionBlockCount : public BC_PopupMenu
{
public:
	MotionBlockCount(MotionMain *plugin, 
		int x, 
		int y,
		int w);
	void create_objects();
	int handle_event();
	MotionMain *plugin;
};

class MotionSearchPositions : public BC_PopupMenu
{
public:
	MotionSearchPositions(MotionMain *plugin, 
		int x, 
		int y,
		int w);
	void create_objects();
	int handle_event();
	MotionMain *plugin;
	int global;
	int rotate;
};

class MotionMagnitude : public BC_IPot
{
public:
	MotionMagnitude(MotionMain *plugin, 
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
};

class MotionReturnSpeed : public BC_IPot
{
public:
	MotionReturnSpeed(MotionMain *plugin, 
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
};

class MotionGlobal : public BC_Radial
{
public:
	MotionGlobal(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionRotate : public BC_Radial
{
public:
	MotionRotate(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};



class MotionStabilize : public BC_Radial
{
public:
	MotionStabilize(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionTrack : public BC_Radial
{
public:
	MotionTrack(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionStabilizePixel : public BC_Radial
{
public:
	MotionStabilizePixel(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionTrackPixel : public BC_Radial
{
public:
	MotionTrackPixel(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionNothing : public BC_Radial
{
public:
	MotionNothing(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionWindow *gui;
	MotionMain *plugin;
};

class MotionDrawVectors : public BC_CheckBox
{
public:
	MotionDrawVectors(MotionMain *plugin, 
		MotionWindow *gui,
		int x, 
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionRecalculate : public BC_Radial
{
public:
	MotionRecalculate(MotionMain *plugin,
		MotionWindow *gui,
		int x,
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionNocalculate : public BC_Radial
{
public:
	MotionNocalculate(MotionMain *plugin,
		MotionWindow *gui,
		int x,
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionSave : public BC_Radial
{
public:
	MotionSave(MotionMain *plugin,
		MotionWindow *gui,
		int x,
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionLoad : public BC_Radial
{
public:
	MotionLoad(MotionMain *plugin,
		MotionWindow *gui,
		int x,
		int y);
	int handle_event();
	MotionMain *plugin;
	MotionWindow *gui;
};

class MotionWindow : public BC_Window
{
public:
	MotionWindow(MotionMain *plugin, int x, int y);
	~MotionWindow();

	int create_objects();
	int close_event();
	void update_mode();
	char* get_radius_title();

	MotionSearchRadius *radius;
	BC_Title *radius_title;
	MotionBlockSize *block_size;
	MotionBlockX *block_x;
	MotionBlockY *block_y;
	MotionBlockXText *block_x_text;
	MotionBlockYText *block_y_text;
	MotionSearchPositions *search_positions;
	MotionMagnitude *magnitude;
	MotionReturnSpeed *return_speed;
	MotionStabilize *stabilize;
	MotionTrack *track;
	MotionStabilizePixel *stabilize_pixel;
	MotionTrackPixel *track_pixel;
	MotionNothing *nothing;
	MotionDrawVectors *vectors;
	MotionGlobal *global;
	MotionRotate *rotate;
	TrackSingleFrame *track_single;
	TrackFrameNumber *track_frame_number;
	TrackPreviousFrame *track_previous;
	MasterLayerTop *master_layer_top;
	MasterLayerBottom *master_layer_bottom;
	MotionRecalculate *recalculate;
	MotionNocalculate *nocalculate;
	MotionSave *save;
	MotionLoad *load;
	
	MotionMain *plugin;
};



PLUGIN_THREAD_HEADER(MotionMain, MotionThread, MotionWindow)
