plugin_LTLIBRARIES = interpolatevideo.la
interpolatevideo_la_LDFLAGS = -avoid-version -module -shared 
interpolatevideo_la_LIBADD = 
interpolatevideo_la_SOURCES = interpolatevideo.C interpolatevideo.h motionscan.h

AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = \
	picon_png.h \
	opticflow.h \
	interpolatewindow.h \
	motionscan.h
EXTRA_DIST = \
	picon.png \
	opticflow.C \
	interpolatevideo.inc \
	opticflow.inc \
	interpolatewindow.C \
	interpolatewindow.inc \
	motionscan.C \
	motionscan.h \
	motionscan.inc
