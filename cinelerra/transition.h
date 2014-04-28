#ifndef TRANSITION_H
#define TRANSITION_H

class PasteTransition;

#include "edit.inc"
#include "filexml.inc"
#include "mwindow.inc"
#include "messages.inc"
#include "plugin.h"
#include "sharedlocation.h"

class TransitionMenuItem : public BC_MenuItem
{
public:
	TransitionMenuItem(MWindow *mwindow, int audio, int video);
	~TransitionMenuItem();
	int handle_event();
	int audio;
	int video;
//	PasteTransition *thread;
};

class PasteTransition : public Thread
{
public:
	PasteTransition(MWindow *mwindow, int audio, int video);
	~PasteTransition();

	void run();

	MWindow *mwindow;
	int audio, video;
};


class Transition : public Plugin
{
public:
	Transition(EDL *edl, Edit *edit, char *title, long unit_length);

	Edit *edit;



	void save_xml(FileXML *file);
	void load_xml(FileXML *file);





	Transition(Transition *that, Edit *edit);
	~Transition();

	KeyFrame* get_keyframe();
	int reset_parameters();
	int update_derived();
	Transition& operator=(Transition &that);
	Plugin& operator=(Plugin &that);
	Edit& operator=(Edit &that);
	int operator==(Transition &that);
	int operator==(Plugin &that);
	int operator==(Edit &that);

// Only the show value from the attachment point is used.
	int set_show_derived(int value) {};

	int popup_transition(int x, int y);
	char* get_module_title();
// Update the widgets after loading
	int update_display();
// Update edit after attaching
	int update_edit(int is_loading);
	char* default_title();
	void dump();

	int audio, video;
	MWindow *mwindow;

private:
// Only used by operator= and copy constructor
	void copy_from(Transition *that);
	int identical(Transition *that);
};

#endif
