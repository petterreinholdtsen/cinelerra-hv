#ifndef MIXERPOPUP_H
#define MIXERPOPUP_H



#include "guicast.h"
#include "mixer.inc"



class MixerMenu : public BC_PopupMenu
{
public:
	MixerMenu(Mixer *mixer);
	~MixerMenu();

	void create_objects();
	
	Mixer *mixer;
};

class MixerMenuConfigure : public BC_MenuItem
{
public:
	MixerMenuConfigure(Mixer *mixer);
	int handle_event();
	Mixer *mixer;
};

class MixerMenuRead : public BC_MenuItem
{
public:
	MixerMenuRead(Mixer *mixer);
	int handle_event();
	Mixer *mixer;
};

class MixerMenuSave : public BC_MenuItem
{
public:
	MixerMenuSave(Mixer *mixer);
	int handle_event();
	Mixer *mixer;
};

class MixerMenuZero : public BC_MenuItem
{
public:
	MixerMenuZero(Mixer *mixer);
	int handle_event();
	Mixer *mixer;
};

#endif
