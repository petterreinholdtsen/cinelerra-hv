#include "assets.h"
#include "cache.h"
#include "datatype.h"
#include "edl.h"
#include "edlsession.h"
#include "file.h"
#include "filesystem.h"
#include "preferences.h"

#include <string.h>

// edl came from a command which won't exist anymore
CICache::CICache(EDL *edl, ArrayList<PluginServer*> *plugindb)
 : List<CICacheItem>()
{
//printf("CICache 1\n");
	this->edl = new EDL;
	this->edl->create_objects();
//printf("CICache 1 %p\n", this->edl);
	*this->edl = *edl;
//printf("CICache 1\n");
	this->plugindb = plugindb;
//printf("CICache 2\n");
}

CICache::~CICache()
{
//printf("CICache::~CICache 1\n");
	while(last) delete last;
//printf("CICache::~CICache 1\n");
	delete edl;
//printf("CICache::~CICache 1\n");
}

void CICache::set_edl(EDL *edl)
{
	*this->edl = *edl;
}

void CICache::update(File* &file)
{
// Check if exists
	for(CICacheItem *current = first; current; current = NEXT)
	{
		if(!current->asset->test_path(file->asset->path))
		{
			if(file != current->file)
			{
				delete file;
				file = current->file;
			}
			return;
		}
	}

	CICacheItem *item;
	append(item = new CICacheItem(this, file));
	item->asset = new Asset(*(file->asset));
	file->set_asset(item->asset);
}

File* CICache::check_out(Asset *asset)
{
	File *result = 0;

//printf("CICache::check_out 1\n");fflush(stdout);
	check_out_lock.lock();
//printf("CICache::check_out 2\n");fflush(stdout);

// search for it in the cache
	CICacheItem *current, *new_item = 0;

	for(current = first; current && !new_item; current = NEXT)
	{
		if(!strcmp(current->asset->path, asset->path))
		{
			current->counter = 0;
			new_item = current;
		}
	}

// didn't find it so create a new one
	if(!new_item)
	{
//printf("CICache::check_out 3 %p\n", asset);fflush(stdout);
//printf("CICache::check_out 3 %s\n", asset->path);
		new_item = append(new CICacheItem(this, asset));
//printf("CICache::check_out 3.1 %s\n", asset->path);
	}

//printf("CICache::check_out 3.2 %s\n", asset->path);
	if(new_item)
	{
//printf("CICache::check_out 3.3 %s\n", asset->path);
		if(new_item->file)
		{
// opened successfully
//printf("CICache::check_out 4 %p %p %s\n", new_item, new_item->asset, new_item->asset->path);fflush(stdout);
			new_item->item_lock.lock();
//printf("CICache::check_out 5\n");fflush(stdout);
			new_item->checked_out = 1;

			result = new_item->file;
		}
		else
		{
// failed
//printf("CICache::check_out 6 %p\n", asset);fflush(stdout);
			delete new_item;
//printf("CICache::check_out 7 %p\n", asset);fflush(stdout);
			new_item = 0;
		}
	}
//printf("CICache::check_out 8\n");fflush(stdout);

	check_out_lock.unlock();
//printf("CICache::check_out 9\n");fflush(stdout);

	return result;
}

int CICache::check_in(Asset *asset)
{
	check_in_lock.lock();

	CICacheItem *current;
	int result = 0;
	total_lock.lock();
	for(current = first; current && !result; current = NEXT)
	{
//printf("CICache::check_in %s %s\n", current->asset->path, asset->path);
// Pointers are different
		if(!strcmp(current->asset->path, asset->path))
		{
			current->checked_out = 0;
			current->item_lock.unlock();
			result = 1;
		}
	}
	total_lock.unlock();

	check_in_lock.unlock();
	return result;
}

int CICache::delete_entry(char *path)
{
	Asset *asset = edl->assets->get_asset(path);
	if(asset) delete_entry(asset);
	return 0;
}

int CICache::delete_entry(Asset *asset)
{
	lock_all();
	int result = 0;
	CICacheItem *current, *temp;

	for(current = first; current; current = temp)
	{
		temp = NEXT;
		if(current->asset == asset && !current->checked_out) delete current;
		current = temp;
	}
	unlock_all();
	return 0;
}

int CICache::age_audio()
{
	age_type(TRACK_AUDIO);
}


int CICache::age_video()
{
	age_type(TRACK_VIDEO);
}

int CICache::age_type(int data_type)
{
	check_out_lock.lock();
	CICacheItem *current;

	for(current = first; current; current = NEXT)
	{
		switch(data_type)
		{
			case TRACK_AUDIO:
				if(current->asset->audio_data)
				{
					current->counter++;
				}
				break;
			
			case TRACK_VIDEO:	
				if(current->asset->video_data)
				{
					current->counter++;
				}
				break;
		}
	}

// delete old assets if memory usage is exceeded
	long memory_usage;
	int result = 0;
	do
	{
		memory_usage = get_memory_usage();
		
		if(memory_usage > edl->session->cache_size)
		{
			result = delete_oldest();
		}
	}while(memory_usage > edl->session->cache_size && !result);

	check_out_lock.unlock();
}

long CICache::get_memory_usage()
{
	CICacheItem *current;
	long result = 0;
	
	for(current = first; current; current = NEXT)
	{
		result++;
	}
	
	return result;
}

int CICache::delete_oldest()
{
	CICacheItem *current;
	int highest_counter = 1;
	CICacheItem *oldest = 0;

	for(current = last; current; current =  PREVIOUS)
	{
		if(current->counter >= highest_counter)
		{
			oldest = current;
			highest_counter = current->counter;
		}
	}

	if(highest_counter > 1 && oldest && !oldest->checked_out)
	{
		total_lock.lock();
		delete oldest;
		total_lock.unlock();
		return 0;    // success
	}
	else
	{
		return 1;    // nothing was old enough to delete
	}
}

int CICache::dump()
{
	lock_all();
	CICacheItem *current;

	for(current = first; current; current = NEXT)
	{
		printf("cache item %x\n", current);
		printf("	asset %x\n", current->asset);
		printf("	%s\n", current->asset->path);
		printf("	counter %ld\n", current->counter);
	}
	
	printf("total size %ld\n", get_memory_usage());
	unlock_all();
}

int CICache::lock_all()
{
	check_in_lock.lock();
	check_out_lock.lock();
}

int CICache::unlock_all()
{
	check_in_lock.unlock();
	check_out_lock.unlock();
}











// File not already opened.
CICacheItem::CICacheItem(CICache *cache, Asset *asset)
 : ListItem<CICacheItem>()
{
	int result = 0;
	counter = 0;
	this->asset = new Asset;
	
// Must copy Asset since this belongs to an EDL which won't exist forever.
	*this->asset = *asset;
	this->cache = cache;
	checked_out = 0;

//printf("CICacheItem::CICacheItem 1\n");
	file = new File;
//printf("CICacheItem::CICacheItem 2\n");
	file->set_processors(cache->edl->session->smp ? 2: 1);
//printf("CICacheItem::CICacheItem 3\n");
	file->set_preload(cache->edl->session->playback_preload);
//printf("CICacheItem::CICacheItem 4\n");
	if(result = file->open_file(cache->plugindb, this->asset, 1, 0, -1, -1))
	{
//printf("CICacheItem::CICacheItem 4.1\n");
		delete file;
//printf("CICacheItem::CICacheItem 4.2\n");
		file = 0;
	}
//printf("CICacheItem::CICacheItem 5\n");
}

// File already opened
CICacheItem::CICacheItem(CICache *cache, File *file)
 : ListItem<CICacheItem>()
{
	counter = 0;
	this->asset = new Asset;
	*this->asset = *file->asset;
	this->file = file;
	this->cache = cache;
	checked_out = 0;

	file->set_processors(cache->edl->session->smp ? 2: 1);
	file->set_preload(cache->edl->session->playback_preload);
}

CICacheItem::~CICacheItem()
{
//printf("CICacheItem::~CICacheItem\n");
	delete file;
	delete asset;
}
