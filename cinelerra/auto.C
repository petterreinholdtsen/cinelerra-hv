#include "auto.h"
#include "autos.h"
#include "filexml.h"

Auto::Auto()
 : ListItem<Auto>()
{
	this->edl = 0;
	this->autos = 0;
	position = 0;
	skip = 0;
	WIDTH = 10;
	HEIGHT = 10;
}

Auto::Auto(EDL *edl, Autos *autos)
 : ListItem<Auto>()
{
	this->edl = edl;
	this->autos = autos;
	position = 0;
	skip = 0;
	WIDTH = 10;
	HEIGHT = 10;
}

Auto& Auto::operator=(Auto& that)
{
	copy_from(&that);
	return *this;
}

int Auto::operator==(Auto &that)
{
	return 0;
}

void Auto::copy(long start, long end, FileXML *file)
{
}

void Auto::copy_from(Auto *that)
{
	this->position = that->position;
}


void Auto::load(FileXML *xml)
{
//	printf("Auto::load\n");
}

int Auto::draw(BC_SubWindow *canvas, 
			int x, 
			int y, 
			int center_pixel, 
			int zoom_track, 
			int vertical, 
			int show_value)
{
// 	static int x1, y1, x2, y2;
// 	static char string[5];
// 	static int j;
// 		
// 	if(skip) return 1;
// 	x1 = x - WIDTH/2; x2 = x + WIDTH/2;
// 	y1 = y - HEIGHT/2; y2 = y + HEIGHT/2;
// 	
// 	if(y1 < -zoom_track / 2) y1 = -zoom_track / 2;
// 	if(y2 > zoom_track / 2) y2 = zoom_track / 2;
// 
// 	if(vertical)
// 	canvas->draw_box(center_pixel - y2, x1, y2 - y1, x2 - x1);
// 	else
// 	canvas->draw_box(x1, center_pixel + y1, x2 - x1, y2 - y1);
// 
// 	if(show_value)
// 	{
// 		value_to_str(string, value);
// 		//canvas->set_font(SMALLFONT);
// 		
// 		if(y > 0)
// 		{
// 			if(vertical)
// 			canvas->draw_text(center_pixel - y, x, string);
// 			else
// 			canvas->draw_text(x + 5, center_pixel + y - 5, string);
// 		}
// 		else
// 		{
// 			if(vertical)
// 			canvas->draw_text(center_pixel - y - 50, x, string);
// 			else
// 			canvas->draw_text(x + 5, center_pixel + y + 20, string);
// 		}
// 		//canvas->set_font(MEDIUMFONT);
// 	}
}

int Auto::selected(int ax, int ay, int cursor_x, int cursor_y, int center_pixel, int zoom_track)
{
	static int x1, y1, x2, y2;
	
	x1 = ax - WIDTH/2; x2 = ax + WIDTH/2;
	y1 = ay - HEIGHT/2; y2 = ay + HEIGHT/2;
	
	if(y1 <  -zoom_track) y1 = -zoom_track;
	if(y2 > zoom_track) y2 = zoom_track;
	
//printf("ay %d cursor_y %d\n", ay, cursor_y);
	if(cursor_x > x1 && cursor_x < x2 && cursor_y > y1 && cursor_y < y2)
	{
		return 1;
	}
	return 0;
}


float Auto::value_to_percentage()
{
	return 0;
}

