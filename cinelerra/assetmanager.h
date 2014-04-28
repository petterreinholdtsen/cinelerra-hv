#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

class AssetManagerIndex;
class AssetManagerList;
class AssetManagerEdit;
class AssetManagerWindow;
class AssetDeleteDisk;
class AssetDeleteProject;
class AssetIndexThread;

#include "arraylist.h"
#include "assetedit.inc"
#include "assetmanager.inc"
#include "assets.inc"
#include "guicast.h"
#include "indexfile.inc"
#include "mwindow.inc"

class AssetManager : public BC_MenuItem
{
public:
	AssetManager(MWindow *mwindow);
	~AssetManager();
	
	int handle_event();
	int interrupt_indexes();

	MWindow *mwindow;
	AssetManagerThread *thread;
};



class AssetManagerItem : public BC_ListBoxItem
{
public:
	AssetManagerItem(char *string, Asset *asset);

	Asset *asset;
};


class AssetManagerThread : public Thread
{
public:
	AssetManagerThread(MWindow *mwindow);
	~AssetManagerThread();

	friend class AssetManagerIndex;

	int create_objects();
	void run();
	Asset* get_selection();
	int update_list();
	int interrupt_indexes();

	AssetManagerList *asset_list;
	AssetManagerEdit *edit_button;

	ArrayList<AssetManagerItem*> assets;
	MWindow *mwindow;
	AssetManagerWindow *window;

private:
	int generate_list();
	AssetIndexThread *index_thread;
};



class AssetManagerWindow : public BC_Window
{
public:
	AssetManagerWindow(AssetManagerThread *thread);
	~AssetManagerWindow();

	int create_objects();

	AssetManagerThread *thread;
	MWindow *mwindow;
	Assets *assets;
};


class AssetManagerEdit : public BC_Button
{
public:
	AssetManagerEdit(MWindow *mwindow, AssetManagerThread *thread, int x, int y);
	~AssetManagerEdit();

	int handle_event();
	int keypress_event();

	AssetEdit *edit_thread;
	AssetManagerThread *thread;
};

class AssetManagerImport : public BC_Button
{
public:
	AssetManagerImport(MWindow *mwindow, AssetManagerThread *thread, int x, int y);
	~AssetManagerImport();

	int handle_event();
	int keypress_event();

	AssetManagerThread *thread;
};

class AssetManagerPaste : public BC_Button
{
public:
	AssetManagerPaste(AssetManagerThread *thread, int x, int y);
	~AssetManagerPaste();

	int handle_event();
	int keypress_event();

	AssetManagerThread *thread;
};

class AssetIndexThread : public Thread
{
public:
	AssetIndexThread(AssetManagerThread *thread);
	~AssetIndexThread();

	int build_index(Asset *asset);
	int interrupt_indexes();

	void run();
	AssetManagerThread *thread;
	Asset *asset;
	IndexFile *index;
	Mutex completion_lock;
};

class AssetManagerIndex : public BC_Button
{
public:
	AssetManagerIndex(MWindow *mwindow, AssetManagerThread *thread, int x, int y);
	~AssetManagerIndex();

	int handle_event();

	AssetManagerThread *thread;
};




class AssetManagerDeleteDisk : public BC_Button
{
public:
	AssetManagerDeleteDisk(MWindow *mwindow, AssetManagerThread *thread, int x, int y);
	~AssetManagerDeleteDisk();

	int handle_event();

	AssetManagerThread *thread;
	AssetDeleteDisk *delete_thread;
};

class AssetManagerDeleteProject : public BC_Button
{
public:
	AssetManagerDeleteProject(MWindow *mwindow, AssetManagerThread *thread, int x, int y);
	~AssetManagerDeleteProject();

	int handle_event();

	AssetManagerThread *thread;
	AssetDeleteProject *delete_thread;
};

class AssetManagerDone : public BC_Button
{
public:
	AssetManagerDone(AssetManagerThread *thread, int x, int y);
	~AssetManagerDone();

	int handle_event();
	int keypress_event();

	AssetManagerThread *thread;
};


class AssetManagerList : public BC_ListBox
{
public:
	AssetManagerList(AssetManagerThread *thread, int x, int y);
	~AssetManagerList();
	
	int handle_event();
	int selection_changed();
	
	AssetManagerThread *thread;
	int selection;
};








class AssetDeleteDisk : public Thread
{
public:
	AssetDeleteDisk(MWindow *mwindow, AssetManagerThread *thread);
	~AssetDeleteDisk();

	int set_asset(Asset *asset);
	void run();

	MWindow *mwindow;
	Asset *asset;
	AssetManagerThread *thread;
};

class AssetDeleteProject : public Thread
{
public:
	AssetDeleteProject(MWindow *mwindow, AssetManagerThread *thread);
	~AssetDeleteProject();

	int set_asset(Asset *asset);
	void run();

	MWindow *mwindow;
	Asset *asset;
	AssetManagerThread *thread;
};


#endif
