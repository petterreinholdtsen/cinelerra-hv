#ifndef SHAREDPLUGINLOCATION_H
#define SHAREDPLUGINLOCATION_H

#include "edl.inc"
#include "filexml.inc"

class SharedLocation
{
public:
	SharedLocation();
	SharedLocation(int module, int plugin);
	
	void save(FileXML *file);
	void load(FileXML *file);
	int operator==(const SharedLocation &that);
	SharedLocation& operator=(const SharedLocation &that);
	int get_type();
	void calculate_title(char *string, 
		EDL *edl, 
		double position, 
		int convert_units,
		int plugin_type);
	
	int module, plugin;
};


// REMOVE
class SharedPluginLocation
{
public:
	SharedPluginLocation();
	SharedPluginLocation(int module, int plugin);

	int save(FileXML *xml);
	int load(FileXML *xml, int track_offset);
	int operator==(const SharedPluginLocation &that);

	int module, plugin;
};

// REMOVE
class SharedModuleLocation
{
public:
	SharedModuleLocation();
	SharedModuleLocation(int module);

	int save(FileXML *xml);
	int load(FileXML *xml, int track_offset);
	int operator==(const SharedModuleLocation &that);

	int module;
};

#endif
