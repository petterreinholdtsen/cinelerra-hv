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
top_srcdir = ../..
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

top_builddir = ../..

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
LDADD = -lm 
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


AUTOMAKE_OPTIONS = foreign $(top_srcdir)/ansi2knr

DEFS = -DHAVE_CONFIG_H 

nasm_sources = \
	choose_table.nas \
	cpu_feat.nas \
	fft3dn.nas \
	scalar.nas


#noinst_LTLIBRARIES = #liblameasmroutines.la
#liblameasmroutines_la_SOURCES = #$(nasm_sources)
#am_liblameasmroutines_la_OBJECTS = #\
#	choose_table$U.lo \
#	cpu_feat$U.lo \
#	fft3dn$U.lo \
#	scalar$U.lo

noinst_HEADERS = nasm.h

INCLUDES = -I$(top_srcdir)/include -I$(srcdir) -I$(top_srcdir)/libmp3lame/i386

SUFFIXES = .nas .lo

EXTRA_liblameasmroutines_la_SOURCES = $(nasm_sources)

CLEANFILES = \
	choose_table.o.lst \
	choose_table.lo.lst \
	cpu_feat.o.lst \
	cpu_feat.lo.lst \
	fft3dn.o.lst \
	fft3dn.lo.lst \
	scalar.o.lst \
	scalar.lo.lst


EXTRA_DIST = \
	fft.nas \
	fftfpu.nas \
	fftsse.nas \
	ffttbl.nas


NASM = no
NASMFLAGS = -f elf -i $(top_srcdir)/libmp3lame/i386/
subdir = libmp3lame/i386
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)

CPPFLAGS = 
LDFLAGS = 
LIBS = 
ANSI2KNR = $(top_srcdir)/ansi2knr
liblameasmroutines_la_LDFLAGS = 
liblameasmroutines_la_LIBADD = 
liblameasmroutines_la_OBJECTS =  $(am_liblameasmroutines_la_OBJECTS)
CFLAGS = -O3 -fomit-frame-pointer -ffast-math -funroll-loops -Wall -pipe -O3 -march=i686 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2 -I/usr/local/include
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(liblameasmroutines_la_SOURCES) \
$(EXTRA_liblameasmroutines_la_SOURCES)
HEADERS =  $(noinst_HEADERS)

DIST_COMMON =  $(noinst_HEADERS) Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
SOURCES = $(liblameasmroutines_la_SOURCES) $(EXTRA_liblameasmroutines_la_SOURCES)
OBJECTS = $(am_liblameasmroutines_la_OBJECTS)

all: all-am
.SUFFIXES:
.SUFFIXES: .nas .lo .c .o
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign libmp3lame/i386/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status


mostlyclean-noinstLTLIBRARIES:

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)

distclean-noinstLTLIBRARIES:

maintainer-clean-noinstLTLIBRARIES:

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

liblameasmroutines.la: $(liblameasmroutines_la_OBJECTS) $(liblameasmroutines_la_DEPENDENCIES)
	$(LINK)  $(liblameasmroutines_la_LDFLAGS) $(liblameasmroutines_la_OBJECTS) $(liblameasmroutines_la_LIBADD) $(LIBS)

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
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(HEADERS)
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs:


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

clean-am: clean-generic clean-libtool clean-noinstLTLIBRARIES \
	mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-generic \
	distclean-libtool distclean-noinstLTLIBRARIES distclean-tags

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic \
	maintainer-clean-noinstLTLIBRARIES

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic mostlyclean-kr \
	mostlyclean-libtool mostlyclean-noinstLTLIBRARIES

.PHONY: check check-am clean clean-generic clean-libtool \
	clean-noinstLTLIBRARIES distclean distclean-compile \
	distclean-generic distclean-libtool distclean-noinstLTLIBRARIES \
	distclean-tags distdir dvi dvi-am info info-am install \
	install-am install-data install-data-am install-exec \
	install-exec-am install-strip installcheck installcheck-am \
	installdirs maintainer-clean maintainer-clean-generic \
	maintainer-clean-noinstLTLIBRARIES mostlyclean \
	mostlyclean-compile mostlyclean-generic mostlyclean-kr \
	mostlyclean-libtool mostlyclean-noinstLTLIBRARIES tags \
	uninstall uninstall-am


.nas.o: $< nasm.h
	$(NASM) $(NASMFLAGS) $< -o $@ -l $@.lst

.nas.lo: $< nasm.h
	$(NASM) $(NASMFLAGS) $< -o $@ -l $@.lst

#$(OBJECTS): libtool
#libtool: $(LIBTOOL_DEPS)
#	$(SHELL) $(top_builddir)/config.status --recheck

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
