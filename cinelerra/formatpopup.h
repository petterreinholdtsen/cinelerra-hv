#ifndef FORMATPOPUP_H
#define FORMATPOPUP_H



#include "guicast.h"
#include "formatpopup.inc"
#include "pluginserver.inc"

class FormatPopup : public BC_ListBox
{
public:
	FormatPopup(ArrayList<PluginServer*> *plugindb, 
		int x, 
		int y);
// set wr to 1 for writable file formats
	FormatPopup(ArrayList<PluginServer*> *plugindb, 
		int x, 
		int y, 
		char *text, 
		int wr = 1);
	~FormatPopup();

	int create_objects();
	virtual int handle_event();  // user copies text to value here
	ArrayList<PluginServer*> *plugindb;
	ArrayList<BC_ListBoxItem*> format_items;
};






#endif
