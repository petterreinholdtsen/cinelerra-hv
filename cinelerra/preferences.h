#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "audioconfig.inc"
#include "defaults.inc"
#include "guicast.h"
#include "maxchannels.h"
#include "preferences.inc"
#include "videoconfig.inc"


class Preferences
{
public:
	Preferences();
	~Preferences();

	Preferences& operator=(Preferences &that);
	int load_defaults(Defaults *defaults);
	int save_defaults(Defaults *defaults);

	void add_node(char *text, int port, int enabled);
	void delete_node(int number);
	void sort_nodes();
	void edit_node(int number, char *new_text, int port, int enabled);
	int get_enabled_nodes();
	char* get_node_hostname(int number);
	int get_node_port(int number);

// ================================= Performance ================================
// directory to look in for indexes
	char index_directory[BCTEXTLEN];   
// size of index file in bytes
	long index_size;                  
	int index_count;
// Use thumbnails in AWindow assets.
	int use_thumbnails;
// Title of theme
	char theme[BCTEXTLEN];
	double render_preroll;
	
	int use_renderfarm;
	int renderfarm_port;
	ArrayList<char*> renderfarm_nodes;
	ArrayList<int>   renderfarm_ports;
	ArrayList<int>   renderfarm_enabled;
	char renderfarm_mountpoint[BCTEXTLEN];
// Jobs per node
	int renderfarm_job_count;
// Consolidate output files
	int renderfarm_consolidate;

// ====================================== Plugin Set ==============================
	char global_plugin_dir[BCTEXTLEN];
	char local_plugin_dir[BCTEXTLEN];
};

#endif
