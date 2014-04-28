#ifndef TIMEAVGWINDOW_H
#define TIMEAVGWINDOW_H


class TimeAvgThread;
class TimeAvgWindow;

#include "guicast.h"
#include "mutex.h"
#include "timeavg.h"

class TimeAvgThread : public Thread
{
public:
	TimeAvgThread(TimeAvgMain *client);
	~TimeAvgThread();

	void run();

	Mutex gui_started, completion;
	TimeAvgMain *client;
	TimeAvgWindow *window;
};

class TimeAvgSlider;

class TimeAvgWindow : public BC_Window
{
public:
	TimeAvgWindow(TimeAvgMain *client, int x, int y);
	~TimeAvgWindow();
	
	int create_objects();
	int close_event();
	
	TimeAvgMain *client;
	TimeAvgSlider *total_frames;
};

class TimeAvgSlider : public BC_ISlider
{
public:
	TimeAvgSlider(TimeAvgMain *client, int x, int y);
	~TimeAvgSlider();
	int handle_event();

	TimeAvgMain *client;
};


#endif
