include ../global_config

# burn must come before any other effecttv plugin
# colors must come before any plugin
# parametric must come before fourier plugins
# motion must come before perspective

DIRS = \
	1080to540 \
	720to480 \
	colors \
	bandslide \
	bandwipe \
	blur \
	brightness \
	burn \
	parametric \
	aging \
	cdripper \
	chromakey \
	chromakeyhsv \
	colorbalance \
	compressor \
	crossfade \
	decimate \
	deinterlace \
	delayaudio \
	delayvideo \
	denoise \
	denoisefft \
	denoiseseltempavg \
	denoisevideo \
	despike \
	diffkey \
	dissolve \
	dot \
	downsample \
	fieldframe \
	flash \
	flip \
	framefield \
	freeverb \
	freezeframe \
	gain \
	gamma \
	gradient \
	graphic \
	histogram \
	holo \
	huesaturation \
	interpolateaudio \
	interpolatevideo \
	invertaudio \
	invertvideo \
	irissquare \
	ivtc \
	lens \
	level \
	linearblur \
	liveaudio \
	livevideo \
	loopaudio \
	loopvideo \
	motion \
	motion2point \
	motionblur \
	normalize \
	oilpainting \
	overlay \
	overlayaudio \
	perspective \
	pitch \
	polar \
	radialblur \
	reframe \
	reframert \
	resample \
	reverb \
	reverseaudio \
	reversevideo \
	rgb601 \
	rotate \
	scale \
	shapewipe \
	sharpen \
	shiftinterlace \
	slide \
	spectrogram \
	suv \
	swapchannels \
	swapframes \
	synthesizer \
	threshold \
	timeavg \
	timefront \
	timestretch \
	titler \
	translate \
	unsharp \
	videoscope \
	wave \
	whirl \
	wipe \
	yuv \
	zoomblur

# Make static table of all plugins
ifeq ($(DO_STATIC), y)

endif


all: $(DIRS) $(OBJS)
	@ for i in $(DIRS) ; \
	do \
		 $(MAKE) -C $$i ; \
	done

clean:
	find \( -name core \
		-o -name '*.o' \
		-o -name '*.plugin' \) -exec rm -rf {} \;

wc:
	cat *.C *.h | wc
	$(LOOP_BEGIN) \
		 $(MAKE) -C $$i wc; \
	$(LOOP_END)


# Can't strip the themes so instead the plugins strip themselves
install:
#	strip ../bin/*.plugin
#	rm -rf ../bin/*.plugin
#	cp $(OBJDIR)/*.plugin ../bin/
	mkdir -p ../bin/fonts
	cp fonts/* ../bin/fonts
	mkdir -p ../bin/shapes
	cp shapes/* ../bin/shapes
	cp ../thirdparty/mjpegtools*/mpeg2enc/mpeg2enc ../bin/mpeg2enc.plugin




