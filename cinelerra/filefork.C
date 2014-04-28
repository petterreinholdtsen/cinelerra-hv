/*
 * CINELERRA
 * Copyright (C) 2009 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */



#ifdef USE_FILEFORK


FileFork::FileFork(File *file)
{
// Create the process & socket pair.
	int sockets[2];
	this->file = file;
	done = 0;
	socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
	parent_fd = sockets[0];
	child_fd = sockets[1];
	pid = fork();

// Child process
	if(!pid)
	{
// Set local copy of the file instance to the forked one
		file->is_fork = 1;
		run();
	
	
		_exit(0);
	}
}

FileFork::~FileFork()
{
	
}

void FileFork::run()
{
	int result = 0;
	while(!done && !result)
	{
		result = read_command();
	}
}

int FileFork::send_command()
{
}

int FileFork::read_command()
{
}





#endif // USE_FILEFORK




