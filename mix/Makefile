HAVE_LIBXXF86VM := $(shell if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then echo yes; fi )

include ../global_config

CFLAGS += -I../guicast -I../quicktime
PREFIX=/usr

ifeq ($(HAVE_LIBXXF86VM), yes)
CFLAGS += -DHAVE_LIBXXF86VM
XXF86VM = -lXxf86vm
endif


OBJS := \
	$(OBJDIR)/main.o \
	$(OBJDIR)/mixer.o

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
	cp $(OUTPUT) $(PREFIX)/bin


clean:
	find -follow \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;
	rm -rf $(OBJDIR)

wc:
	cat *.C *.h | wc

$(OBJDIR)/main.o: main.C
$(OBJDIR)/mixer.o: mixer.C

