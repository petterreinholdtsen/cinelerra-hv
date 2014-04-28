#include "audioalsa.h"
#include "audiodriver.h"
#include "audiooss.h"
#include "keys.h"
#include "mixer.h"
#include "mixergui.h"
#include "mixertree.h"
#include "vframe.h"


Mixer* MixerResources::mixer = 0;

MixerResources::MixerResources(Mixer *mixer)
 : BC_Signals()
{
	this->mixer = mixer;
}

void MixerResources::signal_handler(int signum)
{
//printf("MixerResources::signal_handler %d\n", signum);
	mixer->save_defaults();
	exit(0);
}

#include "theme/reload_dn_png.h"
#include "theme/reload_hi_png.h"
#include "theme/reload_up_png.h"

VFrame *reload_data[] = 
{
	new VFrame(reload_up_png),
	new VFrame(reload_hi_png),
	new VFrame(reload_dn_png)
};

#include "theme/pot_red_dn_png.h"
#include "theme/pot_red_hi_png.h"
#include "theme/pot_red_up_png.h"

static VFrame *pot_red_data[] = 
{
	new VFrame(pot_red_up_png),
	new VFrame(pot_red_hi_png),
	new VFrame(pot_red_dn_png),
};

#include "theme/pot_grey_dn_png.h"
#include "theme/pot_grey_hi_png.h"
#include "theme/pot_grey_up_png.h"

static VFrame *pot_grey_data[] = 
{
	new VFrame(pot_grey_up_png),
	new VFrame(pot_grey_hi_png),
	new VFrame(pot_grey_dn_png),
};

#include "theme/hslider_fg_up_png.h"
#include "theme/hslider_fg_hi_png.h"
#include "theme/hslider_fg_dn_png.h"
#include "theme/hslider_bg_up_png.h"
#include "theme/hslider_bg_hi_png.h"
#include "theme/hslider_bg_dn_png.h"
static VFrame *slider_data[] = 
{
	new VFrame(hslider_fg_up_png),
	new VFrame(hslider_fg_hi_png),
	new VFrame(hslider_fg_dn_png),
	new VFrame(hslider_bg_up_png),
	new VFrame(hslider_bg_hi_png),
	new VFrame(hslider_bg_dn_png)
};

char* Mixer::captions[] =
{
	"Master",
	"Bass",
	"Treble",
	"Line",
	"Dsp",
	"FM",
	"CD",
	"Mic",
	"Rec",
	"IGain",
	"OGain",
	"Speaker",
	"PhoneOut"
};

char* Mixer::default_headers[] = 
{ 
	"MASTER", 
	"BASS", 
	"TREBLE", 
	"LINE", 
	"DSP", 
	"FM", 
	"CD", 
	"MIC",
	"RECLEVEL",
	"IGAIN",
	"OGAIN",
	"SPEAKER",
	"PHONEOUT"
};

int Mixer::device_parameters[] = 
{ 
	SOUND_MIXER_VOLUME, 
	SOUND_MIXER_BASS, 
	SOUND_MIXER_TREBLE, 
	SOUND_MIXER_LINE, 
	SOUND_MIXER_PCM, 
	SOUND_MIXER_SYNTH, 
	SOUND_MIXER_CD, 
	SOUND_MIXER_MIC,
	SOUND_MIXER_RECLEV,
	SOUND_MIXER_IGAIN,
	SOUND_MIXER_OGAIN,
	SOUND_MIXER_SPEAKER,
	SOUND_MIXER_PHONEOUT
};

Mixer::Mixer(char *display, int channels)
{
	reset();
	this->channels = channels;
	this->display = display;
}

Mixer::~Mixer()
{
}

void Mixer::reset()
{
	audio = 0;
	tree = 0;
	defaults = 0;
}

int Mixer::create_objects()
{
	load_defaults();
	init_driver();
	init_display();
	gui->run_window();
	delete gui;
	return 0;
}


void Mixer::init_driver()
{
	if(audio)
	{
		delete audio;
		audio = 0;
	}

	if(tree)
	{
		delete tree;
		tree = 0;
	}

	tree = new MixerTree(this);
	
	switch(audio_driver)
	{
		case AUDIO_OSS:
			audio = new AudioOSS(this);
			break;
		case AUDIO_ALSA:
			audio = new AudioALSA(this);
			break;
	}
	audio->initialize();
}

void Mixer::init_display()
{
	int w, h;
	w = calculate_width();
	h = calculate_height();

	gui = new MixerGUI(this, x, y, w, h);
	gui->create_objects();
}


int Mixer::load_defaults()
{
	int i, j;
	char string[1024];

	if(!defaults)
	{
		defaults = new Defaults("~/.mix2000rc");
	}
	defaults->load();

	audio_driver = defaults->get("AUDIO_DRIVER", AUDIO_OSS);

// load all values
	for(i = 0; i < DEFINEDPOTS; i++)
	{
		sprintf(string, "%s_VISIBLE", default_headers[i]);
		visible[i] = defaults->get(string, 1);

		for(j = 0; j < channels; j++)
		{
			if(visible[i])
			{
				sprintf(string, "%s_%d", default_headers[i], j);
				values[i][j] = defaults->get(string, values[i][j]);
			}
			else
			{
// Default the values to 0.
				values[i][j] = 0;
			}
		}
	}

// Load toggles.
	line = defaults->get("LINE", line);
	mic = defaults->get("MIC", mic);
	cd = defaults->get("CD", cd);
	lock = defaults->get("LOCK", 1);
	x = defaults->get("X", 0);
	y = defaults->get("Y", 0);
	return 0;
}

int Mixer::save_defaults()
{
	int i, j;
	char string[1024];

// load all values
	for(i = 0; i < DEFINEDPOTS; i++)
	{
		sprintf(string, "%s_VISIBLE", default_headers[i]);
		defaults->update(string, visible[i]);

		for(j = 0; j < channels; j++)
		{
			sprintf(string, "%s_%d", default_headers[i], j);
			defaults->update(string, values[i][j]);
		}
	}

// Load toggles.
	defaults->update("LINE", line);
	defaults->update("MIC", mic);
	defaults->update("CD", cd);
	defaults->update("LOCK", lock);
	defaults->update("X", x);
	defaults->update("Y", y);

	defaults->save();
	return 0;
}



void Mixer::read_hardware()
{
}

void Mixer::zero()
{
}


int Mixer::calculate_width()
{
	int w = MIXER_X, i;
	for(i = 0; i < DEFINEDPOTS; i++)
	{
		switch(i)
		{
			case 0:
				if(visible[i]) w += DIVISION0;
				break;
			case 1:
				if(visible[i]) w += DIVISION1;
				break;
			case 2:
				if(visible[i]) w += DIVISION1;
				break;
			case 3:
				w += DIVISION2;
			default:
				if(visible[i]) w += DIVISION3;
				break;
		}
	}
	return w;
}

int Mixer::calculate_height()
{
	int h = 20 + pot_red_data[0]->get_h() * channels;
	if(h < 20 + pot_red_data[0]->get_h() * 2) 
		h = 20 + pot_red_data[0]->get_h() * 2;
	return h;
}

int Mixer::reconfigure_visible(int item, int new_value)
{
	visible[item] = new_value;
	gui->resize_window(calculate_width(), gui->get_h());
	gui->create_controls();
	gui->flush();

	return 0;
}


#if 0

int Mixer::reread_parameters()
{
	device->read_parameters();
	gui->line->update(line);
	gui->mic->update(mic);
	gui->cd->update(cd);
	if(visible[0])
		gui->master->update();
	if(visible[1])
		gui->bass->update();
	if(visible[2])
		gui->treble->update();
	for(int i = 3; i < DEFINEDPOTS; i++)
	{
		if(visible[i])
			gui->pots[i]->update();
	}
	return 0;
}

#endif
