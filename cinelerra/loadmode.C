
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include "clip.h"
#include "language.h"
#include "loadmode.h"
#include "mwindow.h"
#include "theme.h"

// Must match macros
static const char *mode_images[] = 
{
	"loadmode_none",
	"loadmode_new",
	"loadmode_newcat",
	"loadmode_newtracks",
	"loadmode_cat",
	"loadmode_paste",
	"loadmode_resource"
};
static const char *tooltips[] = 
{
	"Insert nothing",
	"Replace current project",
	"Replace current project and concatenate tracks",
	"Append in new tracks",
	"Concatenate to existing tracks",
	"Paste at insertion point",
	"Create new resources only"
};




LoadMode::LoadMode(MWindow *mwindow,
	BC_WindowBase *window, 
	int x, 
	int y, 
	int *output, 
	int use_nothing)
{
	this->mwindow = mwindow;
	this->window = window;
	this->x = x;
	this->y = y;
	this->output = output;
	this->use_nothing = use_nothing;
	for(int i = 0; i < TOTAL_LOADMODES; i++)
		mode[i] = 0;
// 	if(use_nothing) load_modes.append(new LoadModeItem(_("Insert nothing"), LOAD_NOTHING));
// 	load_modes.append(new LoadModeItem(_("Replace current project"), LOAD_REPLACE));
// 	load_modes.append(new LoadModeItem(_("Replace current project and concatenate tracks"), LOAD_REPLACE_CONCATENATE));
// 	load_modes.append(new LoadModeItem(_("Append in new tracks"), LOAD_NEW_TRACKS));
// 	load_modes.append(new LoadModeItem(_("Concatenate to existing tracks"), LOAD_CONCATENATE));
// 	load_modes.append(new LoadModeItem(_("Paste at insertion point"), LOAD_PASTE));
// 	load_modes.append(new LoadModeItem(_("Create new resources only"), LOAD_RESOURCESONLY));
}

LoadMode::~LoadMode()
{
	delete title;
	for(int i = 0; i < TOTAL_LOADMODES; i++)
		if(mode[i]) delete mode[i];
//	delete textbox;
//	delete listbox;
//	for(int i = 0; i < load_modes.total; i++)
//		delete load_modes.values[i];
}

// char* LoadMode::mode_to_text()
// {
// 	for(int i = 0; i < load_modes.total; i++)
// 	{
// 		if(load_modes.values[i]->value == *output) 
// 			return load_modes.values[i]->get_text();
// 	}
// 	return _("Unknown");
// }

int LoadMode::calculate_h(BC_WindowBase *gui, Theme *theme)
{
	int text_line;
	int w;
	int h;
	int toggle_x;
	int toggle_y;
	int text_x;
	int text_y;
	int text_w;
	int text_h;
	BC_Toggle::calculate_extents(gui, 
		theme->get_image_set(mode_images[0]),
		0,
		&text_line,
		&w,
		&h,
		&toggle_x,
		&toggle_y,
		&text_x,
		&text_y, 
		&text_w,
		&text_h, 
		0);
	return h;
//	return BC_TextBox::calculate_h(gui, MEDIUMFONT, 1, 1);
}

int LoadMode::calculate_w(BC_WindowBase *gui, Theme *theme, int use_none)
{
	int total = gui->get_text_width(MEDIUMFONT, _("Insertion strategy:")) + 10;
	for(int i = 0; i < TOTAL_LOADMODES; i++)
	{
		if(i != 0 || use_none)
		{
			int text_line;
			int w;
			int h;
			int toggle_x;
			int toggle_y;
			int text_x;
			int text_y;
			int text_w;
			int text_h;
			BC_Toggle::calculate_extents(gui, 
				theme->get_image_set(mode_images[i]),
				0,
				&text_line,
				&w,
				&h,
				&toggle_x,
				&toggle_y,
				&text_x,
				&text_y, 
				&text_w,
				&text_h, 
				0);
			total += w + 10;
		}
	}
	return total;
}

int LoadMode::get_h()
{
	int result = 0;
	result = MAX(result, title->get_h());
//	result = MAX(result, textbox->get_h());
	result = MAX(result, mode[1]->get_h());
	return result;
}

void LoadMode::create_objects()
{
	int x = this->x, y = this->y;
//	char *default_text;
//	default_text = mode_to_text();

	window->add_subwindow(title = new BC_Title(x, y, _("Insertion strategy:")));
//	y += 20;
	x += title->get_w() + 10;
	int x1 = x;
	for(int i = 0; i < TOTAL_LOADMODES; i++)
	{
		if(i != LOAD_NOTHING || use_nothing)
		{
			VFrame **images = mwindow->theme->get_image_set(mode_images[i]);
			if(x + images[0]->get_w() > window->get_w())
			{
				x = x1;
				y += images[0]->get_h() + 5;
			}
			window->add_subwindow(mode[i] = new LoadModeToggle(x, 
				y, 
				this, 
				i, 
				mode_images[i],
				tooltips[i]));
			x += mode[i]->get_w() + 10;
		}
	}

// 	window->add_subwindow(textbox = new BC_TextBox(x, 
// 		y, 
// 		mwindow->theme->loadmode_w, 
// 		1, 
// 		default_text));
// 	x += textbox->get_w();
// 	window->add_subwindow(listbox = new LoadModeListBox(window, this, x, y));

}

int LoadMode::get_x()
{
	return x;
}

int LoadMode::get_y()
{
	return y;
}

int LoadMode::reposition_window(int x, int y)
{
	this->x = x;
	this->y = y;
	title->reposition_window(x, y);
//	y += 20;
	x += title->get_w() + 10;
	int x1 = x;
	for(int i = 0; i < TOTAL_LOADMODES; i++)
	{
		if(mode[i])
		{
			VFrame **images = mwindow->theme->get_image_set(mode_images[i]);
			if(x + images[0]->get_w() > window->get_w())
			{
				x = x1;
				y += images[0]->get_h() + 5;
			}
			mode[i]->reposition_window(x, y);
			x += mode[i]->get_w() + 10;
		}
	}

// 	textbox->reposition_window(x, y);
// 	x += textbox->get_w();
// 	listbox->reposition_window(x, 
// 		y, 
// 		mwindow->theme->loadmode_w);
	return 0;
}

void LoadMode::update()
{
	for(int i = 0; i < TOTAL_LOADMODES; i++)
	{
		if(mode[i])
		{
			mode[i]->set_value(*output == i);
		}
	}
}



// LoadModeListBox::LoadModeListBox(BC_WindowBase *window, 
// 	LoadMode *loadmode, 
// 	int x, 
// 	int y)
//  : BC_ListBox(x,
//  	y,
// 	loadmode->mwindow->theme->loadmode_w,
// 	150,
// 	LISTBOX_TEXT,
// 	(ArrayList<BC_ListBoxItem *>*)&loadmode->load_modes,
// 	0,
// 	0,
// 	1,
// 	0,
// 	1)
// {
// 	this->window = window;
// 	this->loadmode = loadmode;
// }
// 
// LoadModeListBox::~LoadModeListBox()
// {
// }
// 
// int LoadModeListBox::handle_event()
// {
// 	if(get_selection(0, 0) >= 0)
// 	{
// 		loadmode->textbox->update(get_selection(0, 0)->get_text());
// 		*(loadmode->output) = ((LoadModeItem*)get_selection(0, 0))->value;
// 	}
// 	return 1;
// }
// 
// 
// LoadModeItem::LoadModeItem(char *text, int value)
//  : BC_ListBoxItem(text)
// {
// 	this->value = value;
// }
// 





LoadModeToggle::LoadModeToggle(int x, 
	int y, 
	LoadMode *window, 
	int value, 
	const char *images,
	const char *tooltip)
 : BC_Toggle(x, 
 	y, 
	window->mwindow->theme->get_image_set(images),
	*window->output == value)
{
	this->window = window;
	this->value = value;
	set_tooltip(_(tooltip));
}

int LoadModeToggle::handle_event()
{
	*window->output = value;
	window->update();
	return 1;
}
