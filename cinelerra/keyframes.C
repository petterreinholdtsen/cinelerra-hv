#include "filexml.h"
#include "keyframe.h"
#include "keyframes.h"

KeyFrames::KeyFrames(EDL *edl, Track *track)
 : Autos(edl, track)
{
}

KeyFrames::~KeyFrames()
{
}

Auto* KeyFrames::new_auto()
{
	return new KeyFrame(edl, this);
}


	
