#include "assets.h"
#include "edit.h"
#include "vedit.h"
#include "vedits.h"
#include "filesystem.h"
#include "mwindow.h"
#include "loadfile.h"
#include "vtrack.h"








VEdits::VEdits(EDL *edl, Track *track)
 : Edits(edl, track)
{
}

Edit* VEdits::create_edit()
{
	return new VEdit(edl, this);
}

Edit* VEdits::insert_edit_after(Edit* previous_edit)
{
	VEdit *current = new VEdit(edl, this);

	List<Edit>::insert_after(previous_edit, current);

//printf("VEdits::insert_edit_after %p %p\n", current->track, current->edits);
	return (Edit*)current;
}









Edit* VEdits::append_new_edit()
{
	VEdit *current;
	List<Edit>::append(current = new VEdit(edl, this));
	return (Edit*)current;
}

int VEdits::clone_derived(Edit* new_edit, Edit* old_edit)
{
	return 0;
}

int VEdits::paste_edit(long start, 
				long end, 
				long startsource, 
				long length, 
				int layer, 
				int center_x, 
				int center_y, 
				int center_z, 
				Asset *asset)
{
	VEdit* current = (VEdit*)paste_edit_base(start, end, startsource, length, asset);

	current->channel = layer;
	current->center_x = center_x;
	current->center_y = center_y;
	current->center_z = center_z;

	optimize();
}


