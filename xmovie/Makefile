#AVIDIR = ../avifile
#include ../avifile/makefile_objs
include ../global_config

PREFIX=/usr

OBJS = \
	$(OBJDIR)/arender.o \
	$(OBJDIR)/asset.o \
	$(OBJDIR)/audiodevice.o \
	$(OBJDIR)/file.o \
	$(OBJDIR)/fileavi.o \
	$(OBJDIR)/filebase.o \
	$(OBJDIR)/filemov.o \
	$(OBJDIR)/filempeg.o \
	$(OBJDIR)/filesndfile.o \
	$(OBJDIR)/load.o \
	$(OBJDIR)/mainmenu.o \
	$(OBJDIR)/mwindow.o \
	$(OBJDIR)/mwindowgui.o \
	$(OBJDIR)/playbackengine.o \
	$(OBJDIR)/playbackscroll.o \
	$(OBJDIR)/playlist.o \
	$(OBJDIR)/renderengine.o \
	$(OBJDIR)/theme.o \
	$(OBJDIR)/transportque.o \
	$(OBJDIR)/settings.o \
	$(OBJDIR)/vrender.o \
	$(OBJDIR)/xmovie.o

OUTPUT = $(OBJDIR)/xmovie

CFLAGS += -I../guicast \
	-I../quicktime \
	-I../libmpeg3 \
	-I../libsndfile

LIBS += -L/usr/X11R6/lib \
	../libmpeg3/$(OBJDIR)/libmpeg3.a \
	../quicktime/$(OBJDIR)/libquicktime.a \
	../guicast/$(OBJDIR)/libguicast.a \
	../libsndfile/$(OBJDIR)/libsndfile.a \
 	-lGL -lX11 -lXext -lXv -lpthread -lglib -lm -lpng -lz -ldl $(XXF86VM)

# Static libraries for distribution
ifeq ($(USE_STATIC),y)
LIBS += -static
endif

$(shell echo $(OBJS) $(AVIOBJS) $(CXXREPOSITORY) $(LFLAGS) $(LIBS) > $(OBJDIR)/libs )
$(shell echo $(CFLAGS) > $(OBJDIR)/c_flags )

$(OUTPUT): $(OBJS)
	$(CC) -o $(OUTPUT) `cat $(OBJDIR)/libs`


test: test.o
	$(CC) -o $(OBJDIR)/test $(OBJDIR)/test.o $(LFLAGS) $(LIBS)

clean:
	find -follow \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;
	rm -rf $(OBJDIR)

wc:
	cat *.C *.h | wc

install:
	strip $(OUTPUT)
	rm -f $(PREFIX)/bin/xmovie
	cp $(OUTPUT) $(PREFIX)/bin

dist:
	strip $(OUTPUT) && \
	tar zcf $(OUTPUT).tar.gz $(OUTPUT)


COMPILE = $(CC) -c `cat $(OBJDIR)/c_flags` $? -o $*.o

$(OBJDIR)/arender.o: 		       arender.C
	$(COMPILE)
$(OBJDIR)/asset.o: 		       asset.C
	$(COMPILE)
$(OBJDIR)/audiodevice.o: 	       audiodevice.C
	$(COMPILE)
$(OBJDIR)/file.o: 		       file.C
	$(COMPILE)
$(OBJDIR)/fileavi.o: 		       fileavi.C
	$(COMPILE)
$(OBJDIR)/filebase.o: 		       filebase.C
	$(COMPILE)
$(OBJDIR)/filemov.o: 		       filemov.C
	$(COMPILE)
$(OBJDIR)/filempeg.o: 		       filempeg.C
	$(COMPILE)
$(OBJDIR)/filesndfile.o: 	       filesndfile.C
	$(COMPILE)
$(OBJDIR)/load.o: 		       load.C
	$(COMPILE)
$(OBJDIR)/mainmenu.o: 		       mainmenu.C
	$(COMPILE)
$(OBJDIR)/mwindow.o: 		       mwindow.C
	$(COMPILE)
$(OBJDIR)/mwindowgui.o: 	       mwindowgui.C
	$(COMPILE)
$(OBJDIR)/playbackengine.o: 	       playbackengine.C
	$(COMPILE)
$(OBJDIR)/playbackscroll.o: 	       playbackscroll.C
	$(COMPILE)
$(OBJDIR)/playlist.o: 		       playlist.C
	$(COMPILE)
$(OBJDIR)/renderengine.o: 	       renderengine.C
	$(COMPILE)
$(OBJDIR)/theme.o: 		       theme.C
	$(COMPILE)
$(OBJDIR)/transportque.o: 	       transportque.C
	$(COMPILE)
$(OBJDIR)/settings.o: 		       settings.C
	$(COMPILE)
$(OBJDIR)/vrender.o: 		       vrender.C
	$(COMPILE)
$(OBJDIR)/xmovie.o: 		       xmovie.C
	$(COMPILE)

