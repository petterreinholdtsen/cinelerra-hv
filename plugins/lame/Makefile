include ../../global_config






ifeq ($(OBJDIR), alpha)
endif






CFLAGS += -DHAVE_CONFIG_H \
	-Iinclude \
	-Ilibmp3lame \
	-I.


OBJS = \
	$(OBJDIR)/frontend/brhist.o \
	$(OBJDIR)/frontend/get_audio.o \
	$(OBJDIR)/frontend/lametime.o \
	$(OBJDIR)/frontend/main.o \
	$(OBJDIR)/frontend/parse.o \
	$(OBJDIR)/frontend/portableio.o \
	$(OBJDIR)/frontend/timestatus.o \
	$(OBJDIR)/libmp3lame/VbrTag.o \
	$(OBJDIR)/libmp3lame/bitstream.o \
	$(OBJDIR)/libmp3lame/encoder.o \
	$(OBJDIR)/libmp3lame/fft.o \
	$(OBJDIR)/libmp3lame/id3tag.o \
	$(OBJDIR)/libmp3lame/lame.o \
	$(OBJDIR)/libmp3lame/mpglib_interface.o \
	$(OBJDIR)/libmp3lame/newmdct.o \
	$(OBJDIR)/libmp3lame/pcm.o \
	$(OBJDIR)/libmp3lame/psymodel.o \
	$(OBJDIR)/libmp3lame/quantize.o \
	$(OBJDIR)/libmp3lame/quantize_pvt.o \
	$(OBJDIR)/libmp3lame/reservoir.o \
	$(OBJDIR)/libmp3lame/set_get.o \
	$(OBJDIR)/libmp3lame/tables.o \
	$(OBJDIR)/libmp3lame/takehiro.o \
	$(OBJDIR)/libmp3lame/util.o \
	$(OBJDIR)/libmp3lame/vbrquantize.o \
	$(OBJDIR)/libmp3lame/version.o \
	$(OBJDIR)/libmp3lame/vorbis_interface.o

OUTPUT = ../$(OBJDIR)/lame.plugin


LIBS = -lm -lcurses

DIRS = $(OBJDIR)/frontend $(OBJDIR)/libmp3lame/

$(shell echo $(CFLAGS) > $(OBJDIR)/c_flags)
$(shell echo $(OBJS) $(LIBS) > $(OBJDIR)/libs)

$(OUTPUT): $(DIRS) $(OBJS)
	gcc -o $(OUTPUT) `cat $(OBJDIR)/libs`

clean:
	rm -rf $(OBJDIR)
	find \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;

$(DIRS):
	if ! test -d $@ ; then mkdir -p $@ ; fi



$(OBJS):
	$(GCC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.c) -o $*.o











$(OBJDIR)/frontend/brhist.o: 		    frontend/brhist.c
$(OBJDIR)/frontend/get_audio.o: 	    frontend/get_audio.c
$(OBJDIR)/frontend/lametime.o:  	    frontend/lametime.c
$(OBJDIR)/frontend/main.o: 		    frontend/main.c
$(OBJDIR)/frontend/parse.o: 		    frontend/parse.c
$(OBJDIR)/frontend/portableio.o: 	    frontend/portableio.c
$(OBJDIR)/frontend/timestatus.o: 	    frontend/timestatus.c
$(OBJDIR)/libmp3lame/VbrTag.o:  	    libmp3lame/VbrTag.c
$(OBJDIR)/libmp3lame/bitstream.o: 	    libmp3lame/bitstream.c
$(OBJDIR)/libmp3lame/encoder.o: 	    libmp3lame/encoder.c
$(OBJDIR)/libmp3lame/fft.o: 		    libmp3lame/fft.c
$(OBJDIR)/libmp3lame/id3tag.o:  	    libmp3lame/id3tag.c
$(OBJDIR)/libmp3lame/lame.o: 		    libmp3lame/lame.c
$(OBJDIR)/libmp3lame/mpglib_interface.o:    libmp3lame/mpglib_interface.c
$(OBJDIR)/libmp3lame/newmdct.o: 	    libmp3lame/newmdct.c
$(OBJDIR)/libmp3lame/pcm.o: 		    libmp3lame/pcm.c
$(OBJDIR)/libmp3lame/psymodel.o: 	    libmp3lame/psymodel.c
$(OBJDIR)/libmp3lame/quantize.o: 	    libmp3lame/quantize.c
$(OBJDIR)/libmp3lame/quantize_pvt.o: 	    libmp3lame/quantize_pvt.c
$(OBJDIR)/libmp3lame/reservoir.o: 	    libmp3lame/reservoir.c
$(OBJDIR)/libmp3lame/set_get.o: 	    libmp3lame/set_get.c
$(OBJDIR)/libmp3lame/tables.o:  	    libmp3lame/tables.c
$(OBJDIR)/libmp3lame/takehiro.o: 	    libmp3lame/takehiro.c
$(OBJDIR)/libmp3lame/util.o: 		    libmp3lame/util.c
$(OBJDIR)/libmp3lame/vbrquantize.o: 	    libmp3lame/vbrquantize.c
$(OBJDIR)/libmp3lame/version.o: 	    libmp3lame/version.c
$(OBJDIR)/libmp3lame/vorbis_interface.o:    libmp3lame/vorbis_interface.c
























