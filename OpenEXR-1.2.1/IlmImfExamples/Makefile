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

#noinst_PROGRAMS = imfexamples

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/Iex            -I$(top_srcdir)/Half -I$(top_srcdir)/Imath            -I$(top_srcdir)/IlmImf


LDADD = -L$(top_builddir)/Iex -L$(top_builddir)/Half 	-L$(top_builddir)/Imath -L$(top_builddir)/IlmImf 	-lIlmImf -lImath -lHalf -lIex -lz


imfexamples_SOURCES = main.cpp drawImage.cpp rgbaInterfaceExamples.cpp 		      rgbaInterfaceTiledExamples.cpp 		      generalInterfaceExamples.cpp 		      lowLevelIoExamples.cpp previewImageExamples.cpp 		      generalInterfaceTiledExamples.cpp 	              generalInterfaceTiledExamples.h drawImage.h 		      rgbaInterfaceExamples.h generalInterfaceExamples.h 		      rgbaInterfaceTiledExamples.h 		      lowLevelIoExamples.h previewImageExamples.h


examplesdir = $(datadir)/doc/OpenEXR-1.2.1/examples
examples_DATA = $(imfexamples_SOURCES)

imfexamplesdir = $(examplesdir)
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
#noinst_PROGRAMS =  imfexamples$(EXEEXT)
PROGRAMS =  $(noinst_PROGRAMS)


DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"OpenEXR\" -DVERSION=\"1.2.1\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STRERROR=1 -DHAVE_IOS_BASE=1 -DHAVE_STL_LIMITS=1  -I. -I$(srcdir) 
CPPFLAGS = 
LDFLAGS = 
LIBS = 
imfexamples_OBJECTS =  main.$(OBJEXT) drawImage.$(OBJEXT) \
rgbaInterfaceExamples.$(OBJEXT) rgbaInterfaceTiledExamples.$(OBJEXT) \
generalInterfaceExamples.$(OBJEXT) lowLevelIoExamples.$(OBJEXT) \
previewImageExamples.$(OBJEXT) generalInterfaceTiledExamples.$(OBJEXT)
imfexamples_LDADD = $(LDADD)
imfexamples_DEPENDENCIES = 
imfexamples_LDFLAGS = 
CXXFLAGS = -g -O2
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@
CFLAGS = -O3 -march=athlon -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -falign-loops=2 -falign-jumps=2 -falign-functions=2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DATA =  $(examples_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
DEP_FILES =  .deps/drawImage.P .deps/generalInterfaceExamples.P \
.deps/generalInterfaceTiledExamples.P .deps/lowLevelIoExamples.P \
.deps/main.P .deps/previewImageExamples.P .deps/rgbaInterfaceExamples.P \
.deps/rgbaInterfaceTiledExamples.P
SOURCES = $(imfexamples_SOURCES)
OBJECTS = $(imfexamples_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .cpp .lo .o .obj .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.ac $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu IlmImfExamples/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

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

imfexamples$(EXEEXT): $(imfexamples_OBJECTS) $(imfexamples_DEPENDENCIES)
	@rm -f imfexamples$(EXEEXT)
	$(CXXLINK) $(imfexamples_LDFLAGS) $(imfexamples_OBJECTS) $(imfexamples_LDADD) $(LIBS)
.cpp.o:
	$(CXXCOMPILE) -c $<
.cpp.obj:
	$(CXXCOMPILE) -c `cygpath -w $<`
.cpp.lo:
	$(LTCXXCOMPILE) -c $<

install-examplesDATA: $(examples_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(examplesdir)
	@list='$(examples_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(examplesdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(examplesdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(examplesdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(examplesdir)/$$p; \
	  fi; fi; \
	done

uninstall-examplesDATA:
	@$(NORMAL_UNINSTALL)
	list='$(examples_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(examplesdir)/$$p; \
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
	  || (cd $(srcdir) && etags -o $$here/TAGS $(ETAGS_ARGS) $$tags  $$unique $(LISP))

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = IlmImfExamples

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu IlmImfExamples/Makefile
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

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

-include $(DEP_FILES)

mostlyclean-depend:

clean-depend:

distclean-depend:
	-rm -rf .deps

maintainer-clean-depend:

%.o: %.c
	@echo '$(COMPILE) -c $<'; \
	$(COMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.c
	@echo '$(LTCOMPILE) -c $<'; \
	$(LTCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp

%.o: %.cpp
	@echo '$(CXXCOMPILE) -c $<'; \
	$(CXXCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.cpp
	@echo '$(LTCXXCOMPILE) -c $<'; \
	$(LTCXXCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp
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

install-data-am: install-examplesDATA
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-examplesDATA
uninstall: uninstall-am
all-am: Makefile $(PROGRAMS) $(DATA)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(examplesdir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-noinstPROGRAMS mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstPROGRAMS clean-compile clean-libtool clean-tags \
		clean-depend clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstPROGRAMS distclean-compile \
		distclean-libtool distclean-tags distclean-depend \
		distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstPROGRAMS \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstPROGRAMS distclean-noinstPROGRAMS \
clean-noinstPROGRAMS maintainer-clean-noinstPROGRAMS \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool uninstall-examplesDATA \
install-examplesDATA tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir mostlyclean-depend distclean-depend \
clean-depend maintainer-clean-depend info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
