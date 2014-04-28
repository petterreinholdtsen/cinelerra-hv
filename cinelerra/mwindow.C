#include "assetmanager.h"
#include "assets.h"
#include "awindowgui.h"
#include "awindow.h"
#include "cache.h"
#include "channel.h"
#include "colormodels.h"
#include "console.h"
#include "cplayback.h"
#include "ctimebar.h"
#include "cwindowgui.h"
#include "cwindow.h"
#include "defaults.h"
#include "editpanel.h"
#include "edl.h"
#include "edlsession.h"
#include "errorbox.h"
#include "fileformat.h"
#include "file.h"
#include "filesystem.h"
#include "filexml.h"
#include "indexfile.h"
#include "levelwindowgui.h"
#include "levelwindow.h"
#include "loadfile.inc"
#include "localsession.h"
#include "mainindexes.h"
#include "mainmenu.h"
#include "mainprogress.h"
#include "mainsession.h"
#include "mainundo.h"
#include "mbuttons.h"
#include "module.h"
#include "modules.h"
#include "mwindowgui.h"
#include "mwindow.h"
#include "new.h"
#include "patchbay.h"
#include "playbackengine.h"
#include "plugin.h"
#include "pluginserver.h"
#include "pluginset.h"
#include "preferences.h"
#include "record.h"
#include "recordlabel.h"
#include "render.h"
#include "samplescroll.h"
#include "statusbar.h"
#include "theme.h"
#include "threadloader.h"
#include "timebar.h"
#include "trackcanvas.h"
#include "track.h"
#include "tracking.h"
#include "trackscroll.h"
#include "tracks.h"
#include "transportque.h"
#include "videodevice.inc"
#include "videowindow.h"
#include "vwindowgui.h"
#include "vwindow.h"
#include "zoombar.h"

#include <string.h>



extern "C"
{




// Hack for libdv to remove glib dependancy

void
g_log (const char    *log_domain,
       int  log_level,
       const char    *format,
       ...)
{
}

void
g_logv (const char    *log_domain,
       int  log_level,
       const char    *format,
       ...)
{
}



// Hack for XFree86 4.1.0

int atexit(void (*function)(void))
{
	return 0;
}



}





MWindow::MWindow()
{
}

MWindow::~MWindow()
{
//printf("MWindow::~MWindow 1\n");
	clean_indexes();
//printf("MWindow::~MWindow 1\n");
	delete mainprogress;
//printf("MWindow::~MWindow 1\n");
	delete tracks;
//	delete console;
//	delete level_window;
//	delete video_window;
//	delete assets;
//printf("MWindow::~MWindow 1\n");
	delete audio_cache;             // delete the cache after the assets
//printf("MWindow::~MWindow 1\n");
	delete video_cache;             // delete the cache after the assets
//printf("MWindow::~MWindow 2\n");
//	delete threadloader;
	if(gui) delete gui;
//printf("MWindow::~MWindow 1\n");
	delete undo;
//printf("MWindow::~MWindow 1\n");
	delete preferences;
//printf("MWindow::~MWindow 1\n");
//	delete_plugins();
	delete defaults;
//printf("MWindow::~MWindow 1\n");
	delete render;
//printf("MWindow::~MWindow 1\n");
	delete renderlist;
//printf("MWindow::~MWindow 1\n");
	delete awindow;
//printf("MWindow::~MWindow 1\n");
	delete vwindow;
//printf("MWindow::~MWindow 1\n");
	delete cwindow;
	delete lwindow;
//printf("MWindow::~MWindow 1\n");
	plugin_guis->remove_all_objects();
//printf("MWindow::~MWindow 2\n");
	delete plugin_guis;
//printf("MWindow::~MWindow 2\n");
}

void MWindow::init_defaults(Defaults* &defaults)
{
// set the .bcast directory
	char directory[BCTEXTLEN];
	FileSystem fs;

	sprintf(directory, "%s", BCASTDIR);
	fs.complete_path(directory);
	if(fs.is_dir(directory)) 
	{
		fs.create_dir(directory); 
	}

// load the defaults
	strcat(directory, "Cinelerra_rc");

	defaults = new Defaults(directory);
	defaults->load();
}

void MWindow::init_plugins(Preferences *preferences, ArrayList<PluginServer*>* &plugindb)
{
	FileSystem fs;
	int result, pass; 
	char path[1024], *directory;
	PluginServer *newplugin;
	plugindb = new ArrayList<PluginServer*>;
//printf("MWindow::init_plugins 1\n");

	fs.set_filter("*.plugin");

	for(pass = 0; pass < 2; pass++)
	{
		result = 1;          // default is abort

		switch(pass)
		{
// on first pass use global dir	
			case 0: 
				directory = preferences->global_plugin_dir;
				break;
// on second pass use local dir
			case 1: 
				directory = preferences->local_plugin_dir;
				break;
		}

		if(!strlen(directory)) 
			result = 1;
		else
			result = fs.update(directory);

//printf("MWindow::init_plugins 1 %s\n", directory);
		if(!result)
		{
//printf("MWindow::init_plugins 1 %d\n", fs.dir_list.total);
			for(int i = 0; i < fs.dir_list.total; i++)
			{
				char path[BCTEXTLEN];
//printf("MWindow::init_plugins 2\n");
				strcpy(path, fs.dir_list.values[i]->path);
//printf("                                                                            \r");
//printf("MWindow::init_plugins 2 %s\r", path);
//fflush(stdout);

// File is a directory
				if(!fs.is_dir(path))
				{
					continue;
				}
				else
				{
// Try to query the plugin
//printf("MWindow::init_plugins 3\n", path);
					fs.complete_path(path);
//printf("MWindow::init_plugins 4\n", path);
					PluginServer *new_plugin = new PluginServer(path);
//printf("MWindow::init_plugins 5\n", path);
					if(!new_plugin->open_plugin(1, 0, 0))
					{
//printf("MWindow::init_plugins 4 %s\n", path);
						plugindb->append(new_plugin);
//printf("MWindow::init_plugins 5 %s\n", path);
						new_plugin->close_plugin();
//printf("MWindow::init_plugins 6 %s\n", path);
					}
					else
// Plugin failed to open
					{
						delete new_plugin;
					}
				}
			}
		}
		else
// notify user of failed directory search
		{
			switch(pass)
			{
// on first pass use global dir	
				case 0: 
					if(strlen(preferences->global_plugin_dir)) 
						printf("MWindow::init_plugins: Couldn't open global plugin directory.\n");  
					break;
// on second pass use local dir
				case 1: 
					if(strlen(preferences->local_plugin_dir)) 
						printf("MWindow::init_plugins: Couldn't open local plugin directory.\n");    
					break;
			}
		}
	}
//printf("MWindow::init_plugins 2 \n");
}

void MWindow::delete_plugins()
{
	for(int i = 0; i < plugindb->total; i++)
	{
		delete plugindb->values[i];
	}
	delete plugindb;
}

void MWindow::create_plugindb(int do_audio, 
		int do_video, 
		int is_realtime, 
		int is_transition,
		int is_theme,
		ArrayList<PluginServer*> &plugindb)
{
// Get plugins
//printf("MWindow::create_plugindb 1 %d\n", this->plugindb->total);
	for(int i = 0; i < this->plugindb->total; i++)
	{
		PluginServer *current = this->plugindb->values[i];
//printf("MWindow::create_plugindb 2 %d %d %d %d %d\n", this->plugindb->total,
//	i, 
// 	current->audio,
//	current->video,
//	current->realtime,
//	current->transition);

		if(current->audio == do_audio &&
			current->video == do_video &&
			(current->realtime == is_realtime || is_realtime < 0) &&
			current->transition == is_transition &&
			current->theme == is_theme)
			plugindb.append(current);
	}
//printf("MWindow::create_plugindb 3 %d\n");

// Alphabetize list by title
	int done = 0;
	while(!done)
	{
		done = 1;
		
		for(int i = 0; i < plugindb.total - 1; i++)
		{
			PluginServer *value1 = plugindb.values[i];
			PluginServer *value2 = plugindb.values[i + 1];
			if(strcmp(value1->title, value2->title) > 0)
			{
				done = 0;
				plugindb.values[i] = value2;
				plugindb.values[i + 1] = value1;
			}
		}
	}
//printf("MWindow::create_plugindb 4\n");
}

PluginServer* MWindow::scan_plugindb(char *title)
{
	for(int i = 0; i < plugindb->total; i++)
	{
		if(!strcasecmp(plugindb->values[i]->title, title)) 
			return plugindb->values[i];
	}
	return 0;
}

void MWindow::init_preferences()
{
//printf("MWindow::init_preferences 1\n");
	preferences = new Preferences;
	preferences->load_defaults(defaults);
//printf("MWindow::init_preferences 1\n");
	session = new MainSession(this);
	session->load_defaults(defaults);
//printf("MWindow::init_preferences 2\n");
}

void MWindow::clean_indexes()
{
	FileSystem fs;
	int total_excess;
	long oldest;
	int oldest_item;
	int result;
	char string[1024];

// Delete extra indexes
	fs.set_filter("*.idx");
	fs.complete_path(preferences->index_directory);
	fs.update(preferences->index_directory);
//printf("MWindow::clean_indexes 1 %d\n", fs.dir_list.total);

// Eliminate directories
	result = 1;
	while(result)
	{
		result = 0;
		for(int i = 0; i < fs.dir_list.total && !result; i++)
		{
			fs.join_names(string, preferences->index_directory, fs.dir_list.values[i]->name);
			if(!fs.is_dir(string))
			{
				delete fs.dir_list.values[i];
				fs.dir_list.remove_number(i);
				result = 1;
			}
		}
	}
	total_excess = fs.dir_list.total - preferences->index_count;

//printf("MWindow::clean_indexes 2 %d\n", fs.dir_list.total);
	while(total_excess > 0)
	{
// Get oldest
		for(int i = 0; i < fs.dir_list.total; i++)
		{
			fs.join_names(string, preferences->index_directory, fs.dir_list.values[i]->name);

			if(i == 0 || fs.get_date(string) <= oldest)
			{
				oldest = fs.get_date(string);
				oldest_item = i;
			}
		}

		fs.join_names(string, preferences->index_directory, fs.dir_list.values[oldest_item]->name);
		if(remove(string))
			perror("delete_indexes");
		delete fs.dir_list.values[oldest_item];
		fs.dir_list.remove_number(oldest_item);
		total_excess--;
	}
}

void MWindow::init_awindow()
{
	awindow = new AWindow(this);
	awindow->create_objects();
}

void MWindow::init_theme()
{
	theme = 0;

	for(int i = 0; i < plugindb->total; i++)
	{
		if(plugindb->values[i]->theme &&
			!strcasecmp(preferences->theme, plugindb->values[i]->title))
		{
			PluginServer plugin = *plugindb->values[i];
			plugin.open_plugin(0, 0, 0);
			theme = plugin.new_theme();
			theme->mwindow = this;
			strcpy(theme->path, plugin.path);
			plugin.close_plugin();
		}
	}

	if(!theme)
	{
		fprintf(stderr, "MWindow::init_theme: theme %s not found.\n", preferences->theme);
		exit(1);
	}

	theme->initialize();
}

void MWindow::init_edl()
{
	edl = new EDL;
//printf("MWindow::init_edl 1\n");
	edl->create_objects();
//printf("MWindow::init_edl 1\n");
    edl->load_defaults(defaults);
//printf("MWindow::init_edl 1\n");
	edl->create_default_tracks();
//printf("MWindow::init_edl 1\n");
	edl->tracks->update_y_pixels(theme);
//printf("MWindow::init_edl 2\n");
}

void MWindow::init_compositor()
{
	cwindow = new CWindow(this);
    cwindow->create_objects();
}

void MWindow::init_levelwindow()
{
	lwindow = new LevelWindow(this);
	lwindow->create_objects();
}

void MWindow::init_viewer()
{
	vwindow = new VWindow(this);
	vwindow->load_defaults();
	vwindow->create_objects();
}

void MWindow::init_cache()
{
	audio_cache = new CICache(edl, plugindb);
	video_cache = new CICache(edl, plugindb);
}

void MWindow::init_tuner(ArrayList<Channel*> &channeldb, char *path)
{
	FileSystem fs;
	char directory[1024];
	FileXML file;
	Channel *channel;
	int done;

	sprintf(directory, BCASTDIR);
	fs.complete_path(directory);
	fs.join_names(directory, directory, path);
	done = file.read_from_file(directory, 1);

// Load channels
	while(!done)
	{
		channel = new Channel;
		if(!(done = channel->load(&file)))
			channeldb.append(channel);
		else
		{
			delete channel;
		}
	}
}

void MWindow::save_tuner(ArrayList<Channel*> &channeldb, char *path)
{
	FileSystem fs;
	char directory[1024];
	FileXML file;

	sprintf(directory, BCASTDIR);
	fs.complete_path(directory);
	strcat(directory, path);

	if(channeldb.total)
	{
		for(int i = 0; i < channeldb.total; i++)
		{
// Save channel here
			channeldb.values[i]->save(&file);
		}
		file.terminate_string();
		file.write_to_file(directory);
	}
}

void MWindow::init_menus()
{
	char string[BCTEXTLEN];
	cmodel_to_text(string, BC_RGB888);
	colormodels.append(new ColormodelItem(string, BC_RGB888));
	cmodel_to_text(string, BC_RGBA8888);
	colormodels.append(new ColormodelItem(string, BC_RGBA8888));
	cmodel_to_text(string, BC_RGB161616);
	colormodels.append(new ColormodelItem(string, BC_RGB161616));
	cmodel_to_text(string, BC_RGBA16161616);
	colormodels.append(new ColormodelItem(string, BC_RGBA16161616));
	cmodel_to_text(string, BC_YUV888);
	colormodels.append(new ColormodelItem(string, BC_YUV888));
	cmodel_to_text(string, BC_YUVA8888);
	colormodels.append(new ColormodelItem(string, BC_YUVA8888));
	cmodel_to_text(string, BC_YUV161616);
	colormodels.append(new ColormodelItem(string, BC_YUV161616));
	cmodel_to_text(string, BC_YUVA16161616);
	colormodels.append(new ColormodelItem(string, BC_YUVA16161616));
}

void MWindow::init_indexes()
{
	mainindexes = new MainIndexes(this);
	mainindexes->start_loop();
}

void MWindow::init_gui()
{
	gui = new MWindowGUI(this);
	gui->create_objects();
	gui->load_defaults(defaults);
}

void MWindow::init_render()
{
	render = new Render(this);
	renderlist = new Render(this);
}

int MWindow::load_filenames(ArrayList<char*> *filenames, int load_mode)
{
	ArrayList<EDL*> new_edls;
	ArrayList<Asset*> new_assets;
//printf("load_filenames 1\n");




// Define new_edls and new_assets to load
	int result = 0;
	for(int i = 0; i < filenames->total; i++)
	{
// Get type of file
		File *new_file = new File;
		Asset *new_asset = new Asset(filenames->values[i]);
		EDL *new_edl = new EDL;
		char string[BCTEXTLEN];

//printf("load_filenames 1\n");
		new_edl->create_objects();
		new_edl->copy_session(edl);

		sprintf(string, "Loading %s", new_asset->path);
		gui->show_message(string, BLACK);
		result = new_file->open_file(plugindb, new_asset, 1, 0, 0, 0);
//printf("load_filenames 2\n");

		switch(result)
		{
// Convert media file to EDL
			case FILE_OK:
//printf("load_filenames 1.1\n");
				if(load_mode != LOAD_RESOURCESONLY)
				{
					asset_to_edl(new_edl, new_asset);
//printf("load_filenames 1.2\n");
					new_edls.append(new_edl);
				}
				else
//printf("load_filenames 1.3\n");
				{
					new_assets.append(new_asset);
				}
				if(load_mode == LOAD_REPLACE || 
					load_mode == LOAD_REPLACE_CONCATENATE)
					set_filename("");
				result = 0;
				break;

// File not found
			case FILE_NOT_FOUND:
				sprintf(string, "Failed to open %s", new_asset->path);
				gui->show_message(string, RED);
				result = 1;
				break;

// Unknown format
			case FILE_UNRECOGNIZED_CODEC:
			{
// Test index file
				IndexFile indexfile(this);
				result = indexfile.open_index(this, new_asset);
				if(!result)
				{
					indexfile.close_index();
				}

// Test existing EDLs
				if(result)
				{
					for(int j = 0; j < new_edls.total + 1; j++)
					{
						Asset *old_asset;
						if(j == new_edls.total)
						{
							if(old_asset = edl->assets->get_asset(new_asset->path))
							{
								*new_asset = *old_asset;
								result = 0;
							}
						}
						else
						{
							if(old_asset = new_edls.values[j]->assets->get_asset(new_asset->path))
							{
								*new_asset = *old_asset;
								result = 0;
							}
						}
					}
				}

// Prompt user
				if(result)
				{
					char string[BCTEXTLEN];
					FileSystem fs;
					fs.extract_name(string, new_asset->path);

					strcat(string, "'s format couldn't be determined.");
					new_asset->audio_data = 1;
					new_asset->format = FILE_PCM;
					new_asset->channels = defaults->get("AUDIO_CHANNELS", 2);
					new_asset->sample_rate = defaults->get("SAMPLE_RATE", 44100);
					new_asset->bits = defaults->get("AUDIO_BITS", 16);
					new_asset->byte_order = defaults->get("BYTE_ORDER", 1);
					new_asset->signed_ = defaults->get("SIGNED_", 1);
					new_asset->header = defaults->get("HEADER", 0);

					FileFormat fwindow(this);
					fwindow.create_objects(new_asset, string);
					result = fwindow.run_window();

					defaults->update("AUDIO_CHANNELS", new_asset->channels);
					defaults->update("SAMPLE_RATE", new_asset->sample_rate);
					defaults->update("AUDIO_BITS", new_asset->bits);
					defaults->update("BYTE_ORDER", new_asset->byte_order);
					defaults->update("SIGNED_", new_asset->signed_);
					defaults->update("HEADER", new_asset->header);
					save_defaults();
				}

// Append to list
				if(!result)
				{
// Recalculate length
					delete new_file;
					new_file = new File;
					result = new_file->open_file(plugindb, new_asset, 1, 0, 0, 0);

					if(load_mode != LOAD_RESOURCESONLY)
					{
						asset_to_edl(new_edl, new_asset);
//printf("MWindow::load_filenames 1 %d %d\n", new_asset->video_length, new_asset->audio_length);
//new_edl->dump();
						new_edls.append(new_edl);
					}
					else
					{
						new_assets.append(new_asset);
					}
				}
				else
				{
					result = 1;
				}
				break;
			}

			case FILE_IS_XML:
			{
//printf("load_filenames 2\n");
				FileXML xml_file;
				xml_file.read_from_file(filenames->values[i]);
//printf("load_filenames 3\n");
				strcpy(session->filename, filenames->values[i]);
				gui->lock_window();
				gui->update_title(session->filename);
				gui->unlock_window();
// Load EDL for pasting
//printf("load_filenames 3\n");
				new_edl->load_xml(plugindb, &xml_file, LOAD_ALL);
//printf("load_filenames 3\n");
				strcpy(new_edl->local_session->clip_title, filenames->values[i]);
//printf("load_filenames 3\n");
				set_filename(new_edl->local_session->clip_title);
//printf("load_filenames 3\n");
//new_edl->dump();
//printf("load_filenames 2\n");
				new_edls.append(new_edl);
//printf("load_filenames 4\n");
				result = 0;
				break;
			}
		}
//printf("load_filenames 4\n");

		if(result)
		{
			delete new_edl;
			delete new_asset;
		}
//printf("load_filenames 5\n");

		delete new_file;
//printf("load_filenames 6\n");

	}
//printf("MWindow::load_filenames 5 %d\n", new_edls.total);

//sleep(10);


	if(!result) gui->statusbar->default_message();


//printf("MWindow::load_filenames 7 %d\n", new_edls.total);


// Don't back up here
	if(new_edls.total)
	{
		paste_edls(&new_edls, 
			load_mode,
			0,
			-1,
			edl->session->labels_follow_edits, 
			edl->session->plugins_follow_edits);
	}
//printf("MWindow::load_filenames 8 %d\n", new_edls.total);
//sleep(10);

// Should only happen in LOAD_RESOURCESONLY
	if(new_assets.total)
	{
		for(int i = 0; i < new_assets.total; i++)
		{
			mainindexes->add_next_asset(new_assets.values[i]);
			edl->assets->update(new_assets.values[i]);
		}


		update_project(load_mode);
// Start examining next batch of index files
		mainindexes->start_build();
	}

printf("MWindow::load_filenames 9\n");
//sleep(10);

	new_edls.remove_all_objects();
	new_assets.remove_all_objects();
//printf("MWindow::load_filenames 10 %d\n", edl->session->audio_module_fragment);

	if(load_mode == LOAD_REPLACE ||
		load_mode == LOAD_REPLACE_CONCATENATE)
		session->changes_made = 0;

	return 0;
}

void MWindow::create_objects(int want_gui, int want_new)
{
	char string[1024];
	FileSystem fs;

// Work around X bug
//	BC_WindowBase::get_resources()->use_xvideo = 0;

	edl = 0;

	init_menus();
//printf("MWindow::create_objects 1\n");
	init_defaults(defaults);
//printf("MWindow::create_objects 1\n");
	init_preferences();
//printf("MWindow::create_objects 1\n");
	init_plugins(preferences, plugindb);
//printf("MWindow::create_objects 1\n");
	init_theme();
// Default project created here
//printf("MWindow::create_objects 1\n");
	init_edl();

//printf("MWindow::create_objects 1\n");
	init_awindow();
//printf("MWindow::create_objects 1\n");
	init_compositor();
//printf("MWindow::create_objects 1\n");
	init_levelwindow();
//printf("MWindow::create_objects 1\n");
	init_viewer();
//printf("MWindow::create_objects 1\n");
	init_tuner(channeldb_v4l, "channels_v4l");
//printf("MWindow::create_objects 1\n");
	init_tuner(channeldb_buz, "channels_buz");
//printf("MWindow::create_objects 1\n");
	init_cache();
//printf("MWindow::create_objects 1\n");
	init_indexes();
//printf("MWindow::create_objects 1\n");
	init_gui();
//printf("MWindow::create_objects 1\n");
	init_render();
	mainprogress = new MainProgress(this, gui);
	undo = new MainUndo(this);

	plugin_guis = new ArrayList<PluginServer*>;

//printf("MWindow::create_objects 1\n");
	if(session->show_vwindow) vwindow->gui->show_window();
	if(session->show_cwindow) cwindow->gui->show_window();
	if(session->show_awindow) awindow->gui->show_window();
	if(session->show_lwindow) lwindow->gui->show_window();
//printf("MWindow::create_objects 1\n");

	vwindow->start();
	awindow->start();
	cwindow->start();
	lwindow->start();
//printf("MWindow::create_objects 1\n");

	gui->mainmenu->load_defaults(defaults);
//printf("MWindow::create_objects 1\n");
	gui->mainmenu->update_toggles();
//printf("MWindow::create_objects 1\n");
	gui->patchbay->update();
//printf("MWindow::create_objects 1\n");
	gui->canvas->draw();
//printf("MWindow::create_objects 1\n");
	gui->cursor->draw();
//printf("MWindow::create_objects 1\n");
	gui->raise_window();
//printf("MWindow::create_objects 1\n");
	gui->show_window();
//printf("MWindow::create_objects 1\n");
	gui->flush();
//printf("MWindow::create_objects 2\n");
}

void MWindow::show_vwindow()
{
	session->show_vwindow = 1;
	vwindow->gui->lock_window();
	vwindow->gui->show_window();
	vwindow->gui->raise_window();
	vwindow->gui->flush();
	vwindow->gui->unlock_window();
	gui->mainmenu->show_vwindow->set_checked(1);
}

void MWindow::show_awindow()
{
	session->show_awindow = 1;
	awindow->gui->lock_window();
	awindow->gui->show_window();
	awindow->gui->raise_window();
	awindow->gui->flush();
	awindow->gui->unlock_window();
	gui->mainmenu->show_awindow->set_checked(1);
}

void MWindow::show_cwindow()
{
	session->show_cwindow = 1;
	cwindow->gui->lock_window();
	cwindow->gui->show_window();
	cwindow->gui->raise_window();
	cwindow->gui->flush();
	cwindow->gui->unlock_window();
	gui->mainmenu->show_cwindow->set_checked(1);
}

void MWindow::show_lwindow()
{
	session->show_lwindow = 1;
	lwindow->gui->lock_window();
	lwindow->gui->show_window();
	lwindow->gui->raise_window();
	lwindow->gui->flush();
	lwindow->gui->unlock_window();
	gui->mainmenu->show_lwindow->set_checked(1);
}

void MWindow::tile_windows()
{
	session->default_window_positions();
	gui->default_positions();
}

void MWindow::toggle_loop_playback()
{
	edl->local_session->loop_playback = !edl->local_session->loop_playback;
	set_loop_boundaries();
	save_backup();

	gui->canvas->draw_overlays();
	gui->canvas->flash();
	sync_parameters(CHANGE_PARAMS);
}

void MWindow::set_titles(int value)
{
	edl->session->show_titles = value;
	trackmovement(edl->local_session->track_start);
}

void MWindow::set_auto_keyframes(int value)
{
	gui->lock_window();
	edl->session->auto_keyframes = value;
	gui->mbuttons->edit_panel->keyframe->update(value);
	gui->flush();
	gui->unlock_window();
	cwindow->gui->lock_window();
	cwindow->gui->edit_panel->keyframe->update(value);
	cwindow->gui->flush();
	cwindow->gui->unlock_window();
}

int MWindow::set_editing_mode(int new_editing_mode)
{
	gui->lock_window();
	edl->session->editing_mode = new_editing_mode;
	gui->mbuttons->edit_panel->editing_mode = edl->session->editing_mode;
	gui->mbuttons->edit_panel->update();
	gui->canvas->update_cursor();
	gui->unlock_window();
	cwindow->gui->lock_window();
	cwindow->gui->edit_panel->update();
	cwindow->gui->edit_panel->editing_mode = edl->session->editing_mode;
	cwindow->gui->unlock_window();
	return 0;
}

void MWindow::update_caches()
{
	audio_cache->set_edl(edl);
	video_cache->set_edl(edl);
}

void MWindow::show_plugin(Plugin *plugin)
{
	int done = 0;
	for(int i = 0; i < plugin_guis->total; i++)
	{
		if(plugin_guis->values[i]->plugin == plugin)
		{
			plugin_guis->values[i]->raise_window();
			done = 1;
			break;
		}
	}

	if(!done)
	{
		PluginServer *server = scan_plugindb(plugin->title);

//printf("MWindow::show_plugin %p %d\n", server, server->uses_gui);
		if(server && server->uses_gui)
		{
			PluginServer *gui = plugin_guis->append(new PluginServer(*server));
// Needs mwindow to do GUI
			gui->set_mwindow(this);
			gui->open_plugin(0, edl, plugin);
			gui->show_gui();
			plugin->show = 1;
		}
	}
}

void MWindow::hide_plugin(Plugin *plugin)
{
	plugin->show = 0;
	for(int i = 0; i < plugin_guis->total; i++)
	{
		if(plugin_guis->values[i]->plugin == plugin)
		{
//printf("MWindow::hide_plugin 1\n");
// Last command executed in client side close
			plugin_guis->remove_object(plugin_guis->values[i]);
//printf("MWindow::hide_plugin 2\n");
			break;
		}
	}
}

void MWindow::hide_plugins()
{
	plugin_guis->remove_all_objects();
}

void MWindow::update_plugin_guis()
{
//printf("MWindow::update_plugin_guis 1\n");



	for(int i = 0; i < plugin_guis->total; i++)
	{
		plugin_guis->values[i]->update_gui();
	}
}

void MWindow::update_plugin_states()
{
	int result = 0;
	for(int i = 0; i < plugin_guis->total; i++)
	{
		Plugin *src_plugin = plugin_guis->values[i]->plugin;

// Search for plugin pointer in EDL.  Only the master EDL shows plugin GUIs.
		for(Track *track = edl->tracks->first; 
			track && !result; 
			track = track->next)
		{
			for(int j = 0; 
				j < track->plugin_set.total && !result; 
				j++)
			{
				PluginSet *plugin_set = track->plugin_set.values[j];
				for(Plugin *plugin = (Plugin*)plugin_set->first; 
					plugin && !result; 
					plugin = (Plugin*)plugin->next)
				{
					if(plugin == src_plugin) result = 1;
				}
			}
		}


// Doesn't exist anymore
		if(!result)
		{
			hide_plugin(src_plugin);
			i--;
		}
	}
}


void MWindow::update_plugin_titles()
{
	for(int i = 0; i < plugin_guis->total; i++)
	{
		plugin_guis->values[i]->update_title();
	}
}

int MWindow::asset_to_edl(EDL *new_edl, 
	Asset *new_asset, 
	RecordLabels *labels)
{
//printf("MWindow::asset_to_edl 1\n");
//	new_edl->load_defaults(defaults);

// Keep frame rate, sample rate, and output size unchanged.
// These parameters would revert the project if VWindow displayed an asset
// of different size than the project.
	if(new_asset->video_data)
	{
		new_edl->session->video_tracks = new_asset->layers;
//		new_edl->session->frame_rate = new_asset->frame_rate;
//		new_edl->session->output_w = new_asset->width;
//		new_edl->session->output_h = new_asset->height;
	}
	else
		new_edl->session->video_tracks = 0;






	if(new_asset->audio_data)
	{
		new_edl->session->audio_tracks = new_asset->channels;
//		new_edl->session->sample_rate = new_asset->sample_rate;
	}
	else
		new_edl->session->audio_tracks = 0;
//printf("MWindow::asset_to_edl 2 %d %d\n", new_edl->session->video_tracks, new_edl->session->audio_tracks);

	new_edl->create_default_tracks();

// Disable drawing if the file format isn't fast enough.
	if(new_asset->format == FILE_MPEG)
	{
		for(Track *current = new_edl->tracks->first;
			current;
			current = NEXT)
		{
			if(current->data_type == TRACK_VIDEO) current->draw = 0;
		}
	}



//printf("MWindow::asset_to_edl 3\n");
	new_edl->insert_asset(new_asset, 0, 0, labels);
//printf("MWindow::asset_to_edl 3\n");





	char string[BCTEXTLEN];
	FileSystem fs;
	fs.extract_name(string, new_asset->path);
//printf("MWindow::asset_to_edl 3\n");

	strcpy(new_edl->local_session->clip_title, string);
//printf("MWindow::asset_to_edl 4 %s\n", string);

//	new_edl->dump();
	return 0;
}

// Reset everything after a load.
void MWindow::update_project(int load_mode)
{
//printf("MWindow::update_project 1\n");
	edl->tracks->update_y_pixels(theme);

// Draw timeline
//printf("MWindow::update_project 1\n");
	update_caches();

//printf("MWindow::update_project 1\n");
	gui->update(1, 1, 1, 1, 1, 1, 1);

//printf("MWindow::update_project 1\n");
	cwindow->update(0, 0, 1, 1, 1);

//printf("MWindow::update_project 1\n");






//	vwindow->gui->update_sources("None");
//	vwindow->change_source((EDL*)0);

// Since VWindow splices using paste_edls this won't work for VWindow.
	if(load_mode == LOAD_REPLACE ||
		load_mode == LOAD_REPLACE_CONCATENATE)
	{
		vwindow->change_source(edl->session->vwindow_folder, 
			edl->session->vwindow_source);
	}
	else
	{
		vwindow->update(1);
	}



//printf("MWindow::update_project 1\n");




	cwindow->gui->slider->set_position();
	cwindow->gui->timebar->update(1, 1);
	cwindow->playback_engine->que->send_command(CURRENT_FRAME, 
		CHANGE_ALL,
		edl,
		1);

//printf("MWindow::update_project 1\n");
	awindow->gui->lock_window();
//printf("MWindow::update_project 1\n");
	awindow->gui->update_assets();
//printf("MWindow::update_project 1\n");
	awindow->gui->flush();
//printf("MWindow::update_project 1\n");
	awindow->gui->unlock_window();
//printf("MWindow::update_project 12\n");
	gui->flush();
//printf("MWindow::update_project 13\n");
}

void MWindow::rebuild_indices()
{
	char source_filename[BCTEXTLEN], index_filename[BCTEXTLEN];

	for(int i = 0; i < session->drag_assets->total; i++)
	{
// Erase file
		IndexFile::get_index_filename(source_filename, 
			preferences->index_directory,
			index_filename, 
			session->drag_assets->values[i]->path);
		remove(index_filename);
// Schedule index build
		session->drag_assets->values[i]->index_status = INDEX_NOTTESTED;
		mainindexes->add_next_asset(session->drag_assets->values[i]);
	}
	mainindexes->start_build();
}


void MWindow::save_backup()
{
	FileXML file;
	edl->save_xml(plugindb, 
		&file, 
		BACKUP_PATH);
	file.terminate_string();
	char path[BCTEXTLEN];
	FileSystem fs;
	strcpy(path, BACKUP_PATH);
	fs.complete_path(path);

	if(file.write_to_file(path))
	{
		char string2[256];
		sprintf(string2, "Couldn't open %s for writing.", BACKUP_PATH);
		gui->show_message(string2);
	}
}


int MWindow::create_aspect_ratio(float &w, float &h, int width, int height)
{
	int denominator;
	float fraction = (float)width / height;

	for(denominator = 1; 
		denominator < 100 && 
			fabs(fraction * denominator - (int)(fraction * denominator)) > .001; 
		denominator++)
		;

	w = denominator * width / height;
	h = denominator;
}

void MWindow::render_single()
{
	if(!render->running())
		render->start();
}

void MWindow::render_list()
{
	if(!renderlist->running())
		renderlist->start();
}

void MWindow::dump_plugins()
{
	for(int i = 0; i < plugindb->total; i++)
	{
		printf("audio=%d video=%d realtime=%d transition=%d theme=%d %s\n",
			plugindb->values[i]->audio,
			plugindb->values[i]->video,
			plugindb->values[i]->realtime,
			plugindb->values[i]->transition,
			plugindb->values[i]->theme,
			plugindb->values[i]->title);
	}
}

























int MWindow::save_defaults()
{
//printf("MWindow::save_defaults 1\n");
	gui->save_defaults(defaults);
	edl->save_defaults(defaults);
//printf("MWindow::save_defaults 1\n");
	session->save_defaults(defaults);
	preferences->save_defaults(defaults);
//printf("MWindow::save_defaults 1\n");

	save_tuner(channeldb_v4l, "channels_v4l");
	save_tuner(channeldb_buz, "channels_buz");
//printf("MWindow::save_defaults 1\n");
	defaults->save();
//printf("MWindow::save_defaults 2\n");
	return 0;
}

int MWindow::run_script(FileXML *script)
{
	int result = 0, result2 = 0;
	while(!result && !result2)
	{
		result = script->read_tag();
		if(!result)
		{
			if(script->tag.title_is("new_project"))
			{
// Run new in immediate mode.
//				gui->mainmenu->new_project->run_script(script);
			}
			else
			if(script->tag.title_is("record"))
			{
// Run record as a thread.  It is a terminal command.
				;
// Will read the complete scipt file without letting record read it if not
// terminated.
				result2 = 1;
			}
			else
			{
				printf("MWindow::run_script: Unrecognized command: %s\n",script->tag.get_title() );
			}
		}
	}
	return result2;
}

// ================================= synchronization

int MWindow::lock_resize() { resize_lock = 1; return 0; }

int MWindow::unlock_resize() { resize_lock = 0; return 0; }


// =============================== file operations

// REMOVE
int MWindow::load(FileXML *xml, int import_,
		int edits_only,
		int patches_only,
		int console_only,
		int timebar_only,
		int automation_only)
{

	return 0;
}

// REMOVE
int MWindow::load_video_config(FileXML *xml, int import_)
{
	if(!import_) 
	{
// 		session->frame_rate = xml->tag.get_property("FRAMERATE", session->frame_rate);
// 		preferences->frames_per_foot = xml->tag.get_property("FRAMES_PER_FOOT", preferences->frames_per_foot);
// 		session->track_w = xml->tag.get_property("TRACKW", session->track_w);
// 		session->track_h = xml->tag.get_property("TRACKH", session->track_h);
// 		session->output_w = xml->tag.get_property("OUTPUTW", session->output_w);
// 		session->output_h = xml->tag.get_property("OUTPUTH", session->output_h);
// 		session->aspect_w = xml->tag.get_property("ASPECTW", session->aspect_w);
// 		session->aspect_h = xml->tag.get_property("ASPECTH", session->aspect_h);
// 		video_window->resize_window();
	}
	return 0;
}

// REMOVE
int MWindow::load_audio_config(FileXML *xml, int import_,
		int edits_only, 
		int patches_only,
		int console_only,
		int automation_only)
{
	return 0; 
}

// REMOVE
int MWindow::load_edits(FileXML *xml)
{
	return 0; 
}

// REMOVE
int MWindow::load_patches(FileXML *xml)
{
	return 0; 
}

// REMOVE
int MWindow::load_console(FileXML *xml)	
{
	return 0; 
}

// REMOVE
int MWindow::load_timebar(FileXML *xml)	
{
	return 0; 
}

// REMOVE
int MWindow::load_automation(FileXML *xml)
{
	return 0; 
}

int MWindow::interrupt_indexes()
{
	mainindexes->interrupt_build();
	return 0; 
}



void MWindow::next_time_format()
{
	switch(edl->session->time_format)
	{
		case 0: edl->session->time_format = 1; break;
		case 1: edl->session->time_format = 2; break;
		case 2: edl->session->time_format = 3; break;
		case 3: edl->session->time_format = 4; break;
		case 4: edl->session->time_format = 5; break;
		case 5: edl->session->time_format = 0; break;
	}

	gui->lock_window();
	gui->update(0, 0, 1, 1, 0, 1, 0);
	gui->redraw_time_dependancies();
	char string[BCTEXTLEN], string2[BCTEXTLEN];
	sprintf(string, "Using %s.", Units::print_time_format(edl->session->time_format, string2));
	gui->show_message(string, BLACK);
	gui->flush();
	gui->unlock_window();
}

int MWindow::set_filename(char *filename)
{
	strcpy(session->filename, filename);
	if(gui)
	{
		if(filename[0] == 0)
		{
			gui->set_title(PROGRAM_NAME);
		}
		else
		{
			FileSystem dir;
			char string[1024], string2[1024];
			dir.extract_name(string, filename);
			sprintf(string2, PROGRAM_NAME ": %s", string);
			gui->set_title(string2);
		}
	}
	return 0; 
}

// ========================================== drawing

// REMOVE
int MWindow::draw()
{
	if(gui)
	{
		gui->zoombar->draw();
//		tracks->draw();
		gui->samplescroll->set_position();
		timebar->draw();

//		tracks->show_overlays(1);
	}
	return 0; 
}


// ======================================= cursors


// REMOVE
int MWindow::draw_floating_handle(int flash) 
{ 
//	tracks->draw_floating_handle(flash); 
	return 0; 
}







// ============================================ selecting

int MWindow::set_selectionend(long new_position)
{
	return 0; 
}

int MWindow::set_selectionstart(long new_position)
{
	return 0; 
}

int MWindow::set_selection(long selectionstart, long selectionend)
{
	return 0; 
}

int MWindow::get_affected_range(long *start, long *end, int reverse)
{
	return 0; 
}


int MWindow::set_loop_boundaries()
{
	double start = edl->local_session->get_selectionstart();
	double end = edl->local_session->get_selectionend();
	
	if(start != 
		end) 
	{
		;
	}
	else
	if(edl->tracks->total_length())
	{
		start = 0;
		end = edl->tracks->total_length();
	}
	else
	{
		start = end = 0;
	}

	if(edl->local_session->loop_playback && start != end)
	{
		edl->local_session->loop_start = start;
		edl->local_session->loop_end = end;
	}
	return 0; 
}


// ======================================= playback

int MWindow::update_playback_cursor(long new_position, int view_follows_playback)
{
	return 0; 
}

int MWindow::show_playback_cursor(long position, int flash)
{
	return 0;
}

int MWindow::hide_playback_cursor(int flash)
{
	return 0; 
}

int MWindow::set_playback_range(long start_position, int reverse, float speed)
{
	return 0; 
}

int MWindow::arm_playback(int follow_loop, 
					int use_buttons, 
					int infinite, 
					AudioDevice *audio)
{
	return 0; 
}



int MWindow::start_playback() 
{ 
	return 0; 
}

int MWindow::stop_playback(int update_button) 
{
	return 0; 
}

int MWindow::wait_for_playback()
{
	return 0; 
}




long MWindow::get_playback_position() 
{
	return 0;
}

int MWindow::start_reconfigure(int unlock_window)
{
	return 0; 
}

int MWindow::stop_reconfigure(int unlock_window)
{
	return 0; 
}


int MWindow::reset_meters()
{
	cwindow->gui->lock_window();
	cwindow->gui->meters->reset_meters();
	cwindow->gui->unlock_window();

	vwindow->gui->lock_window();
	vwindow->gui->meters->reset_meters();
	vwindow->gui->unlock_window();

	gui->lock_window();
	gui->patchbay->reset_meters();
	gui->unlock_window();
	return 0; 
}


// ======================================= window sizes


int MWindow::get_top() 
{ 
	if(gui)
	{
		if(session->tracks_vertical)
			return gui->get_w() - 18 - 17;     // timebar + scrollbar
		else
			return timebar->gui->get_y() + timebar->gui->get_h();
	}
	else
	return 0;
}

int MWindow::get_bottom() 
{  
	if(gui)
	{
		if(session->tracks_vertical)
			return BUTTONBARWIDTH;         // mbuttons
		else
			return gui->get_h() - 24 - 17;      // zoombar + scrollbar
	}
	else
	return 0;
}
