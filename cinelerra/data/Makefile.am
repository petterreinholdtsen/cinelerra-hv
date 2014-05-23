noinst_LIBRARIES = libimagedata.a
libimagedata_a_SOURCES =
libimagedata_a_LIBADD = theme.$(OBJEXT)
nodist_libimagedata_a_SOURCES = theme.$(OBJEXT)

PNGS = \
	mode_add.png \
	mode_divide.png \
	mode_multiply.png \
	mode_normal.png \
	mode_replace.png \
	mode_subtract.png

theme.$(OBJEXT): $(PNGS)
	$(top_builddir)/guicast/bootstrap $@ $^ || { rm -f $@; exit 1; }

clean-local:
	rm -f theme theme.$(OBJEXT)

EXTRA_DIST = \
	$(PNGS) \
	about.png \
	about.xcf.bz2 \
	about_bg.png \
	about_bg_png.h \
	about_png.h \
	fake_ntsc.dv \
	fake_ntsc_dv.h \
	fake_pal.dv \
	fake_pal_dv.h \
	heroine_logo10.png \
	heroine_logo10_png.h \
	heroine_logo11.png \
	heroine_logo11_png.h \
	heroine_logo9.png \
	heroine_logo9_png.h \
	lad_picon.png \
	lad_picon.xcf.bz2 \
	lad_picon_png.h \
	modes.xcf.bz2
