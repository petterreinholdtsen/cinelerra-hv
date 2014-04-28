#include "panauto.h"
#include "panautos.h"

PanAutos::PanAutos(EDL *edl, Track *track)
 : Autos(edl, track)
{
}

PanAutos::~PanAutos()
{
}


Auto* PanAutos::new_auto()
{
	return new PanAuto(edl, this);
}


void PanAutos::dump()
{
	printf("	PanAutos::dump %p\n", this);
		printf("	Default: position %ld\n", default_auto->position);
		((PanAuto*)default_auto)->dump();
	for(Auto* current = first; current; current = NEXT)
	{
		printf("	position %ld\n", current->position);
		((PanAuto*)current)->dump();
	}
}
