HAVE_LIBXXF86VM := $(shell if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then echo yes; fi )
ALSA_DIR := $(shell expr ../alsa-lib* )

include ../global_config

CFLAGS += -I../guicast \
	-I../quicktime \
	-I$(ALSA_DIR)/include

PREFIX=/usr

ifeq ($(HAVE_LIBXXF86VM), yes)
CFLAGS += -DHAVE_LIBXXF86VM
XXF86VM = -lXxf86vm
endif


OBJS := \
	$(OBJDIR)/audiodriver.o \
	$(OBJDIR)/audioalsa.o \
	$(OBJDIR)/audiooss.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/mixer.o \
	$(OBJDIR)/mixergui.o \
	$(OBJDIR)/mixerpopup.o \
	$(OBJDIR)/mixertree.o

LIBS := -L/usr/X11R6/lib \
	-lGL -lX11 -lXext -lXv -lpthread -lglib -lm -lpng -lz -ldl $(XXF86VM)

OUTPUT = $(OBJDIR)/mix2000

$(OUTPUT): $(OBJS)
	$(CC) -o $(OUTPUT) \
		$(OBJS) \
		../guicast/$(OBJDIR)/libguicast.a \
		../quicktime/$(OBJDIR)/libquicktime.a \
		$(LIBS)

$(OBJS):
	$(CC) -c $(CFLAGS) $(subst $(OBJDIR)/,, $*.C) -o $*.o

install:
	rm -f $(PREFIX)/bin/mix2000
	cp $(OUTPUT) $(PREFIX)/bin


clean:
	find -follow \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;
	rm -rf $(OBJDIR)

wc:
	cat *.C *.h | wc

$(OBJDIR)/audiodriver.o: audiodriver.C
$(OBJDIR)/audioalsa.o:	 audioalsa.C
$(OBJDIR)/audiooss.o:	 audiooss.C
$(OBJDIR)/main.o:	 main.C
$(OBJDIR)/mixer.o:	 mixer.C
$(OBJDIR)/mixergui.o:	 mixergui.C
$(OBJDIR)/mixerpopup.o:  mixerpopup.C
$(OBJDIR)/mixertree.o:   mixertree.C







