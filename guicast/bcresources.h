#ifndef BCRESOURCES_H
#define BCRESOURCES_H

#include "bcdisplayinfo.inc"
#include "bcfilebox.h"
#include "bcresources.inc"
#include "bcsignals.inc"
#include "bcwindowbase.inc"
#include "vframe.inc"

#include <X11/Xlib.h>

typedef struct
{
	char *suffix;
	int icon_type;
} suffix_to_type_t;

class BC_Resources
{
public:
	BC_Resources(); // The window parameter is used to get the display information initially
	~BC_Resources();

	int initialize_display(BC_WindowBase *window);

	int get_bg_color();          // window backgrounds
	int get_bg_shadow1();        // border for windows
	int get_bg_shadow2();
	int get_bg_light1();
	int get_bg_light2();
// Get window border size created by window manager
	int get_top_border();
	int get_left_border();
	int get_right_border();
	int get_bottom_border();

// Pointer to signal handler class to run after ipc
	static BC_Signals *signal_handler;

// These values should be changed before the first window is created.
// colors
	int bg_color;          // window backgrounds
	int bg_shadow1;        // border for windows
	int bg_shadow2;
	int bg_light1;
	int bg_light2;
	int default_text_color;
	int disabled_text_color;


// beveled box colors
	int button_light;      
	int button_highlighted;
	int button_down;       
	int button_up;         
	int button_shadow;     

// highlighting
	int highlight_inverse;

// 3D box colors for menus
	int menu_light;
	int menu_highlighted;
	int menu_down;
	int menu_up;
	int menu_shadow;
// If these are nonzero, they override the menu backgrounds.
	VFrame *menu_popup_bg;
	VFrame **menu_title_bg;

// Menu bar text color
	int menu_title_text;
// color of popup title
	int popup_title_text;
// color for item text
	int menu_item_text;
// Override the menu item background if nonzero.
	VFrame **menu_item_bg;


// color for progress text
	int progress_text;


// ms for double click
	long double_click;
// ms for cursor flash
	int blink_rate;
// ms for scroll repeats
	int scroll_repeat;
// ms before tooltip
	int tooltip_delay;
	int tooltip_bg_color;
	int tooltips_enabled;

// default color of text
	int text_default;      
// background color of textboxes and list boxes
	int text_border1;
	int text_border2;
	int text_border2_hi;
	int text_background;   
	int text_background_hi;
	int text_background_noborder_hi;
	int text_border3;
	int text_border3_hi;
	int text_border4;
	int text_highlight;
// Not used
	int text_background_noborder;

// Optional background for highlighted text in toggle
	VFrame *toggle_highlight_bg;
	int toggle_text_margin;

// Background images
	static VFrame *bg_image;
	static VFrame *menu_bg;

// Buttons
	VFrame **ok_images;
	VFrame **cancel_images;
	VFrame **filebox_text_images;
	VFrame **filebox_icons_images;
	VFrame **filebox_updir_images;
	VFrame **filebox_newfolder_images;

// Generic button images
	VFrame **generic_button_images;
// Generic button text margin
	int generic_button_margin;
	VFrame **usethis_button_images;

// Toggles
	VFrame **checkbox_images;
	VFrame **radial_images;
	VFrame **label_images;

	VFrame **tumble_data;
	int tumble_duration;

// Listbox
	VFrame *listbox_bg;
	VFrame **listbox_button;
	VFrame **listbox_expand;
	VFrame **listbox_column;
	VFrame *listbox_up;
	VFrame *listbox_dn;

// Sliders
	VFrame **horizontal_slider_data;
	VFrame **vertical_slider_data;
	VFrame **hscroll_data;

	VFrame **vscroll_data;
// Minimum pixels in handle
	int scroll_minhandle;

// Pans
	VFrame **pan_data;
	int pan_text_color;

// Pots
	VFrame **pot_images;
	int pot_x1, pot_y1, pot_r;
// Amoun of deflection of pot when down
	int pot_offset;
	int pot_needle_color;

// Meters
	VFrame **xmeter_images, **ymeter_images;
	int meter_font;
	int meter_font_color;
	int meter_title_w;
	int meter_3d;

// Progress bar
	VFrame **progress_images;

// Motion required to start a drag
	int drag_radius;

// Filebox
	static suffix_to_type_t suffix_to_type[TOTAL_SUFFIXES];
	static VFrame *type_to_icon[TOTAL_ICONS];
// Display mode for fileboxes
	int filebox_mode;
// Filter currently used in filebox
	char filebox_filter[BCTEXTLEN];
	int filebox_w;
	int filebox_h;
	int filebox_columntype[FILEBOX_COLUMNS];
	int filebox_columnwidth[FILEBOX_COLUMNS];
	int filebox_sortcolumn;
	int filebox_sortorder;


// fonts
	static char *large_font;
	static char *medium_font;
	static char *small_font;
// Backup of fonts in case the first choices don't exist
	static char *large_font2;
	static char *medium_font2;
	static char *small_font2;

	static char *large_fontset;
	static char *medium_fontset;
	static char *small_fontset;

	static char *large_font_xft;
	static char *medium_font_xft;
	static char *small_font_xft;

	VFrame **medium_7segment;


	int use_fontset;
// This must be constitutive since applications access the private members here.
	int use_xft;



// Available display extensions
	int use_shm;
	static int error;
// If the program uses recursive_resizing
	int recursive_resizing;
// Work around X server bugs
	int use_xvideo;

private:
// Test for availability of shared memory pixmaps
	int init_shm(BC_WindowBase *window);
	void init_sizes(BC_WindowBase *window);
	static int x_error_handler(Display *display, XErrorEvent *event);
	BC_DisplayInfo *display_info;
 	VFrame **list_pointers[100];
 	int list_lengths[100];
 	int list_total;
};


#endif
