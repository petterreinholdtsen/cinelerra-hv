plugin_LTLIBRARIES = vocoder.la
vocoder_la_LDFLAGS = -avoid-version -module -shared 
vocoder_la_LIBADD = 
vocoder_la_SOURCES = vocoder.C 
AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = picon_png.h vocoder.h 
EXTRA_DIST = picon.png
