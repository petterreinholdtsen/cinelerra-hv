#ifndef RECORDPREVIEW_H
#define RECORDPREVIEW_H


#include "mutex.h"
#include "mwindow.inc"
#include "previewaudio.inc"
#include "previewvideo.inc"
#include "record.inc"
#include "mutex.h"
#include "thread.h"

// Synchronously handle previewing recordings

class RecordPreview : public Thread
{
public:
	RecordPreview(MWindow *mwindow, Record *record);

	MWindow *mwindow;
	Record *record;




	RecordPreview(Record *record, RecordEngine *record_engine);
	~RecordPreview();
	
	int initialize();
	int start_preview(long current_position, File *file);
	int stop_preview(int resume_monitor);
	long absolute_position();
	void run();
	PreviewAudio *preview_audio;
	PreviewVideo *preview_video;

private:
	Mutex completion_lock;
	int resume_monitor;
};



#endif
