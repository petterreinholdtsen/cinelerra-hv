#ifndef GAINWINDOW_H
#define GAINWINDOW_H

#define TOTAL_LOADS 5

class GainThread;
class GainWindow;

#include "filexml.h"
#include "guicast.h"
#include "mutex.h"
#include "gain.h"

class GainThread : public Thread
{
public:
	GainThread(Gain *gain);
	~GainThread();
	
	void run();
	
	Mutex completion, gui_started; // prevent loading data until the GUI is started
	Gain *gain;
	GainWindow *window;
};

class GainLevel;

class GainWindow : public BC_Window
{
public:
	GainWindow(Gain *gain, int x, int y);
	~GainWindow();
	
	int create_objects();
	int close_event();
	
	Gain *gain;
	GainLevel *level;
};

class GainLevel : public BC_FSlider
{
public:
	GainLevel(Gain *gain, int x, int y);
	int handle_event();
	Gain *gain;
};




#endif
