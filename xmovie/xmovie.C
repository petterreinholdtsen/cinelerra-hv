#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "filesystem.h"
#include "mwindow.h"

int usage()
{
	printf("XMovie (c) 2002 Heroine Virtual Ltd.\n");
	return 0;
}

int main(int argc, char *argv[])
{
	ArrayList<char*> init_playlist;
	char *string;
    FileSystem fs;

	usage();
	for(int i = 1, j = 0; i < argc; i++, j++)
	{
		init_playlist.append(string = new char[1024]);
		strcpy(string, argv[i]);
        fs.complete_path(string);
    }

	{
		MWindow mwindow(&init_playlist);
		mwindow.create_objects();

// only loading the first movie on the command line now
// Possibly load an audio file for merge later.
		if(init_playlist.total) mwindow.load_file(init_playlist.values[0], 0);
		mwindow.run_program();
	}

	for(int i = 0; i < init_playlist.total; i++) delete init_playlist.values[i];
	init_playlist.remove_all();
	return 0;
}
