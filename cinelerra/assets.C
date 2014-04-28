#include "assets.h"
#include "awindowgui.inc"
#include "batch.h"
#include "cache.h"
#include "defaults.h"
#include "edl.h"
#include "file.h"
#include "filexml.h"
#include "filesystem.h"
#include "indexfile.h"
#include "quicktime.h"
#include "mainsession.h"
#include "threadindexer.h"
#include <string.h>

Assets::Assets(EDL *edl) : List<Asset>()
{
	this->edl = edl;
}

Assets::~Assets()
{
	delete_all();
}

int Assets::load(ArrayList<PluginServer*> *plugindb, 
	FileXML *file, 
	unsigned long load_flags)
{
	int result = 0;

//printf("Assets::load 1\n");
	while(!result)
	{
		result = file->read_tag();
		if(!result)
		{
			if(file->tag.title_is("/ASSETS"))
			{
				result = 1;
			}
			else
			if(file->tag.title_is("ASSET"))
			{
//printf("Assets::load 2\n");
				char *path = file->tag.get_property("SRC");
//printf("Assets::load 3\n");
				Asset new_asset(path ? path : SILENCE);
//printf("Assets::load 4\n");
				new_asset.read(plugindb, file);
//printf("Assets::load 5\n");
				update(&new_asset);
//printf("Assets::load 6\n");
			}
		}
	}
//printf("Assets::load 7\n");
	return 0;
}

int Assets::save(ArrayList<PluginServer*> *plugindb, FileXML *file, char *path)
{
	file->tag.set_title("ASSETS");
	file->append_tag();
	file->append_newline();

	for(Asset* current = first; current; current = NEXT)
	{
		current->write(plugindb, 
			file, 
			0, 
			path);
	}

	file->tag.set_title("/ASSETS");
	file->append_tag();
	file->append_newline();	
	file->append_newline();	
	return 0;
}

Assets& Assets::operator=(Assets &assets)
{
//printf("Assets::operator= 1 %p\n", last);
	while(last) delete last;

//printf("Assets::operator= 2\n");
	for(Asset *current = assets.first; current; current = NEXT)
	{
//printf("Assets::operator= 3\n");
		Asset *new_asset;
//printf("Assets::operator= 4\n");
		append(new_asset = new Asset);
//printf("Assets::operator= 5\n");
		*new_asset = *current;
//printf("Assets::operator= 6\n");
	}
//printf("Assets::operator= 7\n");

	return *this;
}


void Assets::update_index(Asset *asset)
{
	for(Asset* current = first; current; current = NEXT)
	{
		if(current->test_path(asset->path))
		{
			current->update_index(asset);
		}
	}
}

Asset* Assets::update(Asset *asset)
{
	if(!asset) return 0;

	for(Asset* current = first; current; current = NEXT)
	{
// Asset already exists.
		if(current->test_path(asset->path)) 
		{
			return current;
		}
	}

// Asset doesn't exist.
	Asset *asset_copy = new Asset(*asset);
	append(asset_copy);
	return asset_copy;
}

int Assets::delete_all()
{
	while(last)
	{
		remove(last);
	}
	return 0;
}

Asset* Assets::update(const char *path)
{
	Asset* current = first;

	while(current)
	{
		if(current->test_path(path)) 
		{
			return current;
		}
		current = NEXT;
	}

	return append(new Asset(path));
}

Asset* Assets::get_asset(const char *filename)
{
	Asset* current = first;
	Asset* result = 0;

	while(current)
	{
//printf("Assets::get_asset %p %s\n", filename, filename);
		if(current->test_path(filename))
		{
			result = current;
			break;
		}
		current = current->next;
	}

	return result;	
}

Asset* Assets::remove_asset(Asset *asset)
{
	delete asset;
}


int Assets::number_of(Asset *asset)
{
	int i;
	Asset *current;

	for(i = 0, current = first; current && current != asset; i++, current = NEXT)
		;

	return i;
}

Asset* Assets::asset_number(int number)
{
	int i;
	Asset *current;

	for(i = 0, current = first; i < number && current; i++, current = NEXT)
		;
	
	return current;
}

int Assets::update_old_filename(char *old_filename, char *new_filename)
{
	for(Asset* current = first; current; current = NEXT)
	{
		if(!strcmp(current->path, old_filename))
		{
			current->update_path(new_filename);
		}
	}
	return 0;
}


int Assets::dump()
{
	for(Asset *current = first; current; current = NEXT)
	{
		current->dump();
	}
	return 0;
}


// ==========================================================

Asset::Asset() : ListItem<Asset>()
{
	init_values();
}

Asset::Asset(Asset &asset) : ListItem<Asset>()
{
	init_values();
	*this = asset;
}

Asset::Asset(const char *path) : ListItem<Asset>()
{
	init_values();
	strcpy(this->path, path);
}

Asset::Asset(const int plugin_type, const char *plugin_title) : ListItem<Asset>()
{
	init_values();
}

Asset::~Asset()
{
	if(index_offsets) delete [] index_offsets;
// Don't delete index buffer since it is shared with the index thread.
}


int Asset::init_values()
{
	path[0] = 0;
	strcpy(folder, MEDIA_FOLDER);
	format = 0;
	channels = 0;
	sample_rate = 0;
	bits = 0;
	byte_order = 0;
	signed_ = 0;
	header = 0;
	dither = 0;
	audio_data = 0;
	video_data = 0;
	audio_length = 0;
	video_length = 0;

	layers = 0;
	frame_rate = 0;
	width = 0;
	height = 0;
	strcpy(vcodec, QUICKTIME_YUV2);
	strcpy(acodec, QUICKTIME_TWOS);
	jpeg_quality = 100;
	
	ampeg_bitrate = 256;
	ampeg_derivative = 3;

	vorbis_vbr = 0;
	vorbis_min_bitrate = -1;
	vorbis_bitrate = 128000;
	vorbis_max_bitrate = -1;

	mp3_bitrate = 256000;







// mpeg parameters
	vmpeg_iframe_distance = 45;
	vmpeg_bframe_distance = 0;
	vmpeg_progressive = 0;
	vmpeg_denoise = 1;
	vmpeg_bitrate = 1000000;
	vmpeg_derivative = 1;
	vmpeg_quantization = 15;
	vmpeg_cmodel = 0;
	vmpeg_fix_bitrate = 0;

// Divx parameters.  Defaults from encore2
	divx_bitrate = 2000000;
	divx_rc_period = 50;
	divx_rc_reaction_ratio = 45;
	divx_rc_reaction_period = 10;
	divx_max_key_interval = 250;
	divx_max_quantizer = 31;
	divx_min_quantizer = 1;
	divx_quantizer = 15;
	divx_quality = 5;
	divx_fix_bitrate = 1;

	ms_bitrate = 1000000;
	ms_quantization = 10;
	ms_gop_size = 45;
	ms_fix_bitrate = 1;


	png_use_alpha = 0;



	reset_index();
	id = EDL::next_id();
	return 0;
}

int Asset::reset_index()
{
	index_status = INDEX_NOTTESTED;
	index_start = old_index_end = index_end = 0;
	index_offsets = 0;
	index_zoom = 0;
	index_bytes = 0;
	index_buffer = 0;
	return 0;
}

void Asset::copy_format(Asset *asset)
{
//printf("Asset::copy_format 1\n");
	update_index(asset);

//printf("Asset::copy_format 1\n");
	audio_data = asset->audio_data;
	format = asset->format;
	channels = asset->channels;
	sample_rate = asset->sample_rate;
	bits = asset->bits;
	byte_order = asset->byte_order;
	signed_ = asset->signed_;
	header = asset->header;
	dither = asset->dither;
	mp3_bitrate = asset->mp3_bitrate;



	video_data = asset->video_data;
	layers = asset->layers;
	frame_rate = asset->frame_rate;
	width = asset->width;
	height = asset->height;
	strcpy(vcodec, asset->vcodec);
	strcpy(acodec, asset->acodec);

	this->audio_length = asset->audio_length;
	this->video_length = asset->video_length;


	ampeg_bitrate = asset->ampeg_bitrate;
	ampeg_derivative = asset->ampeg_derivative;


	vorbis_vbr = asset->vorbis_vbr;
	vorbis_min_bitrate = asset->vorbis_min_bitrate;
	vorbis_bitrate = asset->vorbis_bitrate;
	vorbis_max_bitrate = asset->vorbis_max_bitrate;



	jpeg_quality = asset->jpeg_quality;

// mpeg parameters
	vmpeg_iframe_distance = asset->vmpeg_iframe_distance;
	vmpeg_bframe_distance = asset->vmpeg_bframe_distance;
	vmpeg_progressive = asset->vmpeg_progressive;
	vmpeg_denoise = asset->vmpeg_denoise;
	vmpeg_bitrate = asset->vmpeg_bitrate;
	vmpeg_derivative = asset->vmpeg_derivative;
	vmpeg_quantization = asset->vmpeg_quantization;
	vmpeg_cmodel = asset->vmpeg_cmodel;
	vmpeg_fix_bitrate = asset->vmpeg_fix_bitrate;



	divx_bitrate = asset->divx_bitrate;
	divx_rc_period = asset->divx_rc_period;
	divx_rc_reaction_ratio = asset->divx_rc_reaction_ratio;
	divx_rc_reaction_period = asset->divx_rc_reaction_period;
	divx_max_key_interval = asset->divx_max_key_interval;
	divx_max_quantizer = asset->divx_max_quantizer;
	divx_min_quantizer = asset->divx_min_quantizer;
	divx_quantizer = asset->divx_quantizer;
	divx_quality = asset->divx_quality;
	divx_fix_bitrate = asset->divx_fix_bitrate;

	ms_bitrate = asset->ms_bitrate;
	ms_quantization = asset->ms_quantization;
	ms_gop_size = asset->ms_gop_size;
	ms_fix_bitrate = asset->ms_fix_bitrate;

	
	png_use_alpha = asset->png_use_alpha;
//printf("Asset::copy_format 2\n");
}

long Asset::get_index_offset(int channel)
{
	if(channel < channels && index_offsets)
		return index_offsets[channel];
	else
		return 0;
}

Asset& Asset::operator=(Asset &asset)
{
//printf("Asset::operator= 1\n");
	strcpy(this->path, asset.path);
//printf("Asset::operator= 1\n");
	strcpy(this->folder, asset.folder);
//printf("Asset::operator= 1\n");
	copy_format(&asset);
//printf("Asset::operator= 2\n");
	return *this;
}


int Asset::equivalent(Asset &asset, 
	int test_audio, 
	int test_video)
{
//printf("Asset::equivalent %s %s %d %d\n", path, asset.path, format, asset.format);
	int result = (!strcmp(asset.path, path) &&
		format == asset.format);

	if(test_audio && result)
	{
		result = (channels == asset.channels && 
			sample_rate == asset.sample_rate && 
			bits == asset.bits && 
			byte_order == asset.byte_order && 
			signed_ == asset.signed_ && 
			header == asset.header && 
			dither == asset.dither &&
			!strcmp(acodec, asset.acodec));
	}


	if(test_video && result)
	{
		result = (layers == asset.layers && 
			frame_rate == asset.frame_rate &&
			width == asset.width &&
			height == asset.height &&
			!strcmp(vcodec, asset.vcodec));
	}

	return result;
}

int Asset::operator==(Asset &asset)
{

	return equivalent(asset, 
		1, 
		1);
}

int Asset::operator!=(Asset &asset)
{
	return !(*this == asset);
}

int Asset::test_path(const char *path)
{
// 	char old_name[1024], new_name[1024];
// 	FileSystem fs;
// 
// 	fs.extract_name(old_name, this->path);
// 	fs.extract_name(new_name, path);
	if(!strcasecmp(this->path, path)) 
		return 1; 
	else 
		return 0;
}

int Asset::test_plugin_title(const char *path)
{
}

int Asset::read(ArrayList<PluginServer*> *plugindb, FileXML *file)
{
	int result = 0;

// Check for relative path.
	char new_path[1024];
	strcpy(new_path, path);
	char asset_directory[1024], input_directory[1024];
	FileSystem fs;
	fs.set_current_dir("");

	fs.extract_dir(asset_directory, path);

//printf("Asset::read 1\n");
// No path in asset
	if(!asset_directory[0])
	{
		fs.extract_dir(input_directory, file->filename);
//printf("Asset::read 2 %s %s\n", input_directory, new_path);

// Input file has a path
		if(input_directory[0])
		{
			sprintf(path, "%s/%s", input_directory, new_path);
		}
	}

	while(!result)
	{
		result = file->read_tag();
		if(!result)
		{
			if(file->tag.title_is("/ASSET"))
			{
				result = 1;
			}
			else
			if(file->tag.title_is("AUDIO"))
			{
				read_audio(file);
			}
			else
			if(file->tag.title_is("FORMAT"))
			{
				char *string = file->tag.get_property("TYPE");
				format = File::strtoformat(plugindb, string);
			}
			else
			if(file->tag.title_is("FOLDER"))
			{
				strcpy(folder, file->read_text());
			}
			else
			if(file->tag.title_is("VIDEO"))
			{
				read_video(file);
			}
			else
			if(file->tag.title_is("INDEX"))
			{
				read_index(file);
			}
		}
	}

//printf("Asset::read 2\n");
	return 0;
}

int Asset::read_audio(FileXML *file)
{
	channels = file->tag.get_property("CHANNELS", 2);
// This is loaded from the index file after the EDL but this 
// should be overridable in the EDL.
	if(!sample_rate) sample_rate = file->tag.get_property("RATE", 44100);
	bits = file->tag.get_property("BITS", 16);
	byte_order = file->tag.get_property("BYTE_ORDER", 1);
	signed_ = file->tag.get_property("SIGNED", 1);
	header = file->tag.get_property("HEADER", 0);
	dither = file->tag.get_property("DITHER", 0);

	audio_length = file->tag.get_property("AUDIO_LENGTH", 0);
	acodec[0] = 0;
	file->tag.get_property("ACODEC", acodec);
	


	ampeg_bitrate = file->tag.get_property("AMPEG_BITRATE", ampeg_bitrate);
	ampeg_derivative = file->tag.get_property("AMPEG_DERIVATIVE", ampeg_derivative);

	vorbis_vbr = file->tag.get_property("VORBIS_VBR", vorbis_vbr);
	vorbis_min_bitrate = file->tag.get_property("VORBIS_MIN_BITRATE", vorbis_min_bitrate);
	vorbis_bitrate = file->tag.get_property("VORBIS_BITRATE", vorbis_bitrate);
	vorbis_max_bitrate = file->tag.get_property("VORBIS_MAX_BITRATE", vorbis_max_bitrate);

	mp3_bitrate = file->tag.get_property("MP3_BITRATE", mp3_bitrate);


	audio_data = 1;
	return 0;
}

int Asset::read_video(FileXML *file)
{
	height = file->tag.get_property("HEIGHT", height);
	width = file->tag.get_property("WIDTH", width);
	layers = file->tag.get_property("LAYERS", layers);
// This is loaded from the index file after the EDL but this 
// should be overridable in the EDL.
	if(!frame_rate) frame_rate = file->tag.get_property("FRAMERATE", frame_rate);
	vcodec[0] = 0;
	file->tag.get_property("VCODEC", vcodec);

	video_length = file->tag.get_property("VIDEO_LENGTH", 0);




	jpeg_quality = file->tag.get_property("JPEG_QUALITY", jpeg_quality);




	vmpeg_iframe_distance = file->tag.get_property("VMPEG_IFRAME_DISTANCE", vmpeg_iframe_distance);
	vmpeg_bframe_distance = file->tag.get_property("VMPEG_BFRAME_DISTANCE", vmpeg_bframe_distance);
	vmpeg_progressive = file->tag.get_property("VMPEG_PROGRESSIVE", vmpeg_progressive);
	vmpeg_denoise = file->tag.get_property("VMPEG_DENOISE", vmpeg_denoise);
	vmpeg_bitrate = file->tag.get_property("VMPEG_BITRATE", vmpeg_bitrate);
	vmpeg_derivative = file->tag.get_property("VMPEG_DERIVATIVE", vmpeg_derivative);
	vmpeg_quantization = file->tag.get_property("VMPEG_QUANTIZATION", vmpeg_quantization);
	vmpeg_cmodel = file->tag.get_property("VMPEG_CMODEL", vmpeg_cmodel);
	vmpeg_fix_bitrate = file->tag.get_property("VMPEG_FIX_BITRATE", vmpeg_fix_bitrate);


	divx_bitrate = file->tag.get_property("DIVX_BITRATE", divx_bitrate);
	divx_rc_period = file->tag.get_property("DIVX_RC_PERIOD", divx_rc_period);
	divx_rc_reaction_ratio = file->tag.get_property("DIVX_RC_REACTION_RATIO", divx_rc_reaction_ratio);
	divx_rc_reaction_period = file->tag.get_property("DIVX_RC_REACTION_PERIOD", divx_rc_reaction_period);
	divx_max_key_interval = file->tag.get_property("DIVX_MAX_KEY_INTERVAL", divx_max_key_interval);
	divx_max_quantizer = file->tag.get_property("DIVX_MAX_QUANTIZER", divx_max_quantizer);
	divx_min_quantizer = file->tag.get_property("DIVX_MIN_QUANTIZER", divx_min_quantizer);
	divx_quantizer = file->tag.get_property("DIVX_QUANTIZER", divx_quantizer);
	divx_quality = file->tag.get_property("DIVX_QUALITY", divx_quality);
	divx_fix_bitrate = file->tag.get_property("DIVX_FIX_BITRATE", divx_fix_bitrate);

	ms_bitrate = file->tag.get_property("MS_BITRATE", ms_bitrate);
	ms_quantization = file->tag.get_property("MS_QUANTIZATION", ms_quantization);
	ms_gop_size = file->tag.get_property("MS_GOP_SIZE", ms_gop_size);
	ms_fix_bitrate = file->tag.get_property("MS_FIX_BITRATE", ms_fix_bitrate);



	png_use_alpha = file->tag.get_property("PNG_USE_ALPHA", png_use_alpha);




	video_data = 1;
	return 0;
}

int Asset::read_index(FileXML *file)
{
	if(index_offsets) delete [] index_offsets;
	index_offsets = new long[channels];
	for(int i = 0; i < channels; i++) index_offsets[i] = 0;

	int current_offset = 0;
	int result = 0;

	index_zoom = file->tag.get_property("ZOOM", 1);
	index_bytes = file->tag.get_property("BYTES", (longest)0);

	while(!result)
	{
		result = file->read_tag();
		if(!result)
		{
			if(file->tag.title_is("/INDEX"))
			{
				result = 1;
			}
			else
			if(file->tag.title_is("OFFSET"))
			{
				if(current_offset < channels)
				{
					index_offsets[current_offset++] = file->tag.get_property("FLOAT", 0);
//printf("Asset::read_index %d %d\n", current_offset - 1, index_offsets[current_offset - 1]);
				}
			}
		}
	}
	return 0;
}

// Output path is the path of the output file if name truncation is desired.
// It is a "" if complete names should be used.

int Asset::write(ArrayList<PluginServer*> *plugindb, 
	FileXML *file, 
	int include_index, 
	char *output_path)
{
	char new_path[1024];
	char asset_directory[1024], output_directory[1024];
	FileSystem fs;

// Make path relative
	fs.extract_dir(asset_directory, path);
	if(output_path && output_path[0]) 
		fs.extract_dir(output_directory, output_path);
	else
		output_directory[0] = 0;

	if(!strcmp(asset_directory, output_directory))
	{
		fs.extract_name(new_path, path);
	}
	else
	{
		strcpy(new_path, path);
	}

	file->tag.set_title("ASSET");
	file->tag.set_property("SRC", new_path);
	file->append_tag();
	file->append_newline();

	file->tag.set_title("FOLDER");
	file->append_tag();
	file->append_text(folder);
	file->tag.set_title("/FOLDER");
	file->append_tag();
	file->append_newline();

// Write the format information
	file->tag.set_title("FORMAT");

	{
		file->tag.set_property("TYPE", 
			File::formattostr(plugindb, format));
	}

	file->append_tag();
	file->append_newline();

	if(audio_data) write_audio(file);
	if(video_data) write_video(file);
	if(index_status == 0 && include_index) write_index(file);  // index goes after source

	file->tag.set_title("/ASSET");
	file->append_tag();
	file->append_newline();
	return 0;
}

int Asset::write_audio(FileXML *file)
{
	file->tag.set_title("AUDIO");
	file->tag.set_property("CHANNELS", channels);
	file->tag.set_property("RATE", sample_rate);
	file->tag.set_property("BITS", bits);
	file->tag.set_property("BYTE_ORDER", byte_order);
	file->tag.set_property("SIGNED", signed_);
	file->tag.set_property("HEADER", header);
	file->tag.set_property("DITHER", dither);
	if(acodec[0])
		file->tag.set_property("ACODEC", acodec);
	
	file->tag.set_property("AUDIO_LENGTH", audio_length);




	file->tag.set_property("AMPEG_BITRATE", ampeg_bitrate);
	file->tag.set_property("AMPEG_DERIVATIVE", ampeg_derivative);

	file->tag.set_property("VORBIS_VBR", vorbis_vbr);
	file->tag.set_property("VORBIS_MIN_BITRATE", vorbis_min_bitrate);
	file->tag.set_property("VORBIS_BITRATE", vorbis_bitrate);
	file->tag.set_property("VORBIS_MAX_BITRATE", vorbis_max_bitrate);

	file->tag.set_property("MP3_BITRATE", mp3_bitrate);




	file->append_tag();
	file->append_newline();
	return 0;
}

int Asset::write_video(FileXML *file)
{
	file->tag.set_title("VIDEO");
	file->tag.set_property("HEIGHT", height);
	file->tag.set_property("WIDTH", width);
	file->tag.set_property("LAYERS", layers);
	file->tag.set_property("FRAMERATE", frame_rate);
	if(vcodec[0])
		file->tag.set_property("VCODEC", vcodec);

	file->tag.set_property("VIDEO_LENGTH", video_length);



	file->tag.set_property("JPEG_QUALITY", jpeg_quality);

	file->tag.set_property("VMPEG_IFRAME_DISTANCE", vmpeg_iframe_distance);
	file->tag.set_property("VMPEG_BFRAME_DISTANCE", vmpeg_bframe_distance);
	file->tag.set_property("VMPEG_PROGRESSIVE", vmpeg_progressive);
	file->tag.set_property("VMPEG_DENOISE", vmpeg_denoise);
	file->tag.set_property("VMPEG_BITRATE", vmpeg_bitrate);
	file->tag.set_property("VMPEG_DERIVATIVE", vmpeg_derivative);
	file->tag.set_property("VMPEG_QUANTIZATION", vmpeg_quantization);
	file->tag.set_property("VMPEG_CMODEL", vmpeg_cmodel);
	file->tag.set_property("VMPEG_FIX_BITRATE", vmpeg_fix_bitrate);


	file->tag.set_property("DIVX_BITRATE", divx_bitrate);
	file->tag.set_property("DIVX_RC_PERIOD", divx_rc_period);
	file->tag.set_property("DIVX_RC_REACTION_RATIO", divx_rc_reaction_ratio);
	file->tag.set_property("DIVX_RC_REACTION_PERIOD", divx_rc_reaction_period);
	file->tag.set_property("DIVX_MAX_KEY_INTERVAL", divx_max_key_interval);
	file->tag.set_property("DIVX_MAX_QUANTIZER", divx_max_quantizer);
	file->tag.set_property("DIVX_MIN_QUANTIZER", divx_min_quantizer);
	file->tag.set_property("DIVX_QUANTIZER", divx_quantizer);
	file->tag.set_property("DIVX_QUALITY", divx_quality);
	file->tag.set_property("DIVX_FIX_BITRATE", divx_fix_bitrate);


	file->tag.set_property("MS_BITRATE", ms_bitrate);
	file->tag.set_property("MS_QUANTIZATION", ms_quantization);
	file->tag.set_property("MS_GOP_SIZE", ms_gop_size);
	file->tag.set_property("MS_FIX_BITRATE", ms_fix_bitrate);



	file->tag.set_property("PNG_USE_ALPHA", png_use_alpha);





	file->append_tag();
	file->append_newline();
	return 0;
}

int Asset::write_index(FileXML *file)
{
	file->tag.set_title("INDEX");
	file->tag.set_property("ZOOM", index_zoom);
	file->tag.set_property("BYTES", index_bytes);
	file->append_tag();
	file->append_newline();

	if(index_offsets)
	{
		for(int i = 0; i < channels; i++)
		{
			file->tag.set_title("OFFSET");
			file->tag.set_property("FLOAT", index_offsets[i]);
			file->append_tag();
		}
	}

	file->append_newline();
	file->tag.set_title("/INDEX");
	file->append_tag();
	file->append_newline();
	return 0;
}






void Asset::load_defaults(Defaults *defaults)
{

// Can't save codec here because it's specific to render, record, and effect
//	defaults->get("AUDIO_CODEC", acodec);
	ampeg_bitrate = defaults->get("AMPEG_BITRATE", ampeg_bitrate);
	ampeg_derivative = defaults->get("AMPEG_DERIVATIVE", ampeg_derivative);

	vorbis_vbr = defaults->get("VORBIS_VBR", vorbis_vbr);
	vorbis_min_bitrate = defaults->get("VORBIS_MIN_BITRATE", vorbis_min_bitrate);
	vorbis_bitrate = defaults->get("VORBIS_BITRATE", vorbis_bitrate);
	vorbis_max_bitrate = defaults->get("VORBIS_MAX_BITRATE", vorbis_max_bitrate);

	mp3_bitrate = defaults->get("MP3_BITRATE", mp3_bitrate);








//	defaults->get("VIDEO_CODEC", vcodec);


	jpeg_quality = defaults->get("JPEG_QUALITY", jpeg_quality);

// MPEG format information
	vmpeg_iframe_distance = defaults->get("VMPEG_IFRAME_DISTANCE", vmpeg_iframe_distance);
	vmpeg_bframe_distance = defaults->get("VMPEG_BFRAME_DISTANCE", vmpeg_bframe_distance);
	vmpeg_progressive = defaults->get("VMPEG_PROGRESSIVE", vmpeg_progressive);
	vmpeg_denoise = defaults->get("VMPEG_DENOISE", vmpeg_denoise);
	vmpeg_bitrate = defaults->get("VMPEG_BITRATE", vmpeg_bitrate);
	vmpeg_derivative = defaults->get("VMPEG_DERIVATIVE", vmpeg_derivative);
	vmpeg_quantization = defaults->get("VMPEG_QUANTIZATION", vmpeg_quantization);
	vmpeg_cmodel = defaults->get("VMPEG_CMODEL", vmpeg_cmodel);
	vmpeg_fix_bitrate = defaults->get("VMPEG_FIX_BITRATE", vmpeg_fix_bitrate);


	divx_bitrate = defaults->get("DIVX_BITRATE", divx_bitrate);
	divx_rc_period = defaults->get("DIVX_RC_PERIOD", divx_rc_period);
	divx_rc_reaction_ratio = defaults->get("DIVX_RC_REACTION_RATIO", divx_rc_reaction_ratio);
	divx_rc_reaction_period = defaults->get("DIVX_RC_REACTION_PERIOD", divx_rc_reaction_period);
	divx_max_key_interval = defaults->get("DIVX_MAX_KEY_INTERVAL", divx_max_key_interval);
	divx_max_quantizer = defaults->get("DIVX_MAX_QUANTIZER", divx_max_quantizer);
	divx_min_quantizer = defaults->get("DIVX_MIN_QUANTIZER", divx_min_quantizer);
	divx_quantizer = defaults->get("DIVX_QUANTIZER", divx_quantizer);
	divx_quality = defaults->get("DIVX_QUALITY", divx_quality);
	divx_fix_bitrate = defaults->get("DIVX_FIX_BITRATE", divx_fix_bitrate);

	ms_bitrate = defaults->get("MS_BITRATE", ms_bitrate);
	ms_quantization = defaults->get("MS_QUANTIZATION", ms_quantization);
	ms_gop_size = defaults->get("MS_GOP_SIZE", ms_gop_size);
	ms_fix_bitrate = defaults->get("MS_FIX_BITRATE", ms_fix_bitrate);


	png_use_alpha = defaults->get("PNG_USE_ALPHA", png_use_alpha);
}

void Asset::save_defaults(Defaults *defaults)
{

// Can't save codec here because it's specific to render, record, and effect
//	defaults->update("AUDIO_CODEC", acodec);
	defaults->update("AMPEG_BITRATE", ampeg_bitrate);
	defaults->update("AMPEG_DERIVATIVE", ampeg_derivative);

	defaults->update("VORBIS_VBR", vorbis_vbr);
	defaults->update("VORBIS_MIN_BITRATE", vorbis_min_bitrate);
	defaults->update("VORBIS_BITRATE", vorbis_bitrate);
	defaults->update("VORBIS_MAX_BITRATE", vorbis_max_bitrate);
	
	defaults->update("MP3_BITRATE", mp3_bitrate);




//	defaults->update("VIDEO_CODEC", vcodec);

	defaults->update("JPEG_QUALITY", jpeg_quality);

// MPEG format information
	defaults->update("VMPEG_IFRAME_DISTANCE", vmpeg_iframe_distance);
	defaults->update("VMPEG_BFRAME_DISTANCE", vmpeg_bframe_distance);
	defaults->update("VMPEG_PROGRESSIVE", vmpeg_progressive);
	defaults->update("VMPEG_DENOISE", vmpeg_denoise);
	defaults->update("VMPEG_BITRATE", vmpeg_bitrate);
	defaults->update("VMPEG_DERIVATIVE", vmpeg_derivative);
	defaults->update("VMPEG_QUANTIZATION", vmpeg_quantization);
	defaults->update("VMPEG_CMODEL", vmpeg_cmodel);
	defaults->update("VMPEG_FIX_BITRATE", vmpeg_fix_bitrate);



	defaults->update("DIVX_BITRATE", divx_bitrate);
	defaults->update("DIVX_RC_PERIOD", divx_rc_period);
	defaults->update("DIVX_RC_REACTION_RATIO", divx_rc_reaction_ratio);
	defaults->update("DIVX_RC_REACTION_PERIOD", divx_rc_reaction_period);
	defaults->update("DIVX_MAX_KEY_INTERVAL", divx_max_key_interval);
	defaults->update("DIVX_MAX_QUANTIZER", divx_max_quantizer);
	defaults->update("DIVX_MIN_QUANTIZER", divx_min_quantizer);
	defaults->update("DIVX_QUANTIZER", divx_quantizer);
	defaults->update("DIVX_QUALITY", divx_quality);
	defaults->update("DIVX_FIX_BITRATE", divx_fix_bitrate);


	defaults->update("MS_BITRATE", ms_bitrate);
	defaults->update("MS_QUANTIZATION", ms_quantization);
	defaults->update("MS_GOP_SIZE", ms_gop_size);
	defaults->update("MS_FIX_BITRATE", ms_fix_bitrate);

	defaults->update("PNG_USE_ALPHA", png_use_alpha);
}









int Asset::update_path(char *new_path)
{
	strcpy(path, new_path);
	return 0;
}

void Asset::update_index(Asset *asset)
{
//printf("Asset::update_index 1\n");
	index_status = asset->index_status;     // 0 ready  1 not tested  2 being built  3 small source
	index_zoom = asset->index_zoom; 	 // zoom factor of index data
	index_start = asset->index_start;	 // byte start of index data in the index file
	index_bytes = asset->index_bytes;	 // Total bytes in source file for comparison before rebuilding the index
	index_end = asset->index_end;
	old_index_end = asset->old_index_end;	 // values for index build
//printf("Asset::update_index 1\n");

	if(index_offsets)
	{
		delete [] index_offsets;
		index_offsets = 0;
	}
	
	if(asset->index_offsets)
	{
		index_offsets = new long[asset->channels];
//printf("Asset::update_index 1\n");

		int i;
		for(i = 0; i < asset->channels; i++)
		{
			index_offsets[i] = asset->index_offsets[i];  // offsets of channels in index file in floats

		}
	}
//printf("Asset::update_index 1\n");
	index_buffer = asset->index_buffer;    // pointer
//printf("Asset::update_index 2\n");
}


int Asset::dump()
{
	printf("  asset::dump\n");
	printf("   %p %s\n", this, path);
	printf("   index_status %d\n", index_status);
	printf("   format %d\n", format);
	printf("   audio_data %d channels %d samplerate %d bits %d byte_order %d signed %d header %d dither %d acodec %c%c%c%c\n",
		audio_data, channels, sample_rate, bits, byte_order, signed_, header, dither, acodec[0], acodec[1], acodec[2], acodec[3]);
	printf("   audio_length %ld\n", audio_length);
	printf("   video_data %d layers %d framerate %f width %d height %d vcodec %c%c%c%c\n",
		video_data, layers, frame_rate, width, height, vcodec[0], vcodec[1], vcodec[2], vcodec[3]);
	printf("   video_length %ld\n", video_length);
	return 0;
}
