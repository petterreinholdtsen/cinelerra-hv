#include "assets.h"
#include "edits.h"
#include "aedit.h"
#include "cache.h"
#include "edl.h"
#include "edlsession.h"
#include "file.h"
#include "filexml.h"
#include "indexfile.h"
#include "mwindow.h"
#include "patch.h"
#include "pluginbuffer.h"
#include "mainsession.h"
#include "trackcanvas.h"
#include "tracks.h"


AEdit::AEdit(EDL *edl, Edits *edits)
 : Edit(edl, edits)
{
}















AEdit::~AEdit() { }

int AEdit::set_index_file(int flash, int center_pixel, int x, int y, int w, int h)
{
	if(asset && asset->index_zoom <= mwindow->session->zoom_sample)
		draw(flash, center_pixel, x, w, y, h, 1);
}

int AEdit::load_properties_derived(FileXML *xml)
{
	channel = xml->tag.get_property("CHANNEL", (long)0);
	return 0;
}

// ============================= edit drawing commands ===========================

// REMOVE
int AEdit::draw(int flash, 
			int center_pixel, 
			int x, 
			int w, 
			int y, 
			int h, 
			int set_index_file)
{
}

int AEdit::draw_source(int pixel, int center_pixel, int h, long zoom_sample, long zoomy, long startsource, long endsource, int vertical)
{
}

int AEdit::draw_direct(int pixel, int center_pixel, int h, long zoom_sample, long zoomy, long startsource, long endsource, int vertical)
{
}

// ========================================== editing

int AEdit::copy_properties_derived(FileXML *xml, long length_in_selection)
{
	return 0;
}

int AEdit::get_handle_parameters(long &left, 
	long &right, 
	long &left_sample, 
	long &right_sample, 
	float view_start, 
	float zoom_units)
{
// 	left = (long)((startproject - view_start) / zoom_units);
// 	right = (long)(((startproject + length) - view_start) / zoom_units);
// 	left_sample = startproject;
// 	right_sample = startproject + length;
	return 0;
}


int AEdit::dump_derived()
{
	//printf("	channel %d\n", channel);
}


long AEdit::get_source_end(long default_)
{
	if(!asset) return default_;   // Infinity

	return (long)((double)asset->audio_length / asset->sample_rate * edl->session->sample_rate + 0.5);
}


int AEdit::render(PluginBuffer *shared_output, 
			   long offset, 
			   long input_len, 
               long input_position)
{
	return 0;
}
