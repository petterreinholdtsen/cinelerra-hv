#include "automation.h"
#include "cplayback.h"
#include "cwindow.h"
#include "edl.h"
#include "edlsession.h"
#include "intauto.h"
#include "intautos.h"
#include "localsession.h"
#include "mainundo.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "patchbay.h"
#include "patchgui.h"
#include "playbackengine.h"
#include "theme.h"
#include "track.h"
#include "trackcanvas.h"
#include "tracks.h"
#include "transportque.h"
#include "vframe.h"

PatchGUI::PatchGUI(MWindow *mwindow, 
		PatchBay *patchbay, 
		Track *track, 
		int x, 
		int y)
{
	this->mwindow = mwindow;
	this->patchbay = patchbay;
	this->track = track;
	this->x = x;
	this->y = y;
	title = 0;
	record = 0;
	play = 0;
//	automate = 0;
	gang = 0;
	draw = 0;
	mute = 0;
	expand = 0;
	change_source = 0;
}

PatchGUI::~PatchGUI()
{
	if(title) delete title;
	if(record) delete record;
	if(play) delete play;
//	if(automate) delete automate;
	if(gang) delete gang;
	if(draw) delete draw;
	if(mute) delete mute;
	if(expand) delete expand;
}

int PatchGUI::create_objects()
{
	return update(x, y);
}

int PatchGUI::reposition(int x, int y)
{
	int x1 = 0;
	int y1 = 0;

//printf("PatchGUI::reposition 1 %p\n", this);

	if(x != this->x || y != this->y)
	{
		this->x = x;
		this->y = y;

//printf("PatchGUI::reposition 1\n");
		if(title)
		{
			title->reposition_window(x1, y1 + y);
		}
//printf("PatchGUI::reposition 1\n");
		y1 += mwindow->theme->title_h;

		if(play)
		{
			play->reposition_window(x1, y1 + y);
			x1 += play->get_w();
			record->reposition_window(x1, y1 + y);
			x1 += record->get_w();
//			automate->reposition_window(x1, y1 + y);
//			x1 += automate->get_w();
			gang->reposition_window(x1, y1 + y);
			x1 += gang->get_w();
			draw->reposition_window(x1, y1 + y);
			x1 += draw->get_w();
			mute->reposition_window(x1, y1 + y);
			x1 += mute->get_w();

			expand->reposition_window(
				patchbay->get_w() - 10 - mwindow->theme->expandpatch_data[0]->get_w(), 
				y1 + y);
			x1 += expand->get_w();
		}
		y1 += mwindow->theme->play_h;
	}
	else
	{
		y1 += mwindow->theme->title_h;
		y1 += mwindow->theme->play_h;
	}

//printf("PatchGUI::reposition 2\n");
	return y1;
}

int PatchGUI::update(int x, int y)
{
//printf("PatchGUI::update 1 %p\n", this);
	reposition(x, y);
//printf("PatchGUI::update 1\n");

	int h = track->vertical_span(mwindow->theme);
	int y1 = 0;
	int x1 = 0;
//printf("PatchGUI::update 1\n");

	if(title)
	{
		if(h - y1 < 0)
		{
			delete title;
			title = 0;
		}
		else
		{
			title->update(track->title);
		}
	}
	else
	if(h - y1 >= 0)
	{
		patchbay->add_subwindow(title = new TitlePatch(mwindow, this, x1 + x, y1 + y));
	}
	y1 += mwindow->theme->title_h;
//printf("PatchGUI::update 1\n");

	if(play)
	{
		if(h - y1 < mwindow->theme->play_h)
		{
			delete play;
			delete record;
			delete gang;
			delete draw;
			delete mute;
			delete expand;
			play = 0;
			record = 0;
			draw = 0;
			mute = 0;
			expand = 0;
		}
		else
		{
			play->update(play->get_keyframe(mwindow, this)->value);
			record->update(track->record);
			gang->update(track->gang);
			draw->update(track->draw);
			mute->update(mute->get_keyframe(mwindow, this)->value);
			expand->update(track->expand_view);
		}
	}
	else
	if(h - y1 >= mwindow->theme->play_h)
	{
		patchbay->add_subwindow(play = new PlayPatch(mwindow, this, x1 + x, y1 + y));
		x1 += play->get_w();
		patchbay->add_subwindow(record = new RecordPatch(mwindow, this, x1 + x, y1 + y));
		x1 += record->get_w();
		patchbay->add_subwindow(gang = new GangPatch(mwindow, this, x1 + x, y1 + y));
		x1 += gang->get_w();
		patchbay->add_subwindow(draw = new DrawPatch(mwindow, this, x1 + x, y1 + y));
		x1 += draw->get_w();
		patchbay->add_subwindow(mute = new MutePatch(mwindow, this, x1 + x, y1 + y));
		x1 += mute->get_w();


		patchbay->add_subwindow(expand = new ExpandPatch(mwindow, 
			this, 
			patchbay->get_w() - 10 - mwindow->theme->expandpatch_data[0]->get_w(), 
			y1 + y));
		x1 += expand->get_w();
	}
	y1 += mwindow->theme->play_h;
//printf("PatchGUI::update 2\n");

	return y1;
}


void PatchGUI::toggle_behavior(int play, 
		int record, 
		int automate, 
		int gang, 
		int draw, 
		int mute,
		int expand,
		int value,
		BC_Toggle *toggle,
		int *output)
{
	if(toggle->shift_down())
	{
		int total_selected = mwindow->edl->tracks->total_of(play, 
			record, 
			automate, 
			gang,
			draw, 
			mute, 
			expand);

// nothing previously selected
		if(total_selected == 0)
		{
			mwindow->edl->tracks->select_all(play, 
				record, 
				automate, 
				gang,
				draw, 
				mute,
				expand,
				1);
		}
		else
		if(total_selected == 1)
		{
// this patch was previously the only one on
			if(*output)
			{
				mwindow->edl->tracks->select_all(play, 
					record, 
					automate, 
					gang,
					draw, 
					mute,
					expand,
					1);
			}
// another patch was previously the only one on
			else
			{
				mwindow->edl->tracks->select_all(play, 
					record, 
					automate, 
					gang,
					draw, 
					mute,
					expand,
					0);
				*output = 1;
			}
		}
		else
		if(total_selected > 1)
		{
			mwindow->edl->tracks->select_all(play, 
				record, 
				automate, 
				gang,
				draw, 
				mute,
				expand,
				0);
			*output = 1;
		}
		toggle->set_value(*output);
		patchbay->update();
	}
	else
	{
		*output = value;
	}

	if(play)
		mwindow->sync_parameters(CHANGE_EDL);

	if(mute)
		mwindow->sync_parameters(CHANGE_PARAMS);
	
// Update affected tracks in cwindow
	if(record)
		mwindow->cwindow->update(0, 1, 1);
}

PlayPatch::PlayPatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, y, 
		mwindow->theme->playpatch_data,
		get_keyframe(mwindow, patch)->value, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
	set_tooltip("Play track");
}

int PlayPatch::handle_event()
{
	IntAuto *current;
	double position = mwindow->edl->local_session->selectionstart;
	Autos *play_autos = patch->track->automation->play_autos;

	mwindow->undo->update_undo_before("keyframe", LOAD_AUTOMATION);

	current = (IntAuto*)play_autos->get_auto_for_editing(position);

	patch->toggle_behavior(1, 
		0, 
		0, 
		0,
		0, 
		0,
		0,
		get_value(),
		this,
		&current->value);

	current->value = get_value();

	mwindow->undo->update_undo_after();

	if(mwindow->edl->session->auto_conf->play)
	{
		mwindow->gui->canvas->draw_overlays();
		mwindow->gui->canvas->flash();
	}

	mwindow->sync_parameters(CHANGE_EDL);
	return 1;
}

IntAuto* PlayPatch::get_keyframe(MWindow *mwindow, PatchGUI *patch)
{
	Auto *current = 0;
	double unit_position = mwindow->edl->local_session->selectionstart;
	unit_position = mwindow->edl->align_to_frame(unit_position, 0);
	unit_position = patch->track->to_units(unit_position, 0);

	return (IntAuto*)patch->track->automation->play_autos->get_prev_auto(
		(long)unit_position, 
		PLAY_FORWARD,
		current);
}













RecordPatch::RecordPatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, y, 
		mwindow->theme->recordpatch_data,
		patch->track->record, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
	set_tooltip("Arm track");
}

int RecordPatch::handle_event()
{
	patch->toggle_behavior(0, 
		1, 
		0, 
		0,
		0, 
		0,
		0,
		get_value(),
		this,
		&patch->track->record);
	return 1;
}

TitlePatch::TitlePatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_TextBox(x, 
 		y, 
		patch->patchbay->get_w() - 10, 
		1,
		patch->track->title)
{
	this->mwindow = mwindow;
	this->patch = patch;
}

int TitlePatch::handle_event()
{
	strcpy(patch->track->title, get_text());
	mwindow->update_plugin_titles();
	mwindow->gui->canvas->draw_overlays();
	mwindow->gui->canvas->flash();
	return 1;
}









GangPatch::GangPatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, y, 
		mwindow->theme->gangpatch_data,
		patch->track->gang, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
	set_tooltip("Gang faders");
}

int GangPatch::handle_event()
{
	patch->toggle_behavior(0, 
		0, 
		0, 
		1,
		0, 
		0,
		0,
		get_value(),
		this,
		&patch->track->gang);
	return 1;
}

DrawPatch::DrawPatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, y, 
		mwindow->theme->drawpatch_data,
		patch->track->draw, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
	set_tooltip("Draw media");
}

int DrawPatch::handle_event()
{
	patch->toggle_behavior(0, 
		0, 
		0, 
		0,
		1, 
		0,
		0,
		get_value(),
		this,
		&patch->track->draw);
	return 1;
}

MutePatch::MutePatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, y, 
		mwindow->theme->mutepatch_data,
		get_keyframe(mwindow, patch)->value, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
	set_tooltip("Don't send to output");
}

int MutePatch::handle_event()
{
	IntAuto *current;
	double position = mwindow->edl->local_session->selectionstart;
	Autos *mute_autos = patch->track->automation->mute_autos;

	mwindow->undo->update_undo_before("keyframe", LOAD_AUTOMATION);

	current = (IntAuto*)mute_autos->get_auto_for_editing(position);

	patch->toggle_behavior(0, 
		0, 
		0, 
		0,
		0, 
		1,
		0,
		get_value(),
		this,
		&current->value);

	current->value = get_value();

	mwindow->undo->update_undo_after();

	if(mwindow->edl->session->auto_conf->mute)
	{
		mwindow->gui->canvas->draw_overlays();
		mwindow->gui->canvas->flash();
	}
	mwindow->sync_parameters(CHANGE_PARAMS);
	return 1;
}

IntAuto* MutePatch::get_keyframe(MWindow *mwindow, PatchGUI *patch)
{
	Auto *current = 0;
	double unit_position = mwindow->edl->local_session->selectionstart;
	unit_position = mwindow->edl->align_to_frame(unit_position, 0);
	unit_position = patch->track->to_units(unit_position, 0);
	return (IntAuto*)patch->track->automation->mute_autos->get_prev_auto(
		(long)unit_position, 
		PLAY_FORWARD,
		current);
}












ExpandPatch::ExpandPatch(MWindow *mwindow, PatchGUI *patch, int x, int y)
 : BC_Toggle(x, 
 		y, 
		mwindow->theme->expandpatch_data,
		patch->track->expand_view, 
		"",
		0,
		0,
		0)
{
	this->mwindow = mwindow;
	this->patch = patch;
}

int ExpandPatch::handle_event()
{
	patch->toggle_behavior(0, 
		0, 
		0, 
		0,
		0, 
		0,
		1,
		get_value(),
		this,
		&patch->track->expand_view);
	mwindow->trackmovement(mwindow->edl->local_session->track_start);
	return 1;
}





