#include "asset.h"
#include "bcresources.h"
#include "blond/bg_image_png.h"
#include "blond/bar_left_png.h"
#include "blond/bar_mid_png.h"
#include "blond/bar_right_png.h"
#include "blond/checkbox_checked_png.h"
#include "blond/checkbox_checkedhi_png.h"
#include "blond/checkbox_downhi_png.h"
#include "blond/checkbox_up_png.h"
#include "blond/checkbox_uphi_png.h"
#include "blond/files_icon_dn_png.h"
#include "blond/files_icon_hi_png.h"
#include "blond/files_icon_up_png.h"
#include "blond/files_newfolder_dn_png.h"
#include "blond/files_newfolder_hi_png.h"
#include "blond/files_newfolder_up_png.h"
#include "blond/files_prevdir_dn_png.h"
#include "blond/files_prevdir_hi_png.h"
#include "blond/files_prevdir_up_png.h"
#include "blond/files_text_dn_png.h"
#include "blond/files_text_hi_png.h"
#include "blond/files_text_up_png.h"
#include "blond/framebacksmall_downhi_png.h"
#include "blond/framebacksmall_up_png.h"
#include "blond/framebacksmall_uphi_png.h"
#include "blond/framefwdsmall_downhi_png.h"
#include "blond/framefwdsmall_up_png.h"
#include "blond/framefwdsmall_uphi_png.h"
#include "blond/heroine_bg_png.h"
#include "blond/heroine_icon_png.h"
#include "blond/listbutton_dn_png.h"
#include "blond/listbutton_hi_png.h"
#include "blond/listbutton_up_png.h"
#include "blond/pausesmall_downhi_png.h"
#include "blond/pausesmall_up_png.h"
#include "blond/pausesmall_uphi_png.h"
#include "blond/playsmall_downhi_png.h"
#include "blond/playsmall_up_png.h"
#include "blond/playsmall_uphi_png.h"
#include "blond/usethis_dn_png.h"
#include "blond/usethis_up_png.h"
#include "blond/usethis_uphi_png.h"
#include "clip.h"
#include "guicast.h"
#include "mainmenu.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "theme.h"
#include "vframe.h"


Theme::Theme()
{

}

void Theme::build_button(VFrame** &data,
	unsigned char *png_overlay,
	VFrame *up_vframe,
	VFrame *hi_vframe,
	VFrame *dn_vframe)
{
	if(!png_overlay) return;
	VFrame default_data(png_overlay);

	if(!up_vframe || !hi_vframe || !dn_vframe) return;
	data = new VFrame*[3];
	data[0] = new VFrame(0, default_data.get_w(), default_data.get_h(), BC_RGBA8888);
	data[1] = new VFrame(0, default_data.get_w(), default_data.get_h(), BC_RGBA8888);
	data[2] = new VFrame(0, default_data.get_w(), default_data.get_h(), BC_RGBA8888);
	data[0]->copy_from(up_vframe);
	data[1]->copy_from(hi_vframe);
	data[2]->copy_from(dn_vframe);
	for(int i = 0; i < 3; i++)
		overlay(data[i], &default_data);
}

void Theme::overlay(VFrame *dst, VFrame *src)
{
	int w = MIN(src->get_w(), dst->get_w());
	int h = MIN(dst->get_h(), src->get_h());
	unsigned char **in_rows = src->get_rows();
	unsigned char **out_rows = dst->get_rows();

	switch(src->get_color_model())
	{
		case BC_RGBA8888:
			switch(dst->get_color_model())
			{
				case BC_RGBA8888:
					for(int i = 0; i < h; i++)
					{
						unsigned char *in_row = in_rows[i];
						unsigned char *out_row = out_rows[i];
						for(int j = 0; j < w; j++)
						{
							int opacity = in_row[3];
							int transparency = 0xff - opacity;
							out_row[0] = (in_row[0] * opacity + out_row[0] * transparency) / 0xff;
							out_row[1] = (in_row[1] * opacity + out_row[1] * transparency) / 0xff;
							out_row[2] = (in_row[2] * opacity + out_row[2] * transparency) / 0xff;
							out_row[3] = MAX(in_row[3], out_row[3]);
							out_row += 4;
							in_row += 4;
						}
					}
					break;
				case BC_RGB888:
					for(int i = 0; i < h; i++)
					{
						unsigned char *in_row = in_rows[i];
						unsigned char *out_row = out_rows[i];
						for(int j = 0; j < w; j++)
						{
							int opacity = in_row[3];
							int transparency = 0xff - opacity;
							out_row[0] = (in_row[0] * opacity + out_row[0] * transparency) / 0xff;
							out_row[1] = (in_row[1] * opacity + out_row[1] * transparency) / 0xff;
							out_row[2] = (in_row[2] * opacity + out_row[2] * transparency) / 0xff;
							out_row += 3;
							in_row += 4;
						}
					}
					break;
			}
			break;
	}
}

GoldTheme::GoldTheme()
 : Theme()
{
	static VFrame *blond_checkbox[5] =
	{
		new VFrame(checkbox_up_png),
		new VFrame(checkbox_uphi_png),
		new VFrame(checkbox_checked_png),
		new VFrame(checkbox_downhi_png),
		new VFrame(checkbox_checkedhi_png)
	};

	static VFrame *blond_listbutton[3] = 
	{
		new VFrame(listbutton_up_png),
		new VFrame(listbutton_hi_png),
		new VFrame(listbutton_dn_png)
	};

	static VFrame *blond_bg_image = new VFrame(bg_image_png);

	static VFrame *blond_pause[] = 
	{
		new VFrame(pausesmall_up_png),
		new VFrame(pausesmall_uphi_png),
		new VFrame(pausesmall_downhi_png)
	};

	static VFrame *blond_filebox_text_images[] = 
	{
		new VFrame(files_text_up_png),
		new VFrame(files_text_hi_png),
		new VFrame(files_text_dn_png)
	};

	static VFrame *blond_filebox_icons_images[] = 
	{
		new VFrame(files_icon_up_png),
		new VFrame(files_icon_hi_png),
		new VFrame(files_icon_dn_png)
	};

	static VFrame *blond_filebox_updir_images[] = 
	{
		new VFrame(files_prevdir_up_png),
		new VFrame(files_prevdir_hi_png),
		new VFrame(files_prevdir_dn_png)
	};

	static VFrame *blond_filebox_newfolder_images[] = 
	{
		new VFrame(files_newfolder_up_png),
		new VFrame(files_newfolder_hi_png),
		new VFrame(files_newfolder_dn_png)
	};

	static VFrame *blond_play[] = 
	{
		new VFrame(playsmall_up_png),
		new VFrame(playsmall_uphi_png),
		new VFrame(playsmall_downhi_png)
	};


#include "blond/slider_bg_up_png.h"
#include "blond/slider_dn_png.h"
#include "blond/slider_up_png.h"
#include "blond/slider_uphi_png.h"

	static VFrame *blond_slider_data[] = 
	{
		new VFrame(slider_up_png),
		new VFrame(slider_uphi_png),
		new VFrame(slider_dn_png),
		new VFrame(slider_bg_up_png),
		new VFrame(slider_bg_up_png),
		new VFrame(slider_bg_up_png)
	};

	static VFrame *blond_frame_fwd[] = 
	{
		new VFrame(framefwdsmall_up_png),
		new VFrame(framefwdsmall_uphi_png),
		new VFrame(framefwdsmall_downhi_png)
	};

	static VFrame *blond_frame_bck[] = 
	{
		new VFrame(framebacksmall_up_png),
		new VFrame(framebacksmall_uphi_png),
		new VFrame(framebacksmall_downhi_png)
	};

	icon = new VFrame(heroine_icon_png);
	BC_WindowBase::get_resources()->bg_image = 0;
	BC_WindowBase::get_resources()->bg_color = BLOND;
//	BC_WindowBase::get_resources()->ok_images = blond_ok_images;
//	BC_WindowBase::get_resources()->cancel_images = blond_cancel_images;
//	BC_WindowBase::get_resources()->checkbox_images = blond_checkbox;
	BC_WindowBase::get_resources()->button_up = 0xffc000;
	BC_WindowBase::get_resources()->button_highlighted = 0xffe000;
//	BC_WindowBase::get_resources()->filebox_text_images = blond_filebox_text_images;
//	BC_WindowBase::get_resources()->filebox_icons_images = blond_filebox_icons_images;
//	BC_WindowBase::get_resources()->filebox_updir_images = blond_filebox_updir_images;
//	BC_WindowBase::get_resources()->filebox_newfolder_images = blond_filebox_newfolder_images;
//	BC_WindowBase::get_resources()->listbox_button = blond_listbutton;
	BC_WindowBase::get_resources()->horizontal_slider_data = blond_slider_data;


#include "blond/hscroll_center_up_png.h"
#include "blond/hscroll_center_hi_png.h"
#include "blond/hscroll_center_dn_png.h"
#include "blond/hscroll_bg_png.h"
#include "blond/hscroll_back_up_png.h"
#include "blond/hscroll_back_hi_png.h"
#include "blond/hscroll_back_dn_png.h"
#include "blond/hscroll_fwd_up_png.h"
#include "blond/hscroll_fwd_hi_png.h"
#include "blond/hscroll_fwd_dn_png.h"

	static VFrame* hscroll_data[] = 
	{
		new VFrame(hscroll_center_up_png),
		new VFrame(hscroll_center_hi_png),
		new VFrame(hscroll_center_dn_png),
		new VFrame(hscroll_bg_png),
		new VFrame(hscroll_back_up_png),
		new VFrame(hscroll_back_hi_png),
		new VFrame(hscroll_back_dn_png),
		new VFrame(hscroll_fwd_up_png),
		new VFrame(hscroll_fwd_hi_png),
		new VFrame(hscroll_fwd_dn_png)
	};
	BC_WindowBase::get_resources()->hscroll_data = hscroll_data;

#include "blond/vscroll_center_up_png.h"
#include "blond/vscroll_center_hi_png.h"
#include "blond/vscroll_center_dn_png.h"
#include "blond/vscroll_bg_png.h"
#include "blond/vscroll_back_up_png.h"
#include "blond/vscroll_back_hi_png.h"
#include "blond/vscroll_back_dn_png.h"
#include "blond/vscroll_fwd_up_png.h"
#include "blond/vscroll_fwd_hi_png.h"
#include "blond/vscroll_fwd_dn_png.h"

	static VFrame* vscroll_data[] = 
	{
		new VFrame(vscroll_center_up_png),
		new VFrame(vscroll_center_hi_png),
		new VFrame(vscroll_center_dn_png),
		new VFrame(vscroll_bg_png),
		new VFrame(vscroll_back_up_png),
		new VFrame(vscroll_back_hi_png),
		new VFrame(vscroll_back_dn_png),
		new VFrame(vscroll_fwd_up_png),
		new VFrame(vscroll_fwd_hi_png),
		new VFrame(vscroll_fwd_dn_png)
	};
	BC_WindowBase::get_resources()->vscroll_data = vscroll_data;

#include "blond/generic_up_png.h"
#include "blond/generic_hi_png.h"
#include "blond/generic_dn_png.h"
	static VFrame *default_button_images[] = 
	{
		new VFrame(generic_up_png), 
		new VFrame(generic_hi_png), 
		new VFrame(generic_dn_png)
	};
	BC_WindowBase::get_resources()->generic_button_images = default_button_images;

#include "blond/ok_png.h"
	build_button(BC_WindowBase::get_resources()->ok_images,
		ok_png, 
		default_button_images[0],
		default_button_images[1],
		default_button_images[2]);

#include "blond/cancel_png.h"
	build_button(BC_WindowBase::get_resources()->cancel_images,
		cancel_png, 
		default_button_images[0],
		default_button_images[1],
		default_button_images[2]);

#include "blond/listbox_button_up_png.h"
#include "blond/listbox_button_hi_png.h"
#include "blond/listbox_button_dn_png.h"
	static VFrame* default_listbox_data[] =
	{
		new VFrame(listbox_button_up_png),
		new VFrame(listbox_button_hi_png),
		new VFrame(listbox_button_dn_png)
	};
	BC_WindowBase::get_resources()->listbox_button = default_listbox_data;

	bar_left = new VFrame(bar_left_png);
	bar_mid = new VFrame(bar_mid_png);
	bar_right = new VFrame(bar_right_png);
	heroine_bg = new VFrame(heroine_bg_png);
	play = blond_play;
	frame_fwd = blond_frame_fwd;
	frame_bck = blond_frame_bck;
	pause = blond_pause;
}

void GoldTheme::update_positions_from_canvas(MWindow *mwindow, MWindowGUI *gui)
{
	mwindow->mwindow_w = canvas_w;
	mwindow->mwindow_h = canvas_h + gui->menu->get_h() + bar_left->get_h();
	update_positions(mwindow, gui);
}


int GoldTheme::update_positions(MWindow *mwindow, 
	MWindowGUI *gui)
{
	int x = 0, y = gui->menu->get_h();

// Get canvas dimensions
	canvas_x = x;
	canvas_y = y;
	canvas_w = mwindow->mwindow_w;
	canvas_h = mwindow->mwindow_h - bar_left->get_h() - canvas_y;

	x += 5;
	y += canvas_h;
	time_x = x;
	time_y = y + 3;
	time_w = 80;
	x += time_w + 5;
	play_x = x;
	play_y = y + 3;
	x += play[0]->get_w();
	frameback_x = x;
	frameback_y = y + 3;
	x += frame_bck[0]->get_w();
	framefwd_x = x;
	framefwd_y = y + 3;
	x += frame_fwd[0]->get_w() + bar_mid->get_w();
	scroll_x = x;
	scroll_y = y + 7;
	scroll_w = canvas_w - scroll_x - 5;
	return 0;
}

void GoldTheme::draw_canvas_bg(BC_WindowBase *canvas)
{
	canvas->set_color(BLACK);
	canvas->draw_box(0, 0, canvas->get_w(), canvas->get_h());

// 	canvas->draw_vframe(heroine_bg, 
// 		canvas->get_w() / 2 - heroine_bg->get_w() / 2,
// 		canvas->get_h() / 2 - heroine_bg->get_h() / 2);
	canvas->flash();
}

int GoldTheme::draw_mwindow_bg(MWindow *mwindow, MWindowGUI *gui)
{
	int x;

	gui->clear_box(canvas_x, canvas_y, canvas_w, canvas_h);
	scroll_x -= bar_mid->get_w();
	for(x = 0; x < scroll_x; )
	{
		int dest_w = (x + bar_left->get_w() - 1 < scroll_x) ? 
			(bar_left->get_w() - 1) : 
			(scroll_x - x);

		gui->draw_vframe(bar_left, 
			x, 
			mwindow->mwindow_h - bar_left->get_h(), 
			dest_w);

		x += dest_w;
	}

	gui->draw_vframe(bar_mid, 
			x, 
			mwindow->mwindow_h - bar_mid->get_h());
	x += bar_mid->get_w() - 1;

	while(x < mwindow->mwindow_w)
	{
		int dest_w = (x + bar_right->get_w() - 1 < mwindow->mwindow_w) ? 
			(bar_right->get_w() - 1) : 
			(mwindow->mwindow_w - x);

		gui->draw_vframe(bar_right, 
			x, 
			mwindow->mwindow_h - bar_right->get_h(), 
			dest_w);
		x += dest_w;
	}

	scroll_x += bar_mid->get_w();
	gui->flash(0, 0, mwindow->mwindow_w, mwindow->mwindow_h);
	return 0;
}



