#ifndef AUDIOALSA_H
#define AUDIOALSA_H

#include "audiodriver.h"
#include "alsa/asoundlib.h"




class AudioALSA : public AudioDriver
{
public:
	AudioALSA(Mixer *mixer);
	~AudioALSA();

	int construct_tree();

	void *mixer_sid;
	int mixer_n_selems;
	int mixer_n_elems;
	snd_mixer_t *mixer_handle;
	int *mixer_type;
	int *mixer_grpidx;
};





#endif
