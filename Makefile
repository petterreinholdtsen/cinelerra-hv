#### THE MASTER MAKEFILE
OBJDIR := $(shell uname --machine)

FREETYPE_DIR := $(shell expr freetype* )

ifeq ($(HAVE_AVIFILE), yes)
AVIFILE_DIR := $(shell expr avifile* )
endif

include global_config

DIRS := \
	$(AVIFILE_DIR) \
	doc \
	libmpeg3 \
	tiff \
	esound \
	$(FREETYPE_DIR) \
	libsndfile \
	mplexhi \
	mplexlo \
	quicktime \
	guicast \
	cinelerra \
	plugins \
	mix \
	xmovie \


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

backup: clean
	cd .. && \
	tar Icvf hvirtual.tar.bz2 hvirtual

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
	make -C mix install
	make -C xmovie install
	make -C mplexhi install
	make -C mplexlo install

COMPILE = $(GCC) -c $(CFLAGS) $? -o $*.o

$(OBJDIR)/ipc.o: ipc.c
	$(COMPILE)
$(OBJDIR)/soundtest.o: soundtest.c
	$(COMPILE)
