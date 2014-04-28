#include "affine.h"
#include "bcdisplayinfo.h"
#include "clip.h"
#include "defaults.h"
#include "filexml.h"
#include "keyframe.h"
#include "language.h"
#include "motion.h"
#include "motionwindow.h"
#include "overlayframe.h"
#include "picon_png.h"
#include "rotateframe.h"
#include "transportque.h"


#include <errno.h>
#include <unistd.h>

REGISTER_PLUGIN(MotionMain)

static void sort(int *array, int total)
{
	int done = 0;
	while(!done)
	{
		done = 1;
		for(int i = 0; i < total - 1; i++)
		{
			if(array[i] > array[i + 1])
			{
				array[i] ^= array[i + 1];
				array[i + 1] ^= array[i];
				array[i] ^= array[i + 1];
				done = 0;
			}
		}
	}
}



MotionConfig::MotionConfig()
{
	global_range = 5;
	rotation_range = 5;
	block_count = 1;
	block_size = MAX_BLOCK;
	block_x = 50;
	block_y = 50;
	global_positions = 256;
	rotate_positions = 4;
	magnitude = 100;
	return_speed = 0;
	mode1 = STABILIZE;
	global = 1;
	rotate = 1;
	mode3 = NO_CALCULATE;
	draw_vectors = 1;
	tracksingle = 0;
	track_frame = 0;
	bottom_is_master = 1;
}

void MotionConfig::boundaries()
{
	CLAMP(global_range, MIN_RADIUS, MAX_RADIUS);
	CLAMP(rotation_range, MIN_ROTATION, MAX_ROTATION);
	CLAMP(block_count, MIN_BLOCKS, MAX_BLOCKS);
	CLAMP(block_size, MIN_BLOCK, MAX_BLOCK);
}

int MotionConfig::equivalent(MotionConfig &that)
{
	return global_range == that.global_range &&
		rotation_range == that.rotation_range &&
		mode1 == that.mode1 &&
		global == that.global &&
		rotate == that.rotate &&
		draw_vectors == that.draw_vectors &&
		block_count == that.block_count &&
		block_size == that.block_size &&
		EQUIV(block_x, that.block_x) &&
		EQUIV(block_y, that.block_y) &&
		global_positions == that.global_positions &&
		rotate_positions == that.rotate_positions &&
		magnitude == that.magnitude &&
		return_speed == that.return_speed &&
		tracksingle == that.tracksingle &&
		track_frame == that.track_frame &&
		bottom_is_master == that.bottom_is_master;
}

void MotionConfig::copy_from(MotionConfig &that)
{
	global_range = that.global_range;
	rotation_range = that.rotation_range;
	mode1 = that.mode1;
	global = that.global;
	rotate = that.rotate;
	mode3 = that.mode3;
	draw_vectors = that.draw_vectors;
	block_count = that.block_count;
	block_x = that.block_x;
	block_y = that.block_y;
	global_positions = that.global_positions;
	rotate_positions = that.rotate_positions;
	block_size = that.block_size;
	magnitude = that.magnitude;
	return_speed = that.return_speed;
	tracksingle = that.tracksingle;
	track_frame = that.track_frame;
	bottom_is_master = that.bottom_is_master;
}

void MotionConfig::interpolate(MotionConfig &prev, 
	MotionConfig &next, 
	int64_t prev_frame, 
	int64_t next_frame, 
	int64_t current_frame)
{
	copy_from(prev);
}



















MotionMain::MotionMain(PluginServer *server)
 : PluginVClient(server)
{
	PLUGIN_CONSTRUCTOR_MACRO
	engine = 0;
	match_frame = 0;
	rotate_engine = 0;
	motion_rotate = 0;
	current_dx = 0;
	current_dy = 0;
	total_dx = 0;
	total_dy = 0;
	current_angle = 0;
	total_angle = 0;
	overlayer = 0;
	search_area = 0;
	search_size = 0;
	temp_frame = 0;
	match_frame_number = -1;
}

MotionMain::~MotionMain()
{
	PLUGIN_DESTRUCTOR_MACRO
	delete engine;
	delete match_frame;
	delete overlayer;
	delete [] search_area;
	delete temp_frame;
	delete rotate_engine;
	delete motion_rotate;
}

char* MotionMain::plugin_title() { return N_("Motion"); }
int MotionMain::is_realtime() { return 1; }
int MotionMain::is_multichannel() { return 1; }

NEW_PICON_MACRO(MotionMain)

SHOW_GUI_MACRO(MotionMain, MotionThread)

SET_STRING_MACRO(MotionMain)

RAISE_WINDOW_MACRO(MotionMain)

LOAD_CONFIGURATION_MACRO(MotionMain, MotionConfig)



void MotionMain::update_gui()
{
	if(thread)
	{
		if(load_configuration())
		{
			thread->window->lock_window("MotionMain::update_gui");
			
			char string[BCTEXTLEN];
//			sprintf(string, "%d", config.block_count);
//			thread->window->block_count->set_text(string);

			if(config.global)
				sprintf(string, "%d", config.global_positions);
			else
				sprintf(string, "%d", config.rotate_positions);
			thread->window->search_positions->create_objects();
			thread->window->search_positions->set_text(string);


			thread->window->block_size->update(config.block_size);
			thread->window->block_x->update(config.block_x);
			thread->window->block_y->update(config.block_y);
			thread->window->block_x_text->update(config.block_x);
			thread->window->block_y_text->update(config.block_y);
			thread->window->magnitude->update(config.magnitude);
			thread->window->return_speed->update(config.return_speed);


			thread->window->track_single->update(config.tracksingle);
			thread->window->track_frame_number->update(config.track_frame);
			thread->window->track_previous->update(!config.tracksingle);
			if(!config.tracksingle) 
				thread->window->track_frame_number->disable();
			else
				thread->window->track_frame_number->enable();

			thread->window->master_layer_top->update(!config.bottom_is_master);
			thread->window->master_layer_bottom->update(config.bottom_is_master);


			thread->window->update_mode();
			thread->window->unlock_window();
		}
	}
}


int MotionMain::load_defaults()
{
	char directory[BCTEXTLEN], string[BCTEXTLEN];
// set the default directory
	sprintf(directory, "%smotion.rc", BCASTDIR);

// load the defaults
	defaults = new Defaults(directory);
	defaults->load();

	config.block_count = defaults->get("BLOCK_COUNT", config.block_count);
	config.global_positions = defaults->get("GLOBAL_POSITIONS", config.global_positions);
	config.rotate_positions = defaults->get("ROTATE_POSITIONS", config.rotate_positions);
	config.block_size = defaults->get("BLOCK_SIZE", config.block_size);
	config.block_x = defaults->get("BLOCK_X", config.block_x);
	config.block_y = defaults->get("BLOCK_Y", config.block_y);
	config.global_range = defaults->get("GLOBAL_RANGE", config.global_range);
	config.rotation_range = defaults->get("ROTATION_RANGE", config.rotation_range);
	config.magnitude = defaults->get("MAGNITUDE", config.magnitude);
	config.return_speed = defaults->get("RETURN_SPEED", config.return_speed);
	config.mode1 = defaults->get("MODE1", config.mode1);
	config.global = defaults->get("GLOBAL", config.global);
	config.rotate = defaults->get("ROTATE", config.rotate);
	config.mode3 = defaults->get("MODE3", config.mode3);
	config.draw_vectors = defaults->get("DRAW_VECTORS", config.draw_vectors);
	config.tracksingle = defaults->get("TRACK_SINGLE", config.tracksingle);
	config.track_frame = defaults->get("TRACK_FRAME", config.track_frame);
	config.bottom_is_master = defaults->get("BOTTOM_IS_MASTER", config.bottom_is_master);
	config.boundaries();
	return 0;
}


int MotionMain::save_defaults()
{
	defaults->update("BLOCK_COUNT", config.block_count);
	defaults->update("GLOBAL_POSITIONS", config.global_positions);
	defaults->update("ROTATE_POSITIONS", config.rotate_positions);
	defaults->update("BLOCK_SIZE", config.block_size);
	defaults->update("BLOCK_X", config.block_x);
	defaults->update("BLOCK_Y", config.block_y);
	defaults->update("GLOBAL_RANGE", config.global_range);
	defaults->update("ROTATION_RANGE", config.rotation_range);
	defaults->update("MAGNITUDE", config.magnitude);
	defaults->update("RETURN_SPEED", config.return_speed);
	defaults->update("MODE1", config.mode1);
	defaults->update("GLOBAL", config.global);
	defaults->update("ROTATE", config.rotate);
	defaults->update("MODE3", config.mode3);
	defaults->update("DRAW_VECTORS", config.draw_vectors);
	defaults->update("TRACK_SINGLE", config.tracksingle);
	defaults->update("TRACK_FRAME", config.track_frame);
	defaults->update("BOTTOM_IS_MASTER", config.bottom_is_master);
	defaults->save();
	return 0;
}



void MotionMain::save_data(KeyFrame *keyframe)
{
	FileXML output;

// cause data to be stored directly in text
	output.set_shared_string(keyframe->data, MESSAGESIZE);
	output.tag.set_title("MOTION");

	output.tag.set_property("BLOCK_COUNT", config.block_count);
	output.tag.set_property("GLOBAL_POSITIONS", config.global_positions);
	output.tag.set_property("ROTATE_POSITIONS", config.rotate_positions);
	output.tag.set_property("BLOCK_SIZE", config.block_size);
	output.tag.set_property("BLOCK_X", config.block_x);
	output.tag.set_property("BLOCK_Y", config.block_y);
	output.tag.set_property("GLOBAL_RANGE", config.global_range);
	output.tag.set_property("ROTATION_RANGE", config.rotation_range);
	output.tag.set_property("MAGNITUDE", config.magnitude);
	output.tag.set_property("RETURN_SPEED", config.return_speed);
	output.tag.set_property("MODE1", config.mode1);
	output.tag.set_property("GLOBAL", config.global);
	output.tag.set_property("ROTATE", config.rotate);
	output.tag.set_property("MODE3", config.mode3);
	output.tag.set_property("DRAW_VECTORS", config.draw_vectors);
	output.tag.set_property("TRACK_SINGLE", config.tracksingle);
	output.tag.set_property("TRACK_FRAME", config.track_frame);
	output.tag.set_property("BOTTOM_IS_MASTER", config.bottom_is_master);
	output.append_tag();
	output.terminate_string();
}

void MotionMain::read_data(KeyFrame *keyframe)
{
	FileXML input;

	input.set_shared_string(keyframe->data, strlen(keyframe->data));

	int result = 0;

	while(!result)
	{
		result = input.read_tag();

		if(!result)
		{
			if(input.tag.title_is("MOTION"))
			{
				config.block_count = input.tag.get_property("BLOCK_COUNT", config.block_count);
				config.global_positions = input.tag.get_property("GLOBAL_POSITIONS", config.global_positions);
				config.rotate_positions = input.tag.get_property("ROTATE_POSITIONS", config.rotate_positions);
				config.block_size = input.tag.get_property("BLOCK_SIZE", config.block_size);
				config.block_x = input.tag.get_property("BLOCK_X", config.block_x);
				config.block_y = input.tag.get_property("BLOCK_Y", config.block_y);
				config.global_range = input.tag.get_property("GLOBAL_RANGE", config.global_range);
				config.rotation_range = input.tag.get_property("ROTATION_RANGE", config.rotation_range);
				config.magnitude = input.tag.get_property("MAGNITUDE", config.magnitude);
				config.return_speed = input.tag.get_property("RETURN_SPEED", config.return_speed);
				config.mode1 = input.tag.get_property("MODE1", config.mode1);
				config.global = input.tag.get_property("GLOBAL", config.global);
				config.rotate = input.tag.get_property("ROTATE", config.rotate);
				config.mode3 = input.tag.get_property("MODE3", config.mode3);
				config.draw_vectors = input.tag.get_property("DRAW_VECTORS", config.draw_vectors);
				config.tracksingle = input.tag.get_property("TRACK_SINGLE", config.tracksingle);
				config.track_frame = input.tag.get_property("TRACK_FRAME", config.track_frame);
				config.bottom_is_master = input.tag.get_property("BOTTOM_IS_MASTER", config.bottom_is_master);
			}
		}
	}
	config.boundaries();
}








void MotionMain::remove_error(int use_quadrants)
{
	if(engine->get_total_packages() == 1) return;

// Get median derivative for all quadrants and global.
// quadrant 4 is the global value
	int median_dx[5] = { 0, 0, 0, 0, 0 };
	int median_dy[5] = { 0, 0, 0, 0, 0 };
	int avg_dx[5] = { 0, 0, 0, 0, 0 };
	int avg_dy[5] = { 0, 0, 0, 0, 0 };
	int dx_sorted[5][engine->get_total_packages()];
	int dy_sorted[5][engine->get_total_packages()];
	int total[5] = { 0, 0, 0, 0, 0 };

	for(int i = 0; i < engine->get_total_packages(); i++)
	{
		MotionPackage *pkg = (MotionPackage*)engine->get_package(i);
		if(pkg->valid)
		{
			int quadrant = pkg->get_quadrant(master_frame->get_w(),
				master_frame->get_h());
			dx_sorted[quadrant][total[quadrant]] = 
				dx_sorted[4][total[4]] = 
				pkg->dx;
			dy_sorted[quadrant][total[quadrant]] =
				dy_sorted[4][total[4]] =
				pkg->dy;
			avg_dx[quadrant] += pkg->dx;
			avg_dy[quadrant] += pkg->dy;
			avg_dx[4] += pkg->dx;
			avg_dy[4] += pkg->dy;
			total[quadrant]++;
			total[4]++;
		}
	}


	for(int i = 0; i < 5; i++)
	{
		if(total[i])
		{
			sort(dx_sorted[i], total[i]);
			sort(dy_sorted[i], total[i]);
			median_dx[i] = dx_sorted[i][total[i] / 2];
			median_dy[i] = dy_sorted[i][total[i] / 2];
			avg_dx[i] /= total[i];
			avg_dy[i] /= total[i];
		}
	}


// Throw out vectors too far from median derivative
	for(int i = 0; i < engine->get_total_packages(); i++)
	{
		MotionPackage *pkg = (MotionPackage*)engine->get_package(i);
		int quadrant = pkg->get_quadrant(master_frame->get_w(),
			master_frame->get_h());

		if(!use_quadrants)
		{
// Use average
// 			int dx_limit = abs(avg_dx[4] * 2);
// 			int dy_limit = abs(avg_dy[4] * 2);
// 			if(abs(pkg->dx - avg_dx[4]) > dx_limit ||
// 				abs(pkg->dy - avg_dy[4]) > dy_limit)
// 			{
// 				pkg->valid = 0;
// 			}

// Use median
			int dx_limit = abs(median_dx[4]) * 2;
			int dy_limit = abs(median_dy[4]) * 2;
			if(abs(pkg->dx - median_dx[4]) > dx_limit ||
				abs(pkg->dy - median_dy[4]) > dy_limit)
			{
				pkg->valid = 0;
			}
		}
		else
		{
			int dx_limit = abs(avg_dx[quadrant] * 2);
			int dy_limit = abs(avg_dy[quadrant] * 2);
			if(abs(pkg->dx - avg_dx[quadrant]) > dx_limit ||
				abs(pkg->dy - avg_dy[quadrant]) > dy_limit)
			{
				pkg->valid = 0;
			}
// 			int dx_limit = abs(median_dx[quadrant]) * 2;
// 			int dy_limit = abs(median_dy[quadrant]) * 2;
// 			if(abs(pkg->dx - median_dx[quadrant]) > dx_limit ||
// 				abs(pkg->dy - m`edian_dy[quadrant]) > dy_limit)
// 			{
// 				pkg->valid = 0;
// 			}
		}
	}
}







void MotionMain::calculate_global()
{
	current_dx = 0;
	current_dy = 0;
	int total = 0;

	remove_error(0);
// printf("MotionEngine::scan_frame 2 avg_dx=%d avg_dy=%d median_dx=%d median_dy=%d\n", 
// avg_dx,
// avg_dy,
// median_dx,
// median_dy);



	for(int i = 0; i < engine->get_total_packages(); i++)
	{
		MotionPackage *pkg = (MotionPackage*)engine->get_package(i);
		if(pkg->valid)
		{
			current_dx += pkg->dx;
			current_dy += pkg->dy;
			total++;
		}
	}

	if(total)
	{
		current_dx /= total;
		current_dy /= total;
	}
	else
	{
		current_dx = 0;
		current_dy = 0;
	}

	switch(config.mode1)
	{
		case MotionConfig::TRACK:
		case MotionConfig::TRACK_PIXEL:
			total_dx += current_dx;
			total_dy += current_dy;
			break;
		case MotionConfig::STABILIZE:
		case MotionConfig::STABILIZE_PIXEL:
			total_dx -= current_dx;
			total_dy -= current_dy;
			current_dx *= -1;
			current_dy *= -1;
			break;
	}

// Return total to center over time
	total_dx = total_dx * (100 - config.return_speed) / 100;
	total_dy = total_dy * (100 - config.return_speed) / 100;
// Clamp totals
	total_dx = MIN(total_dx,
		(int64_t)master_frame->get_w() * 
			OVERSAMPLE * 
			config.magnitude / 
			100);
	total_dy = MIN(total_dy,
		(int64_t)master_frame->get_h() * 
			OVERSAMPLE * 
			config.magnitude / 
			100);
}


void MotionMain::allocate_temp(int w, int h, int color_model)
{
	if(temp_frame && 
		(temp_frame->get_w() != w ||
		temp_frame->get_h() != h))
	{
		delete temp_frame;
		temp_frame = 0;
	}
	if(!temp_frame)
		temp_frame = new VFrame(0, w, h, color_model);
}



void MotionMain::process_global(VFrame **frame)
{
	if(!engine) engine = new MotionEngine(this,
		PluginClient::get_project_smp() + 1,
		PluginClient::get_project_smp() + 1);
printf("MotionMain::process_global 1 %d\n", get_source_position());

	engine->scan_frame(match_frame,
		frame[master_layer]);

	calculate_global();



// Get src and dst for target frame
	VFrame *src, *dst;
// Shift output frame and discard.  Leave match frame
	if(config.tracksingle)
	{
		allocate_temp(target_frame->get_w(), 
			target_frame->get_h(),
			target_frame->get_color_model());
		src = temp_frame;
		dst = target_frame;

		if(master_layer == target_layer)
			temp_frame->copy_from(master_frame);
		else
			read_frame(temp_frame,
				target_layer,
				get_source_position(),
				get_framerate());
	}
	else
// Make output frame match frame and then shift the target frame by the total distance
	{
		match_frame->copy_from(master_frame);
		match_frame_number = get_source_position();
		if(master_layer == target_layer)
		{
			src = match_frame;
			dst = target_frame;
		}
		else
		{
			allocate_temp(target_frame->get_w(), 
				target_frame->get_h(),
				target_frame->get_color_model());
			src = temp_frame;
			dst = target_frame;
			read_frame(temp_frame,
				target_layer,
				get_source_position(),
				get_framerate());
		}
	}

// Draw vectors and quit
	if(config.mode1 == MotionConfig::NOTHING)
	{
		dst->copy_from(src);
		if(config.draw_vectors) draw_global_vectors(dst);
		return;
	}

	if(!overlayer) overlayer = new OverlayFrame(PluginClient::get_project_smp() + 1);

	int dx;
	int dy;
	if(config.tracksingle)
	{
		dx = current_dx;
		dy = current_dy;
	}
	else
	{
		dx = total_dx;
		dy = total_dy;
	}



printf("MotionMain::process_global 2 %d %d\n", dx, dy);
	if(config.mode1 == MotionConfig::TRACK_PIXEL ||
		config.mode1 == MotionConfig::STABILIZE_PIXEL)
	{
		overlayer->overlay(dst,
			src,
			0,
			0,
			src->get_w(),
			src->get_h(),
			(int)(dx / OVERSAMPLE),
			(int)(dy / OVERSAMPLE),
			(int)src->get_w() + (int)(dx / OVERSAMPLE),
			(int)src->get_h() + (int)(dy / OVERSAMPLE),
			1,
			TRANSFER_REPLACE,
			CUBIC_LINEAR);
	}
	else
	{
		overlayer->overlay(dst,
			src,
			0,
			0,
			src->get_w(),
			src->get_h(),
			(float)dx / OVERSAMPLE,
			(float)dy / OVERSAMPLE,
			(float)src->get_w() + (float)dx / OVERSAMPLE,
			(float)src->get_h() + (float)dy / OVERSAMPLE,
			1,
			TRANSFER_REPLACE,
			CUBIC_LINEAR);
	}


	if(config.draw_vectors)
	{
printf("MotionMain::process_global %p %p\n", dst, frame[0]);
		draw_global_vectors(dst);
	}
}



void MotionMain::process_rotation(VFrame **frame)
{
printf("MotionMain::process_rotation 1 frame=%d\n", get_source_position());
	if(!motion_rotate)
		motion_rotate = new MotionRotateEngine(this, 
			get_project_smp() + 1, 
			get_project_smp() + 1);

// Calculate reference frame and current frame for the rotation tracker
	VFrame *src, *dst;
	if(config.global)
	{
		int dx;
		int dy;
		if(config.tracksingle)
		{
			dx = current_dx;
			dy = current_dy;
		}
		else
		{
			dx = total_dx;
			dy = total_dy;
		}

		allocate_temp(match_frame->get_w(), 
			match_frame->get_h(),
			match_frame->get_color_model());
		overlayer->overlay(temp_frame,
			match_frame,
			0,
			0,
			match_frame->get_w(),
			match_frame->get_h(),
			(float)dx / OVERSAMPLE,
			(float)dy / OVERSAMPLE,
			(float)master_frame->get_w() + (float)dx / OVERSAMPLE,
			(float)master_frame->get_h() + (float)dy / OVERSAMPLE,
			1,
			TRANSFER_REPLACE,
			CUBIC_LINEAR);
		src = temp_frame;
		dst = master_frame;
	}
	else
	{
		src = match_frame;
		dst = master_frame;
	}




// Get rotation
	if(!rotate_engine)
		motion_rotate = new MotionRotateEngine(this, 
			get_project_smp() + 1, 
			get_project_smp() + 1);

	current_angle = motion_rotate->scan_frame(src, dst);

// Accumulate angle
	switch(config.mode1)
	{
		case MotionConfig::TRACK:
		case MotionConfig::TRACK_PIXEL:
			total_angle += current_angle;
			break;
		case MotionConfig::STABILIZE:
		case MotionConfig::STABILIZE_PIXEL:
			total_angle -= current_angle;
			current_angle *= -1;
			break;
	}



// Get source and destination frame for rotation
	allocate_temp(target_frame->get_w(), 
		target_frame->get_h(),
		target_frame->get_color_model());
	src = temp_frame;
	dst = target_frame;

	if(config.global)
	{
// Use result from global as source
		temp_frame->copy_from(dst);
	}
	else
	{
// Read target layer
		if(master_layer == target_layer)
			temp_frame->copy_from(master_frame);
		else
			read_frame(temp_frame,
				target_layer,
				get_source_position(),
				get_framerate());
	}

// Abort
	if(config.mode1 == MotionConfig::NOTHING)
	{
		dst->copy_from(src);
		if(config.draw_vectors)
		{
			draw_rotation_vectors(dst);
		}
		return;
	}

// Rotate target frame
	float angle;
	if(config.tracksingle)
	{
		angle = current_angle;
	}
	else
	{
		angle = total_angle;
		match_frame_number = get_source_position();
	}

	if(!rotate_engine)
		rotate_engine = new AffineEngine(PluginClient::get_project_smp() + 1,
			PluginClient::get_project_smp() + 1);

	rotate_engine->set_pivot((int)(config.block_x * dst->get_w() / 100),
		(int)(config.block_y * dst->get_h() / 100));
	rotate_engine->rotate(dst, 
		src, 
		angle);

printf("MotionMain::process_rotation 2 angle=%f\n", angle);

	if(config.draw_vectors)
	{
		draw_rotation_vectors(dst);
	}
}









int MotionMain::process_buffer(VFrame **frame,
	int64_t start_position,
	double frame_rate)
{
	int need_reconfigure = load_configuration();
	int color_model = frame[0]->get_color_model();
	w = frame[0]->get_w();
	h = frame[0]->get_h();


	if(!match_frame)
	{
		match_frame = new VFrame(0, w, h, color_model);
		match_frame_number = -1;
	}


// The layer to do the motion tracking in
	master_layer = config.bottom_is_master ?
		PluginClient::total_in_buffers - 1 :
		0;
	master_frame = frame[master_layer];

// The layer to move
	target_layer = config.bottom_is_master ?
		0 :
		PluginClient::total_in_buffers - 1;
	target_frame = frame[target_layer];

// The position of the frame to compare the current frame with
	int64_t actual_match_frame_number;
// Skip if match frame not available
	int skip_current = 0;
	
	
	if(config.tracksingle)
	{
		actual_match_frame_number = config.track_frame;
		if(get_direction() == PLAY_REVERSE)
			actual_match_frame_number++;
		if(actual_match_frame_number == start_position)
			skip_current = 1;
	}
	else
	{
		actual_match_frame_number = start_position;
		if(get_direction() == PLAY_FORWARD)
			actual_match_frame_number--;
		else
			actual_match_frame_number++;
		CLAMP(master_layer, 0, get_total_buffers() - 1);

// Assume media extends to beginning of timeline
		if(actual_match_frame_number < 0)
			skip_current = 1;
	}

//printf("process_realtime %d %lld %lld\n", skip_current, match_frame_number, actual_match_frame_number);
// Load match frame and reset vectors
	if(!skip_current && 
		match_frame_number != actual_match_frame_number)
	{
		read_frame(match_frame, 
			master_layer, 
			actual_match_frame_number, 
			frame_rate);
		current_dx = 0;
		current_dy = 0;
		total_dx = 0;
		total_dy = 0;
		current_angle = 0;
		total_angle = 0;
		match_frame_number = actual_match_frame_number;
	}

// Load current frame
	read_frame(master_frame,
		master_layer,
		start_position,
		frame_rate);



// Get position change from match frame to current frame
	if(!skip_current)
	{
		if(config.global) process_global(frame);
		if(config.rotate) process_rotation(frame);
	}
	else
	{
		match_frame->copy_from(frame[master_layer]);
		match_frame_number = start_position;
		if(target_layer != master_layer)
			read_frame(target_frame,
				target_layer,
				start_position,
				frame_rate);
	}
//printf("%d %d\n", frame[0]->get_w(), frame[0]->get_h());

	return 0;
}

void MotionMain::draw_rotation_vectors(VFrame *frame)
{
	int x1, y1, x2, y2, x3, y3, x4, y4, center_x, center_y;
	int block_x = 0;
	int block_y = 0;
	int block_w = 0;
	int block_h = 0;
	if(motion_rotate)
	{
		block_x = motion_rotate->block_x1;
		block_y = motion_rotate->block_y1;
		block_w = motion_rotate->block_x2 - motion_rotate->block_x1;
		block_h = motion_rotate->block_y2 - motion_rotate->block_y1;
		center_x = block_x + block_w / 2;
		center_y = block_y + block_h / 2;
		float angle = motion_rotate->result;
		angle = angle * 2 * M_PI / 360;
		double base_angle1 = atan((float)block_h / block_w);
		double base_angle2 = atan((float)block_w / block_h);
		double target_angle1 = base_angle1 + angle;
		double target_angle2 = base_angle2 + angle;
		double radius = sqrt(block_w * block_w + block_h * block_h) / 2;
		x1 = (int)(center_x - cos(target_angle1) * radius);
		y1 = (int)(center_y - sin(target_angle1) * radius);
		x2 = (int)(center_x + sin(target_angle2) * radius);
		y2 = (int)(center_y - cos(target_angle2) * radius);
		x3 = (int)(center_x - sin(target_angle2) * radius);
		y3 = (int)(center_y + cos(target_angle2) * radius);
		x4 = (int)(center_x + cos(target_angle1) * radius);
		y4 = (int)(center_y + sin(target_angle1) * radius);
	}
	else
	{
		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = 0;
		x3 = 0;
		x4 = 0;
		y3 = 0;
		y4 = 0;
		center_x = 0;
		center_y = 0;
	}

// Rotation vector
	draw_line(frame, x1, y1, x2, y2);
	draw_line(frame, x2, y2, x4, y4);
	draw_line(frame, x4, y4, x3, y3);
	draw_line(frame, x3, y3, x1, y1);

// Center
	draw_line(frame, center_x, center_y - 5, center_x, center_y + 6);
	draw_line(frame, center_x - 5, center_y, center_x + 6, center_y);
}


void MotionMain::draw_pixel(VFrame *frame, int x, int y)
{
	if(!(x >= 0 && y >= 0 && x < frame->get_w() && y < frame->get_h())) return;
#define DRAW_PIXEL(x, y, components, do_yuv, max, type) \
{ \
	type **rows = (type**)frame->get_rows(); \
	rows[y][x * components] = max - rows[y][x * components]; \
	if(!do_yuv) \
	{ \
		rows[y][x * components + 1] = max - rows[y][x * components + 1]; \
		rows[y][x * components + 2] = max - rows[y][x * components + 2]; \
	} \
	else \
	{ \
		rows[y][x * components + 1] = (max / 2 + 1) - rows[y][x * components + 1]; \
		rows[y][x * components + 2] = (max / 2 + 1) - rows[y][x * components + 2]; \
	} \
	if(components == 4) \
		rows[y][x * components + 3] = max; \
}


	switch(frame->get_color_model())
	{
		case BC_RGB888:
			DRAW_PIXEL(x, y, 3, 0, 0xff, unsigned char);
			break;
		case BC_RGBA8888:
			DRAW_PIXEL(x, y, 4, 0, 0xff, unsigned char);
			break;
		case BC_RGB_FLOAT:
			DRAW_PIXEL(x, y, 3, 0, 1.0, float);
			break;
		case BC_RGBA_FLOAT:
			DRAW_PIXEL(x, y, 4, 0, 1.0, float);
			break;
		case BC_YUV888:
			DRAW_PIXEL(x, y, 3, 1, 0xff, unsigned char);
			break;
		case BC_YUVA8888:
			DRAW_PIXEL(x, y, 4, 1, 0xff, unsigned char);
			break;
		case BC_RGB161616:
			DRAW_PIXEL(x, y, 3, 0, 0xffff, uint16_t);
			break;
		case BC_YUV161616:
			DRAW_PIXEL(x, y, 3, 1, 0xffff, uint16_t);
			break;
		case BC_RGBA16161616:
			DRAW_PIXEL(x, y, 4, 0, 0xffff, uint16_t);
			break;
		case BC_YUVA16161616:
			DRAW_PIXEL(x, y, 4, 1, 0xffff, uint16_t);
			break;
	}
}


void MotionMain::draw_line(VFrame *frame, int x1, int y1, int x2, int y2)
{
	CLAMP(x1, 0, frame->get_w() - 1);
	CLAMP(y1, 0, frame->get_h() - 1);
	CLAMP(x2, 0, frame->get_w() - 1);
	CLAMP(y2, 0, frame->get_h() - 1);
	int w = labs(x2 - x1);
	int h = labs(y2 - y1);
//printf("MotionMain::draw_line 1 %d %d %d %d\n", x1, y1, x2, y2);

	if(!w && !h)
	{
		draw_pixel(frame, x1, y1);
	}
	else
	if(w > h)
	{
// Flip coordinates so x1 < x2
		if(x2 < x1)
		{
			y2 ^= y1;
			y1 ^= y2;
			y2 ^= y1;
			x1 ^= x2;
			x2 ^= x1;
			x1 ^= x2;
		}
		int numerator = y2 - y1;
		int denominator = x2 - x1;
		for(int i = x1; i < x2; i++)
		{
			int y = y1 + (int64_t)(i - x1) * (int64_t)numerator / (int64_t)denominator;
			draw_pixel(frame, i, y);
		}
	}
	else
	{
// Flip coordinates so y1 < y2
		if(y2 < y1)
		{
			y2 ^= y1;
			y1 ^= y2;
			y2 ^= y1;
			x1 ^= x2;
			x2 ^= x1;
			x1 ^= x2;
		}
		int numerator = x2 - x1;
		int denominator = y2 - y1;
		for(int i = y1; i < y2; i++)
		{
			int x = x1 + (int64_t)(i - y1) * (int64_t)numerator / (int64_t)denominator;
			draw_pixel(frame, x, i);
		}
	}
//printf("MotionMain::draw_line 2\n");
}

#define ARROW_SIZE 10
void MotionMain::draw_arrow(VFrame *frame, int x1, int y1, int x2, int y2)
{
	double angle = atan((float)(y2 - y1) / (float)(x2 - x1));
	double angle1 = angle + (float)145 / 360 * 2 * 3.14159265;
	double angle2 = angle - (float)145 / 360 * 2 * 3.14159265;
	int x3;
	int y3;
	int x4;
	int y4;
	if(x2 < x1)
	{
		x3 = x2 - (int)(ARROW_SIZE * cos(angle1));
		y3 = y2 - (int)(ARROW_SIZE * sin(angle1));
		x4 = x2 - (int)(ARROW_SIZE * cos(angle2));
		y4 = y2 - (int)(ARROW_SIZE * sin(angle2));
	}
	else
	{
		x3 = x2 + (int)(ARROW_SIZE * cos(angle1));
		y3 = y2 + (int)(ARROW_SIZE * sin(angle1));
		x4 = x2 + (int)(ARROW_SIZE * cos(angle2));
		y4 = y2 + (int)(ARROW_SIZE * sin(angle2));
	}

// Main vector
	draw_line(frame, x1, y1, x2, y2);
//	draw_line(frame, x1, y1 + 1, x2, y2 + 1);

// Arrow line
	if(abs(y2 - y1) || abs(x2 - x1)) draw_line(frame, x2, y2, x3, y3);
//	draw_line(frame, x2, y2 + 1, x3, y3 + 1);
// Arrow line
	if(abs(y2 - y1) || abs(x2 - x1)) draw_line(frame, x2, y2, x4, y4);
//	draw_line(frame, x2, y2 + 1, x4, y4 + 1);
}

void MotionMain::draw_global_vectors(VFrame *frame)
{
	for(int i = 0; i < engine->get_total_packages(); i++)
	{
		MotionPackage *pkg = (MotionPackage*)engine->get_package(i);
// Block border
		draw_line(frame, 
			pkg->block_x1, 
			pkg->block_y1,
			pkg->block_x2 - 1, 
			pkg->block_y1);
		draw_line(frame, 
			pkg->block_x2 - 1, 
			pkg->block_y1,
			pkg->block_x2 - 1, 
			pkg->block_y2 - 1);
		draw_line(frame, 
			pkg->block_x2 - 1, 
			pkg->block_y2 - 1,
			pkg->block_x1, 
			pkg->block_y2 - 1);
		draw_line(frame, 
			pkg->block_x1, 
			pkg->block_y2 - 1,
			pkg->block_x1, 
			pkg->block_y1);

		int x1 = (pkg->block_x1 + pkg->block_x2) / 2;
		int y1 = (pkg->block_y1 + pkg->block_y2) / 2;
		int x2 = x1 + pkg->dx / OVERSAMPLE;
		int y2 = y1 + pkg->dy / OVERSAMPLE;
		draw_arrow(frame, x1, y1, x2, y2);

		if(!pkg->valid)
		{
			draw_line(frame,
				pkg->block_x1, 
				pkg->block_y1,
				pkg->block_x2, 
				pkg->block_y2);
			draw_line(frame,
				pkg->block_x2, 
				pkg->block_y1,
				pkg->block_x1, 
				pkg->block_y2);
		}
	}

// Draw search area
	if(config.block_size < MAX_BLOCK)
	{
		int scan_w = frame->get_w() * config.block_size / 100;
		int scan_h = frame->get_h() * config.block_size / 100;
		int x1 = (int)(config.block_x * frame->get_w() / 100 - scan_w / 2);
		int x2 = (int)(config.block_x * frame->get_w() / 100 + scan_w / 2);
		int y1 = (int)(config.block_y * frame->get_h() / 100 - scan_h / 2);
		int y2 = (int)(config.block_y * frame->get_h() / 100 + scan_h / 2);
		draw_line(frame, x1, y1, x2, y1);
		draw_line(frame, x2, y1, x2, y2);
		draw_line(frame, x2, y2, x1, y2);
		draw_line(frame, x1, y2, x1, y1);
	}
}



#define ABS_DIFF(type, temp_type, multiplier, components) \
{ \
	temp_type result_temp = 0; \
	for(int i = 0; i < h; i++) \
	{ \
		type *prev_row = (type*)prev_ptr; \
		type *current_row = (type*)current_ptr; \
		for(int j = 0; j < w; j++) \
		{ \
			for(int k = 0; k < 3; k++) \
			{ \
				temp_type difference; \
				difference = *prev_row++ - *current_row++; \
				if(difference < 0) \
					result_temp -= difference; \
				else \
					result_temp += difference; \
			} \
			if(components == 4) \
			{ \
				prev_row++; \
				current_row++; \
			} \
		} \
		prev_ptr += row_bytes; \
		current_ptr += row_bytes; \
	} \
	result = (int64_t)(result_temp * multiplier); \
}

int64_t MotionMain::abs_diff(unsigned char *prev_ptr,
	unsigned char *current_ptr,
	int row_bytes,
	int w,
	int h,
	int color_model)
{
	int64_t result = 0;
	switch(color_model)
	{
		case BC_RGB888:
			ABS_DIFF(unsigned char, int64_t, 1, 3)
			break;
		case BC_RGBA8888:
			ABS_DIFF(unsigned char, int64_t, 1, 4)
			break;
		case BC_RGB_FLOAT:
			ABS_DIFF(float, double, 0x10000, 3)
			break;
		case BC_RGBA_FLOAT:
			ABS_DIFF(float, double, 0x10000, 4)
			break;
		case BC_YUV888:
			ABS_DIFF(unsigned char, int64_t, 1, 3)
			break;
		case BC_YUVA8888:
			ABS_DIFF(unsigned char, int64_t, 1, 4)
			break;
		case BC_YUV161616:
			ABS_DIFF(uint16_t, int64_t, 1, 3)
			break;
		case BC_YUVA16161616:
			ABS_DIFF(uint16_t, int64_t, 1, 4)
			break;
	}
	return result;
}



#define ABS_DIFF_SUB(type, temp_type, multiplier, components) \
{ \
	temp_type result_temp = 0; \
	temp_type y2_fraction = sub_y * 0x100 / OVERSAMPLE; \
	temp_type y1_fraction = 0x100 - y2_fraction; \
	temp_type x2_fraction = sub_x * 0x100 / OVERSAMPLE; \
	temp_type x1_fraction = 0x100 - x2_fraction; \
	for(int i = 0; i < h_sub; i++) \
	{ \
		type *prev_row1 = (type*)prev_ptr; \
		type *prev_row2 = (type*)prev_ptr + components; \
		type *prev_row3 = (type*)(prev_ptr + row_bytes); \
		type *prev_row4 = (type*)(prev_ptr + row_bytes) + components; \
		type *current_row = (type*)current_ptr; \
		for(int j = 0; j < w_sub; j++) \
		{ \
			for(int k = 0; k < 3; k++) \
			{ \
				temp_type difference; \
				temp_type prev_value = \
					(*prev_row1++ * x1_fraction * y1_fraction + \
					*prev_row2++ * x2_fraction * y1_fraction + \
					*prev_row3++ * x1_fraction * y2_fraction + \
					*prev_row4++ * x2_fraction * y2_fraction) / \
					0x100 / 0x100; \
				temp_type current_value = *current_row++; \
				difference = prev_value - current_value; \
				if(difference < 0) \
					result_temp -= difference; \
				else \
					result_temp += difference; \
			} \
 \
			if(components == 4) \
			{ \
				prev_row1++; \
				prev_row2++; \
				prev_row3++; \
				prev_row4++; \
				current_row++; \
			} \
		} \
		prev_ptr += row_bytes; \
		current_ptr += row_bytes; \
	} \
	result = (int64_t)(result_temp * multiplier); \
}




int64_t MotionMain::abs_diff_sub(unsigned char *prev_ptr,
	unsigned char *current_ptr,
	int row_bytes,
	int w,
	int h,
	int color_model,
	int sub_x,
	int sub_y)
{
	int h_sub = h - 1;
	int w_sub = w - 1;
	int64_t result = 0;

	switch(color_model)
	{
		case BC_RGB888:
			ABS_DIFF_SUB(unsigned char, int64_t, 1, 3)
			break;
		case BC_RGBA8888:
			ABS_DIFF_SUB(unsigned char, int64_t, 1, 4)
			break;
		case BC_RGB_FLOAT:
			ABS_DIFF_SUB(float, double, 0x10000, 3)
			break;
		case BC_RGBA_FLOAT:
			ABS_DIFF_SUB(float, double, 0x10000, 4)
			break;
		case BC_YUV888:
			ABS_DIFF_SUB(unsigned char, int64_t, 1, 3)
			break;
		case BC_YUVA8888:
			ABS_DIFF_SUB(unsigned char, int64_t, 1, 4)
			break;
		case BC_YUV161616:
			ABS_DIFF_SUB(uint16_t, int64_t, 1, 3)
			break;
		case BC_YUVA16161616:
			ABS_DIFF_SUB(uint16_t, int64_t, 1, 4)
			break;
	}
	return result;
}





MotionPackage::MotionPackage()
 : LoadPackage()
{
	valid = 1;
}

void MotionPackage::clamp_scan(int w, int h)
{
	CLAMP(block_x1, 0, w);
	CLAMP(block_x2, 0, w);
	CLAMP(block_y1, 0, h);
	CLAMP(block_y2, 0, h);
	CLAMP(scan_x1, 0, w - (block_x2 - block_x1));
	CLAMP(scan_y1, 0, h - (block_y2 - block_y1));
	CLAMP(scan_x2, 0, w - (block_x2 - block_x1));
	CLAMP(scan_y2, 0, h - (block_y2 - block_y1));
}

int MotionPackage::get_quadrant(int w, int h)
{
	int subscript = 4;
	if(block_x1 < w / 2 && block_y1 < h / 2)
	{
		subscript = 0;
	}
	else
	if(block_x1 >= w / 2 && block_y1 < h / 2)
	{
		subscript = 1;
	}
	else
	if(block_x1 < w / 2 && block_y1 >= h / 2)
	{
		subscript = 2;
	}
	else
	if(block_x1 >= w / 2 && block_y1 >= h / 2)
	{
		subscript = 3;
	}
	return subscript;
}






MotionUnit::MotionUnit(MotionEngine *server, 
	MotionMain *plugin)
 : LoadClient(server)
{
	this->plugin = plugin;
	this->server = server;
	cache_lock = new Mutex("MotionUnit::cache_lock");
}

MotionUnit::~MotionUnit()
{
	delete cache_lock;
}



void MotionUnit::process_package(LoadPackage *package)
{
	if(server->skip) return;

	MotionPackage *pkg = (MotionPackage*)package;
	int w = server->current_frame->get_w();
	int h = server->current_frame->get_h();
	int color_model = server->current_frame->get_color_model();
	int pixel_size = cmodel_calculate_pixelsize(color_model);
	int row_bytes = server->current_frame->get_bytes_per_line();












// Single macroblock
	if(plugin->config.block_count == 1)
	{

// Single pixel
		if(!server->subpixel)
		{
			int search_x = pkg->scan_x1 + (pkg->pixel % (pkg->scan_x2 - pkg->scan_x1));
			int search_y = pkg->scan_y1 + (pkg->pixel / (pkg->scan_x2 - pkg->scan_x1));
// Try cache
			pkg->difference = server->get_cache(search_x, search_y);
			if(pkg->difference < 0)
			{
// Pointers to first pixel in each block
				unsigned char *prev_ptr = server->match_frame->get_rows()[search_y] +
					search_x * pixel_size;
				unsigned char *current_ptr = server->current_frame->get_rows()[
					pkg->block_y1] +
					pkg->block_x1 * pixel_size;
// Scan block
				pkg->difference = plugin->abs_diff(prev_ptr,
					current_ptr,
					row_bytes,
					pkg->block_x2 - pkg->block_x1,
					pkg->block_y2 - pkg->block_y1,
					color_model);
				server->put_cache(search_x, search_y, pkg->difference);
//printf("MotionUnit::process_package %lld %d %d %d\n", pkg->difference, pkg->pixel, search_x, search_y);
			}
		}
		else

// Sub pixel
		{
			int sub_x = pkg->pixel % (OVERSAMPLE * 2 - 1) + 1;
			int sub_y = pkg->pixel / (OVERSAMPLE * 2 - 1) + 1;
			int search_x = pkg->scan_x1 + sub_x / OVERSAMPLE;
			int search_y = pkg->scan_y1 + sub_y / OVERSAMPLE;
			sub_x %= OVERSAMPLE;
			sub_y %= OVERSAMPLE;

			unsigned char *prev_ptr = server->match_frame->get_rows()[search_y] +
				search_x * pixel_size;
			unsigned char *current_ptr = server->current_frame->get_rows()[
				pkg->block_y1] +
				pkg->block_x1 * pixel_size;
			pkg->difference = plugin->abs_diff_sub(prev_ptr,
				current_ptr,
				row_bytes,
				pkg->block_x2 - pkg->block_x1,
				pkg->block_y2 - pkg->block_y1,
				color_model,
				sub_x,
				sub_y);
		}
	}
	else













// Multiple macroblocks
	{
		cache.remove_all_objects();

		int total_pixels = (pkg->scan_x2 - pkg->scan_x1) *
			(pkg->scan_y2 - pkg->scan_y1);
		int total_steps = MIN(plugin->config.global_positions, total_pixels);
		int x_result;
		int y_result;

// printf("MotionUnit::process_package 1 %d,%d %dx%d\n", 
// pkg->block_x1, pkg->block_y1, pkg->block_x2 - pkg->block_x1, pkg->block_y2 - pkg->block_y1);
		while(1)
		{
			for(int i = 0; 
				i < total_steps; 
				i++)
			{
				int pixel = (int64_t)i * (int64_t)total_pixels / (int64_t)total_steps;
// Position in current_frame in which to compare block from match_frame
				int search_x = pkg->scan_x1 + (pixel % (pkg->scan_x2 - pkg->scan_x1));
				int search_y = pkg->scan_y1 + (pixel / (pkg->scan_x2 - pkg->scan_x1));

				int64_t difference;
				if((difference = get_cache(search_x, search_y)) < 0)
				{
// Pointers to first pixel in each block
					unsigned char *prev_ptr = server->match_frame->get_rows()[search_y] +
						search_x * pixel_size;
					unsigned char *current_ptr = server->current_frame->get_rows()[
						pkg->block_y1] +
						pkg->block_x1 * pixel_size;
// Scan block
					difference = plugin->abs_diff(prev_ptr,
						current_ptr,
						row_bytes,
						pkg->block_x2 - pkg->block_x1,
						pkg->block_y2 - pkg->block_y1,
						color_model);
					put_cache(search_x, search_y, difference);
				}

				if(difference < pkg->min_difference || pkg->min_difference == -1)
				{
					pkg->min_difference = difference;
					pkg->dx = (pkg->block_x1 - search_x) * OVERSAMPLE;
					pkg->dy = (pkg->block_y1 - search_y) * OVERSAMPLE;
					x_result = search_x;
					y_result = search_y;
				}
				if(difference > pkg->max_difference || pkg->max_difference == -1)
				{
					pkg->max_difference = difference;
				}
			}

			if(total_steps >= total_pixels)
			{
// printf("MotionUnit::process_package 10 %d %d %d,%d %d,%d\n", 
// total_steps, 
// total_pixels, 
// pkg->block_x1, 
// pkg->block_y1, 
// pkg->dx, 
// pkg->dy);
				break;
			}
			else
			{
				int scan_w = (pkg->scan_x2 - pkg->scan_x1) / 2;
				int scan_h = (pkg->scan_y2 - pkg->scan_y1) / 2;
				pkg->scan_x1 = x_result - scan_w / 2;
				pkg->scan_x2 = x_result + scan_w / 2;
				pkg->scan_y1 = y_result - scan_h / 2;
				pkg->scan_y2 = y_result + scan_h / 2;
				pkg->clamp_scan(w, h);
				total_pixels = (pkg->scan_x2 - pkg->scan_x1) *
					(pkg->scan_y2 - pkg->scan_y1);
				total_steps = MIN(plugin->config.global_positions, total_pixels);
			}
		}
	}
}










int64_t MotionUnit::get_cache(int x, int y)
{
	int64_t result = -1;
	cache_lock->lock("MotionUnit::get_cache");
	for(int i = 0; i < cache.total; i++)
	{
		MotionCache *ptr = cache.values[i];
		if(ptr->x == x && ptr->y == y)
		{
			result = ptr->difference;
			break;
		}
	}
	cache_lock->unlock();
	return result;
}

void MotionUnit::put_cache(int x, int y, int64_t difference)
{
	MotionCache *ptr = new MotionCache(x, y, difference);
	cache_lock->lock("MotionUnit::put_cache");
	cache.append(ptr);
	cache_lock->unlock();
}











MotionEngine::MotionEngine(MotionMain *plugin, 
	int total_clients,
	int total_packages)
 : LoadServer(
//1, 1 
total_clients, total_packages 
)
{
	this->plugin = plugin;
	cache_lock = new Mutex("MotionEngine::cache_lock");
}

MotionEngine::~MotionEngine()
{
	delete cache_lock;
}


void MotionEngine::init_packages()
{
	int w = current_frame->get_w();
	int h = current_frame->get_h();
	int scan_w = w * 
		plugin->config.global_range *
		plugin->config.block_size / 100 / 100;
	int scan_h = h * 
		plugin->config.global_range *
		plugin->config.block_size / 100 / 100;
// Get blocks per side
	int x_blocks = (int)sqrt(plugin->config.block_count);
	int y_blocks = (int)sqrt(plugin->config.block_count);
// Get total border from search radius
	int block_w = w * plugin->config.block_size / 100 - scan_w;
	int block_h = h * plugin->config.block_size / 100 - scan_h;
	int total_x1 = (int)(w * plugin->config.block_x / 100 - block_w / 2);
	int total_y1 = (int)(h * plugin->config.block_y / 100 - block_h / 2);
	int total_x2 = (int)(w * plugin->config.block_x / 100 + block_w / 2);
	int total_y2 = (int)(h * plugin->config.block_y / 100 + block_h / 2);
	skip = 0;

// Set package coords
	for(int i = 0; i < get_total_packages(); i++)
	{
		MotionPackage *pkg = (MotionPackage*)get_package(i);






// Single macroblock
		if(plugin->config.block_count == 1)
		{
			pkg->block_x1 = block_x1;
			pkg->block_x2 = block_x2;
			pkg->block_y1 = block_y1;
			pkg->block_y2 = block_y2;
			pkg->scan_x1 = scan_x1;
			pkg->scan_x2 = scan_x2;
			pkg->scan_y1 = scan_y1;
			pkg->scan_y2 = scan_y2;
			pkg->pixel = (int64_t)i * (int64_t)total_pixels / (int64_t)total_steps;
			pkg->difference = 0;
			pkg->dx = 0;
			pkg->dy = 0;
			pkg->valid = 1;
		}
		else





// Multiple macroblocks
		{
			int x_block = i % x_blocks;
			int y_block = i / x_blocks;
			pkg->block_x1 = total_x1 + x_block * (total_x2 - total_x1) / x_blocks;
			pkg->block_x2 = total_x1 + (x_block + 1) * (total_x2 - total_x1) / x_blocks;
			pkg->block_y1 = total_y1 + y_block * (total_y2 - total_y1) / y_blocks;
			pkg->block_y2 = total_y1 + (y_block + 1) * (total_y2 - total_y1) / y_blocks;
			pkg->scan_x1 = pkg->block_x1 - scan_w / 2;
			pkg->scan_x2 = pkg->block_x1 + scan_w / 2;
			pkg->scan_y1 = pkg->block_y1 - scan_h / 2;
			pkg->scan_y2 = pkg->block_y1 + scan_h / 2;
			pkg->max_difference = -1;
			pkg->min_difference = -1;
			pkg->min_pixel = -1;
			pkg->dx = 0;
			pkg->dy = 0;
			pkg->clamp_scan(current_frame->get_w(),
				current_frame->get_h());
			pkg->is_border = (!x_block || 
				!y_block ||
				(x_block == x_blocks - 1) ||
				(y_block == y_blocks - 1));
			pkg->valid = 1;
		}
	}










// Fill in values from disk
	if(plugin->config.block_count != 1)
	{
		if(plugin->config.mode3 == MotionConfig::NO_CALCULATE)
		{
			for(int i = 0; i < get_total_packages(); i++)
			{
				MotionPackage *pkg = (MotionPackage*)get_package(i);
				pkg->dx = 0;
				pkg->dy = 0;
			}
			skip = 1;
		}
		else
		if(plugin->config.mode3 == MotionConfig::LOAD)
		{
			char string[BCTEXTLEN];
			sprintf(string, "%s%06d", MOTION_FILE, plugin->get_source_position());
			FILE *input = fopen(string, "r");
			if(input)
			{
				for(int i = 0; i < get_total_packages(); i++)
				{
					MotionPackage *pkg = (MotionPackage*)get_package(i);
					fscanf(input, "%d %d", 
						&pkg->dx,
						&pkg->dy);
				}
				fclose(input);
				skip = 1;
			}
			else
			{
				printf("MotionEngine::init_packages LOAD %s: %s\n",
					string,
					strerror(errno));
				skip = 0;
			}
		}
	}
}

LoadClient* MotionEngine::new_client()
{
	return new MotionUnit(this, plugin);
}

LoadPackage* MotionEngine::new_package()
{
	return new MotionPackage;
}

void MotionEngine::clamp_scan(int w, int h)
{
	CLAMP(block_x1, 0, w);
	CLAMP(block_x2, 0, w);
	CLAMP(block_y1, 0, h);
	CLAMP(block_y2, 0, h);
	CLAMP(scan_x1, 0, w - (block_x2 - block_x1));
	CLAMP(scan_y1, 0, h - (block_y2 - block_y1));
	CLAMP(scan_x2, 0, w - (block_x2 - block_x1));
	CLAMP(scan_y2, 0, h - (block_y2 - block_y1));
}


void MotionEngine::scan_frame(VFrame *match_frame,
	VFrame *current_frame)
{
	this->match_frame = match_frame;
	this->current_frame = current_frame;
	subpixel = 0;

	cache.remove_all_objects();












// Single macroblock
	if(plugin->config.block_count == 1)
	{
		int w = current_frame->get_w();
		int h = current_frame->get_h();

// Initial search parameters
		int scan_w = w * plugin->config.global_range *
			plugin->config.block_size / 100 / 100;
		int scan_h = h * plugin->config.global_range *
			plugin->config.block_size / 100 / 100;
		int block_w = w * plugin->config.block_size / 100 - scan_w;
		int block_h = h * plugin->config.block_size / 100 - scan_h;
		block_x1 = (int)(w * plugin->config.block_x / 100 - block_w / 2);
		block_y1 = (int)(h * plugin->config.block_y / 100 - block_h / 2);
		block_x2 = (int)(w * plugin->config.block_x / 100 + block_w / 2);
		block_y2 = (int)(h * plugin->config.block_y / 100 + block_h / 2);
		int x_result = block_x1;
		int y_result = block_y1;
		skip = 0;

// Don't calculate
		if(plugin->config.mode3 == MotionConfig::NO_CALCULATE)
		{
			set_package_count(1);
			MotionPackage *pkg = (MotionPackage*)get_package(0);
			pkg->dx = 0;
			pkg->dy = 0;
			pkg->valid = 1;
			skip = 1;
		}
		else
// Load result from disk
		if(plugin->config.mode3 == MotionConfig::LOAD)
		{
			char string[BCTEXTLEN];
			sprintf(string, "%s%06d", MOTION_FILE, plugin->get_source_position());
			FILE *input = fopen(string, "r");
			if(input)
			{
				fscanf(input, 
					"%d %d", 
					&x_result,
					&y_result);
				fclose(input);
				skip = 1;
				x_result += block_x1;
				y_result += block_y1;
			}
			else
			{
				printf("MotionEngine::scan_frame %s: %s\n", 
					string, 
					strerror(errno));
				skip = 0;
			}

			set_package_count(1);
			MotionPackage *pkg = (MotionPackage*)get_package(0);
			pkg->dx = x_result - block_x1;
			pkg->dy = y_result - block_y1;
			pkg->valid = 1;
		}

// Perform scan
		if(!skip)
		{
			while(1)
			{
				scan_x1 = x_result - scan_w / 2;
				scan_y1 = y_result - scan_h / 2;
				scan_x2 = x_result + scan_w / 2;
				scan_y2 = y_result + scan_h / 2;
				clamp_scan(w, h);

// For subpixel, the top row and left column are skipped
				if(subpixel)
				{
					total_pixels = 4 * OVERSAMPLE * OVERSAMPLE - 4 * OVERSAMPLE;
					total_steps = total_pixels;
				}
				else
				{
					total_pixels = (scan_x2 - scan_x1) * (scan_y2 - scan_y1);
					total_steps = MIN(plugin->config.global_positions, total_pixels);
				}

				set_package_count(total_steps);
				process_packages();

// Get least difference
				int64_t min_difference = -1;
				int min_pixel = -1;
				for(int i = 0; i < get_total_packages(); i++)
				{
					MotionPackage *pkg = (MotionPackage*)get_package(i);
					if(pkg->difference < min_difference || min_difference == -1)
					{
						min_difference = pkg->difference;
						min_pixel = pkg->pixel;
					}
				}

// Get new search position
				if(!subpixel)
				{
					x_result = scan_x1 + (min_pixel % (scan_x2 - scan_x1));
					y_result = scan_y1 + (min_pixel / (scan_x2 - scan_x1));
					x_result *= OVERSAMPLE;
					y_result *= OVERSAMPLE;
				}
				else
				{
					x_result = scan_x1 * OVERSAMPLE + 
						(min_pixel % (OVERSAMPLE * 2 - 1)) + 1;
					y_result = scan_y1 * OVERSAMPLE + 
						(min_pixel / (OVERSAMPLE * 2 - 1)) + 1;
				}

printf("MotionEngine::scan_frame 10 total_steps=%d total_pixels=%d subpixel=%d\n",
total_steps, 
total_pixels,
subpixel);

printf("	scan w=%d h=%d scan x1=%d y1=%d x2=%d y2=%d\n",
scan_w,
scan_h, 
scan_x1,
scan_y1,
scan_x2,
scan_y2);

printf("	block x1=%d y1=%d x2=%d y2=%d result x=%.2f y=%.2f\n", 
block_x1, 
block_y1, 
block_x2,
block_y2,
(float)x_result / 4, 
(float)y_result / 4);

				if(total_steps >= total_pixels || subpixel)
				{
// Single pixel accuracy reached.  Now do exhaustive subpixel
					if(!subpixel &&
						(plugin->config.mode1 == MotionConfig::STABILIZE ||
						plugin->config.mode1 == MotionConfig::TRACK ||
						plugin->config.mode1 == MotionConfig::NOTHING))
					{
						x_result /= OVERSAMPLE;
						y_result /= OVERSAMPLE;
						scan_w = 2;
						scan_h = 2;
						subpixel = 1;
					}
					else
						break;
				}
				else
// Reduce scan area and try again
				{
					scan_w = (scan_x2 - scan_x1) / 2;
					scan_h = (scan_y2 - scan_y1) / 2;
					x_result /= OVERSAMPLE;
					y_result /= OVERSAMPLE;
				}
			}



// Fill in package values so the plugin can get them
			set_package_count(1);
			MotionPackage *pkg = (MotionPackage*)get_package(0);
			pkg->dx = block_x1 * OVERSAMPLE - x_result;
			pkg->dy = block_y1 * OVERSAMPLE - y_result;
		}

		MotionPackage *pkg = (MotionPackage*)get_package(0);
// Fill in package values so the plugin can get them
		pkg->block_x1 = block_x1;
		pkg->block_y1 = block_y1;
		pkg->block_x2 = block_x2;
		pkg->block_y2 = block_y2;
		pkg->valid = 1;

// Write results
		if(plugin->config.mode3 == MotionConfig::SAVE)
		{
			char string[BCTEXTLEN];
			sprintf(string, 
				"%s%06d", 
				MOTION_FILE, 
				plugin->get_source_position());
			FILE *output = fopen(string, "w");
			if(output)
			{
				fprintf(output, 
					"%d %d\n",
					pkg->dx,
					pkg->dy);
				fclose(output);
			}
			else
			{
				perror("MotionEngine::scan_frame SAVE 1");
			}
		}

printf("MotionEngine::scan_frame 10 dx=%.2f dy=%.2f\n", 
(float)pkg->dx / 4,
(float)pkg->dy / 4);

	}












	else













// Multiple blocks
	{
		set_package_count(plugin->config.block_count);
		process_packages();

// Write results
		if(plugin->config.mode3 == MotionConfig::SAVE)
		{
			char string[BCTEXTLEN];
			sprintf(string, 
				"%s%06d", 
				MOTION_FILE, 
				plugin->get_source_position());
			FILE *output = fopen(string, "w");
			if(output)
			{
				for(int i = 0; i < get_total_packages(); i++)
				{
					MotionPackage *pkg = (MotionPackage*)get_package(i);
					fprintf(output, 
						"%d %d\n",
						pkg->dx,
						pkg->dy);
				}
				fclose(output);
			}
			else
			{
				perror("MotionEngine::scan_frame SAVE 2");
			}
		}
	}
}

















int64_t MotionEngine::get_cache(int x, int y)
{
	int64_t result = -1;
	cache_lock->lock("MotionEngine::get_cache");
	for(int i = 0; i < cache.total; i++)
	{
		MotionCache *ptr = cache.values[i];
		if(ptr->x == x && ptr->y == y)
		{
			result = ptr->difference;
			break;
		}
	}
	cache_lock->unlock();
	return result;
}

void MotionEngine::put_cache(int x, int y, int64_t difference)
{
	MotionCache *ptr = new MotionCache(x, y, difference);
	cache_lock->lock("MotionEngine::put_cache");
	cache.append(ptr);
	cache_lock->unlock();
}





MotionCache::MotionCache(int x, int y, int64_t difference)
{
	this->x = x;
	this->y = y;
	this->difference = difference;
}














MotionRotatePackage::MotionRotatePackage()
{
}


MotionRotateUnit::MotionRotateUnit(MotionRotateEngine *server, MotionMain *plugin)
 : LoadClient(server)
{
	this->server = server;
	this->plugin = plugin;
	rotater = 0;
	temp = 0;
}

MotionRotateUnit::~MotionRotateUnit()
{
	delete rotater;
	delete temp;
}

void MotionRotateUnit::process_package(LoadPackage *package)
{
	if(server->skip) return;
	MotionRotatePackage *pkg = (MotionRotatePackage*)package;

	if((pkg->difference = server->get_cache(pkg->angle)) < 0)
	{
		int color_model = server->match_frame->get_color_model();
		int pixel_size = cmodel_calculate_pixelsize(color_model);
		int row_bytes = server->match_frame->get_bytes_per_line();

		if(!rotater)
			rotater = new AffineEngine(1, 1);
		if(!temp) temp = new VFrame(0,
			server->match_frame->get_w(),
			server->match_frame->get_h(),
			color_model);


// Rotate original block size
		rotater->set_viewport(server->block_x1, 
			server->block_y1,
			server->block_x2 - server->block_x1,
			server->block_y2 - server->block_y1);
		rotater->rotate(temp,
			server->match_frame,
			pkg->angle);

// Scan reduced block size
		pkg->difference = plugin->abs_diff(
			temp->get_rows()[server->scan_y] + server->scan_x * pixel_size,
			server->current_frame->get_rows()[server->scan_y] + server->scan_x * pixel_size,
			row_bytes,
			server->scan_w,
			server->scan_h,
			color_model);
		server->put_cache(pkg->angle, pkg->difference);
//printf("MotionRotateUnit::process_package 10 %f %lld\n", pkg->angle, pkg->difference);
	}
}






















MotionRotateEngine::MotionRotateEngine(MotionMain *plugin, 
	int total_clients, 
	int total_packages)
 : LoadServer(
//1, 1 
total_clients, total_packages 
)
{
	this->plugin = plugin;
	cache_lock = new Mutex("MotionRotateEngine::cache_lock");
}


MotionRotateEngine::~MotionRotateEngine()
{
	delete cache_lock;
}

void MotionRotateEngine::init_packages()
{
	for(int i = 0; i < get_total_packages(); i++)
	{
		MotionRotatePackage *pkg = (MotionRotatePackage*)get_package(i);
		pkg->angle = i * 
			(scan_angle2 - scan_angle1) / 
			(total_steps - 1) + 
			scan_angle1;
	}
}

LoadClient* MotionRotateEngine::new_client()
{
	return new MotionRotateUnit(this, plugin);
}

LoadPackage* MotionRotateEngine::new_package()
{
	return new MotionRotatePackage;
}


float MotionRotateEngine::scan_frame(VFrame *match_frame,
	VFrame *current_frame)
{
	skip = 0;

	if(plugin->config.mode3 == MotionConfig::NO_CALCULATE)
	{
		result = 0;
		skip = 1;
	}
	else
	if(plugin->config.mode3 == MotionConfig::LOAD)
	{
		char string[BCTEXTLEN];
		sprintf(string, "%s%06d", ROTATION_FILE, plugin->get_source_position());
		FILE *input = fopen(string, "r");
		if(input)
		{
			fscanf(input, "%f", &result);
			fclose(input);
			skip = 1;
		}
		else
		{
			perror("MotionRotateEngine::scan_frame LOAD");
		}
	}

// Need these numbers to draw rotation vector
	this->match_frame = match_frame;
	this->current_frame = current_frame;
	int w = current_frame->get_w();
	int h = current_frame->get_h();
	int block_w = w * plugin->config.block_size / 100;
	int block_h = h * plugin->config.block_size / 100;

	block_x1 = (int)(w * plugin->config.block_x / 100 - block_w / 2);
	block_x2 = (int)(w * plugin->config.block_x / 100 + block_w / 2);
	block_y1 = (int)(h * plugin->config.block_y / 100 - block_h / 2);
	block_y2 = (int)(h * plugin->config.block_y / 100 + block_h / 2);


// Calculate the maximum area available to scan after rotation.
// Must be calculated from the starting range because of cache.
// Get coords of rectangle after rotation.
	double center_x = w * plugin->config.block_x / 100;
	double center_y = h * plugin->config.block_y / 100;
	double max_angle = plugin->config.rotation_range;
	double base_angle1 = atan((float)block_h / block_w);
	double base_angle2 = atan((float)block_w / block_h);
	double target_angle1 = base_angle1 + max_angle * 2 * M_PI / 360;
	double target_angle2 = base_angle2 + max_angle * 2 * M_PI / 360;
	double radius = sqrt(block_w * block_w + block_h * block_h) / 2;
	double x1 = center_x - cos(target_angle1) * radius;
	double y1 = center_y - sin(target_angle1) * radius;
	double x2 = center_x + sin(target_angle2) * radius;
	double y2 = center_y - cos(target_angle2) * radius;
	double x3 = center_x - sin(target_angle2) * radius;
	double y3 = center_y + cos(target_angle2) * radius;

// Track top edge to find greatest area.
	double max_area1 = 0;
	double max_x1 = 0;
	double max_y1 = 0;
	for(double x = x1; x < x2; x++)
	{
		double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
		if(x >= center_x && x < block_x2 && y >= block_y1 && y < center_y)
		{
			double area = fabs(x - center_x) * fabs(y - center_y);
			if(area > max_area1)
			{
				max_area1 = area;
				max_x1 = x;
				max_y1 = y;
			}
		}
	}

// Track left edge to find greatest area.
	double max_area2 = 0;
	double max_x2 = 0;
	double max_y2 = 0;
	for(double y = y1; y < y3; y++)
	{
		double x = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
		if(x >= block_x1 && x < center_x && y >= block_y1 && y < center_y)
		{
			double area = fabs(x - center_x) * fabs(y - center_y);
			if(area > max_area2)
			{
				max_area2 = area;
				max_x2 = x;
				max_y2 = y;
			}
		}
	}

	double max_x, max_y;
	max_x = max_x2;
	max_y = max_y1;

// Get reduced scan coords
	scan_w = (int)(fabs(max_x - center_x) * 2);
	scan_h = (int)(fabs(max_y - center_y) * 2);
	scan_x = (int)(center_x - scan_w / 2);
	scan_y = (int)(center_y - scan_h / 2);
// printf("MotionRotateEngine::scan_frame scan = %d,%d %dx%d\n", scan_x, scan_y, scan_w, scan_h);
// printf("	angle_range=%f block= %d,%d,%d,%d\n", max_angle, block_x1, block_y1, block_x2, block_y2);

// Determine min angle from size of block
	double angle1 = atan((double)block_h / block_w);
	double angle2 = atan((double)(block_h - 1) / (block_w + 1));
	double min_angle = fabs(angle2 - angle1) / OVERSAMPLE;
	min_angle = MAX(min_angle, MIN_ANGLE);
printf("MotionRotateEngine::scan_frame min_angle=%f\n", min_angle);

	cache.remove_all_objects();
	if(!skip)
	{
// Initial search range
		float angle_range = (float)plugin->config.rotation_range;
		result = 0;
		total_steps = plugin->config.rotate_positions;


		while(angle_range >= min_angle * total_steps)
		{
			scan_angle1 = result - angle_range;
			scan_angle2 = result + angle_range;


			set_package_count(total_steps);
			process_packages();

			int64_t min_difference = -1;
			for(int i = 0; i < get_total_packages(); i++)
			{
				MotionRotatePackage *pkg = (MotionRotatePackage*)get_package(i);
				if(pkg->difference < min_difference || min_difference == -1)
				{
					min_difference = pkg->difference;
					result = pkg->angle;
				}
			}

			angle_range /= 2;
		}
	}


	if(!skip && plugin->config.mode3 == MotionConfig::SAVE)
	{
		char string[BCTEXTLEN];
		sprintf(string, 
			"%s%06d", 
			ROTATION_FILE, 
			plugin->get_source_position());
		FILE *output = fopen(string, "w");
		if(output)
		{
			fprintf(output, "%f\n", result);
			fclose(output);
		}
		else
		{
			perror("MotionRotateEngine::scan_frame SAVE");
		}
	}

printf("MotionRotateEngine::scan_frame 10 %f\n", result);

	


	return result;
}

int64_t MotionRotateEngine::get_cache(float angle)
{
	int64_t result = -1;
	cache_lock->lock("MotionRotateEngine::get_cache");
	for(int i = 0; i < cache.total; i++)
	{
		MotionRotateCache *ptr = cache.values[i];
		if(fabs(ptr->angle - angle) <= MIN_ANGLE)
		{
			result = ptr->difference;
			break;
		}
	}
	cache_lock->unlock();
	return result;
}

void MotionRotateEngine::put_cache(float angle, int64_t difference)
{
	MotionRotateCache *ptr = new MotionRotateCache(angle, difference);
	cache_lock->lock("MotionRotateEngine::put_cache");
	cache.append(ptr);
	cache_lock->unlock();
}









MotionRotateCache::MotionRotateCache(float angle, int64_t difference)
{
	this->angle = angle;
	this->difference = difference;
}



