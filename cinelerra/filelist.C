#include "assets.h"
#include "file.h"
#include "filelist.h"
#include "guicast.h"
#include "mwindow.inc"
#include "vframe.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


FileList::FileList(Asset *asset, 
	File *file, 
	char *list_prefix,
	char *file_extension, 
	int frame_type,
	int list_type)
 : FileBase(asset, file)
{
	reset_parameters();
	asset->video_data = 1;
	this->list_prefix = list_prefix;
	this->file_extension = file_extension;
	this->frame_type = frame_type;
	this->list_type = list_type;
	table_lock = new Mutex;
}

FileList::~FileList()
{
	close_file();
	delete table_lock;
}

int FileList::reset_parameters_derived()
{
	data = 0;
	writer = 0;
	temp = 0;
}

int FileList::open_file(int rd, int wr)
{
	this->rd = rd;
	this->wr = wr;
	int result = 0;

// skip header for write
	if(wr)
	{
// Frame files are created in write_frame and list index is created when
// file is closed.
//printf("FileList::open_file 1\n");
		path_list.remove_all_objects();
//printf("FileList::open_file 1\n");
		writer = new FrameWriter(this, asset->format == list_type ? file->cpus : 1);
//printf("FileList::open_file 3\n");
	}
	else
	if(rd)
	{
// Determine type of file
		FILE *stream = fopen(asset->path, "rb");
		if(stream)
		{
			char string[BCTEXTLEN];
			fread(string, strlen(list_prefix), 1, stream);
			fclose(stream);
//printf("FileList::open_file 1\n");

			if(!strncasecmp(string, list_prefix, strlen(list_prefix)))
			{
//printf("FileList::open_file 2\n");

				asset->format = list_type;

// Open index here or get frame size from file.
				result = read_list_header();
				if(!result) result = read_frame_header(path_list.values[0]);
			}
			else
			{
//printf("FileList::open_file 3\n");
				asset->format = frame_type;
				result = read_frame_header(asset->path);
				asset->layers = 1;
				if(!asset->frame_rate)
					asset->frame_rate = 1;
				asset->video_length = -1;
//printf("FileList::open_file 4\n");
			}
		}
	}

	file->current_frame = 0;
// Compressed data storage
	data = new VFrame;

	return result;
}


int FileList::close_file()
{
//printf("FileList::close_file 1 %d %d %d %d\n", 
//	path_list.total, asset->format, list_type, wr);
	if(asset->format == list_type && path_list.total)
	{
		if(wr) write_list_header();
		path_list.remove_all_objects();
	}
//printf("FileList::close_file 1\n");
	if(data) delete data;
//printf("FileList::close_file 1\n");
	if(writer) delete writer;
	if(temp) delete temp;
//printf("FileList::close_file 1\n");
	reset_parameters();
//printf("FileList::close_file 1\n");

	FileBase::close_file();
//printf("FileList::close_file 2\n");
	return 0;
}

int FileList::write_list_header()
{
	FILE *stream = fopen(asset->path, "w");
	fprintf(stream, "%s\n", list_prefix);
	fprintf(stream, "# First line is always %s\n", list_prefix);
	fprintf(stream, "# Frame rate:\n");
	fprintf(stream, "%f\n", asset->frame_rate);
	fprintf(stream, "# Width:\n");
	fprintf(stream, "%d\n", asset->width);
	fprintf(stream, "# Height:\n");
	fprintf(stream, "%d\n", asset->height);
	fprintf(stream, "# List of image files follows\n");

//printf("write_list_header 1 %d\n", path_list.total);
	for(int i = 0; i < path_list.total; i++)
	{
		fprintf(stream, "%s\n", path_list.values[i]);
	}
	fclose(stream);
	return 0;
}

int FileList::read_list_header()
{
	char string[BCTEXTLEN], *new_entry;

	FILE *stream = fopen(asset->path, "r");
	
	
	if(stream)
	{
// Get information about the frames
		do
		{
			fgets(string, BCTEXTLEN, stream);
		}while(!feof(stream) && (string[0] == '#' || string[0] == ' ' || isalpha(string[0])));

// Don't want a user configured frame rate to get destroyed
		if(asset->frame_rate == 0)
			asset->frame_rate = atof(string);

		do
		{
			fgets(string, BCTEXTLEN, stream);
		}while(!feof(stream) && (string[0] == '#' || string[0] == ' '));
		asset->width = atol(string);

		do
		{
			fgets(string, BCTEXTLEN, stream);
		}while(!feof(stream) && (string[0] == '#' || string[0] == ' '));
		asset->height = atol(string);

		asset->layers = 1;
		asset->audio_data = 0;
		asset->video_data = 1;

// Get all the paths
		while(!feof(stream))
		{
			fgets(string, BCTEXTLEN, stream);
			if(strlen(string) && string[0] != '#' && string[0] != ' ' && !feof(stream))
			{
				string[strlen(string) - 1] = 0;
				path_list.append(new_entry = new char[strlen(string) + 1]);
				strcpy(new_entry, string);
			}
		}

//for(int i = 0; i < path_list.total; i++) printf("%s\n", path_list.values[i]);
		fclose(stream);
		asset->video_length = path_list.total;
	}
	else
		return 1;

	return 0;
}

int FileList::read_frame(VFrame *frame)
{
	int result = 0;
//printf("FileList::read_frame 1\n");
	if(asset->format == list_type)
	{
		char *path = path_list.values[file->current_frame];
		FILE *in;



		if(!(in = fopen(path, "rb")))
		{
			fprintf(stderr, "FileList::read_frame %s: %s\n", path, strerror(errno));
		}
		else
		{
			struct stat ostat;
			stat(path, &ostat);

			switch(frame->get_color_model())
			{
				case BC_COMPRESSED:
					frame->allocate_compressed_data(ostat.st_size);
					frame->set_compressed_size(ostat.st_size);
					fread(frame->get_data(), ostat.st_size, 1, in);
					break;
				default:
					data->allocate_compressed_data(ostat.st_size);
					data->set_compressed_size(ostat.st_size);
					fread(data->get_data(), ostat.st_size, 1, in);
					return read_frame(frame, data);
					break;
			}

			fclose(in);
		}
	}
	else
	{
//printf("FileList::read_frame 2 %p\n", temp);

// Allocate and decompress once into temporary
		if(!temp || temp->get_color_model() != frame->get_color_model())
		{
//printf("FileList::read_frame 3\n");
			if(temp) delete temp;
			temp = 0;
//printf("FileList::read_frame 3\n");
		
			FILE *fd = fopen(asset->path, "rb");
//printf("FileList::read_frame 3\n");
			if(fd)
			{
				struct stat ostat;
				stat(asset->path, &ostat);
//printf("FileList::read_frame 4\n");

				switch(frame->get_color_model())
				{
					case BC_COMPRESSED:
						frame->allocate_compressed_data(ostat.st_size);
						frame->set_compressed_size(ostat.st_size);
						fread(frame->get_data(), ostat.st_size, 1, fd);
						break;
					default:
						data->allocate_compressed_data(ostat.st_size);
						data->set_compressed_size(ostat.st_size);
						fread(data->get_data(), ostat.st_size, 1, fd);
						temp = new VFrame(0, 
							asset->width, 
							asset->height, 
							frame->get_color_model());
						read_frame(temp, data);
						break;
				}
//printf("FileList::read_frame 4\n");
				fclose(fd);
//printf("FileList::read_frame 5\n");
				
			}
			else
			{
				fprintf(stderr, "FileList::read_frame %s: %s", asset->path, strerror(errno));
				result = 1;
			}
//printf("FileList::read_frame 6\n");
		}
//printf("FileList::read_frame 7\n");

		if(!temp) return result;

//printf("FileList::read_frame 8\n");
		if(frame->get_color_model() == temp->get_color_model())
		{
//printf("FileList::read_frame 9\n");
			frame->copy_from(temp);
//printf("FileList::read_frame 10\n");
		}
		else
		{
// Never happens
//printf("FileList::read_frame 11\n");
			cmodel_transfer(frame->get_rows(), /* Leave NULL if non existent */
				temp->get_rows(),
				frame->get_y(), /* Leave NULL if non existent */
				frame->get_u(),
				frame->get_v(),
				temp->get_y(), /* Leave NULL if non existent */
				temp->get_u(),
				temp->get_v(),
				0,        /* Dimensions to capture from input frame */
				0, 
				asset->width, 
				asset->height,
				0,       /* Dimensions to project on output frame */
				0, 
				asset->width, 
				asset->height,
				temp->get_color_model(), 
				frame->get_color_model(),
				0,         /* When transfering BC_RGBA8888 to non-alpha this is the background color in 0xRRGGBB hex */
				temp->get_w(),       /* For planar use the luma rowspan */
				frame->get_w());
//printf("FileList::read_frame 12\n");
		}
	}


//printf("FileList::read_frame 13\n");


	return result;
}

int FileList::write_frames(VFrame ***frames, int len)
{
	return_value = 0;

//printf("FileList::write_frames 1 %d\n", len);
	if(frames[0][0]->get_color_model() == BC_COMPRESSED)
	{
//printf("FileList::write_frames 1\n");
		for(int i = 0; i < asset->layers && !return_value; i++)
		{
//printf("FileList::write_frames 1\n");
			for(int j = 0; j < len && !return_value; j++)
			{
//printf("FileList::write_frames 1\n");
				char *path = create_path();
				FILE *fd = fopen(path, "wb");
				if(fd)
				{
					return_value += fwrite(frames[i][j]->get_data(),
						frames[i][j]->get_compressed_size(),
						1,
						fd);
					
					fclose(fd);
				}
				else
					return_value++;
					
//printf("FileList::write_frames 2\n");
			}
		}
	}
	else
	{
//printf("FileList::write_frames 3\n");
		writer->write_frames(frames, len);
	}
	return return_value;
}









void FileList::add_return_value(int amount)
{
	table_lock->lock();
	return_value += amount;
	table_lock->unlock();
}

char* FileList::create_path()
{
//printf("FileList::create_path 1 %d %d\n", asset->format, list_type);
	if(asset->format != list_type) return asset->path;

	table_lock->lock();



	int k;
	char *path = "";
	char output[BCTEXTLEN];
	if(file->current_frame >= path_list.total)
	{
		strcpy(output, asset->path);
		for(k = strlen(output) - 1; k > 0 && output[k] != '.'; k--)
			;
		if(k <= 0) k = strlen(output);
		sprintf(&output[k], "%06d%s", file->current_frame++, file_extension);
		path = new char[strlen(output) + 1];
		strcpy(path, output);
		path_list.append(path);
	}
	else
	{
// Overwrite an old path
		path = path_list.values[file->current_frame];
	}





	table_lock->unlock();
	
	return path;
}

FrameWriterUnit* FileList::new_writer_unit(FrameWriter *writer)
{
	return new FrameWriterUnit(writer);
}










FrameWriterPackage::FrameWriterPackage()
{
}

FrameWriterPackage::~FrameWriterPackage()
{
}











FrameWriterUnit::FrameWriterUnit(FrameWriter *server)
 : LoadClient(server)
{
// Don't use server here since subclasses call this with no server.
	this->server = server;
	output = new VFrame;
}

FrameWriterUnit::~FrameWriterUnit()
{
	delete output;
}

void FrameWriterUnit::process_package(LoadPackage *package)
{
	FrameWriterPackage *ptr = (FrameWriterPackage*)package;

	FILE *file;
	if(!(file = fopen(ptr->path, "wb"))) return;
	
	
	int result = server->file->write_frame(ptr->input, output, this);
	
	if(!result) result = !fwrite(output->get_data(), output->get_compressed_size(), 1, file);
	fclose(file);
	
	server->file->add_return_value(result);
}











FrameWriter::FrameWriter(FileList *file, int cpus)
 : LoadServer(cpus, 0)
{
	this->file = file;
}


FrameWriter::~FrameWriter()
{
}

void FrameWriter::init_packages()
{
	for(int i = 0, layer = 0, number = 0; 
		i < get_total_packages(); 
		i++)
	{
		FrameWriterPackage *package = (FrameWriterPackage*)get_package(i);
//printf("FrameWriter::init_packages %d %d\n", layer, number);
		package->input = frames[layer][number];
		package->path = file->create_path();
		number++;
		if(number >= len)
		{
			layer++;
			number = 0;
		}
	}
}

void FrameWriter::write_frames(VFrame ***frames, int len)
{
	this->frames = frames;
	this->len = len;
	set_package_count(len * file->asset->layers);
	
	process_packages();
}

LoadClient* FrameWriter::new_client()
{
	return file->new_writer_unit(this);
}

LoadPackage* FrameWriter::new_package()
{
	return new FrameWriterPackage;
}



