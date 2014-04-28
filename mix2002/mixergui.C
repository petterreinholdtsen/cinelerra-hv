#include "keys.h"
#include "mixer.h"
#include "mixergui.h"
#include "mixerpopup.h"
#include "mixertree.h"








MixerGUI::MixerGUI(Mixer *mixer, int x, int y, int w, int h)
 : BC_Window("Mix 2000", 
 	x, 
	y, 
	w, 
	h, 
	MIXER_X, 
	h,
	0,
	0,
	1,
	BLOND)
{
	this->mixer = mixer;
}

MixerGUI::~MixerGUI()
{
}

int MixerGUI::create_objects()
{
// build permanent controls
	add_subwindow(lock = new MixerLock(10, 10, mixer));
	add_subwindow(menu = new MixerMenu(mixer));
	menu->create_objects();
	update_display();
	show_window();
	return 0;
}

void MixerGUI::update_display()
{
	int x = 40;
	int y = 10;
	int current_control = 0;

	for(int i = 0; i < mixer->tree->total; i++)
	{
		MixerNode *node = mixer->tree->values[i];

		if(node->show)
		{
			MixerControl *control;
			if(controls.total > current_control)
				control = controls.values[current_control];
			else
				control = 0;

// Get a control with the right type
			while(control && 
				!control->equivalent(node))
			{
				controls.remove_object(control);
				if(controls.total > current_control)
					control = controls.values[current_control];
				else
					control = 0;
			}

// Create or update a control
			if(!control)
			{
				control = new MixerControl(mixer, node, x, y);
				controls.append(control);
				control->create_objects();
			}
			else
			{
				control->update(node);
			}
			current_control++;
		}
	}
// Delete excess controls
	for(int i = current_control; i < controls.total; i++)
		controls.remove_object();
}







int MixerGUI::close_event()
{
	set_done(0);
	return 1;
}

int MixerGUI::keypress_event()
{
	switch(get_keypress())
	{
		case 'q':
		case ESC:
			set_done(0);
			return 1;
			break;
	}
	return 0;
}



int MixerGUI::button_press_event()
{
	if(get_buttonpress() == 3)
	{
		menu->activate_menu();
		return 1;
	}
	return 0;
}

int MixerGUI::translation_event()
{
	mixer->x = get_x();
	mixer->y = get_y();
	return 0;
}












MixerControl::MixerControl(Mixer *mixer, MixerNode *tree_node, int x, int y)
{
	this->mixer = mixer;
	this->node = tree_node;
	total_controls = 0;
}

MixerControl::~MixerControl()
{
// Can't access node here since it's normally called after the node is deleted.
	for(int i = 0; i < total_controls; i++)
	{
		delete controls[i];
	}
}

int MixerControl::update(MixerNode *node)
{
	return 0;
}









MixerSlider::MixerSlider(int x, int y, int w, 
	int highlight, 
	int color, 
	Mixer *mixer, 
	MixerControl<MixerSlider> *control, 
	int channel)
 : BC_PercentageSlider(x, 
 	y, 
	0,
	w, 
	200, 
	0, 
	100, 
	mixer->values[MASTER_NUMBER][channel],
	0,
	slider_data)
{
	this->mixer = mixer;
	this->control = control;
	this->channel = channel;
	this->last_value = mixer->values[MASTER_NUMBER][channel];
}


int MixerSlider::handle_event()
{
	if(mixer->lock)
	{
//printf("MixerSlider::handle_event %f\n", get_value() - last_value);
		control->change_value(MASTER_NUMBER, 
			get_value() - last_value, 
			channel);
	}
	last_value = mixer->values[MASTER_NUMBER][channel] = get_value();
	mixer->device->write_parameters(1, 0);
	return 1;
}




MixerPot::MixerPot(int x, 
			int y, 
			VFrame **data, 
			Mixer *mixer, 
			MixerControl<MixerPot> *control, 
			int parameter_number, 
			int channel)
 : BC_PercentagePot(x, 
 	y, 
	mixer->values[parameter_number][channel], 
	0, 
	100,
	data)
{
	this->mixer = mixer;
	this->control = control;
	this->channel = channel;
	this->parameter_number = parameter_number;
	this->last_value = mixer->values[parameter_number][channel];
}


int MixerPot::handle_event()
{
	if(mixer->lock)
	{
//printf("MixerPot::handle_event %f\n", get_value() - last_value);
		control->change_value(parameter_number, 
			get_value() - last_value, channel);
	}
	last_value = mixer->values[parameter_number][channel] = get_value();
	mixer->device->write_parameters(1, 0);
	return 1;
}






MixerToggle::MixerToggle(int x, int y, int *output, Mixer *mixer, char *title)
 : BC_Radial(x, y, *output, title, MEDIUMFONT_3D, WHITE)
{
	this->mixer = mixer;
	this->output = output;
}

int MixerToggle::handle_event()
{
	*output = get_value();
	mixer->device->write_parameters(1, 0);
	return 1;
}





MixerLock::MixerLock(int x, int y, Mixer *mixer)
 : BC_CheckBox(x, y, mixer->lock)
{
	this->mixer = mixer; 
}

int MixerLock::handle_event()
{
	mixer->lock = get_value();
	return 1;
}
