#ifndef INTAUTO_H
#define INTAUTO_H

#include "auto.h"
#include "edl.inc"
#include "filexml.inc"
#include "maxchannels.h"
#include "intautos.inc"

class IntAuto : public Auto
{
public:
	IntAuto(EDL *edl, IntAutos *autos);
	~IntAuto();

	void copy_from(Auto *that);
	void copy_from(IntAuto *that);
	int operator==(Auto &that);
	int operator==(IntAuto &that);

	int identical(IntAuto *that);
	void load(FileXML *file);
	void copy(long start, long end, FileXML *file);
	float value_to_percentage();
	int percentage_to_value(float percentage);

	int value;
};

#endif