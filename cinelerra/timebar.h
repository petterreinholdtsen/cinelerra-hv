#ifndef TIMEBAR_H
#define TIMEBAR_H

#include "edl.inc"
#include "guicast.h"
#include "filexml.inc"
#include "labels.inc"
#include "mwindow.inc"
#include "recordlabel.inc"
#include "testobject.h"
#include "timebar.inc"
#include "vwindowgui.inc"

class TimeBarLeftArrow;
class TimeBarRightArrow;

class LabelGUI;
class InPointGUI;
class OutPointGUI;
class PresentationGUI;

// Operations for cursor
#define TIMEBAR_NONE        0
#define TIMEBAR_DRAG        1
#define TIMEBAR_DRAG_LEFT   2
#define TIMEBAR_DRAG_RIGHT  3
#define TIMEBAR_DRAG_CENTER 4

class LabelGUI : public BC_Toggle
{
public:
	LabelGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		int64_t pixel, 
		int y, 
		double position,
		VFrame **data = 0);
	virtual ~LabelGUI();

	static int translate_pixel(MWindow *mwindow, int pixel);
	virtual int handle_event();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
	void reposition();

	MWindow *mwindow;
	VWindowGUI *gui;
	TimeBar *timebar;
	int64_t pixel;
	double position;
};

class TestPointGUI : public LabelGUI
{
public:
	TestPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		int64_t pixel, 
		double position);
};

class InPointGUI : public LabelGUI
{
public:
	InPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		int64_t pixel, 
		double position);
	virtual ~InPointGUI();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
};

class OutPointGUI : public LabelGUI
{
public:
	OutPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		int64_t pixel, 
		double position);
	virtual ~OutPointGUI();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
};

class PresentationGUI : public LabelGUI
{
public:
	PresentationGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		int64_t pixel, 
		double position);
	~PresentationGUI();
};

class TimeBar : public BC_SubWindow
{
public:
	TimeBar(MWindow *mwindow, 
		BC_WindowBase *gui,
		int x, 
		int y,
		int w,
		int h);
	virtual ~TimeBar();

	int create_objects();
	int update_defaults();
	int button_press_event();
	int button_release_event();
	int cursor_motion_event();
	int repeat_event(int64_t duration);

// Synchronize label, in/out, presentation display with master EDL
	void update(int do_range = 1, int do_others = 1);
	virtual void draw_time();
// Called by update and draw_time.
	virtual void draw_range();
	virtual void select_label(double position);
	virtual void stop_playback();
	virtual EDL* get_edl();
	virtual int test_preview(int buttonpress);
	virtual void update_preview();
	virtual int64_t position_to_pixel(double position);
	int move_preview(int &redraw);


	void update_labels();
	void update_points();
	void update_presentations();
// Make sure widgets are highlighted according to selection status
	void update_highlights();
// Update highlight cursor during a drag
	void update_cursor();

// ================================= file operations

	int save(FileXML *xml);
	int load(FileXML *xml, int undo_type);

	int delete_project();        // clear timebar of labels

	int draw();                  // draw everything over
	int samplemovement();
	int refresh_labels();

// ========================================= editing

	int copy(int64_t start, int64_t end, FileXML *xml);
	int paste(int64_t start, int64_t end, int64_t sample_length, FileXML *xml);
	int paste_output(int64_t startproject, int64_t endproject, int64_t startsource, int64_t endsource, RecordLabels *new_labels);
	int clear(int64_t start, int64_t end);
	int paste_silence(int64_t start, int64_t end);
	int modify_handles(int64_t oldposition, int64_t newposition, int currentend);
	int select_region(double position);
	void get_edl_length();

	MWindow *mwindow;
	BC_WindowBase *gui;
	int flip_vertical(int w, int h);
	int delete_arrows();    // for flipping vertical

// Operation started by a buttonpress
	int current_operation;


private:
	int get_preview_pixels(int &x1, int &x2);
	int draw_bevel();
	ArrayList<LabelGUI*> labels;
	InPointGUI *in_point;
	OutPointGUI *out_point;
	ArrayList<PresentationGUI*> presentations;


// Records for dragging operations
	double start_position;
	double starting_start_position;
	double starting_end_position;
	double time_per_pixel;
	double edl_length;
	int start_cursor_x;
};


#endif
