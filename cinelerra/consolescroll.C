#include "console.h"
#include "consolescroll.h"
#include "modules.h"

ConsoleMainScroll::ConsoleMainScroll(ConsoleWindow *gui)
{
	this->gui = gui;
	oldposition = 0;
	cxscroll = 0;
	cyscroll = 0;
}

ConsoleMainScroll::~ConsoleMainScroll()
{
}
	
int ConsoleMainScroll::create_objects(int w, int h)
{
	if(gui->console->vertical)
	gui->add_subwindow(cxscroll = new CXScroll(this, w, h));
	else
	gui->add_subwindow(cyscroll = new CYScroll(this, w, h));
}

int ConsoleMainScroll::resize_event(int w, int h)
{
// 	if(cxscroll)
// 		cxscroll->reposition_window(0, h - SCROLL_SPAN, w, SCROLL_SPAN);	
// 	else
// 	if(cyscroll)
// 		cyscroll->reposition_window(w - SCROLL_SPAN, 0, SCROLL_SPAN, h);
// 
// 	update();
}

int ConsoleMainScroll::update()               // reflect new console view
{
	Console *console = gui->console;

//	if(cxscroll)
//		cxscroll->reposition_window(console->modules->total_pixels(), console->pixel_start, gui->get_w());
//	else
//	if(cyscroll)
//		cyscroll->reposition_window(console->modules->total_pixels(), console->pixel_start, gui->get_h());

	oldposition = console->pixel_start;
}

int ConsoleMainScroll::handle_event(long position)
{
	if(position != oldposition)
	{
		long distance = position - oldposition;
		oldposition = position;

		gui->console->pixel_start = position;
		gui->console->redo_pixels();
		//gui->console->pixelmovement(distance);
	}
}

int ConsoleMainScroll::flip_vertical(int w, int h)
{
	if(cxscroll) delete cxscroll;
	if(cyscroll) delete cyscroll;
	
	cxscroll = 0;
	cyscroll = 0;
	
	if(gui->console->vertical)
	gui->add_subwindow(cxscroll = new CXScroll(this, w, h));
	else
	gui->add_subwindow(cyscroll = new CYScroll(this, w, h));

	update();
}

CXScroll::CXScroll(ConsoleMainScroll *scroll, int w, int h)
 : BC_ScrollBar(0, h - 17, SCROLL_HORIZ, w, 0, 0, 0)
{
	this->scroll = scroll;
}

CXScroll::~CXScroll()
{
}

int CXScroll::handle_event()
{
	scroll->handle_event(get_position());
}

CYScroll::CYScroll(ConsoleMainScroll *scroll, int w, int h)
 : BC_ScrollBar(w - 17, 0, SCROLL_VERT, h, 0, 0, 0)
{
	this->scroll = scroll;
}

CYScroll::~CYScroll()
{
}

int CYScroll::handle_event()
{
	scroll->handle_event(get_position());
}
