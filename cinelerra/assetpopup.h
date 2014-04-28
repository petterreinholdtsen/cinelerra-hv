#ifndef ASSETPOPUP_H
#define ASSETPOPUP_H

class AssetPopupInfo;
class AssetPopupBuildIndex;
class AssetPopupView;

#include "assetedit.inc"
#include "awindowgui.inc"
#include "awindowmenu.inc"
#include "edl.inc"
#include "guicast.h"
#include "mwindow.inc"
#include "assets.inc"



class AssetPopup : public BC_PopupMenu
{
public:
	AssetPopup(MWindow *mwindow, AWindowGUI *gui);
	~AssetPopup();

	void create_objects();
// Set mainsession with the current selections
	int update();
	void paste_assets();
	void match_size();

	MWindow *mwindow;
	AWindowGUI *gui;


	AssetPopupInfo *info;
	AssetPopupBuildIndex *index;
	AssetPopupView *view;
	AssetListFormat *format;
};

class AssetPopupInfo : public BC_MenuItem
{
public:
	AssetPopupInfo(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupInfo();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};

class AssetPopupBuildIndex : public BC_MenuItem
{
public:
	AssetPopupBuildIndex(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupBuildIndex();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};


class AssetPopupView : public BC_MenuItem
{
public:
	AssetPopupView(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupView();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};

class AssetPopupPaste : public BC_MenuItem
{
public:
	AssetPopupPaste(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupPaste();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};

class AssetMatchSize : public BC_MenuItem
{
public:
	AssetMatchSize(MWindow *mwindow, AssetPopup *popup);
	~AssetMatchSize();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};

class AssetPopupProjectRemove : public BC_MenuItem
{
public:
	AssetPopupProjectRemove(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupProjectRemove();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};

class AssetPopupDiskRemove : public BC_MenuItem
{
public:
	AssetPopupDiskRemove(MWindow *mwindow, AssetPopup *popup);
	~AssetPopupDiskRemove();

	int handle_event();

	MWindow *mwindow;
	AssetPopup *popup;
};


#endif
