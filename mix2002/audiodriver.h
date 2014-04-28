#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H


#include "mixer.inc"

class AudioDriver
{
public:
	AudioDriver(Mixer *mixer);
	virtual ~AudioDriver();

	virtual int construct_tree() { return 1; };
	void initialize();

	Mixer *mixer;
	int device_open;
};



#endif
