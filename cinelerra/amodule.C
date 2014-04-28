#include "aattachmentpoint.h"
#include "aedit.h"
#include "amodule.h"
#include "aplugin.h"
#include "arender.h"
#include "assets.h"
#include "atrack.h"
#include "cache.h"
#include "console.h"
#include "edits.h"
#include "edl.h"
#include "edlsession.h"
#include "file.h"
#include "filexml.h"
#include "floatautos.h"
#include "mwindow.h"
#include "module.h"
#include "modules.h"
#include "patch.h"
#include "plugin.h"
#include "pluginbuffer.h"
#include "preferences.h"
#include "renderengine.h"
#include "mainsession.h"
#include "sharedlocation.h"
#include "theme.h"
#include "transition.h"
#include "transportque.h"
#include <string.h>


AModule::AModule(RenderEngine *renderengine, 
	CommonRender *commonrender, 
	Track *track)
 : Module(renderengine, commonrender, track)
{
	data_type = TRACK_AUDIO;
	transition_temp = 0;
	level_history = 0;
}




AModule::~AModule()
{
	if(transition_temp) delete [] transition_temp;
	if(level_history)
	{
		delete [] level_history;
		delete [] level_samples;
	}
}

AttachmentPoint* AModule::new_attachment(Plugin *plugin)
{
	return new AAttachmentPoint(renderengine, plugin);
}


void AModule::create_objects()
{
	Module::create_objects();

// Not needed in pluginarray
	if(commonrender)
	{
		level_history = new double[((ARender*)commonrender)->history_size()];
		level_samples = new long[((ARender*)commonrender)->history_size()];
		current_level = 0;

		for(int i = 0; i < ((ARender*)commonrender)->history_size(); i++)
		{
			level_history[i] = 0;
			level_samples[i] = -1;
		}
	}
}


void AModule::reverse_buffer(double *buffer, long len)
{
	register long start, end;
	double temp;

	for(start = 0, end = len - 1; end > start; start++, end--)
	{
		temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
	}
}


CICache* AModule::get_cache()
{
	if(renderengine) 
		return renderengine->get_acache();
	else
		return cache;
}

int AModule::render(double *buffer, 
	long input_len, 
	long input_position,
	int direction)
{
	AEdit *playable_edit;
	long start_project = input_position;
	long end_project = input_position + input_len;
	long buffer_offset = 0;
	int result = 0;


//printf("AModule::render 1 %d\n", input_len);

// Flip range around so start_project < end_project
	if(direction == PLAY_REVERSE)
	{
		start_project -= input_len;
		end_project -= input_len;
	}

	bzero(buffer, input_len * sizeof(double));

//printf("AModule::render 2\n");

// Get first edit containing range
	for(playable_edit = (AEdit*)track->edits->first; 
		playable_edit;
		playable_edit = (AEdit*)playable_edit->next)
	{
		double edit_start = playable_edit->startproject;
		double edit_end = playable_edit->startproject + playable_edit->length;

		if(start_project < edit_end && start_project + input_len > edit_start)
		{
			break;
		}
	}





//printf("AModule::render 3\n");


// Fill output one fragment at a time
	while(start_project < end_project)
	{
		long fragment_len = input_len;

//printf("AModule::render 4 %d\n", fragment_len);
		if(fragment_len + start_project > end_project)
			fragment_len = end_project - start_project;

//printf("AModule::render 5 %d\n", fragment_len);
		update_transition(start_project, PLAY_FORWARD);


		if(playable_edit)
		{
//printf("AModule::render 6\n");


// Trim fragment_len
			if(fragment_len + start_project > 
				playable_edit->startproject + playable_edit->length)
				fragment_len = playable_edit->startproject + 
					playable_edit->length - start_project;

//printf("AModule::render 8 %d\n", fragment_len);

			if(playable_edit->asset)
			{
				File *source;

//printf("AModule::render 9\n");


				if(!(source = get_cache()->check_out(playable_edit->asset)))
				{
// couldn't open source file / skip the edit
					result = 1;
					printf("VirtualAConsole::load_track Couldn't open %s.\n", playable_edit->asset->path);
				}
				else
				{
					int result = 0;

//printf("AModule::render 10\n");

//printf("AModule::load_track 7 %d\n", start_project - 
//							playable_edit->startproject + 
//							playable_edit->startsource);

					result = source->set_audio_position(start_project - 
							playable_edit->startproject + 
							playable_edit->startsource, 
						get_edl()->session->sample_rate);
//printf("AModule::render 10\n");

					if(result) printf("AModule::render start_project=%d playable_edit->startproject=%d playable_edit->startsource=%d\n"
						"source=%p playable_edit=%p edl=%p edlsession=%p sample_rate=%d\n",
						start_project, playable_edit->startproject, playable_edit->startsource, 
						source, playable_edit, get_edl(), get_edl()->session, get_edl()->session->sample_rate);

					source->set_channel(playable_edit->channel);

// printf("AModule::render 11 %p %p %d %d\n", 
// 	source, 
// 	buffer, 
// 	buffer_offset, 
// 	fragment_len);
//printf("AModule::render 10 %p %p\n", source, playable_edit);
					source->read_samples(buffer + buffer_offset, 
						fragment_len,
						get_edl()->session->sample_rate);

//printf("AModule::render 12 %d %d\n", fragment_len, get_edl()->session->sample_rate);
					get_cache()->check_in(playable_edit->asset);
					get_cache()->age_audio();
//printf("AModule::render 13\n");
				}
			}






// Read transition into temp and render
			AEdit *previous_edit = (AEdit*)playable_edit->previous;
			if(transition && previous_edit)
			{

// Read into temp buffers
// Temp + master or temp + temp ? temp + master
				if(!transition_temp)
				{
					transition_temp = new double[get_edl()->session->audio_read_length];
				}



// Trim transition_len
				int transition_len = fragment_len;
				if(fragment_len + start_project > 
					playable_edit->startproject + transition->length)
					fragment_len = playable_edit->startproject + 
						playable_edit->transition->length - 
						start_project;

				if(transition_len > 0)
				{
					if(previous_edit->asset)
					{
						File *source;
						if(!(source = get_cache()->check_out(previous_edit->asset)))
						{
// couldn't open source file / skip the edit
							printf("VirtualAConsole::load_track Couldn't open %s.\n", playable_edit->asset->path);
							result = 1;
						}
						else
						{
							int result = 0;

							result = source->set_audio_position(start_project - 
									previous_edit->startproject + 
									previous_edit->startsource, 
								get_edl()->session->sample_rate);

							if(result) printf("AModule::render start_project=%d playable_edit->startproject=%d playable_edit->startsource=%d\n"
								"source=%p playable_edit=%p edl=%p edlsession=%p sample_rate=%d\n",
								start_project, 
								previous_edit->startproject, 
								previous_edit->startsource, 
								source, 
								playable_edit, 
								get_edl(), 
								get_edl()->session, 
								get_edl()->session->sample_rate);

							source->set_channel(previous_edit->channel);

							source->read_samples(transition_temp, 
								transition_len,
								get_edl()->session->sample_rate);

							get_cache()->check_in(previous_edit->asset);
							get_cache()->age_audio();
						}
					}
					else
					{
						bzero(transition_temp, transition_len * sizeof(double));
					}

					double *output = buffer + buffer_offset;
					transition_server->process_realtime(
						&transition_temp,
						&output,
						start_project - playable_edit->startproject,
						transition_len,
						transition->length);
				}
			}
		}

//printf("AModule::render 13\n");
		buffer_offset += fragment_len;
		start_project += fragment_len;
		if(playable_edit &&
			start_project >= playable_edit->startproject + playable_edit->length)
			playable_edit = (AEdit*)playable_edit->next;
	}

//printf("AModule::render 14\n");

// Reverse buffer here so plugins always render forward.
	if(direction == PLAY_REVERSE) 
		reverse_buffer(buffer, input_len);

//printf("AModule::render 15 %d\n", start_project);
	return result;
}

// REMOVE
int AModule::create_plugins(int &x, int &y)
{
	return 0;
}

// REMOVE
int AModule::flip_plugins(int &x, int &y)
{
	return 0;
}


// REMOVE
int AModule::set_pixel(int pixel)
{
	return 0;
}








// ================================ file operations

int AModule::save(FileXML *xml)
{
	xml->tag.set_title("MODULE");
	xml->append_tag();
	xml->append_newline();


// title must come before plugins to allow plugins to boot
	xml->tag.set_title("TITLE");
	xml->append_tag();
	xml->append_text(title);
	xml->tag.set_title("/TITLE");
	xml->append_tag();
	xml->append_newline();

	int i;
	
	if(inv)
	{
		xml->tag.set_title("INV");
		xml->append_tag();
		xml->append_newline();
	}
	
	if(mute)
	{
		xml->tag.set_title("MUTE");
		xml->append_tag();
		xml->append_newline();
	}

//	for(i = 0; i < PLUGINS; i++) plugins[i]->save(xml, "PLUGIN");
	
	xml->tag.set_title("PAN");
	xml->tag.set_property("X", (long)pan_x);
	xml->tag.set_property("Y", (long)pan_y);
	for(i = 0; i < mwindow->session->audio_channels; i++)
	{
		sprintf(string, "PAN%d", i);
		xml->tag.set_property(string, pan[i]);
	}

	xml->append_tag();
	
	xml->tag.set_title("FADE");
	xml->tag.set_property("VALUE", fade);
	xml->append_tag();
	xml->append_newline();
	
	xml->tag.set_title("/MODULE");
	xml->append_tag();
	xml->append_newline();
	return 0;
}

int AModule::load(FileXML *xml, int track_offset)
{
	int result = 0;
	int current_pan = 0;
	int current_plugin = 0;
	int i;
	DB db;

//printf("AModule::load 1\n");
	do{
		result = xml->read_tag();
		
		if(!result)
		{
			if(!strcmp(xml->tag.get_title(), "/MODULE"))
			{
				result = 1;
			}
			else
			if(!strcmp(xml->tag.get_title(), "PLUGIN"))
			{
// title must come before plugins to allow plugins to boot
				if(current_plugin < PLUGINS)
				{
//					plugins[current_plugin]->load(xml, track_offset, "/PLUGIN");
					current_plugin++;
				}
			}
			else
			if(!strcmp(xml->tag.get_title(), "TITLE"))
			{
// title must come before plugins to allow plugins to boot properly
				if(gui) 
				{
					strcpy(title, xml->read_text());
					gui->title->update(title);
				}
			}
			else
			if(!strcmp(xml->tag.get_title(), "INV"))
			{
				inv = 1;
				if(gui) gui->inv_toggle->update(inv);
			}
			else
			if(!strcmp(xml->tag.get_title(), "MUTE"))
			{
				mute = 1;
				if(gui) gui->mute_toggle->update(mute);
			}
			else
			if(!strcmp(xml->tag.get_title(), "PAN"))
			{
				pan_x = xml->tag.get_property("X", (long)0);
				pan_y = xml->tag.get_property("Y", (long)0);

				for(i = 0; i < mwindow->session->audio_channels; i++)
				{
					sprintf(string, "PAN%d", i);
					pan[i] = xml->tag.get_property(string, (float)0);
//printf("AModule::load pan%d %f\n", i, pan[i]);
				}

				if(gui)
				{
					gui->pan_stick->update(pan_x, pan_y);
				}
			}
			else
			if(!strcmp(xml->tag.get_title(), "FADE"))
			{
				fade = xml->tag.get_property("VALUE", (float)0);
				if(gui) gui->fade_slider->update(fade);
			}
		}
	}while(!result);
//printf("AModule::load 2\n");
	return 0;
}

int AModule::dump()
{
	printf("AModule::dump %x\n", this);
	printf("	pan0 %f pan1 %f\n", pan[0], pan[1]);
	return 0;
}

int AModule::update_peak(int peak_number, float value)
{
//	peak_history[peak_number] = value;
	return 0;
}

int AModule::update_meter(int peak_number, int last_peak, int total_peaks)
{
// 	if(peak_history && gui)
// 	{
// // zero unused peaks
// 		for(int i = last_peak; i != peak_number; )
// 		{
// 			peak_history[i] = 0;
// 			i++;
// 			if(i >= total_peaks) i = 0;
// 		}
// 
// 		gui->meter->update(peak_history[peak_number], peak_history[peak_number] > 1);
// 	}
	return 0;
}

int AModule::init_meters(int total_peaks)
{
// 	peak_history = new float[total_peaks];
// 	
// 	for(int i = 0; i < total_peaks; i++)
// 	{
// 		peak_history[i] = 0;
// 	}
	return 0;
}

int AModule::stop_meters()
{
// 	delete [] peak_history;
// 	peak_history = 0;
// 	if(gui) gui->meter->reset();
	return 0;
}

int AModule::set_title(char *text)
{
	strcpy(title, text);
	if(gui) gui->title->update(text);

	for(int i = 0; i < PLUGINS; i++)
	{
		((APlugin*)plugins[i])->set_string();
	}
	return 0;
}

int AModule::change_format() { 
//if(gui) gui->meter->change_format(mwindow->preferences->meter_format); 	return 0;
}

int AModule::flip_vertical(int pixel) { if(gui) gui->flip_vertical(pixel); 	return 0;
}

int AModule::reset_meter() { gui->meter->reset_over(); 	return 0;
}

int AModule::change_channels(int new_channels, int *value_positions)
{
	if(gui)
	{
		gui->pan_stick->change_channels(new_channels, value_positions);
		gui->pan_stick->stick_to_values();
		for(int i = 0; i < gui->pan_stick->get_total_values(); i++)
			pan[i] = gui->pan_stick->get_value(i);
	}

//	for(int i = mwindow->session->audio_channels; i < new_channels; i++)
//	{
//		pan[i] = 0;
//	}
	return 0;
}

int AModule::set_mute(int value)
{
	mute = value;
	if(gui) gui->mute_toggle->update(value);
	return 0;
}








AModuleGUI::AModuleGUI(MWindow *mwindow, AModule *module, int x, int y, int w, int h)
 : BC_SubWindow(x, y, w, h, MEGREY)
{
	this->mwindow = mwindow;
	this->console = mwindow->console;
	this->module = module;
}

AModuleGUI::~AModuleGUI()
{
	delete title;
	delete inv_toggle;
	delete fade_slider;
	delete pan_stick;
	delete meter;
}

int AModuleGUI::create_objects()
{
	if(console->vertical)
	{
		int x, y;
// ================================== title

		x = 3; y = 3;
		add_subwindow(this->title = new AModuleTitle(module, module->get_patch_of(), x, y));

// =================================== inv

		y += 22;
		add_subwindow(inv_toggle = new AModuleInv(mwindow->console, module, x + 32, y));
		add_subwindow(inv_title = new BC_Title(x + 52, y+2, "Inv", SMALLFONT));

		y += 20;
		module->create_plugins(x, y);

// ================================== pans

		add_subwindow(pan_stick = new AModulePan(module, x, y, 30, 150, 1, mwindow->session->audio_channels, mwindow->session->achannel_positions, module->pan));

// ================================ fader

		add_subwindow(fade_title = new BC_Title(70, y - 3, "Fade", SMALLFONT));
		y += 10;
		add_subwindow(fade_slider = new AModuleFade(module, 70, y, 30, 184));

// ================================ meter

		y += 55;
//		add_subwindow(meter = new BC_Meter(5, y, 30, 129, mwindow->preferences->min_meter_db, mwindow->preferences->meter_format));

// =================================== mute

		y += 10;
		add_subwindow(mute_toggle = new AModuleMute(mwindow->console, module, 40, y));
		add_subwindow(mute_title = new BC_Title(40, y + 20, "Mute", SMALLFONT));

		y += 80;
		add_subwindow(reset = new AModuleReset(mwindow, module, 40, y));
	}
	else
	{
		int x, y;
// ================================== title

		x = 3; y = 3;
		add_subwindow(this->title = new AModuleTitle(module, module->get_patch_of(), x, y));

// =================================== inv

		y += 22;
		add_subwindow(inv_toggle = new AModuleInv(mwindow->console, module, x + 32, y));
		add_subwindow(inv_title = new BC_Title(x + 52, y+2, "Inv", SMALLFONT));
		
		y = 3;
		x += AMODULEWIDTH - 3;
		module->create_plugins(x, y);

// ================================== pans

		add_subwindow(pan_stick = new AModulePan(module, x, y, 30, 150, 1, mwindow->session->audio_channels, mwindow->session->achannel_positions, module->pan));

// ================================ fader

		x += 70;
		add_subwindow(fade_title = new BC_Title(x, y + 30, "Fade", SMALLFONT));
		//y += 10;
		add_subwindow(fade_slider = new AModuleFade(module, x, y, 184, 30));


// =================================== mute

		y += 30;
		add_subwindow(mute_toggle = new AModuleMute(mwindow->console, module, x + 32, y));
		add_subwindow(mute_title = new BC_Title(x + 52, y+2, "Mute", SMALLFONT));

// ================================ meter

		add_subwindow(reset = new AModuleReset(mwindow, module, x + 150, y));

		y += 23;
//		add_subwindow(meter = new BC_Meter(x, y, 184, 30, mwindow->preferences->min_meter_db, mwindow->preferences->meter_format));
	}

	pan_stick->handle_event();    // update the pan_x and pan_y in amodule
	return 0;
}

int AModuleGUI::flip_vertical(int pixel)
{
	module->pixel = pixel;

	if(console->vertical)
	reposition_window(pixel, 0, AMODULEWIDTH, console->gui->get_h() - 17);
	else
	reposition_window(0, pixel, console->gui->get_w() - 17, AMODULEHEIGHT);
	
	if(console->vertical)
	{
		int x, y;
// ================================== title
		x = 3; y = 3;
		title->reposition_window(x, y);

// =================================== inv
		y += 22;
		inv_toggle->reposition_window(x + 32, y);
		inv_title->reposition_window(x + 52, y+2);

// =================================== plugins
		y += 20;
		module->flip_plugins(x, y);

// ================================== pans
		pan_stick->reposition_window(x, y);

// ================================ fader
		fade_title->reposition_window(70, y - 3);
		y += 10;
		fade_slider->reposition_window(70, y, 30, 184);

// ================================ meter
		y += 55;
//		meter->reposition_window(5, y, 30, 129);


// =================================== mute
		y += 10;
		mute_toggle->reposition_window(40, y);
		mute_title->reposition_window(40, y+20);

		y += 80;
		reset->reposition_window(40, y);
	}
	else
	{
		int x, y;
// ================================== title
		x = 3; y = 3;
		title->reposition_window(x, y);

// =================================== inv
		y += 22;
		inv_toggle->reposition_window(x + 32, y);
		inv_title->reposition_window(x + 52, y+2);

		y = 3;
		x += AMODULEWIDTH - 3;
		module->flip_plugins(x, y);

// ================================== pans
		pan_stick->reposition_window(x, y);

// ================================ fader
		x += 70;
		fade_title->reposition_window(x, y + 30);
		fade_slider->reposition_window(x, y, 184, 30);

// =================================== mute
		y += 30;
		mute_toggle->reposition_window(x + 32, y);
		mute_title->reposition_window(x + 52, y+2);

// ================================ meter

		reset->reposition_window(x + 150, y);

		y += 23;
//		meter->reposition_window(x, y, 184, 30);
	}
	return 0;
}


AModuleTitle::AModuleTitle(AModule *module, Patch *patch, int x, int y)
 : BC_TextBox(x, y, 100, 1, patch->title, 0)
{
	this->module = module;
	this->patch = patch;
}

int AModuleTitle::handle_event()
{
	patch->set_title(get_text());
	strcpy(module->title, get_text());

	for(int i = 0; i < PLUGINS; i++)
	{
		((APlugin*)module->plugins[i])->set_string();
	}
	return 0;
}

AModuleInv::AModuleInv(Console *console, AModule *module, int x, int y)
 : BC_CheckBox(x, y, 0)
{
	this->console = console;
	this->module = module;
}

int AModuleInv::handle_event()
{
	// value is changed before this
	console->button_down = 1;
	console->new_status = get_value();
	module->inv = get_value();
	return 0;
}

int AModuleInv::cursor_moved_over()
{
	if(console->button_down && console->new_status != get_value())
	{
		update(console->new_status);
		module->inv = get_value();
		return 1;
	}
	return 0;
}

int AModuleInv::button_release()
{
	console->button_down = 0;
	return 0;
}

AModuleMute::AModuleMute(Console *console, AModule *module, int x, int y)
 : BC_CheckBox(x, y, 0)
{
	this->console = console;
	this->module = module;
}

int AModuleMute::handle_event()
{
	// value is changed before this
	console->button_down = 1;
	console->new_status = get_value();
	module->mute = get_value();
	return 0;
}

int AModuleMute::cursor_moved_over()
{
	if(console->button_down && console->new_status != get_value())
	{
		update(console->new_status);
		module->mute = get_value();
		return 1;
	}
	return 0;
	return 0;
}

int AModuleMute::button_release()
{
	console->button_down = 0;
	return 0;
}

AModulePan::AModulePan(AModule *module, 
	int x, 
	int y, 
	int r, 
	int virtual_r, 
	float maxvalue, 
	int total_values, 
	int *value_positions, 
	float *values)
 : BC_Pan(x, y, virtual_r, maxvalue, total_values, value_positions, 0, 0, values)
{
	this->module = module;
}

AModulePan::~AModulePan()
{
}

int AModulePan::handle_event()
{
// value is changed before this
	for(int i = 0; i < get_total_values(); i++)
		module->pan[i] = get_value(i);

	module->pan_x = get_stick_x();
	module->pan_y = get_stick_y();
	return 0;
}


AModuleFade::AModuleFade(AModule *module, int x, int y, int w, int h)
 : BC_FSlider(x, y, 1, h, 200, (float)INFINITYGAIN, (float)6, 0.0)
{
	this->module = module;
}

AModuleFade::~AModuleFade()
{
}

int AModuleFade::handle_event()
{
// value is changed before this
	module->fade = get_value();
	return 0;
}

AModuleReset::AModuleReset(MWindow *mwindow, AModule *module, int x, int y)
 : BC_Button(x, y, mwindow->theme->over_button)
{
	this->module = module;
}

AModuleReset::~AModuleReset()
{
}

int AModuleReset::handle_event()
{
	module->mwindow->reset_meters();
	return 0;
}
