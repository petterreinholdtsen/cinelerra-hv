#include "assetedit.h"
#include "assetmanager.h"
#include "assets.h"
#include "cache.h"
#include "errorbox.h"
#include "filesystem.h"
#include "indexfile.h"
#include "keys.h"
#include "mwindow.h"
#include "bcprogressbox.h"
#include "question.h"
#include "theme.h"


AssetManager::AssetManager(MWindow *mwindow)
 : BC_MenuItem("Assets...")
{
	this->mwindow = mwindow;
	thread = new AssetManagerThread(mwindow);
	thread->create_objects();
}

AssetManager::~AssetManager()
{
	delete thread;
}

int AssetManager::interrupt_indexes()
{
	thread->interrupt_indexes();
}

int AssetManager::handle_event()
{
	thread->start();
}




AssetManagerThread::AssetManagerThread(MWindow *mwindow)
 : Thread()
{
	this->mwindow = mwindow;
	window = 0;
}

AssetManagerThread::~AssetManagerThread()
{
	delete index_thread;
}

int AssetManagerThread::create_objects()
{
	index_thread = new AssetIndexThread(this);
}

int AssetManagerThread::interrupt_indexes()
{
	index_thread->interrupt_indexes();
	if(window) window->set_done(0);
}

void AssetManagerThread::run()
{
	generate_list();

	window = new AssetManagerWindow(this);
	window->create_objects();
	window->run_window();
	delete window;
	window = 0;

// delete list
	int i;
	for(i = 0; i < assets.total; i++) delete assets.values[i];
	assets.remove_all();
}

Asset* AssetManagerThread::get_selection()
{
	int current_selection = asset_list->get_selection_number(0, 0);

	if(current_selection > -1)
		return assets.values[current_selection]->asset;
	else
		return 0;
}

int AssetManagerThread::generate_list()
{
// delete old list
	int i;
	for(i = 0; i < assets.total; i++) delete assets.values[i];
	assets.remove_all();

// create new list
	Asset *asset = mwindow->assets->first;
	
	char string[1024];
	FileSystem fs;

	for(int i = 0; asset && i < 1000; i++, asset = asset->next)
	{
//		if(!asset->silence)
		{
			fs.extract_name(string, asset->path);
			assets.append(new AssetManagerItem(string, asset));
		}
	}
	return 0;
}

int AssetManagerThread::update_list()
{
	generate_list();
	asset_list->update((ArrayList<BC_ListBoxItem*> *)&assets,
						0,
						0,
						1);
}







AssetManagerItem::AssetManagerItem(char *string, Asset *asset)
 : BC_ListBoxItem(string, BLACK)
{
	this->asset = asset;
}


AssetManagerWindow::AssetManagerWindow(AssetManagerThread *thread)
 : BC_Window(PROGRAM_NAME ": Assets", (int)INFINITY, (int)INFINITY, 490, 340)
{
	this->thread = thread;
	this->mwindow = thread->mwindow;
	this->assets = mwindow->assets;
}

AssetManagerWindow::~AssetManagerWindow()
{
}

int AssetManagerWindow::create_objects()
{
	int y = 10, x = 10;

	add_subwindow(thread->asset_list = new AssetManagerList(thread, x, y));

	x = 320;
	add_subwindow(new AssetManagerDeleteDisk(mwindow, thread, x, y));
	y += 30;
	add_subwindow(new AssetManagerDeleteProject(mwindow, thread, x, y));
	y += 30;
	add_subwindow(new AssetManagerImport(mwindow, thread, x, y));
	y += 30;
	add_subwindow(thread->edit_button = new AssetManagerEdit(mwindow, thread, x, y));
	y += 30;
	add_subwindow(new AssetManagerIndex(mwindow, thread, x, y));
	return 0;
}



AssetIndexThread::AssetIndexThread(AssetManagerThread *thread)
 : Thread()
{
	this->thread = thread;
	this->asset = 0;
	index = new IndexFile(thread->mwindow);
}

AssetIndexThread::~AssetIndexThread()
{
	delete index;
}

int AssetIndexThread::build_index(Asset *asset)
{
	this->asset = asset;
	completion_lock.lock();
	start();
}


int AssetIndexThread::interrupt_indexes()
{
	index->interrupt_index();
	completion_lock.lock();
	completion_lock.unlock();
}

void AssetIndexThread::run()
{
	if(asset)
	{
// 		thread->window->disable_window();
// 		BC_ProgressBox progress(INFINITY, INFINITY, "Rebuilding Index...", 1);
// 		progress.start();
// 		index->create_index(thread->mwindow, asset, &progress);
// 		progress.stop_progress();
// 		thread->window->enable_window();
	}
	completion_lock.unlock();
}




AssetManagerEdit::AssetManagerEdit(MWindow *mwindow, AssetManagerThread *thread, int x, int y)
 : BC_Button(x, y, mwindow->theme->asset_info_data)
{
	this->thread = thread;
	edit_thread = new AssetEdit(thread);
}

AssetManagerEdit::~AssetManagerEdit()
{
	delete edit_thread;
}

int AssetManagerEdit::handle_event()
{
	edit_thread->set_asset(thread->get_selection());
	edit_thread->start();
}

int AssetManagerEdit::keypress_event()
{
	if(get_keypress() == 13) { handle_event(); return 1; }
	return 0;
}






AssetManagerImport::AssetManagerImport(MWindow *mwindow, AssetManagerThread *thread, int x, int y)
 : BC_Button(x, y, mwindow->theme->asset_append_data)
{
	this->thread = thread;
}

AssetManagerImport::~AssetManagerImport()
{
}

int AssetManagerImport::handle_event()
{
	Asset *asset = thread->get_selection();
//	if(asset) thread->mwindow->load(asset->path, 1);
}

int AssetManagerImport::keypress_event()
{
	if(get_keypress() == 'i') { handle_event(); return 1; }
	return 0;
}






AssetManagerIndex::AssetManagerIndex(MWindow *mwindow, AssetManagerThread *thread, int x, int y)
 : BC_Button(x, y, mwindow->theme->asset_index_data)
{
	this->thread = thread;
}

AssetManagerIndex::~AssetManagerIndex()
{
}

int AssetManagerIndex::handle_event()
{
	thread->index_thread->build_index(thread->get_selection());
}





AssetManagerDeleteDisk::AssetManagerDeleteDisk(MWindow *mwindow, AssetManagerThread *thread, int x, int y)
 : BC_Button(x, y, mwindow->theme->asset_disk_data)
{
	this->thread = thread;
	delete_thread = new AssetDeleteDisk(mwindow, thread);
}

AssetManagerDeleteDisk::~AssetManagerDeleteDisk()
{
	delete delete_thread;
}

int AssetManagerDeleteDisk::handle_event()
{
	delete_thread->set_asset(thread->get_selection());
	delete_thread->start();
}




AssetManagerDeleteProject::AssetManagerDeleteProject(MWindow *mwindow, AssetManagerThread *thread, int x, int y)
 : BC_Button(x, y, mwindow->theme->asset_project_data)
{
	this->thread = thread;
	delete_thread = new AssetDeleteProject(mwindow, thread);
}

AssetManagerDeleteProject::~AssetManagerDeleteProject()
{
	delete delete_thread;
}

int AssetManagerDeleteProject::handle_event()
{
	delete_thread->set_asset(thread->get_selection());
	delete_thread->start();
}












AssetManagerList::AssetManagerList(AssetManagerThread *thread, int x, int y)
 : BC_ListBox(x, 
 		y, 
		280, 
		300, 
		LISTBOX_TEXT,
		(ArrayList<BC_ListBoxItem*> *)&(thread->assets))
{
	this->thread = thread;
	selection = 0;
}

AssetManagerList::~AssetManagerList()
{
}

int AssetManagerList::handle_event()
{
	if(get_keypress() == ESC) 
		set_done(0);
	else
	{
		thread->edit_button->handle_event();
		//printf("%s\n", get_selection());
	}
}

int AssetManagerList::selection_changed()
{
	if(get_selection_number(0, 0) != -1) selection = get_selection_number(0, 0);
}


AssetDeleteDisk::AssetDeleteDisk(MWindow *mwindow, AssetManagerThread *thread)
 : Thread()
{
	this->mwindow = mwindow;
	this->thread = thread;
	this->asset = 0;
}

AssetDeleteDisk::~AssetDeleteDisk()
{
}

	
int AssetDeleteDisk::set_asset(Asset *asset)
{
	this->asset = asset;
}

void AssetDeleteDisk::run()
{
}





AssetDeleteProject::AssetDeleteProject(MWindow *mwindow, AssetManagerThread *thread)
 : Thread()
{
	this->mwindow = mwindow;
	this->thread = thread;
	this->asset = 0;
}

AssetDeleteProject::~AssetDeleteProject()
{
}
	
int AssetDeleteProject::set_asset(Asset *asset)
{
	this->asset = asset;
}

void AssetDeleteProject::run()
{
}
