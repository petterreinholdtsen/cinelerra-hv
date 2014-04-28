#include "mixer.h"
#include "mixerpopup.h"


MixerMenu::MixerMenu(Mixer *mixer)
 : BC_PopupMenu(0, 
		0, 
		0, 
		"", 
		0)
{
	this->mixer = mixer;
}

MixerMenu::~MixerMenu()
{
}

void MixerMenu::create_objects()
{
	add_item(new MixerMenuConfigure(mixer));
	add_item(new MixerMenuRead(mixer));
	add_item(new MixerMenuSave(mixer));
	add_item(new MixerMenuZero(mixer));
}





MixerMenuConfigure::MixerMenuConfigure(Mixer *mixer)
{
	this->mixer = mixer;
}
int MixerMenuConfigure::handle_event()
{
	mixer->configure();
	return 1;
}





MixerMenuRead::MixerMenuRead(Mixer *mixer)
 : BC_MenuItem("Read hardware")
{
	this->mixer = mixer;
}
int MixerMenuRead::handle_event()
{
	mixer->read_hardware();
	return 1;
}





MixerMenuSave::MixerMenuSave(Mixer *mixer)
 : BC_MenuItem("Save settings")
{
	this->mixer = mixer;
}
int MixerMenuSave::handle_event()
{
	mixer->save_defaults();
	return 1;
}





MixerMenuZero::MixerMenuZero(Mixer *mixer)
 : BC_MenuItem("Zero settings")
{
	this->mixer = mixer;
}
int MixerMenuZero::handle_event()
{
	mixer->zero();
	return 1;
}


