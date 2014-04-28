#include "audioconfig.h"
#include "audiodevice.inc"
#include "bcmeter.inc"
#include "clip.h"
#include "defaults.h"
#include "filesystem.h"
#include "guicast.h"
#include "preferences.h"
#include "theme.h"
#include "videoconfig.h"
#include "videodevice.inc"
#include <string.h>

#define CLAMP(x, y, z) (x) = ((x) < (y) ? (y) : ((x) > (z) ? (z) : (x)))












Preferences::Preferences()
{
// Set defaults
	FileSystem fs;
	
	sprintf(index_directory, BCASTDIR);
	if(strlen(index_directory))
		fs.complete_path(index_directory);
	index_size = 1000000;
	index_count = 100;
	use_thumbnails = 1;
	theme[0] = 0;
	use_renderfarm = 0;
	renderfarm_port = DEAMON_PORT;
	render_preroll = 0;
	renderfarm_mountpoint[0] = 0;
	renderfarm_job_count = 1;
}

Preferences::~Preferences()
{
}

Preferences& Preferences::operator=(Preferences &that)
{
// ================================= Performance ================================
	strcpy(index_directory, that.index_directory);
	index_size = that.index_size;
	index_count = that.index_count;
	use_thumbnails = that.use_thumbnails;
	strcpy(global_plugin_dir, that.global_plugin_dir);
	strcpy(local_plugin_dir, that.local_plugin_dir);
	strcpy(theme, that.theme);

	renderfarm_nodes.remove_all_objects();
	renderfarm_ports.remove_all();
	renderfarm_enabled.remove_all();
	for(int i = 0; i < that.renderfarm_nodes.total; i++)
	{
		add_node(that.renderfarm_nodes.values[i], 
			that.renderfarm_ports.values[i],
			that.renderfarm_enabled.values[i]);
	}
	use_renderfarm = that.use_renderfarm;
	renderfarm_port = that.renderfarm_port;
	render_preroll = that.render_preroll;
	renderfarm_job_count = that.renderfarm_job_count;
	strcpy(renderfarm_mountpoint, that.renderfarm_mountpoint);
	renderfarm_consolidate = that.renderfarm_consolidate;

// Check boundaries

	FileSystem fs;
	if(strlen(index_directory))
	{
		fs.complete_path(index_directory);
		fs.add_end_slash(index_directory);
	}
	
	if(strlen(global_plugin_dir))
	{
		fs.complete_path(global_plugin_dir);
		fs.add_end_slash(global_plugin_dir);
	}
	
	if(strlen(local_plugin_dir))
	{
		fs.complete_path(local_plugin_dir);
		fs.add_end_slash(local_plugin_dir);
	}

	renderfarm_job_count = MAX(renderfarm_job_count, 1);

	return *this;
}

int Preferences::load_defaults(Defaults *defaults)
{
	char string[BCTEXTLEN];

	defaults->get("INDEX_DIRECTORY", index_directory);
	index_size = defaults->get("INDEX_SIZE", index_size);
	index_count = defaults->get("INDEX_COUNT", index_count);
	use_thumbnails = defaults->get("USE_THUMBNAILS", use_thumbnails);

	sprintf(global_plugin_dir, "/usr/lib/cinelerra");
	sprintf(local_plugin_dir, "");
	strcpy(theme, DEFAULT_THEME);
	defaults->get("GLOBAL_PLUGIN_DIR", global_plugin_dir);
	defaults->get("LOCAL_PLUGIN_DIR", local_plugin_dir);
	defaults->get("THEME", theme);
	
	
	use_renderfarm = defaults->get("USE_RENDERFARM", use_renderfarm);
	renderfarm_port = defaults->get("RENDERFARM_PORT", renderfarm_port);
	render_preroll = defaults->get("RENDERFARM_PREROLL", render_preroll);
	renderfarm_job_count = defaults->get("RENDERFARM_JOBS_COUNT", renderfarm_job_count);
	renderfarm_consolidate = defaults->get("RENDERFARM_CONSOLIDATE", renderfarm_consolidate);
	defaults->get("RENDERFARM_MOUNTPOINT", renderfarm_mountpoint);
	int renderfarm_total = defaults->get("RENDERFARM_TOTAL", 0);

	for(int i = 0; i < renderfarm_total; i++)
	{
		sprintf(string, "RENDERFARM_NODE%d", i);
		char result[BCTEXTLEN];
		int result_port;
		int result_enabled;

		result[0] = 0;
		defaults->get(string, result);

		sprintf(string, "RENDERFARM_PORT%d", i);
		result_port = defaults->get(string, renderfarm_port);

		sprintf(string, "RENDERFARM_ENABLED%d", i);
		result_enabled = defaults->get(string, result_enabled);

		if(result[0] != 0)
		{
			add_node(result, result_port, result_enabled);
		}
	}

	return 0;
}

int Preferences::save_defaults(Defaults *defaults)
{
	char string[BCTEXTLEN];
	defaults->update("INDEX_DIRECTORY", index_directory);
	defaults->update("INDEX_SIZE", index_size);
	defaults->update("INDEX_COUNT", index_count);
	defaults->update("USE_THUMBNAILS", use_thumbnails);
	defaults->update("GLOBAL_PLUGIN_DIR", global_plugin_dir);
	defaults->update("LOCAL_PLUGIN_DIR", local_plugin_dir);
	defaults->update("THEME", theme);



	defaults->update("USE_RENDERFARM", use_renderfarm);
	defaults->update("RENDERFARM_PORT", renderfarm_port);
	defaults->update("RENDERFARM_PREROLL", render_preroll);
	defaults->update("RENDERFARM_MOUNTPOINT", renderfarm_mountpoint);
	defaults->update("RENDERFARM_JOBS_COUNT", renderfarm_job_count);
	defaults->update("RENDERFARM_CONSOLIDATE", renderfarm_consolidate);
	defaults->update("RENDERFARM_TOTAL", renderfarm_nodes.total);
	for(int i = 0; i < renderfarm_nodes.total; i++)
	{
		sprintf(string, "RENDERFARM_NODE%d", i);
		defaults->update(string, renderfarm_nodes.values[i]);
		sprintf(string, "RENDERFARM_PORT%d", i);
		defaults->update(string, renderfarm_ports.values[i]);
		sprintf(string, "RENDERFARM_ENABLED%d", i);
		defaults->update(string, renderfarm_enabled.values[i]);
	}
	return 0;
}


void Preferences::add_node(char *text, int port, int enabled)
{
	if(text[0] == 0) return;

	char *new_item = new char[strlen(text) + 1];
	strcpy(new_item, text);
	renderfarm_nodes.append(new_item);
	renderfarm_ports.append(port);
	renderfarm_enabled.append(enabled);
}

void Preferences::delete_node(int number)
{
	if(number < renderfarm_nodes.total)
	{
		delete [] renderfarm_nodes.values[number];
		renderfarm_nodes.remove_number(number);
		renderfarm_ports.remove_number(number);
		renderfarm_enabled.remove_number(number);
	}
}

void Preferences::sort_nodes()
{
	int done = 0;

	while(!done)
	{
		done = 1;
		for(int i = 0; i < renderfarm_nodes.total - 1; i++)
		{
			if(strcmp(renderfarm_nodes.values[i], renderfarm_nodes.values[i + 1]) > 0)
			{
				char *temp = renderfarm_nodes.values[i];
				int temp_port = renderfarm_ports.values[i];

				renderfarm_nodes.values[i] = renderfarm_nodes.values[i + 1];
				renderfarm_nodes.values[i + 1] = temp;

				renderfarm_ports.values[i] = renderfarm_ports.values[i + 1];
				renderfarm_ports.values[i + 1] = temp_port;

				renderfarm_enabled.values[i] = renderfarm_enabled.values[i + 1];
				renderfarm_enabled.values[i + 1] = temp_port;
				done = 0;
			}
		}
	}
}

void Preferences::edit_node(int number, 
	char *new_text, 
	int new_port, 
	int new_enabled)
{
	char *new_item = new char[strlen(new_text) + 1];
	strcpy(new_item, new_text);

	delete [] renderfarm_nodes.values[number];
	renderfarm_nodes.values[number] = new_item;
	renderfarm_ports.values[number] = new_port;
	renderfarm_enabled.values[number] = new_enabled;
}

int Preferences::get_enabled_nodes()
{
	int result = 0;
	for(int i = 0; i < renderfarm_enabled.total; i++)
		if(renderfarm_enabled.values[i]) result++;
	return result;
}

char* Preferences::get_node_hostname(int number)
{
	int total = 0;
	for(int i = 0; i < renderfarm_nodes.total; i++)
	{
		if(renderfarm_enabled.values[i])
		{
			if(total == number)
				return renderfarm_nodes.values[i];
			else
				total++;
		}
	}
	return "";
}

int Preferences::get_node_port(int number)
{
	int total = 0;
	for(int i = 0; i < renderfarm_ports.total; i++)
	{
		if(renderfarm_enabled.values[i])
		{
			if(total == number)
				return renderfarm_ports.values[i];
			else
				total++;
		}
	}
	return -1;
}















