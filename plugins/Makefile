include ../global_config

# burn must come before any other effecttv plugin
# colors must come before any plugin
# parametric must come before fourier plugins

DIRS = \
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
	colorbalance \
	compressor \
	crossfade \
	defaulttheme \
	deinterlace \
	delayaudio \
	delayvideo \
	denoise \
	denoisefft \
	denoisevideo \
	despike \
	dissolve \
	dot \
	fieldframe \
	flip \
	framefield \
	freeverb \
	freezeframe \
	gain \
	holo \
	huesaturation \
	interpolateall \
	invertaudio \
	invertvideo \
	irissquare \
	ivtc \
	lame \
	mpeg2enc \
	normalize \
	oilpainting \
	pitch \
	polar \
	reframe \
	resample \
	reverb \
	reverseaudio \
	reversevideo \
	rgb601 \
	rotate \
	scale \
	sharpen \
	shiftinterlace \
	slide \
	spectrogram \
	swapchannels \
	synthesizer \
	timeavg \
	timestretch \
	titler \
	toolame \
	translate \
	videoscope \
	wave \
	whirl \
	wipe \
	yuv

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
#	strip $(OBJDIR)/*.plugin
	rm -rf /usr/lib/cinelerra
	mkdir -p /usr/lib/cinelerra
	cp $(OBJDIR)/*.plugin /usr/lib/cinelerra
	cp -a titler/fonts /usr/lib/cinelerra





