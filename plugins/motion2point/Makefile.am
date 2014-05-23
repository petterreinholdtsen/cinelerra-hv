plugin_LTLIBRARIES = motion2point.la
motion2point_la_LDFLAGS = -avoid-version -module -shared 
motion2point_la_LIBADD = 
motion2point_la_SOURCES = downsampleengine.C motion.C motionscan.C \
	motionwindow.C
AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = \
	downsampleengine.h \
	motion.h \
	motionscan.h \
	motionwindow.h \
	picon_png.h
EXTRA_DIST = \
	downsampleengine.inc \
	motion.inc \
	motionscan.inc \
	motionwindow.inc \
	picon.png
