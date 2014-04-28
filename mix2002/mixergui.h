#ifndef MIXERGUI_H
#define MIXERGUI_H


#include "arraylist.h"
#include "guicast.h"
#include "mixer.inc"
#include "mixergui.inc"
#include "mixerpopup.inc"


class MixerLock;
class MixerSlider;
class MixerPot;

class MixerGUI : public BC_Window
{
public:
	MixerGUI(Mixer *mixer, int x, int y, int w, int h);
	~MixerGUI();

	int create_objects();
	int create_controls();
	int close_event();
	int keypress_event();
	int button_press_event();
	int translation_event();
	void update_display();

	int locked();

	ArrayList<MixerControl*> controls;
	MixerLock *lock;
	Mixer *mixer;
	MixerMenu *menu;
};


class MixerControl
{
public:
	MixerControl(Mixer *mixer, MixerNode *tree_node, int x, int y);
	~MixerControl();
	
	void create_objects();
	
	int update(MixerNode *node);        // store last_value and update everyone
	int change_value(int parameter_number, float offset, int omit_channel);
// adjust value of everyone by offset
	int reposition(int x);
// Whether or not the control represents the tree node.
	int equivalent(MixerNode *node);
// Update values with the new node
	void update(MixerNode *node, int x, int y);

	int parameter_number;
	int total_controls;
	int type;
	Mixer *mixer;
	MixerNode *node;
	MixerSlider *sliders[MAX_CHANNELS];
	MixerPot *pots[MAX_CHANNELS];
};





class MixerSlider : public BC_PercentageSlider
{
public:
	MixerSlider(int x, int y, int w, int highlight, int color, Mixer *mixer, MixerControl *control, int channel);
	
	int handle_event();

	int channel;
	float last_value;
	
	Mixer *mixer;
	MixerControl *control;
};

class MixerPot : public BC_PercentagePot
{
public:
	MixerPot(int x, 
			int y, 
			VFrame **data, 
			Mixer *mixer, 
			MixerControl *control, 
			int parameter_number, 
			int channel);

	int handle_event();

	int parameter_number;
	int channel;
	float last_value;

	Mixer *mixer;
	MixerControl *control;
};

class MixerToggle : public BC_Radial
{
public:
	MixerToggle(int x, 
		int y, 
		int *output, 
		Mixer *mixer, 
		char *title);

	int handle_event();

	int *output;
	Mixer *mixer;
};



class MixerLock : public BC_CheckBox
{
public:
	MixerLock(int x, int y, Mixer *mixer);

	int handle_event();

	Mixer *mixer;
};


#endif
