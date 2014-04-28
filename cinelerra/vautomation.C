#include "bezierautos.h"
#include "clip.h"
#include "colors.h"
#include "edl.h"
#include "edlsession.h"
#include "floatauto.h"
#include "floatautos.h"
#include "intauto.h"
#include "intautos.h"
#include "maskautos.h"
#include "overlayframe.inc"
#include "transportque.inc"
#include "vautomation.h"


VAutomation::VAutomation(EDL *edl, Track *track)
 : Automation(edl, track)
{
}



VAutomation::~VAutomation()
{
}


int VAutomation::create_objects()
{
	Automation::create_objects();
	fade_autos = new FloatAutos(edl, track, LTGREY, 0, 100);
	fade_autos->create_objects();
	((FloatAuto*)fade_autos->default_auto)->value = 100;
	mode_autos = new IntAutos(edl, track);
	mode_autos->create_objects();
	mask_autos = new MaskAutos(edl, track);
	mask_autos->create_objects();
	((IntAuto*)mode_autos->default_auto)->value = TRANSFER_NORMAL;
	camera_autos = new BezierAutos(edl, 
									track, 
									WHITE, 
									0, 
									0, 
									1, 
									edl->session->output_w,
									edl->session->output_h);
	camera_autos->create_objects();

	projector_autos = new BezierAutos(edl, 
									track,
									WHITE,
									0,
									0,
									1,
									edl->session->output_w,
									edl->session->output_h);
	projector_autos->create_objects();
	czoom_autos = new FloatAutos(edl, track, LTGREY, 0, 10);
	czoom_autos->create_objects();
	pzoom_autos = new FloatAutos(edl, track, LTGREY, 0, 10);
	pzoom_autos->create_objects();

	
	return 0;
}

int VAutomation::direct_copy_possible(long start, int direction)
{
	BezierAuto *before = 0, *after = 0;
	float x, y, z;
	long end = (direction == PLAY_FORWARD) ? (start + 1) : (start - 1);

	if(!Automation::direct_copy_possible(start, direction))
		return 0;

//printf("VAutomation::direct_copy_possible 1\n");
// Automation is constant
	if(fade_autos->automation_is_constant(start, end))
	{
//printf("VAutomation::direct_copy_possible 2 %f\n", fade_autos->get_automation_constant(start, end));
		if(fade_autos->get_automation_constant(start, end) != 100)
			return 0;
	}
	else
// Automation varies
		return 0;

//printf("VAutomation::direct_copy_possible 3\n");
// Track must not be muted
	if(mute_autos->automation_is_constant(start, end))
	{
//printf("VAutomation::direct_copy_possible 4 %d\n", mute_autos->get_automation_constant(start, end));
		if(mute_autos->get_automation_constant(start, end) > 0)
			return 0;
	}
	else
		return 0;

//printf("VAutomation::direct_copy_possible 5\n");
// Projector must be centered in an output channel
	FloatAuto *previous = 0, *next = 0;
	z = pzoom_autos->get_value(start, direction, previous, next);






	projector_autos->get_center(x, 
				y, 
				z, 
				start,
				direction,
				&before, 
				&after);
// FIXME develop channel search using track->get_projection
	if(!EQUIV(x, 0) || 
		!EQUIV(y, 0) || 
		!EQUIV(z, 1)) return 0;

//printf("VAutomation::direct_copy_possible 6 %f %f %f\n", x, y, z);


// No mask must exist
//printf("VAutomation::direct_copy_possible 1\n");
	if(mask_autos->mask_exists(start, direction))
		return 0;
//printf("VAutomation::direct_copy_possible 7\n");


// Camera must be centered
	z = czoom_autos->get_value(start, direction, previous, next);



	before = 0;
	after = 0;
	camera_autos->get_center(x, 
				y, 
				z, 
				start,
				direction,
				&before, 
				&after);

//printf("VAutomation::direct_copy_possible 8 %f %f %f\n", x, y, z);
// Translation no longer used
	if(!EQUIV(x, 0) || 
		!EQUIV(y, 0) || 
		!EQUIV(z, 1)) return 0;
//printf("VAutomation::direct_copy_possible 9\n");

	return 1;
}

