#ifndef VEDITS_H
#define VEDITS_H

#include "edits.h"
#include "edl.inc"
#include "filexml.h"
#include "mwindow.inc"
#include "vtrack.inc"



class VEdits : public Edits
{
public:
	VEdits(EDL *edl, Track *track);


// Editing
	Edit* create_edit();












	VEdits() {printf("default edits constructor called\n");};
	~VEdits() {};


// ========================================= editing

	Edit* append_new_edit();
	Edit* insert_edit_after(Edit* previous_edit);
	int clone_derived(Edit* new_edit, Edit* old_edit);

// also known as paste_output
	int paste_edit(long start, 
				long end, 
				long startsource, 
				long length, 
				int layer, 
				int center_x, 
				int center_y, 
				int center_z, 
				Asset *asset);

	VTrack *vtrack;
};


#endif
