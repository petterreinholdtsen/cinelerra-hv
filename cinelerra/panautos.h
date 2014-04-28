#ifndef PANAUTOS_H
#define PANAUTOS_H

#include "autos.h"
#include "edl.inc"
#include "track.inc"

class PanAutos : public Autos
{
public:
	PanAutos(EDL *edl, Track *track);
	~PanAutos();
	
	Auto* new_auto();
	void dump();
};

#endif
