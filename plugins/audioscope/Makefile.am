plugin_LTLIBRARIES = audioscope.la
audioscope_la_LDFLAGS = -avoid-version -module -shared 
audioscope_la_LIBADD = 
audioscope_la_SOURCES = audioscope.C  audioscope.h
AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)
