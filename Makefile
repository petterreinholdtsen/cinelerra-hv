#### THE MASTER MAKEFILE
OBJDIR := $(shell uname --machine)

FREETYPE_DIR := $(shell expr freetype* )

ifeq ($(HAVE_AVIFILE), yes)
AVIFILE_DIR := $(shell expr avifile* )
endif

include global_config

DIRS += \
	$(AVIFILE_DIR) \
	libmpeg3 \
	tiff \
	esound \
	$(FREETYPE_DIR) \
	libsndfile \
	quicktime \
	guicast \
	cinelerra \
	plugins


DISTNAME = bcast2000demo.tar
SOURCENAME = heroinesrc.tar
DISTOBJS = \
	$(INSTDIR)/bcast/bcast2000 \
	$(INSTDIR)/plugins/*.plugin



all: $(OBJDIR)/ipc $(OBJDIR)/soundtest
	make -f build/Makefile.libavc
	@ for i in $(DIRS) ; \
	do \
		 $(MAKE) -C $$i ; \
	done

$(OBJDIR)/ipc: $(OBJDIR)/ipc.o
	$(CC) -o $(OBJDIR)/ipc $(OBJDIR)/ipc.o -lm

$(OBJDIR)/soundtest: $(OBJDIR)/soundtest.o
	$(CC) -o $(OBJDIR)/soundtest $(OBJDIR)/soundtest.o -lm

root:
	cd .. && \
	tar cvf cinelerra.tar \
		cinelerra/COPYING \
		cinelerra/Makefile \
		cinelerra/README \
		cinelerra/global_config \
		cinelerra/ipc.c \
		cinelerra/libmpeg3 \
		cinelerra/plugin_config \
		cinelerra/quicktime \
		cinelerra/soundtest.c \
		cinelerra/swab.h

backup: clean
	cd .. && \
	tar Icvf cinelerra.tar.bz2 cinelerra

clean:
	find -follow \( -name core \
		-o -name '*.lo' \
		-o -name '*.o' \
		-o -name '*.la' \
		-o -name '*.a' \
		-o -name '*.plugin' \
		-o -name '*.so' \) -exec rm -f {} \;
	find -follow \( -name $(OBJDIR) \) -exec rm -rf {} \;

wc:
	cat *.C *.h | wc
	@ for i in $(DIRS) ; \
	do \
		 $(MAKE) -C $$i wc; \
	done

# From make_packages
install:
	make -C cinelerra install
	make -C plugins install
	make -C libmpeg3 install

COMPILE = $(GCC) -c $(CFLAGS) $? -o $*.o

$(OBJDIR)/ipc.o: ipc.c
	$(COMPILE)
$(OBJDIR)/soundtest.o: soundtest.c
	$(COMPILE)
