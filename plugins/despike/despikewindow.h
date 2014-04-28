#ifndef DESPIKEWINDOW_H
#define DESPIKEWINDOW_H

class DespikeThread;
class DespikeWindow;

#include "guicast.h"
#include "mutex.h"
#include "despike.h"

class DespikeThread : public Thread
{
public:
	DespikeThread(Despike *despike);
	~DespikeThread();
	
	void run();
	
	Mutex completion, gui_started; // prevent loading data until the GUI is started
	Despike *despike;
	DespikeWindow *window;
};

class DespikeLevel;
class DespikeSlope;

class DespikeWindow : public BC_Window
{
public:
	DespikeWindow(Despike *despike, int x, int y);
	~DespikeWindow();
	
	int create_objects();
	int close_event();
	
	Despike *despike;
	DespikeLevel *level;
	DespikeSlope *slope;
};

class DespikeLevel : public BC_FSlider
{
public:
	DespikeLevel(Despike *despike, int x, int y);
	int handle_event();
	Despike *despike;
};

class DespikeSlope : public BC_FSlider
{
public:
	DespikeSlope(Despike *despike, int x, int y);
	int handle_event();
	Despike *despike;
};




#endif
