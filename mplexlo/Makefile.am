bin_PROGRAMS=mplexlo-hv

AM_CPPFLAGS=-I$(top_srcdir)/libmpeg3
AM_CFLAGS = $(LARGEFILE_CFLAGS)
mplexlo_hv_LDADD=../libmpeg3/libmpeg3hv.la
mplexlo_hv_LDFLAGS=-lm -lpthread
mplexlo_hv_SOURCES=mplex.c
