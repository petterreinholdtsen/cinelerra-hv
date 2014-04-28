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
INCLUDES = -I$(top_srcdir)/include -I$(srcdir)
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


AUTOMAKE_OPTIONS = foreign ansi2knr

docdir = $(datadir)/doc
pkgdocdir = $(docdir)/$(PACKAGE)
htmldir = $(docdir)/html
pkghtmldir = $(pkgdocdir)/html

pkghtml_DATA = \
        basic.html \
	contributors.html \
	examples.html \
	history.html \
	id3.html \
	index.html \
	lame.css \
	modes.html \
	node6.html \
	switchs.html


EXTRA_DIST = $(pkghtml_DATA)
subdir = doc/html
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
DIST_SOURCES = 
DATA =  $(pkghtml_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
all: all-am
.SUFFIXES:
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign doc/html/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status


install-pkghtmlDATA: $(pkghtml_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkghtmldir)
	@list='$(pkghtml_DATA)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkghtmldir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkghtmldir)/$$f; \
	done

uninstall-pkghtmlDATA:
	@$(NORMAL_UNINSTALL)
	@list='$(pkghtml_DATA)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(pkghtmldir)/$$f"; \
	  rm -f $(DESTDIR)$(pkghtmldir)/$$f; \
	done
tags: TAGS
TAGS:


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

install-data-am: install-pkghtmlDATA
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-pkghtmlDATA
uninstall: uninstall-am
all-am: Makefile $(DATA)
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(pkghtmldir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
clean: clean-am

clean-am: clean-generic mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-generic

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-generic

.PHONY: check check-am clean clean-generic distclean distclean-generic \
	distdir dvi dvi-am info info-am install install-am install-data \
	install-data-am install-exec install-exec-am \
	install-pkghtmlDATA install-strip installcheck installcheck-am \
	installdirs maintainer-clean maintainer-clean-generic \
	mostlyclean mostlyclean-generic uninstall uninstall-am \
	uninstall-pkghtmlDATA


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
