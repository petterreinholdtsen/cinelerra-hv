#ifndef DOTWINDOW_H
#define DOTWINDOW_H

#include "guicast.h"

class DotThread;
class DotWindow;

#include "filexml.h"
#include "mutex.h"
#include "dot.h"

class DotThread : public Thread
{
public:
	DotThread(DotMain *client);
	~DotThread();

	void run();

// prevent loading data until the GUI is started
 	Mutex gui_started, completion;
	DotMain *client;
	DotWindow *window;
};

class DotWindow : public BC_Window
{
public:
	DotWindow(DotMain *client, int x, int y);
	~DotWindow();

	int create_objects();
	int close_event();

	DotMain *client;
};






#endif
