#include "previewaudio.h"
#include "previewvideo.h"
#include "record.h"
#include "recordpreview.h"

RecordPreview::RecordPreview(MWindow *mwindow, Record *record)
{
	this->mwindow = mwindow;
	this->record = record;
}










RecordPreview::~RecordPreview()
{
}

int RecordPreview::initialize()
{
	if(record->do_audio)
		preview_audio = new PreviewAudio(record, this);
	if(record->do_video)
		preview_video = new PreviewVideo(record, this);
	return 0;
}

int RecordPreview::start_preview(long current_position, File *file)
{
	if(record->do_audio)
		preview_audio->start_preview(current_position, file);
	if(record->do_video)
		preview_video->start_preview(current_position, file);

	resume_monitor = 0;
	completion_lock.lock();
	set_synchronous(0);
	Thread::start();
	return 0;
}

int RecordPreview::stop_preview(int resume_monitor)
{
	this->resume_monitor = resume_monitor;
	if(record->do_audio) preview_audio->stop_preview();
	if(record->do_video) preview_video->stop_preview();
	completion_lock.lock();
	completion_lock.unlock();
	return 0;
}

long RecordPreview::absolute_position()
{
	if(record->do_audio) return preview_audio->sync_position();
	return -1;
}

void RecordPreview::run()
{
	if(record->do_audio) preview_audio->join();
	if(record->do_video) preview_video->join();

//	record_engine->is_previewing = 0;
//	record_engine->close_output_devices();
//	if(resume_monitor) record_engine->resume_monitor();

	completion_lock.unlock();
}
