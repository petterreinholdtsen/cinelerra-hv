# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4d from Makefile.am

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
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

pkgdatadir = $(datadir)/lame
pkglibdir = $(libdir)/lame
pkgincludedir = $(includedir)/lame

top_builddir = ..

ACLOCAL = ${SHELL} /amazon/root/cinelerra/lame/missing --run aclocal
AUTOCONF = ${SHELL} /amazon/root/cinelerra/lame/missing --run autoconf
AUTOMAKE = ${SHELL} /amazon/root/cinelerra/lame/missing --run automake
AUTOHEADER = ${SHELL} /amazon/root/cinelerra/lame/missing --run autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
INSTALL_STRIP_FLAG =
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :


build_alias = i686-pc-linux-gnu
build_triplet = i686-pc-linux-gnu
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
target_alias = i686-pc-linux-gnu
target_triplet = i686-pc-linux-gnu
AMDEP = 
AMTAR = ${SHELL} /amazon/root/cinelerra/lame/missing --run tar
AS = @AS@
AWK = gawk
CC = gcc
CONFIG_DEFS = 
CPP = cc -E
CPUTYPE = i386
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
EXEEXT = 
FRONTEND_CFLAGS = 
FRONTEND_LDADD = -lncurses 
FRONTEND_LDFLAGS = 
GTK_CFLAGS = -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include -I/usr/X11R6/include
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS = -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBTOOL_DEPS = ./ltconfig ./ltmain.sh
LIB_MAJOR_VERSION = 0
LIB_MINOR_VERSION = 0
LN_S = ln -s
MAINT = #
MAKEDEP = -M
MAKEINFO = ${SHELL} /amazon/root/cinelerra/lame/missing --run makeinfo
NASM = no
NASM_FORMAT = -f elf
OBJDUMP = @OBJDUMP@
PACKAGE = lame
RANLIB = ranlib
RM_F = rm -f
U = 
VERSION = 3.89
WITH_MP3RTP = 
WITH_MP3X = 
_am_include = include
install_sh = ./install-sh


# global section for every Makefile.am


AUTOMAKE_OPTIONS = 1.4d foreign $(top_srcdir)/ansi2knr

bin_PROGRAMS = lame  
EXTRA_PROGRAMS = mp3rtp mp3x

brhist_sources = brhist.c brhist.h

EXTRA_DIST = \
	$(brhist_sources) \
	console.c \
	console.h \
	lame.dsp \
	lame.dsw \
	mp3x.dsp \
	mp3x.dsw \
	amiga_mpega.c


DEFS = -DHAVE_CONFIG_H 

common_sources = \
	get_audio.c \
	lametime.c \
	parse.c \
	portableio.c \
	timestatus.c


noinst_HEADERS = get_audio.h \
	gtkanal.h \
	gpkplotting.h \
	lametime.h \
	main.h \
	parse.h \
	portableio.h \
	timestatus.h


EXTRA_lame_SOURCES = $(brhist_sources)
EXTRA_mp3rtp_SOURCES = $(brhist_sources)
EXTRA_mp3x_SOURCES = $(brhist_sources)
lame_SOURCES = main.c $(common_sources) $(brhist_sources)
#lame_SOURCES = #main.c $(common_sources)
mp3rtp_SOURCES = mp3rtp.c rtp.c rtp.h $(common_sources) $(brhist_sources)
#mp3rtp_SOURCES = #mp3rtp.c rtp.c rtp.h $(common_sources)
mp3x_SOURCES = mp3x.c gtkanal.c gpkplotting.c \
	$(common_sources) $(brhist_sources)
#mp3x_SOURCES = #mp3x.c gtkanal.c gpkplotting.c $(common_sources)

CFLAGS = -O3 -fomit-frame-pointer -ffast-math -funroll-loops -Wall -pipe -O3 -march=i686 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2 -I/usr/local/include -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include -I/usr/X11R6/include 
LDFLAGS =   -static

INCLUDES = -I$(top_srcdir)/libmp3lame -I$(top_srcdir)/include -I$(top_builddir)

LDADD = -lm  -lncurses  \
	$(top_builddir)/libmp3lame/libmp3lame.la


mp3x_LDADD = $(LDADD) -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm

CLEANFILES = lclint.txt $(EXTRA_PROGRAMS)

LCLINTFLAGS = \
	+posixlib \
	+showsummary \
	+showalluses \
	+whichlib \
	+forcehints \
	-fixedformalarray \
	+matchanyintegral \
	-Dlint

subdir = frontend
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
EXTRA_PROGRAMS =  mp3rtp$(EXEEXT) mp3x$(EXEEXT)
bin_PROGRAMS =  lame$(EXEEXT)  
PROGRAMS =  $(bin_PROGRAMS)

CPPFLAGS = 
LIBS = 
ANSI2KNR = $(top_srcdir)/ansi2knr
#am_lame_OBJECTS =  main$U.o get_audio$U.o \
#lametime$U.o parse$U.o portableio$U.o timestatus$U.o
am_lame_OBJECTS =  main$U.o get_audio$U.o lametime$U.o \
parse$U.o portableio$U.o timestatus$U.o brhist$U.o
lame_OBJECTS =  $(am_lame_OBJECTS)
lame_LDADD = $(LDADD)
lame_DEPENDENCIES =  $(top_builddir)/libmp3lame/libmp3lame.la
lame_LDFLAGS = 
#am_mp3rtp_OBJECTS =  mp3rtp$U.o rtp$U.o get_audio$U.o \
#lametime$U.o parse$U.o portableio$U.o timestatus$U.o
am_mp3rtp_OBJECTS =  mp3rtp$U.o rtp$U.o get_audio$U.o \
lametime$U.o parse$U.o portableio$U.o timestatus$U.o \
brhist$U.o
mp3rtp_OBJECTS =  $(am_mp3rtp_OBJECTS)
mp3rtp_LDADD = $(LDADD)
mp3rtp_DEPENDENCIES =  $(top_builddir)/libmp3lame/libmp3lame.la
mp3rtp_LDFLAGS = 
#am_mp3x_OBJECTS =  mp3x$U.o gtkanal$U.o \
#gpkplotting$U.o get_audio$U.o lametime$U.o parse$U.o \
#portableio$U.o timestatus$U.o
am_mp3x_OBJECTS =  mp3x$U.o gtkanal$U.o \
gpkplotting$U.o get_audio$U.o lametime$U.o parse$U.o \
portableio$U.o timestatus$U.o brhist$U.o
mp3x_OBJECTS =  $(am_mp3x_OBJECTS)
mp3x_DEPENDENCIES =  $(top_builddir)/libmp3lame/libmp3lame.la
mp3x_LDFLAGS = 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(lame_SOURCES) $(EXTRA_lame_SOURCES) $(mp3rtp_SOURCES) \
$(EXTRA_mp3rtp_SOURCES) $(mp3x_SOURCES) $(EXTRA_mp3x_SOURCES)
HEADERS =  $(noinst_HEADERS)

depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES =  $(DEPDIR)/brhist$U.Po $(DEPDIR)/get_audio$U.Po \
$(DEPDIR)/gpkplotting$U.Po $(DEPDIR)/gtkanal$U.Po \
$(DEPDIR)/lametime$U.Po $(DEPDIR)/main$U.Po \
$(DEPDIR)/mp3rtp$U.Po $(DEPDIR)/mp3x$U.Po $(DEPDIR)/parse$U.Po \
$(DEPDIR)/portableio$U.Po $(DEPDIR)/rtp$U.Po \
$(DEPDIR)/timestatus$U.Po
DIST_COMMON =  $(noinst_HEADERS) Makefile.am Makefile.in depcomp


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
SOURCES = $(lame_SOURCES) $(EXTRA_lame_SOURCES) $(mp3rtp_SOURCES) $(EXTRA_mp3rtp_SOURCES) $(mp3x_SOURCES) $(EXTRA_mp3x_SOURCES)
OBJECTS = $(am_lame_OBJECTS) $(am_mp3rtp_OBJECTS) $(am_mp3x_OBJECTS)

all: all-am
.SUFFIXES:
.SUFFIXES: .c .h .lo .o
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) $(top_srcdir)/Makefile.am.global
	cd $(top_srcdir) && $(AUTOMAKE) --foreign frontend/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status


mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    f="`echo $$p|sed -e 's/$(EXEEXT)$$//' -e '$(transform)' -e 's/$$/$(EXEEXT)/'`"; \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(bindir)/$$f"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(bindir)/$$f; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f="`echo $$p|sed -e 's/$(EXEEXT)$$//' -e '$(transform)' -e 's/$$/$(EXEEXT)/'`"; \
	  echo " rm -f $(DESTDIR)$(bindir)/$$f"; \
	  rm -f $(DESTDIR)$(bindir)/$$f; \
	done

mostlyclean-compile:
	-rm -f *.o core *.core

distclean-compile:
	-rm -f *.tab.c

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
$(top_srcdir)/ansi2knr: $(top_srcdir)/ansi2knr.o
	cd $(top_srcdir) && $(MAKE) $(AM_MAKEFLAGS) ansi2knr

$(top_srcdir)/ansi2knr.o:
	cd $(top_srcdir) && $(MAKE) $(AM_MAKEFLAGS) ansi2knr.o


mostlyclean-kr:
	-rm -f *_.c

lame$(EXEEXT): $(lame_OBJECTS) $(lame_DEPENDENCIES)
	@rm -f lame$(EXEEXT)
	$(LINK) $(lame_LDFLAGS) $(lame_OBJECTS) $(lame_LDADD) $(LIBS)

mp3rtp$(EXEEXT): $(mp3rtp_OBJECTS) $(mp3rtp_DEPENDENCIES)
	@rm -f mp3rtp$(EXEEXT)
	$(LINK) $(mp3rtp_LDFLAGS) $(mp3rtp_OBJECTS) $(mp3rtp_LDADD) $(LIBS)

mp3x$(EXEEXT): $(mp3x_OBJECTS) $(mp3x_DEPENDENCIES)
	@rm -f mp3x$(EXEEXT)
	$(LINK) $(mp3x_LDFLAGS) $(mp3x_OBJECTS) $(mp3x_LDADD) $(LIBS)
brhist_.c: brhist.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/brhist.c; then echo $(srcdir)/brhist.c; else echo brhist.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > brhist_.c
get_audio_.c: get_audio.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/get_audio.c; then echo $(srcdir)/get_audio.c; else echo get_audio.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > get_audio_.c
gpkplotting_.c: gpkplotting.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/gpkplotting.c; then echo $(srcdir)/gpkplotting.c; else echo gpkplotting.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > gpkplotting_.c
gtkanal_.c: gtkanal.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/gtkanal.c; then echo $(srcdir)/gtkanal.c; else echo gtkanal.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > gtkanal_.c
lametime_.c: lametime.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/lametime.c; then echo $(srcdir)/lametime.c; else echo lametime.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > lametime_.c
main_.c: main.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/main.c; then echo $(srcdir)/main.c; else echo main.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > main_.c
mp3rtp_.c: mp3rtp.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/mp3rtp.c; then echo $(srcdir)/mp3rtp.c; else echo mp3rtp.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > mp3rtp_.c
mp3x_.c: mp3x.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/mp3x.c; then echo $(srcdir)/mp3x.c; else echo mp3x.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > mp3x_.c
parse_.c: parse.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/parse.c; then echo $(srcdir)/parse.c; else echo parse.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > parse_.c
portableio_.c: portableio.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/portableio.c; then echo $(srcdir)/portableio.c; else echo portableio.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > portableio_.c
rtp_.c: rtp.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/rtp.c; then echo $(srcdir)/rtp.c; else echo rtp.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > rtp_.c
timestatus_.c: timestatus.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/timestatus.c; then echo $(srcdir)/timestatus.c; else echo timestatus.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > timestatus_.c
brhist_.o brhist_.lo get_audio_.o get_audio_.lo gpkplotting_.o \
gpkplotting_.lo gtkanal_.o gtkanal_.lo lametime_.o lametime_.lo main_.o \
main_.lo mp3rtp_.o mp3rtp_.lo mp3x_.o mp3x_.lo parse_.o parse_.lo \
portableio_.o portableio_.lo rtp_.o rtp_.lo timestatus_.o \
timestatus_.lo : $(ANSI2KNR)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS: $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || etags $(ETAGS_ARGS) $$tags  $$unique $(LISP)

GTAGS:
	here=`CDPATH=: && cd $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $$here

distclean-tags:
	-rm -f TAGS ID

include $(DEPDIR)/brhist$U.Po
include $(DEPDIR)/get_audio$U.Po
include $(DEPDIR)/gpkplotting$U.Po
include $(DEPDIR)/gtkanal$U.Po
include $(DEPDIR)/lametime$U.Po
include $(DEPDIR)/main$U.Po
include $(DEPDIR)/mp3rtp$U.Po
include $(DEPDIR)/mp3x$U.Po
include $(DEPDIR)/parse$U.Po
include $(DEPDIR)/portableio$U.Po
include $(DEPDIR)/rtp$U.Po
include $(DEPDIR)/timestatus$U.Po

distclean-depend:
	-rm -rf $(DEPDIR)

CCDEPMODE = depmode=gcc

.c.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

.c.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LTCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)


distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(distdir) \
	    || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-binPROGRAMS
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-binPROGRAMS
uninstall: uninstall-am
all-am: Makefile $(PROGRAMS) $(HEADERS)
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(bindir)


mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic clean-libtool mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-binPROGRAMS distclean-compile \
	distclean-depend distclean-generic distclean-libtool \
	distclean-tags

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-binPROGRAMS \
	maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-binPROGRAMS mostlyclean-compile \
	mostlyclean-generic mostlyclean-kr mostlyclean-libtool

.PHONY: check check-am clean clean-binPROGRAMS clean-generic \
	clean-libtool distclean distclean-binPROGRAMS distclean-compile \
	distclean-depend distclean-generic distclean-libtool \
	distclean-tags distdir dvi dvi-am info info-am install \
	install-am install-binPROGRAMS install-data install-data-am \
	install-exec install-exec-am install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-binPROGRAMS maintainer-clean-generic \
	mostlyclean mostlyclean-binPROGRAMS mostlyclean-compile \
	mostlyclean-generic mostlyclean-kr mostlyclean-libtool tags \
	uninstall uninstall-am uninstall-binPROGRAMS


# end global section

lclint.txt: ${lame_SOURCES} ${noinst_HEADERS}
	@lclint ${LCLINTFLAGS} ${INCLUDES} ${DEFS} ${lame_SOURCES} 2>&1 >lclint.txt || true

lclint: lclint.txt
	more lclint.txt

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
