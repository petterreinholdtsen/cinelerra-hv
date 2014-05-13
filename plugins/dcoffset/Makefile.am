plugin_LTLIBRARIES = dcoffset.la
dcoffset_la_LDFLAGS = -avoid-version -module -shared 
dcoffset_la_LIBADD = 
dcoffset_la_SOURCES = dcoffset.C 

AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = picon_png.h dcoffset.h 
EXTRA_DIST = picon.png
