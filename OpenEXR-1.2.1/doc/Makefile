# Makefile.in generated automatically by automake 1.4-p6 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
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

pkgdatadir = $(datadir)/OpenEXR
pkglibdir = $(libdir)/OpenEXR
pkgincludedir = $(includedir)/OpenEXR

top_builddir = ..

ACLOCAL = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run aclocal-1.7
AUTOCONF = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run autoconf
AUTOMAKE = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run automake-1.7
AUTOHEADER = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu
AMDEPBACKSLASH = \
AMTAR = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run tar
AM_CFLAGS = -pipe -march=i686
AM_CXXFLAGS = -pipe -march=i686
AR = ar
AS = @AS@
AWK = gawk
CC = gcc
CXX = g++
CXXCPP = g++ -E
CYGPATH_W = echo
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
ECHO = echo
EGREP = grep -E
EXEEXT = 
F77 = g77
FLTK_CONFIG = no
FLTK_CXXFLAGS = 
FLTK_LDFLAGS = 
GCJ = @GCJ@
GCJFLAGS = @GCJFLAGS@
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBTOOL_VERSION = 2:1:0
LN_S = ln -s
MAINT = #
MAKEINFO = ${SHELL} /root/hvirtual/OpenEXR-1.2.1/missing --run makeinfo
NVSDK_CXXFLAGS = 
OBJDUMP = @OBJDUMP@
OBJEXT = o
OPENEXR_VERSION = 1.2.1
PACKAGE = OpenEXR
RANLIB = ranlib
RC = @RC@
STRIP = strip
VERSION = 1.2.1
am__include = include
am__leading_dot = .
am__quote = 
install_sh = /root/hvirtual/OpenEXR-1.2.1/install-sh

EXTRA_DIST = api.html details.html images/screenwin.png images/tiles.png 	     images/windowExample1.png images/windowExample2.png 	     images/cubeMap.png images/latlongMap.png


DOC_DIR = $(datadir)/doc/OpenEXR
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
all: all-redirect
.SUFFIXES:
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.ac $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu doc/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = doc

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu doc/Makefile
	$(mkinstalldirs) $(distdir)/images
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
all-am: Makefile
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: tags distdir info-am info dvi-am dvi check check-am \
installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
