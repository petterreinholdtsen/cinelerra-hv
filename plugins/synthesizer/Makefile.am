plugin_LTLIBRARIES = synthesizer.la
synthesizer_la_LDFLAGS = -avoid-version -module -shared 
synthesizer_la_LIBADD = 
synthesizer_la_SOURCES = synthesizer.C 
AM_CXXFLAGS = $(LARGEFILE_CFLAGS)

AM_CPPFLAGS = -I$(top_srcdir)/guicast -I$(top_srcdir)/cinelerra -I$(top_srcdir)/quicktime
LIBTOOL = $(SHELL) $(top_builddir)/libtool $(LTCXX_FLAGS)

noinst_HEADERS = \
	black_checkedhi_png.h \
	black_checked_png.h \
	black_dn_png.h \
	black_hi_png.h \
	black_up_png.h \
	picon_png.h \
	synthesizer.h \
	white_checkedhi_png.h \
	white_checked_png.h \
	white_dn_png.h \
	white_hi_png.h \
	white_up_png.h
EXTRA_DIST = \
	black_checkedhi.png \
	black_checked.png \
	black_dn.png \
	black_hi.png \
	black_up.png \
	keyboard.xcf.bz2 \
	picon.png \
	white_checkedhi.png \
	white_checked.png \
	white_dn.png \
	white_hi.png \
	white_up.png
