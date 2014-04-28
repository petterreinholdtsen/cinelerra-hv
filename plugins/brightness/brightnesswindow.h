#ifndef BRIGHTNESSWINDOW_H
#define BRIGHTNESSWINDOW_H


class BrightnessThread;
class BrightnessWindow;
class BrightnessSlider;
class BrightnessLuma;

#include "brightness.h"
#include "guicast.h"
#include "mutex.h"
#include "thread.h"

class BrightnessThread : public Thread
{
public:
	BrightnessThread(BrightnessMain *client);
	~BrightnessThread();

	void run();

	Mutex gui_started, completion; // prevent loading data until the GUI is started
	BrightnessMain *client;
	BrightnessWindow *window;
	BrightnessLuma *luma;
};

class BrightnessWindow : public BC_Window
{
public:
	BrightnessWindow(BrightnessMain *client, int x, int y);
	~BrightnessWindow();

	int create_objects();
	int close_event();

	BrightnessMain *client;
	BrightnessSlider *brightness;
	BrightnessSlider *contrast;
	BrightnessLuma *luma;
};

class BrightnessSlider : public BC_FSlider
{
public:
	BrightnessSlider(BrightnessMain *client, float *output, int x, int y);
	~BrightnessSlider();
	int handle_event();

	BrightnessMain *client;
	float *output;
};

class BrightnessLuma : public BC_CheckBox
{
public:
	BrightnessLuma(BrightnessMain *client, int x, int y);
	~BrightnessLuma();
	int handle_event();

	BrightnessMain *client;
};

#endif
