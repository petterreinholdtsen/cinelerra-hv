#ifndef DEINTERWINDOW_H
#define DEINTERWINDOW_H


class DeInterlaceThread;
class DeInterlaceWindow;

#include "guicast.h"
#include "mutex.h"
#include "deinterlace.h"

class DeInterlaceThread : public Thread
{
public:
	DeInterlaceThread(DeInterlaceMain *client);
	~DeInterlaceThread();

	void run();

	Mutex gui_started, completion;
	DeInterlaceMain *client;
	DeInterlaceWindow *window;
};

class DeInterlaceOption;

class DeInterlaceWindow : public BC_Window
{
public:
	DeInterlaceWindow(DeInterlaceMain *client, int x, int y);
	~DeInterlaceWindow();
	
	int create_objects();
	int close_event();
	int set_mode(int mode, int recursive);
	
	DeInterlaceMain *client;
	DeInterlaceOption *odd_fields;
	DeInterlaceOption *even_fields;
	DeInterlaceOption *average_fields;
	DeInterlaceOption *swap_fields;
};

class DeInterlaceOption : public BC_Radial
{
public:
	DeInterlaceOption(DeInterlaceMain *client, 
		DeInterlaceWindow *window, 
		int output, 
		int x, 
		int y, 
		char *text);
	~DeInterlaceOption();
	int handle_event();

	DeInterlaceMain *client;
	DeInterlaceWindow *window;
	int output;
};

#endif
