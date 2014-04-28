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


# global section for every Makefile.am


AUTOMAKE_OPTIONS = 1.4d foreign $(top_srcdir)/ansi2knr

SUBDIRS = i386

lib_LTLIBRARIES = libmp3lame.la
#nasm_ldadd = #$(top_builddir)/libmp3lame/i386/liblameasmroutines.la
nasm_ldadd = 
decoder_ldadd = $(top_builddir)/mpglib/libmpgdecoder.la
#decoder_ldadd = 

libmp3lame_la_LIBADD = $(nasm_ldadd) $(decoder_ldadd)
libmp3lame_la_LDFLAGS = -version-info 0:0

INCLUDES = -I$(top_srcdir)/include -I$(srcdir) -I$(top_srcdir)/mpglib/ -I$(top_builddir)

DEFS = -DHAVE_CONFIG_H 

EXTRA_DIST = \
	debugscalefac.c \
	libmp3lame.dsp \
	lameerror.h \
	tools.c \
	tools.h


libmp3lame_la_SOURCES = \
        VbrTag.c \
	bitstream.c \
	encoder.c \
	fft.c \
        id3tag.c \
        lame.c \
        newmdct.c \
	pcm.c \
	psymodel.c \
	quantize.c \
	quantize_pvt.c \
	reservoir.c \
	set_get.c \
	tables.c \
	takehiro.c \
	util.c \
	vbrquantize.c \
	version.c \
	mpglib_interface.c \
	vorbis_interface.c


noinst_HEADERS = \
	VbrTag.h \
	bitstream.h \
	encoder.h \
	fft.h \
	id3tag.h \
	l3side.h \
	lame-analysis.h \
	lame_global_flags.h \
	machine.h \
	newmdct.h \
	pcm.h \
	psymodel.h \
	quantize.h  \
	quantize_pvt.h \
	reservoir.h \
	tables.h \
	util.h \
	version.h


CLEANFILES = lclint.txt

LCLINTFLAGS = \
	+posixlib \
	+showsummary \
	+showalluses \
	+whichlib \
	+forcehints \
	-fixedformalarray \
	+matchanyintegral \
	-Dlint

subdir = libmp3lame
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)

CPPFLAGS = 
LDFLAGS = 
LIBS = 
ANSI2KNR = $(top_srcdir)/ansi2knr
#libmp3lame_la_DEPENDENCIES = 
libmp3lame_la_DEPENDENCIES =  \
$(top_builddir)/mpglib/libmpgdecoder.la
##libmp3lame_la_DEPENDENCIES =  \
##$(top_builddir)/libmp3lame/i386/liblameasmroutines.la
#libmp3lame_la_DEPENDENCIES =  \
#$(top_builddir)/libmp3lame/i386/liblameasmroutines.la \
#$(top_builddir)/mpglib/libmpgdecoder.la
am_libmp3lame_la_OBJECTS =  VbrTag$U.lo bitstream$U.lo encoder$U.lo \
fft$U.lo id3tag$U.lo lame$U.lo newmdct$U.lo pcm$U.lo psymodel$U.lo \
quantize$U.lo quantize_pvt$U.lo reservoir$U.lo set_get$U.lo tables$U.lo \
takehiro$U.lo util$U.lo vbrquantize$U.lo version$U.lo \
mpglib_interface$U.lo vorbis_interface$U.lo
libmp3lame_la_OBJECTS =  $(am_libmp3lame_la_OBJECTS)
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CFLAGS = -O3 -fomit-frame-pointer -ffast-math -funroll-loops -Wall -pipe -O3 -march=i686 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2 -I/usr/local/include
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(libmp3lame_la_SOURCES)
HEADERS =  $(noinst_HEADERS)

depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES =  $(DEPDIR)/VbrTag$U.Plo $(DEPDIR)/bitstream$U.Plo \
$(DEPDIR)/encoder$U.Plo $(DEPDIR)/fft$U.Plo \
$(DEPDIR)/id3tag$U.Plo $(DEPDIR)/lame$U.Plo \
$(DEPDIR)/mpglib_interface$U.Plo $(DEPDIR)/newmdct$U.Plo \
$(DEPDIR)/pcm$U.Plo $(DEPDIR)/psymodel$U.Plo \
$(DEPDIR)/quantize$U.Plo $(DEPDIR)/quantize_pvt$U.Plo \
$(DEPDIR)/reservoir$U.Plo $(DEPDIR)/set_get$U.Plo \
$(DEPDIR)/tables$U.Plo $(DEPDIR)/takehiro$U.Plo \
$(DEPDIR)/util$U.Plo $(DEPDIR)/vbrquantize$U.Plo \
$(DEPDIR)/version$U.Plo $(DEPDIR)/vorbis_interface$U.Plo
DIST_COMMON =  $(noinst_HEADERS) Makefile.am Makefile.in depcomp


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
DIST_SUBDIRS =  $(SUBDIRS)
SOURCES = $(libmp3lame_la_SOURCES)
OBJECTS = $(am_libmp3lame_la_OBJECTS)

all: all-recursive
.SUFFIXES:
.SUFFIXES: .c .lo .o
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) $(top_srcdir)/Makefile.am.global
	cd $(top_srcdir) && $(AUTOMAKE) --foreign libmp3lame/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status


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
	    echo " $(LIBTOOL)  --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  echo " $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p"; \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
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

libmp3lame.la: $(libmp3lame_la_OBJECTS) $(libmp3lame_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libmp3lame_la_LDFLAGS) $(libmp3lame_la_OBJECTS) $(libmp3lame_la_LIBADD) $(LIBS)
VbrTag_.c: VbrTag.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/VbrTag.c; then echo $(srcdir)/VbrTag.c; else echo VbrTag.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > VbrTag_.c
bitstream_.c: bitstream.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/bitstream.c; then echo $(srcdir)/bitstream.c; else echo bitstream.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > bitstream_.c
encoder_.c: encoder.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/encoder.c; then echo $(srcdir)/encoder.c; else echo encoder.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > encoder_.c
fft_.c: fft.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/fft.c; then echo $(srcdir)/fft.c; else echo fft.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > fft_.c
id3tag_.c: id3tag.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/id3tag.c; then echo $(srcdir)/id3tag.c; else echo id3tag.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > id3tag_.c
lame_.c: lame.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/lame.c; then echo $(srcdir)/lame.c; else echo lame.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > lame_.c
mpglib_interface_.c: mpglib_interface.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/mpglib_interface.c; then echo $(srcdir)/mpglib_interface.c; else echo mpglib_interface.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > mpglib_interface_.c
newmdct_.c: newmdct.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/newmdct.c; then echo $(srcdir)/newmdct.c; else echo newmdct.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > newmdct_.c
pcm_.c: pcm.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/pcm.c; then echo $(srcdir)/pcm.c; else echo pcm.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > pcm_.c
psymodel_.c: psymodel.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/psymodel.c; then echo $(srcdir)/psymodel.c; else echo psymodel.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > psymodel_.c
quantize_.c: quantize.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/quantize.c; then echo $(srcdir)/quantize.c; else echo quantize.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > quantize_.c
quantize_pvt_.c: quantize_pvt.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/quantize_pvt.c; then echo $(srcdir)/quantize_pvt.c; else echo quantize_pvt.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > quantize_pvt_.c
reservoir_.c: reservoir.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/reservoir.c; then echo $(srcdir)/reservoir.c; else echo reservoir.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > reservoir_.c
set_get_.c: set_get.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/set_get.c; then echo $(srcdir)/set_get.c; else echo set_get.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > set_get_.c
tables_.c: tables.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/tables.c; then echo $(srcdir)/tables.c; else echo tables.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > tables_.c
takehiro_.c: takehiro.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/takehiro.c; then echo $(srcdir)/takehiro.c; else echo takehiro.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > takehiro_.c
util_.c: util.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/util.c; then echo $(srcdir)/util.c; else echo util.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > util_.c
vbrquantize_.c: vbrquantize.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/vbrquantize.c; then echo $(srcdir)/vbrquantize.c; else echo vbrquantize.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > vbrquantize_.c
version_.c: version.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/version.c; then echo $(srcdir)/version.c; else echo version.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > version_.c
vorbis_interface_.c: vorbis_interface.c $(ANSI2KNR)
	$(CPP) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) `if test -f $(srcdir)/vorbis_interface.c; then echo $(srcdir)/vorbis_interface.c; else echo vorbis_interface.c; fi` | sed 's/^# \([0-9]\)/#line \1/' | $(ANSI2KNR) > vorbis_interface_.c
VbrTag_.o VbrTag_.lo bitstream_.o bitstream_.lo encoder_.o encoder_.lo \
fft_.o fft_.lo id3tag_.o id3tag_.lo lame_.o lame_.lo \
mpglib_interface_.o mpglib_interface_.lo newmdct_.o newmdct_.lo pcm_.o \
pcm_.lo psymodel_.o psymodel_.lo quantize_.o quantize_.lo \
quantize_pvt_.o quantize_pvt_.lo reservoir_.o reservoir_.lo set_get_.o \
set_get_.lo tables_.o tables_.lo takehiro_.o takehiro_.lo util_.o \
util_.lo vbrquantize_.o vbrquantize_.lo version_.o version_.lo \
vorbis_interface_.o vorbis_interface_.lo : $(ANSI2KNR)

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.
all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	case "$@" in \
	  distclean-* | maintainer-clean-*) list='$(DIST_SUBDIRS)' ;; \
	  *) list='$(SUBDIRS)' ;; \
	esac; \
	rev=''; for subdir in $$list; do \
	  if test "$$subdir" = "."; then :; else \
	    rev="$$subdir $$rev"; \
	  fi; \
	done; \
	rev="$$rev ."; \
	target=`echo $@ | sed s/-recursive//`; \
	for subdir in $$rev; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS: tags-recursive $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
   if test "$$subdir" = .; then :; else \
	    test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
   fi; \
	done; \
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

include $(DEPDIR)/VbrTag$U.Plo
include $(DEPDIR)/bitstream$U.Plo
include $(DEPDIR)/encoder$U.Plo
include $(DEPDIR)/fft$U.Plo
include $(DEPDIR)/id3tag$U.Plo
include $(DEPDIR)/lame$U.Plo
include $(DEPDIR)/mpglib_interface$U.Plo
include $(DEPDIR)/newmdct$U.Plo
include $(DEPDIR)/pcm$U.Plo
include $(DEPDIR)/psymodel$U.Plo
include $(DEPDIR)/quantize$U.Plo
include $(DEPDIR)/quantize_pvt$U.Plo
include $(DEPDIR)/reservoir$U.Plo
include $(DEPDIR)/set_get$U.Plo
include $(DEPDIR)/tables$U.Plo
include $(DEPDIR)/takehiro$U.Plo
include $(DEPDIR)/util$U.Plo
include $(DEPDIR)/vbrquantize$U.Plo
include $(DEPDIR)/version$U.Plo
include $(DEPDIR)/vorbis_interface$U.Plo

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
	for subdir in $(SUBDIRS); do \
	  if test "$$subdir" = .; then :; else \
	    test -d $(distdir)/$$subdir \
	    || mkdir $(distdir)/$$subdir \
	    || exit 1; \
	    (cd $$subdir && \
	      $(MAKE) $(AM_MAKEFLAGS) top_distdir=../$(top_distdir) distdir=../$(distdir)/$$subdir distdir) \
	      || exit 1; \
	  fi; \
	done
info-am:
info: info-recursive
dvi-am:
dvi: dvi-recursive
check-am: all-am
check: check-recursive
installcheck-am:
installcheck: installcheck-recursive
install-exec-am: install-libLTLIBRARIES
install-exec: install-exec-recursive

install-data-am:
install-data: install-data-recursive

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-recursive
uninstall-am: uninstall-libLTLIBRARIES
uninstall: uninstall-recursive
all-am: Makefile $(LTLIBRARIES) $(HEADERS)
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs: installdirs-recursive
installdirs-am:
	$(mkinstalldirs)  $(DESTDIR)$(libdir)


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
clean: clean-recursive

clean-am: clean-generic clean-libLTLIBRARIES clean-libtool \
	mostlyclean-am

distclean: distclean-recursive

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-libLTLIBRARIES distclean-libtool \
	distclean-tags

maintainer-clean: maintainer-clean-recursive

maintainer-clean-am: distclean-am maintainer-clean-generic \
	maintainer-clean-libLTLIBRARIES

mostlyclean: mostlyclean-recursive

mostlyclean-am: mostlyclean-compile mostlyclean-generic mostlyclean-kr \
	mostlyclean-libLTLIBRARIES mostlyclean-libtool

.PHONY: all-recursive check check-am check-recursive clean clean-generic \
	clean-libLTLIBRARIES clean-libtool clean-recursive distclean \
	distclean-compile distclean-depend distclean-generic \
	distclean-libLTLIBRARIES distclean-libtool distclean-recursive \
	distclean-tags distdir dvi dvi-am dvi-recursive info info-am \
	info-recursive install install-am install-data install-data-am \
	install-data-recursive install-exec install-exec-am \
	install-exec-recursive install-libLTLIBRARIES install-recursive \
	install-strip installcheck installcheck-am \
	installcheck-recursive installdirs installdirs-am \
	installdirs-recursive maintainer-clean maintainer-clean-generic \
	maintainer-clean-libLTLIBRARIES maintainer-clean-recursive \
	mostlyclean mostlyclean-compile mostlyclean-generic \
	mostlyclean-kr mostlyclean-libLTLIBRARIES mostlyclean-libtool \
	mostlyclean-recursive tags tags-recursive uninstall \
	uninstall-am uninstall-libLTLIBRARIES uninstall-recursive


# end global section

lclint.txt: ${libmp3lame_la_SOURCES} ${noinst_HEADERS}
	@lclint ${LCLINTFLAGS} ${INCLUDES} ${DEFS} ${libmp3lame_la_SOURCES} 2>&1 >lclint.txt || true

lclint: lclint.txt
	more lclint.txt

#$(OBJECTS): libtool
#libtool: $(LIBTOOL_DEPS)
#	$(SHELL) $(top_builddir)/config.status --recheck

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
