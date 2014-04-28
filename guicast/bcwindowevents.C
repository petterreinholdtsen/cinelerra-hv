
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include "bcdisplay.h"
#include "bcwindowbase.h"
#include "bcwindowevents.h"

BC_WindowEvents::BC_WindowEvents(BC_WindowBase *window)
 : Thread(1, 0, 0)
{
	this->window = window;
	display = 0;
	done = 0;
}


BC_WindowEvents::BC_WindowEvents(BC_Display *display)
 : Thread(1, 0, 0)
{
	this->display = display;
	window = 0;
	done = 0;
}

BC_WindowEvents::~BC_WindowEvents()
{
	done = 1;
	Thread::join();
}

void BC_WindowEvents::start()
{
	done = 0;
	Thread::start();
}


void BC_WindowEvents::run()
{
	XEvent *event;

	while(!done)
	{
		event = new XEvent;

// Can't cancel in XNextEvent because X server never figures out it's not
// listening anymore and XCloseDisplay locks up.
#ifdef SINGLE_THREAD
		XNextEvent(display->display, event);
		display->put_event(event);
#else
		XNextEvent(window->display, event);
		window->put_event(event);
#endif

	}
}



