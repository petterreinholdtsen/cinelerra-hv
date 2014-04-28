#ifndef CPLAYBACK_H
#define CPLAYBACK_H

#include "cwindow.inc"
#include "playbackengine.h"

class CPlayback : public PlaybackEngine
{
public:
	CPlayback(MWindow *mwindow, CWindow *cwindow, Canvas *output);

	int create_render_engines();
	void init_cursor();
	void stop_cursor();

// Reset the transport after completion
	void update_transport(int command, int paused);
	CWindow *cwindow;
};

#endif
