include ../global_config

$(shell sh -c 'if ! test -d $(OBJDIR)\; then mkdir $(OBJDIR)\; fi' )

OBJS = \
	$(OBJDIR)/tif_aux.o \
	$(OBJDIR)/tif_close.o \
	$(OBJDIR)/tif_codec.o \
	$(OBJDIR)/tif_compress.o \
	$(OBJDIR)/tif_dir.o \
	$(OBJDIR)/tif_dirinfo.o \
	$(OBJDIR)/tif_dirread.o \
	$(OBJDIR)/tif_dirwrite.o \
	$(OBJDIR)/tif_dumpmode.o \
	$(OBJDIR)/tif_error.o \
	$(OBJDIR)/tif_fax3.o \
	$(OBJDIR)/tif_fax3sm.o \
	$(OBJDIR)/tif_getimage.o \
	$(OBJDIR)/tif_jpeg.o \
	$(OBJDIR)/tif_flush.o \
	$(OBJDIR)/tif_lzw.o \
	$(OBJDIR)/tif_next.o \
	$(OBJDIR)/tif_open.o \
	$(OBJDIR)/tif_packbits.o \
	$(OBJDIR)/tif_pixarlog.o \
	$(OBJDIR)/tif_predict.o \
	$(OBJDIR)/tif_print.o \
	$(OBJDIR)/tif_read.o \
	$(OBJDIR)/tif_swab.o \
	$(OBJDIR)/tif_strip.o \
	$(OBJDIR)/tif_thunder.o \
	$(OBJDIR)/tif_tile.o \
	$(OBJDIR)/tif_unix.o \
	$(OBJDIR)/tif_version.o \
	$(OBJDIR)/tif_warning.o \
	$(OBJDIR)/tif_write.o \
	$(OBJDIR)/tif_zip.o

OUTPUT = $(OBJDIR)/libtiff.a

SHARED_OUTPUT = $(OBJDIR)/libtiff.so

all: $(OUTPUT) $(SHARED_OUTPUT)

$(OUTPUT): $(OBJS)
	ar rc $(OUTPUT) $(OBJS)

$(SHARED_OUTPUT): $(OBJS)
	gcc -shared -o $(SHARED_OUTPUT) $(OBJS)

clean:
	find \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;

COMPILE = $(GCC) -c $(CFLAGS) $? -o $*.o

$(OBJDIR)/tif_aux.o: 	     tif_aux.c
	$(COMPILE)
$(OBJDIR)/tif_close.o:       tif_close.c
	$(COMPILE)
$(OBJDIR)/tif_codec.o:       tif_codec.c
	$(COMPILE)
$(OBJDIR)/tif_compress.o:    tif_compress.c
	$(COMPILE)
$(OBJDIR)/tif_dir.o: 	     tif_dir.c
	$(COMPILE)
$(OBJDIR)/tif_dirinfo.o:     tif_dirinfo.c
	$(COMPILE)
$(OBJDIR)/tif_dirread.o:     tif_dirread.c
	$(COMPILE)
$(OBJDIR)/tif_dirwrite.o:    tif_dirwrite.c
	$(COMPILE)
$(OBJDIR)/tif_dumpmode.o:    tif_dumpmode.c
	$(COMPILE)
$(OBJDIR)/tif_error.o:       tif_error.c
	$(COMPILE)
$(OBJDIR)/tif_fax3.o: 	     tif_fax3.c
	$(COMPILE)
$(OBJDIR)/tif_fax3sm.o:      tif_fax3sm.c
	$(COMPILE)
$(OBJDIR)/tif_getimage.o:    tif_getimage.c
	$(COMPILE)
$(OBJDIR)/tif_jpeg.o: 	     tif_jpeg.c
	$(COMPILE)
$(OBJDIR)/tif_flush.o:       tif_flush.c
	$(COMPILE)
$(OBJDIR)/tif_lzw.o: 	     tif_lzw.c
	$(COMPILE)
$(OBJDIR)/tif_next.o: 	     tif_next.c
	$(COMPILE)
$(OBJDIR)/tif_open.o: 	     tif_open.c
	$(COMPILE)
$(OBJDIR)/tif_packbits.o:    tif_packbits.c
	$(COMPILE)
$(OBJDIR)/tif_pixarlog.o:    tif_pixarlog.c
	$(COMPILE)
$(OBJDIR)/tif_predict.o:     tif_predict.c
	$(COMPILE)
$(OBJDIR)/tif_print.o:       tif_print.c
	$(COMPILE)
$(OBJDIR)/tif_read.o: 	     tif_read.c
	$(COMPILE)
$(OBJDIR)/tif_swab.o: 	     tif_swab.c
	$(COMPILE)
$(OBJDIR)/tif_strip.o:       tif_strip.c
	$(COMPILE)
$(OBJDIR)/tif_thunder.o:     tif_thunder.c
	$(COMPILE)
$(OBJDIR)/tif_tile.o: 	     tif_tile.c
	$(COMPILE)
$(OBJDIR)/tif_unix.o: 	     tif_unix.c
	$(COMPILE)
$(OBJDIR)/tif_version.o:     tif_version.c
	$(COMPILE)
$(OBJDIR)/tif_warning.o:     tif_warning.c
	$(COMPILE)
$(OBJDIR)/tif_write.o:       tif_write.c
	$(COMPILE)
$(OBJDIR)/tif_zip.o: 	     tif_zip.c
	$(COMPILE)

































