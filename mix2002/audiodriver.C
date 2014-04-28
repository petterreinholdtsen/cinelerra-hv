#include "audiodriver.h"



AudioDriver::AudioDriver(Mixer *mixer)
{
	this->mixer = mixer;
	device_open = 0;
}

AudioDriver::~AudioDriver()
{
}

void AudioDriver::initialize()
{
	int result = construct_tree();
}


