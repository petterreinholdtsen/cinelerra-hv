plugin_LTLIBRARIES = removegaps.la
removegaps_la_LDFLAGS = -avoid-version -module -shared 
removegaps_la_LIBADD = 
removegaps_la_SOURCES = removegaps.C 
AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = removegaps.h 
