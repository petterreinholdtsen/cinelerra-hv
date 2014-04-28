#include "arender.h"
#include "assets.h"
#include "auto.h"
#include "cache.h"
#include "clip.h"
#include "cwindow.h"
#include "cwindowgui.h"
#include "edit.h"
#include "edits.h"
#include "edl.h"
#include "edlsession.h"
#include "errorbox.h"
#include "file.h"
#include "filesystem.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include "packagerenderer.h"
#include "playabletracks.h"
#include "playbackconfig.h"
#include "pluginserver.h"
#include "preferences.h"
#include "render.h"
#include "renderengine.h"
#include "tracks.h"
#include "transportque.h"
#include "vedit.h"
#include "vframe.h"
#include "videodevice.h"
#include "vrender.h"







RenderPackage::RenderPackage()
{
	audio_start = 0;
	audio_end = 0;
	video_start = 0;
	video_end = 0;
	path[0] = 0;
	done = 0;
}

RenderPackage::~RenderPackage()
{
}








// Used by RenderFarm and in the future, Render, to do packages.
PackageRenderer::PackageRenderer()
{
}

PackageRenderer::~PackageRenderer()
{
	delete command;
	delete audio_cache;
	delete video_cache;
//	delete playback_config;
	delete vconfig;
}

int PackageRenderer::initialize(MWindow *mwindow,
		EDL *edl, 
		Preferences *preferences, 
		Asset *default_asset,
		ArrayList<PluginServer*> *plugindb)
{
	int result = 0;

//printf("PackageRenderer::initialize 1\n");
	this->mwindow = mwindow;
	this->edl = edl;
	this->preferences = preferences;
	this->default_asset = default_asset;
	this->plugindb = plugindb;
//printf("PackageRenderer::initialize 1\n");

	command = new TransportCommand;
	command->command = NORMAL_FWD;
	*command->get_edl() = *edl;
	command->change_type = CHANGE_ALL;
	command->set_playback_range();

	default_asset->frame_rate = command->get_edl()->session->frame_rate;
	default_asset->sample_rate = command->get_edl()->session->sample_rate;
	Render::check_asset(edl, *default_asset);
//printf("PackageRenderer::initialize 1 interpolation_type=%d\n",
//	command->get_edl()->session->interpolation_type);

	audio_cache = new CICache(command->get_edl(), plugindb);
	video_cache = new CICache(command->get_edl(), plugindb);
//printf("PackageRenderer::initialize 1\n");

	vconfig = new VideoOutConfig(PLAYBACK_LOCALHOST, 0);
//	playback_config = new PlaybackConfig(PLAYBACK_LOCALHOST, 0);
	for(int i = 0; i < MAX_CHANNELS; i++)
	{
		vconfig->do_channel[i] = (i < command->get_edl()->session->video_channels);
// 		playback_config->vconfig->do_channel[i] = (i < command->get_edl()->session->video_channels);
// 		playback_config->aconfig->do_channel[i] = (i < command->get_edl()->session->audio_channels);
	}

//printf("PackageRenderer::initialize 2\n");

	return result;
}

void PackageRenderer::create_output()
{
	FileSystem fs;
	asset = new Asset(*default_asset);



//printf("PackageRenderer::create_output 1\n");

	if(mwindow)
		strcpy(asset->path, package->path);
	else
		fs.join_names(asset->path, preferences->renderfarm_mountpoint, package->path);
//asset->dump();
	
//printf("PackageRenderer::create_output 2\n");
	file = new File;

//printf("PackageRenderer::create_output 3\n");
	file->set_processors(command->get_edl()->session->smp + 1);
//printf("PackageRenderer::create_output 4\n");

	result = file->open_file(plugindb, 
					asset, 
					0, 
					1, 
					command->get_edl()->session->sample_rate, 
					command->get_edl()->session->frame_rate);
//printf("PackageRenderer::create_output 5\n");

	if(result && mwindow)
	{
// open failed
		char string[BCTEXTLEN];
		sprintf(string, "Couldn't open %s", asset->path);
		ErrorBox error(PROGRAM_NAME ": Error",
			mwindow->gui->get_abs_cursor_x(),
			mwindow->gui->get_abs_cursor_y());
		error.create_objects(string);
		error.run_window();
	}
//printf("PackageRenderer::create_output 6\n");
}

void PackageRenderer::create_engine()
{
	int current_achannel = 0, current_vchannel = 0;
	audio_read_length = command->get_edl()->session->sample_rate;

//printf("PackageRenderer::create_engine 1\n");
	command->get_edl()->session->audio_module_fragment = 
		command->get_edl()->session->audio_read_length = 
		audio_read_length;


//printf("PackageRenderer::create_engine 1\n");
	render_engine = new RenderEngine(0,
		preferences,
		command,
		0,
		plugindb,
		0,
		0);
//printf("PackageRenderer::create_engine 1\n");
	render_engine->set_acache(audio_cache);
//printf("PackageRenderer::create_engine 1\n");
	render_engine->set_vcache(video_cache);
//printf("PackageRenderer::create_engine 1\n");
	render_engine->arm_command(command, current_achannel, current_vchannel);
//printf("PackageRenderer::create_engine 1\n");

	audio_preroll = Units::to_long(preferences->render_preroll * default_asset->sample_rate);
	video_preroll = Units::to_long(preferences->render_preroll * default_asset->frame_rate);
	audio_position = package->audio_start - audio_preroll;
	video_position = package->video_start - video_preroll;




// Create output buffers
	if(asset->audio_data)
	{
		file->start_audio_thread(audio_read_length, 
			command->get_edl()->session->smp ? 2 : 1);
	}


	if(asset->video_data)
	{
		compressed_output = new VFrame;
		video_write_length = command->get_edl()->session->smp + 1;
		video_write_position = 0;
		direct_frame_copying = 0;


		file->start_video_thread(video_write_length,
			command->get_edl()->session->color_model,
			command->get_edl()->session->smp ? 2 : 1,
			0);


		if(mwindow)
		{
			video_device = new VideoDevice;
 			video_device->open_output(vconfig, 
 				command->get_edl()->session->frame_rate, 
				command->get_edl()->session->output_w, 
				command->get_edl()->session->output_h, 
 				mwindow->cwindow->gui->canvas,
				0);
//printf("Render 14\n");
			video_device->start_playback();
		}
	}


	playable_tracks = new PlayableTracks(render_engine, 
		vconfig->do_channel, 
		video_position, 
		TRACK_VIDEO);

}




void PackageRenderer::do_audio()
{
// Do audio data
	if(asset->audio_data)
	{
		audio_output = file->get_audio_buffer();
//printf("PackageRenderer::do_audio 1\n");
// Zero unused channels in output vector
		for(int i = 0; i < MAX_CHANNELS; i++)
			audio_output_ptr[i] = (i < asset->channels) ? 
				audio_output[i] : 
				0;



//printf("PackageRenderer::do_audio 2 %d %d\n", audio_read_length, audio_position);

// Call render engine
		result = render_engine->arender->process_buffer(audio_output_ptr, 
			audio_read_length, 
			audio_position,
			0);

//printf("PackageRenderer::do_audio 3\n");


// Fix buffers for preroll
		long output_length = audio_read_length;
		if(audio_preroll > 0)
		{
			if(audio_preroll >= output_length)
				output_length = 0;
			else
			{
				output_length -= audio_preroll;
				for(int i = 0; i < MAX_CHANNELS; i++)
				{
					if(audio_output_ptr[i])
						for(int j = 0; j < output_length; j++)
						{
							audio_output_ptr[i][j] = audio_output_ptr[i][j + audio_read_length - output_length];
						}
				}
			}
//printf("PackageRenderer::do_audio 4\n");

			audio_preroll -= audio_read_length;
		}

// Must perform writes even if 0 length so get_audio_buffer doesn't block
		result |= file->write_audio_buffer(output_length);
//printf("PackageRenderer::do_audio 5\n");
	}

	audio_position += audio_read_length;
}


void PackageRenderer::do_video()
{
// Do video data
	if(asset->video_data)
	{
// get the absolute video position from the audio position
		long video_end = video_position + video_read_length;

		if(video_end > package->video_end)
			video_end = package->video_end;

//printf("PackageRenderer::do_video 1\n");
//	video_position, audio_position, video_end);
		while(video_position < video_end && !result)
		{
// Try to copy the compressed frame directly from the input to output files
//printf("PackageRenderer::do_video 2 video_position=%ld\n", video_position);
			if(direct_frame_copy(command->get_edl(), 
				video_position, 
				file, 
				result))
			{
// Direct frame copy failed.
//printf("PackageRenderer::do_video 3\n");
// Switch back to background compression
				if(direct_frame_copying)
				{
					file->start_video_thread(video_write_length, 
						command->get_edl()->session->color_model,
						command->get_edl()->session->smp ? 2 : 1,
						0);
					direct_frame_copying = 0;
				}

//printf("PackageRenderer::do_video 4\n");
// Try to use the rendering engine to write the frame.
// Get a buffer for background writing.



				if(video_write_position == 0)
					video_output = file->get_video_buffer();





//printf("PackageRenderer::do_video 5\n");
// Construct layered output buffer
				for(int i = 0; i < MAX_CHANNELS; i++)
					video_output_ptr[i] = 
						(i < asset->layers) ? 
							video_output[i][video_write_position] : 
							0;
//printf("PackageRenderer::do_video 6\n");
 				result |= render_engine->vrender->process_buffer(video_output_ptr, 
					video_position, 
					0);



 				if(mwindow && video_device->output_visible())
				{
// Vector for video device
					VFrame *preview_output[MAX_CHANNELS];

//printf("PackageRenderer::do_video 7\n");
					video_device->new_output_buffers(preview_output,
						command->get_edl()->session->color_model);
//printf("PackageRenderer::do_video 8\n");

					for(int i = 0; i < MAX_CHANNELS; i++)
						if(preview_output[i])
							preview_output[i]->copy_from(video_output_ptr[i]);
//printf("PackageRenderer::do_video 9\n");
					video_device->write_buffer(preview_output, 
						command->get_edl());
//printf("PackageRenderer::do_video 10\n");
				}


//printf("PackageRenderer::do_video 11 %d %d\n", video_write_position, video_preroll);

// Write to file
				if(video_preroll)
				{
					video_preroll--;
// Keep the write position at 0 until ready to write real frames
					result |= file->write_video_buffer(0);
					video_write_position = 0;
				}
				else
	 			{
					video_write_position++;
					if(video_write_position >= video_write_length)
					{
						result |= file->write_video_buffer(video_write_position);
						video_write_position = 0;
					}
				}


//printf("PackageRenderer::do_video 12\n");
			}

//printf("PackageRenderer::do_video 13 %d\n", result);
			video_position++;
			if(get_result()) result = 1;
			if(progress_cancelled()) result = 1;
		}
	}
	else
		video_position += video_read_length;
}


void PackageRenderer::stop_engine()
{

	render_engine->stop_playback();
	delete render_engine;
	delete playable_tracks;
}


void PackageRenderer::stop_output()
{
	if(asset->audio_data)
	{
// stop file I/O
		file->stop_audio_thread();
	}

	if(asset->video_data)
	{
		delete compressed_output;
		if(video_write_position)
			file->write_video_buffer(video_write_position);
		video_write_position = 0;	
		file->stop_video_thread();
		if(mwindow)
		{
			video_device->close_all();
			delete video_device;
		}
	}
}


void PackageRenderer::close_output()
{
	file->close_file();
	delete file;
	delete asset;
}

// Aborts and returns 1 if an error is encountered.
int PackageRenderer::render_package(RenderPackage *package)
{
	int audio_done = 0;
	int video_done = 0;
	int samples_rendered = 0;

	result = 0;
	this->package = package;

printf("PackageRenderer::render_package: "
	"audio=%d + %d video=%d + %d\n",
	package->audio_start, 
	package->audio_end - package->audio_start, 
	package->video_start, 
	package->video_end - package->video_start);
	
//printf("PackageRenderer::render_package 1\n");

	create_output();
//printf("PackageRenderer::render_package 1 %s\n", preferences->renderfarm_mountpoint);


// Create render engine
	if(!result)
	{
		create_engine();

// Main loop
		while((!audio_done || !video_done) && !result)
		{
//printf("PackageRenderer::render_package 5\n");
			int need_audio = 0, need_video = 0;




// Calculate lengths to process
			if(!audio_done)
			{
				if(audio_position + audio_read_length >= package->audio_end)
				{
					audio_done = 1;
					audio_read_length = package->audio_end - audio_position;
				}

				samples_rendered = audio_read_length;
				need_audio = 1;
			}

//printf("PackageRenderer::render_package 6 %d\n", samples_rendered);

			if(!video_done)
			{
				if(audio_done)
				{
					video_read_length = package->video_end - video_position;
					samples_rendered = Units::round((double)video_read_length /
						asset->frame_rate *
						asset->sample_rate);
				}
				else
// Guide video with audio
				{
					video_read_length = Units::to_long(
						(double)(audio_position + audio_read_length) / 
						asset->sample_rate * 
						asset->frame_rate) - 
						video_position;
				}

				if(video_position + video_read_length >= package->video_end)
				{
					video_done = 1;
					video_read_length = package->video_end - video_position;
				}

				need_video = 1;
			}

//printf("PackageRenderer::render_package 7 %d %d\n", video_read_length, audio_read_length);
			if(need_video) do_video();
//printf("PackageRenderer::render_package 8\n");
			if(need_audio) do_audio();
//printf("PackageRenderer::render_package 9\n");


			set_progress(samples_rendered);

//printf("PackageRenderer::render_package 10 %d %d %d\n", audio_read_length, video_read_length, samples_rendered);




			if(progress_cancelled()) result = 1;

			if(result) 
				set_result(result);
			else
				result = get_result();
//printf("PackageRenderer::render_package 11 %d %d %d\n", audio_read_length, video_read_length, samples_rendered);
		}
//printf("PackageRenderer::render_package 12\n");

		stop_engine();


		stop_output();



//printf("PackageRenderer::render_package 11 %d\n", result);
	}



	close_output();


	set_result(result);

//printf("PackageRenderer::render_package 12 %d\n", result);


	return result;
}








// Try to copy the compressed frame directly from the input to output files
// Return 1 on failure and 0 on success
int PackageRenderer::direct_frame_copy(EDL *edl, 
	long &video_position, 
	File *file,
	int &error)
{
	Track *playable_track;
	Edit *playable_edit;
	long frame_size;
	int result = 0;

//printf("Render::direct_frame_copy 1\n");
	if(direct_copy_possible(edl, 
		video_position, 
		playable_track, 
		playable_edit, 
		file))
	{
// Switch to direct copying
		if(!direct_frame_copying)
		{
			if(video_write_position)
			{
				error |= file->write_video_buffer(video_write_position);
				video_write_position = 0;
			}
			file->stop_video_thread();
			direct_frame_copying = 1;
		}
//printf("Render::direct_frame_copy 2\n");

		error |= ((VEdit*)playable_edit)->read_frame(compressed_output, 
			video_position,
			PLAY_FORWARD,
			video_cache);

//printf("Render::direct_frame_copy 3 %d\n", compressed_output->get_compressed_size());

		if(!error && video_preroll > 0)
			video_preroll--;
		else
		if(!error)
		{
			VFrame ***temp_output = new VFrame**[1];
			temp_output[0] = new VFrame*[1];
			temp_output[0][0] = compressed_output;
//printf("Render::direct_frame_copy 4\n");
			error |= file->write_frames(temp_output, 1);
			delete temp_output[0];
			delete temp_output;
		}
//printf("Render::direct_frame_copy 4\n");
		return 0;
	}
	else
		return 1;
}

int PackageRenderer::direct_copy_possible(EDL *edl,
				long current_position, 
				Track* playable_track,  // The one track which is playable
				Edit* &playable_edit, // The edit which is playing
				File *file)   // Output file
{
	int result = 1;
	int total_playable_tracks = 0;
	Track* current_track;
	Patch* current_patch;
	Auto* current_auto;
	int temp;

// Number of playable tracks must equal 1
	for(current_track = edl->tracks->first;
		current_track && result; 
		current_track = current_track->next)
	{
		if(current_track->data_type == TRACK_VIDEO)
		{
			if(playable_tracks->is_playable(current_track, current_position))
			{
				playable_track = current_track;
				total_playable_tracks++;
			}
		}
	}

//printf("Render::direct_copy_possible 1 %d\n", result);
	if(total_playable_tracks != 1) result = 0;
//printf("Render::direct_copy_possible 2 %d\n", result);

// Edit must have a source file
	if(result)
	{
//printf("Render::direct_copy_possible 3 %d\n", result);
		playable_edit = playable_track->edits->get_playable_edit(current_position);
//printf("Render::direct_copy_possible 4 %d %p\n", result, playable_edit);
		if(!playable_edit)
			result = 0;
	}

// Source file must be able to copy to destination file.
// Source file must be same size as project output.
	if(result)
	{
//printf("Render::direct_copy_possible 5 %d\n", result);
		if(!file->can_copy_from(playable_edit, 
			current_position,
			edl->session->output_w, 
			edl->session->output_h))
			result = 0;
	}
//printf("Render::direct_copy_possible 6 %d\n", result);

// Test conditions mutual between vrender.C and this.
	if(result && 
		!playable_track->direct_copy_possible(current_position, PLAY_FORWARD))
		result = 0;
//printf("Render::direct_copy_possible 7 %d\n", result);

	return result;
}











// Get result status from server
int PackageRenderer::get_result()
{
	return 0;
}

void PackageRenderer::set_result(int value)
{
}

void PackageRenderer::set_progress(long value)
{
}	
	
int PackageRenderer::progress_cancelled()
{
	return 0;
}	



























PackageDispatcher::PackageDispatcher()
{
	packages = 0;
	package_lock = new Mutex;
}

PackageDispatcher::~PackageDispatcher()
{
	if(packages) delete [] packages;
	delete package_lock;
}

int PackageDispatcher::create_packages(MWindow *mwindow,
	EDL *edl,
	Preferences *preferences,
	int strategy, 
	Asset *default_asset, 
	double total_start, 
	double total_end)
{
	int result = 0;

	this->mwindow = mwindow;
	this->edl = edl;
	this->preferences = preferences;
	this->strategy = strategy;
	this->default_asset = default_asset;
	this->total_start = total_start;
	this->total_end = total_end;

	nodes = preferences->get_enabled_nodes();
	audio_position = Units::to_long(total_start * default_asset->sample_rate);
	video_position = Units::to_long(total_start * default_asset->frame_rate);
	audio_end = Units::to_long(total_end * default_asset->sample_rate);
	video_end = Units::to_long(total_end * default_asset->frame_rate);
	current_package = 0;

	if(strategy == SINGLE_PASS)
	{
		total_len = this->total_end - this->total_start;
		package_len = total_len;
		total_packages = 1;
		total_allocated = 1;
		packages = new RenderPackage[total_allocated];
		packages[0].audio_start = audio_position;
		packages[0].audio_end = audio_end;
		packages[0].video_start = video_position;
		packages[0].video_end = video_end;
		strcpy(packages[0].path, default_asset->path);
	}
	else
	if(strategy == SINGLE_PASS_FARM)
	{
		total_len = this->total_end - this->total_start;
		total_packages = preferences->renderfarm_job_count;
		total_allocated = total_packages + nodes;
		packages = new RenderPackage[total_allocated];
		package_len = total_len / total_packages;


//printf("PackageDispatcher::create_packages: %f / %d = %f\n", total_len, total_packages, package_len);
		Render::get_starting_number(default_asset->path, 
			current_number,
			number_start, 
			total_digits);

		for(int i = 0; i < total_allocated; i++)
		{
			RenderPackage *package = &packages[i];

			Render::create_filename(package->path, 
				default_asset->path, 
				current_number,
				total_digits,
				number_start);
			current_number++;
		}
	}
	else
	if(strategy == FILE_PER_LABEL || strategy == FILE_PER_LABEL_FARM)
	{
		Label *label = edl->labels->first;
		total_packages = 0;
		packages = new RenderPackage[edl->labels->total() + 2];

		Render::get_starting_number(default_asset->path, 
			current_number,
			number_start, 
			total_digits);

		while(audio_position < audio_end)
		{
			RenderPackage *package = &packages[total_packages];
			package->audio_start = audio_position;
			package->video_start = video_position;


			while(label && 
				(label->position < (double)audio_position / default_asset->sample_rate ||
				EQUIV(label->position, (double)audio_position / default_asset->sample_rate)))
			{
				label = label->next;
			}

			if(!label)
			{
				package->audio_end = Units::to_long(total_end * default_asset->sample_rate);
				package->video_end = Units::to_long(total_end * default_asset->frame_rate);
			}
			else
			{
				package->audio_end = Units::to_long(label->position * default_asset->sample_rate);
				package->video_end = Units::to_long(label->position * default_asset->frame_rate);
			}

			if(package->audio_end > audio_end)
			{
				package->audio_end = audio_end;
			}

			if(package->video_end > video_end)
			{
				package->video_end = video_end;
			}

			audio_position = package->audio_end;
			video_position = package->video_end;
			Render::create_filename(package->path, 
				default_asset->path, 
				current_number,
				total_digits,
				number_start);
			current_number++;

			total_packages++;
		}
		
		total_allocated = total_packages;
	}

// Test existence of every output file
	for(int i = 0; i < total_allocated && !result; i++)
	{
		Asset temp_asset(packages[i].path);
//printf("Render::run 1 %s\n", packages[i].path);
		result = Render::test_existence(mwindow, &temp_asset);
	}
	
	return result;
}

RenderPackage* PackageDispatcher::get_package(double frames_per_second, 
	double avg_frames_per_second)
{
	package_lock->lock();

	RenderPackage *result = 0;
	if(strategy == SINGLE_PASS ||
		strategy == FILE_PER_LABEL ||
		strategy == FILE_PER_LABEL_FARM)
	{
		if(current_package < total_packages)
		{
			result = &packages[current_package];
			current_package++;
		}
	}
	else
	if(strategy == SINGLE_PASS_FARM)
	{
		if(audio_position < audio_end ||
			video_position < video_end)
		{
// Last package
			if(current_package >= total_allocated - 1)
			{
				result = &packages[current_package];
				result->audio_start = audio_position;
				result->video_start = video_position;
				result->audio_end = audio_end;
				result->video_end = video_end;
				audio_position = result->audio_end;
				video_position = result->video_end;
				current_package++;
			}
			else
// No useful speed data
			if(EQUIV(frames_per_second, 0) || 
				EQUIV(avg_frames_per_second, 0))
			{
				result = &packages[current_package];

				result->audio_start = audio_position;
				result->video_start = video_position;
				result->audio_end = audio_position + 
					Units::round(package_len * default_asset->sample_rate);
				result->video_end = video_position + 
					Units::round(package_len * default_asset->frame_rate);

// printf("Dispatcher::get_package 1 %f %d = %f\n", 
// 	package_len, default_asset->sample_rate, package_len * default_asset->sample_rate);

// If we get here without any useful speed data render the whole thing.
				if(current_package >= total_packages - 1)
				{
					result->audio_end = audio_end;
					result->video_end = video_end;
				}
				else
				{
					result->audio_end = MIN(audio_end, result->audio_end);
					result->video_end = MIN(video_end, result->video_end);
				}

				audio_position = result->audio_end;
				video_position = result->video_end;
				current_package++;
			}
			else
// Useful speed data and future packages exist.  Scale the 
// package size to fit the requestor.
			{
				result = &packages[current_package];

				result->audio_start = audio_position;
				result->video_start = video_position;


				double scaled_len = package_len * 
					frames_per_second / 
					avg_frames_per_second;
				result->audio_end = result->audio_start + 
					Units::to_long(scaled_len * default_asset->sample_rate);
				result->video_end = result->video_start +
					Units::to_long(scaled_len * default_asset->frame_rate);

				result->audio_end = MIN(audio_end, result->audio_end);
				result->video_end = MIN(video_end, result->video_end);

				audio_position = result->audio_end;
				video_position = result->video_end;

// printf("Dispatcher::get_package 2 %f * %f / %f = %f\n", 
// 	package_len, 
// 	frames_per_second, 
// 	avg_frames_per_second,
// 	scaled_len);

// Package size is no longer touched between total_packages and total_allocated
				if(current_package < total_packages - 1)
				{
					package_len = (double)(audio_end - audio_position) / 
						(double)default_asset->sample_rate /
						(double)(total_packages - current_package);
				}

				current_package++;
			}
//printf("Dispatcher::get_package 2 %d %d %d %d\n", 
//	result->audio_start, result->video_start, result->audio_end, result->video_end);
		}
	}
	
	package_lock->unlock();
	
	return result;
}


ArrayList<Asset*>* PackageDispatcher::get_asset_list()
{
	ArrayList<Asset*> *assets = new ArrayList<Asset*>;
	
	for(int i = 0; i < current_package; i++)
	{
		Asset *asset = new Asset;
		*asset = *default_asset;
		strcpy(asset->path, packages[i].path);
		asset->video_length = packages[i].video_end - packages[i].video_start;
		asset->audio_length = packages[i].audio_end - packages[i].audio_start;
		assets->append(asset);
	}

	return assets;
}

