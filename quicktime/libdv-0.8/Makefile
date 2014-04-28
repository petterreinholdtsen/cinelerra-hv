# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/libdv
pkglibdir = $(libdir)/libdv
pkgincludedir = $(includedir)/libdv

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux
host_triplet = i686-pc-linux-gnu
AS = @AS@
CC = gcc
CFLAGS = -O3 -march=i686 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2 -I/usr/local/include -I . -I .. -Wall -O2 -g -I/usr/lib/glib/include -I/usr/lib/glib/include -I/usr/X11R6/include
DLLTOOL = @DLLTOOL@
GLIB_CFLAGS = -I/usr/lib/glib/include
GLIB_CONFIG = /usr/bin/glib-config
GLIB_LIBS = -L/usr/lib -lglib
GTK_CFLAGS = -I/usr/lib/glib/include -I/usr/X11R6/include
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS = -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MAKEINFO = makeinfo
OBJDUMP = @OBJDUMP@
PACKAGE = libdv
RANLIB = ranlib
RPM_RELEASE = 1
SDL_CFLAGS = 
SDL_CONFIG = 
SDL_LIBS = 
VERSION = 0.8
ac_aux_dir = config

MAINTAINERCLEANFILES = Makefile.in $(AUX_DIST)

CLEANFILES = asmoff.h 

lib_LTLIBRARIES = libdv.la
GASMOFF = gasmoff
#GASMOFF = 

noinst_PROGRAMS = dovlc testvlc testbitstream $(GASMOFF)

#
# If HOST_X86 is set, we build all the x86 asm stuff..
#

BUILT_SOURCES = asmoff.h

libdv_la_ASMS = vlc_x86.S quant_x86.S idct_block_mmx.S dct_block_mmx.S rgbtoyuv.S encode_x86.S transpose_x86.S
libdv_la_ASM_HS = asmoff.h  mmx.h 
gasmoff_SOURCES = gasmoff.c bitstream.c bitstream.h 
#gasmoff_SOURCES = 

#ASMS = 

pkginclude_HEADERS = dv_types.h dv.h 

noinst_HEADERS = YUY2.h      bitstream.h parse.h  rgb.h 	YV12.h   dct.h       idct_248.h  place.h  vlc.h 	quant.h  weighting.h audio.h     rgb.h    audio.h 	encode.h enc_input.h enc_audio_input.h 	  enc_output.h         headers.h 	     util.h   $(libdv_la_ASM_HS)


libdv_la_SOURCES = dv.c dct.c idct_248.c weighting.c quant.c vlc.c place.c 	parse.c bitstream.c YUY2.c YV12.c rgb.c audio.c util.c         encode.c headers.c enc_input.c enc_audio_input.c enc_output.c 	$(libdv_la_ASMS)


libdv_la_LDFLAGS = -version-info 1:0:0

dovlc_SOURCES = dovlc.c 
dovlc_LDADD = libdv.la

testvlc_SOURCES = testvlc.c 
testvlc_LDADD = libdv.la

testbitstream_SOURCES = testbitstream.c bitstream.c bitstream.h
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS = -lpopt -lm -lXv  -L/usr/lib -lglib -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm
libdv_la_LIBADD = 
#libdv_la_OBJECTS =  dv.lo dct.lo idct_248.lo \
#weighting.lo quant.lo vlc.lo place.lo parse.lo \
#bitstream.lo YUY2.lo YV12.lo rgb.lo audio.lo util.lo \
#encode.lo headers.lo enc_input.lo enc_audio_input.lo \
#enc_output.lo
libdv_la_OBJECTS =  dv.lo dct.lo idct_248.lo weighting.lo \
quant.lo vlc.lo place.lo parse.lo bitstream.lo YUY2.lo \
YV12.lo rgb.lo audio.lo util.lo encode.lo headers.lo \
enc_input.lo enc_audio_input.lo enc_output.lo vlc_x86.lo \
quant_x86.lo idct_block_mmx.lo dct_block_mmx.lo \
rgbtoyuv.lo encode_x86.lo transpose_x86.lo
PROGRAMS =  $(noinst_PROGRAMS)

dovlc_OBJECTS =  dovlc.o
dovlc_DEPENDENCIES =  libdv.la
dovlc_LDFLAGS = 
testvlc_OBJECTS =  testvlc.o
testvlc_DEPENDENCIES =  libdv.la
testvlc_LDFLAGS = 
testbitstream_OBJECTS =  testbitstream.o bitstream.o
testbitstream_LDADD = $(LDADD)
testbitstream_DEPENDENCIES = 
testbitstream_LDFLAGS = 
#gasmoff_OBJECTS = 
gasmoff_OBJECTS =  gasmoff.o bitstream.o
gasmoff_LDADD = $(LDADD)
gasmoff_DEPENDENCIES = 
gasmoff_LDFLAGS = 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(noinst_HEADERS) $(pkginclude_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libdv_la_SOURCES) $(dovlc_SOURCES) $(testvlc_SOURCES) $(testbitstream_SOURCES) $(gasmoff_SOURCES)
OBJECTS = $(libdv_la_OBJECTS) $(dovlc_OBJECTS) $(testvlc_OBJECTS) $(testbitstream_OBJECTS) $(gasmoff_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign --include-deps libdv/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libdv.la: $(libdv_la_OBJECTS) $(libdv_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libdv_la_LDFLAGS) $(libdv_la_OBJECTS) $(libdv_la_LIBADD) $(LIBS)

mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

dovlc: $(dovlc_OBJECTS) $(dovlc_DEPENDENCIES)
	@rm -f dovlc
	$(LINK) $(dovlc_LDFLAGS) $(dovlc_OBJECTS) $(dovlc_LDADD) $(LIBS)

testvlc: $(testvlc_OBJECTS) $(testvlc_DEPENDENCIES)
	@rm -f testvlc
	$(LINK) $(testvlc_LDFLAGS) $(testvlc_OBJECTS) $(testvlc_LDADD) $(LIBS)

testbitstream: $(testbitstream_OBJECTS) $(testbitstream_DEPENDENCIES)
	@rm -f testbitstream
	$(LINK) $(testbitstream_LDFLAGS) $(testbitstream_OBJECTS) $(testbitstream_LDADD) $(LIBS)

gasmoff: $(gasmoff_OBJECTS) $(gasmoff_DEPENDENCIES)
	@rm -f gasmoff
	$(LINK) $(gasmoff_LDFLAGS) $(gasmoff_OBJECTS) $(gasmoff_LDADD) $(LIBS)

install-pkgincludeHEADERS: $(pkginclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	@list='$(pkginclude_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; \
	done

uninstall-pkgincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(pkginclude_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(pkgincludedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = libdv

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
YUY2.lo YUY2.o : YUY2.c dv_types.h ../config.h YUY2.h dv_types.h mmx.h
YV12.lo YV12.o : YV12.c dv_types.h ../config.h YV12.h dv_types.h mmx.h
audio.lo audio.o : audio.c dv_types.h ../config.h util.h dv_types.h \
	audio.h
bitstream.lo bitstream.o : bitstream.c ../config.h bitstream.h \
	dv_types.h
dct.lo dct.o : dct.c dv_types.h ../config.h dct.h dv_types.h weighting.h \
	mmx.h
dovlc.o: dovlc.c dv_types.h ../config.h vlc.h dv_types.h bitstream.h \
	bitstream.h
dv.lo dv.o : dv.c dv_types.h ../config.h dv.h dv_types.h encode.h dct.h \
	enc_input.h enc_output.h enc_audio_input.h util.h dct.h audio.h \
	idct_248.h quant.h weighting.h vlc.h bitstream.h parse.h \
	place.h rgb.h YUY2.h YV12.h mmx.h
enc_audio_input.lo enc_audio_input.o : enc_audio_input.c \
	enc_audio_input.h dv_types.h ../config.h
enc_input.lo enc_input.o : enc_input.c enc_input.h dv_types.h \
	../config.h encode.h dct.h dv_types.h enc_input.h enc_output.h \
	enc_audio_input.h dct.h mmx.h
enc_output.lo enc_output.o : enc_output.c enc_audio_input.h dv_types.h \
	../config.h enc_output.h headers.h
encode.lo encode.o : encode.c encode.h dct.h dv_types.h ../config.h \
	enc_input.h enc_output.h enc_audio_input.h idct_248.h quant.h \
	weighting.h vlc.h bitstream.h dv_types.h parse.h place.h mmx.h \
	enc_input.h enc_output.h
gasmoff.o: gasmoff.c dv_types.h ../config.h
headers.lo headers.o : headers.c headers.h
idct_248.lo idct_248.o : idct_248.c dv_types.h ../config.h
parse.lo parse.o : parse.c dv_types.h ../config.h util.h dv_types.h dv.h \
	bitstream.h vlc.h bitstream.h parse.h audio.h
place.lo place.o : place.c dv_types.h ../config.h place.h dv_types.h
quant.lo quant.o : quant.c dv_types.h ../config.h mmx.h quant.h \
	dv_types.h idct_248.h
rgb.lo rgb.o : rgb.c dv_types.h ../config.h
testbitstream.o: testbitstream.c dv_types.h ../config.h bitstream.h
testvlc.o: testvlc.c dv_types.h ../config.h vlc.h dv_types.h bitstream.h
util.lo util.o : util.c util.h dv_types.h ../config.h
vlc.lo vlc.o : vlc.c dv_types.h ../config.h vlc.h dv_types.h bitstream.h
weighting.lo weighting.o : weighting.c dv_types.h ../config.h \
	weighting.h dv_types.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-libLTLIBRARIES
install-exec: install-exec-am

install-data-am: install-pkgincludeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLTLIBRARIES uninstall-pkgincludeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(pkgincludedir)


mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	-test -z "$(BUILT_SOURCES)$(MAINTAINERCLEANFILES)" || rm -f $(BUILT_SOURCES) $(MAINTAINERCLEANFILES)
mostlyclean-am:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-noinstPROGRAMS \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLTLIBRARIES clean-compile clean-libtool \
		clean-noinstPROGRAMS clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLTLIBRARIES distclean-compile \
		distclean-libtool distclean-noinstPROGRAMS \
		distclean-tags distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-noinstPROGRAMS maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool mostlyclean-noinstPROGRAMS \
distclean-noinstPROGRAMS clean-noinstPROGRAMS \
maintainer-clean-noinstPROGRAMS uninstall-pkgincludeHEADERS \
install-pkgincludeHEADERS tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Automake doesn't do dependency tracking for asm
quant_x86.lo vlc_x86.lo: asmoff.h

asmoff.h: gasmoff
	./gasmoff > asmoff.h

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
