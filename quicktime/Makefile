OBJDIR := $(shell uname --machine)
USE_MMX = 0
CC = gcc
HAVE_CFLAGS := $(shell if [ -n "$CFLAGS" ]; then echo y; fi )
#ENCORE_DIR := encore2/src
#DECORE_DIR := decore2/src
ENCORE_DIR := encore50
DECORE_DIR := decore50
ENCORE_INCLUDE := $(ENCORE_DIR)/encore.h
DECORE_INCLUDE := $(DECORE_DIR)/decore.h
MPEG3_DIR := ../libmpeg3
OBJS := 





ifneq ($(HAVE_CFLAGS), y)


ifeq ($(OBJDIR), alpha)
  CFLAGS = -O4 -arch ev67 -ieee -accept c99_keywords -gcc_messages
endif

ifeq ($(OBJDIR), i686)
  CFLAGS = -O3 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2 -I/usr/local/include
endif





endif

$(shell if ! test -d $(OBJDIR) \; then mkdir -p $(OBJDIR) \; fi )

DIRS = $(OBJDIR)




include Makefile.jpeg
include Makefile.libraw
include Makefile.vorbis
include Makefile.ffmpeg
include Makefile.lame
include Makefile.dv







#CFLAGS += -g
CFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -DHAVE_FIREWIRE
CFLAGS += -I. \
	-I$(JPEG_DIR) \
	-I$(MPEG3_DIR) \
	-I$(LAME_DIR)/include \
	$(shell glib-config --cflags)








OBJS += \
	$(OBJDIR)/atom.o \
	$(OBJDIR)/avi_hdrl.o \
	$(OBJDIR)/avi_idx1.o \
	$(OBJDIR)/avi_movi.o \
	$(OBJDIR)/avi_strh.o \
	$(OBJDIR)/cmodel_default.o \
	$(OBJDIR)/cmodel_yuv420p.o \
	$(OBJDIR)/cmodel_yuv422.o \
	$(OBJDIR)/codecs.o \
	$(OBJDIR)/colormodels.o \
	$(OBJDIR)/ctab.o \
	$(OBJDIR)/dinf.o \
	$(OBJDIR)/div3.o \
	$(OBJDIR)/divx.o \
	$(OBJDIR)/dref.o \
	$(OBJDIR)/dv.o \
	$(OBJDIR)/edts.o \
	$(OBJDIR)/elst.o \
	$(OBJDIR)/graphics.o \
	$(OBJDIR)/hdlr.o \
	$(OBJDIR)/ima4.o \
	$(OBJDIR)/jpeg.o \
	$(OBJDIR)/jpeg_old.o \
	$(OBJDIR)/libdv.o \
	$(OBJDIR)/libmjpeg.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/mdat.o \
	$(OBJDIR)/mdhd.o \
	$(OBJDIR)/mdia.o \
	$(OBJDIR)/minf.o \
	$(OBJDIR)/moov.o \
	$(OBJDIR)/mvhd.o \
	$(OBJDIR)/plugin.o \
	$(OBJDIR)/qtpng.o \
	$(OBJDIR)/qtmp3.o \
	$(OBJDIR)/quicktime.o \
	$(OBJDIR)/raw.o \
	$(OBJDIR)/rawaudio.o \
	$(OBJDIR)/smhd.o \
	$(OBJDIR)/stbl.o \
	$(OBJDIR)/stco.o \
	$(OBJDIR)/stsc.o \
	$(OBJDIR)/stsd.o \
	$(OBJDIR)/stsdtable.o \
	$(OBJDIR)/stss.o \
	$(OBJDIR)/stsz.o \
	$(OBJDIR)/stts.o \
	$(OBJDIR)/tkhd.o \
	$(OBJDIR)/trak.o \
	$(OBJDIR)/twos.o \
	$(OBJDIR)/udta.o \
	$(OBJDIR)/ulaw.o \
	$(OBJDIR)/util.o \
	$(OBJDIR)/v308.o \
	$(OBJDIR)/v408.o \
	$(OBJDIR)/v410.o \
	$(OBJDIR)/vmhd.o \
	$(OBJDIR)/vorbis.o \
	$(OBJDIR)/workarounds.o \
	$(OBJDIR)/yuv2.o \
	$(OBJDIR)/yuv4.o \
	$(OBJDIR)/yv12.o




# OBJS += \
# 	$(OBJDIR)/$(DECORE_DIR)/bitstream.o \
# 	$(OBJDIR)/$(DECORE_DIR)/colorspace.o \
# 	$(OBJDIR)/$(DECORE_DIR)/decoder.o \
# 	$(OBJDIR)/$(DECORE_DIR)/decore2.o \
# 	$(OBJDIR)/$(DECORE_DIR)/idct.o \
# 	$(OBJDIR)/$(DECORE_DIR)/image.o \
# 	$(OBJDIR)/$(DECORE_DIR)/mbcoding.o \
# 	$(OBJDIR)/$(DECORE_DIR)/mbprediction.o \
# 	$(OBJDIR)/$(DECORE_DIR)/quantize.o \
# 	$(OBJDIR)/$(DECORE_DIR)/transfer.o

OBJS += \
	$(OBJDIR)/$(DECORE_DIR)/debug.o \
	$(OBJDIR)/$(DECORE_DIR)/basic_prediction.o \
	$(OBJDIR)/$(DECORE_DIR)/clearblock.o \
	$(OBJDIR)/$(DECORE_DIR)/decore.o \
	$(OBJDIR)/$(DECORE_DIR)/getbits.o \
	$(OBJDIR)/$(DECORE_DIR)/idct.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_vars.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_block.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_decoder.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_header.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_iquant.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_mblock.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_picture.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_predict.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_recon.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_tables.o \
	$(OBJDIR)/$(DECORE_DIR)/mp4_vld.o \
	$(OBJDIR)/$(DECORE_DIR)/postprocess.o \
	$(OBJDIR)/$(DECORE_DIR)/yuv2rgb.o \
	$(OBJDIR)/$(DECORE_DIR)/transferidct.o

#OBJS += \
#	$(OBJDIR)/$(ENCORE_DIR)/bitstream.o \
#	$(OBJDIR)/$(ENCORE_DIR)/colorspace/colorspace.o \
#	$(OBJDIR)/$(ENCORE_DIR)/encoder.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/encore.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/image.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbcoding/mbcoding.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbcoding/putvlc.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/compensate.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/halfpel.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/mad.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/sad.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/mbmotionestcomp.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbprediction/mbprediction.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/dct.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/fdct.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/idct.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/mbtransquant.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/quantize.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/transfer.o \
#	$(OBJDIR)/$(ENCORE_DIR)/ratecontrol.o \
#	$(OBJDIR)/$(ENCORE_DIR)/rgb2yuv.o  \
#	$(OBJDIR)/$(ENCORE_DIR)/timer.o


ifeq ($(USE_MMX), 1)


#CFLAGS += -D_MMX_
#ENCOREMMX_OBJS= \
#	$(OBJDIR)/$(ENCORE_DIR)/colorspace/intel_mmx/colorspace_mmx.o \
#	$(OBJDIR)/$(ENCORE_DIR)/colorspace/intel_mmx/yuv_to_yuv.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/fdct_mmx.o \
#	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/idct_sse.o \
#        $(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/transfer_mmx.o \
#        $(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/quant_mmx.o \
#        $(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/halfpel_mmx.o \
#        $(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/compensate_mmx.o \
#        $(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/sad_mmx.o 



endif


OBJS += \
	$(OBJDIR)/$(ENCORE_DIR)/bitstream.o \
	$(OBJDIR)/$(ENCORE_DIR)/encore.o \
	$(OBJDIR)/$(ENCORE_DIR)/mom_access.o \
	$(OBJDIR)/$(ENCORE_DIR)/mom_util.o \
	$(OBJDIR)/$(ENCORE_DIR)/mot_code.o \
	$(OBJDIR)/$(ENCORE_DIR)/mot_est_comp.o \
	$(OBJDIR)/$(ENCORE_DIR)/mot_est_mb.o \
	$(OBJDIR)/$(ENCORE_DIR)/mot_util.o \
	$(OBJDIR)/$(ENCORE_DIR)/putvlc.o \
	$(OBJDIR)/$(ENCORE_DIR)/rate_ctl.o \
	$(OBJDIR)/$(ENCORE_DIR)/text_bits.o \
	$(OBJDIR)/$(ENCORE_DIR)/text_code.o \
	$(OBJDIR)/$(ENCORE_DIR)/text_code_mb.o \
	$(OBJDIR)/$(ENCORE_DIR)/text_dct.o \
	$(OBJDIR)/$(ENCORE_DIR)/vop_code.o






OUTPUT = $(OBJDIR)/libquicktime.a
SHAREDOUTPUT = $(OBJDIR)/libquicktime.so

CFLAGS += \
	-I$(DECORE_DIR) \
	-I$(ENCORE_DIR) \
	-DDECORE_INCLUDE=\"$(DECORE_INCLUDE)\" \
	-DENCORE_INCLUDE=\"$(ENCORE_INCLUDE)\" \
	-DLINUX \
	-D_DECORE



CFLAGS += \
	-ffast-math \
	-mno-ieee-fp \
	-I$(OGG_DIR)/include \
	-I$(VORBIS_DIR)/include \
	-I$(VORBIS_DIR)/lib \
	-DHAVE_ALLOCA_H=1 \
	-DHAVE_ALLOCA=1 \
	-DHAVE_SQRTF=1 \
	-DHAVE_LOGF=1 \
	-DHAVE_EXPF=1 \
	-DHAVE_ACOSF=1 \
	-DHAVE_ATANF=1 \
	-DHAVE_FREXPF=1 \
	-DHAVE_RINTF=1 \
	-DUSE_MEMORY_H \
	-D_REENTRANT \
	-DHAVE_ALLOCA=1 \
	-DHAVE_ALLOCA_H=1 \
	-DVORBIS_IEEE_FLOAT32 \
	-DHAVE_DLFCN_H=1 \
	-DSTDC_HEADERS=1 \
	-DSIZEOF_SHORT=2 \
	-DSIZEOF_INT=4 \
	-DSIZEOF_LONG=4 \
	-DSIZEOF_LONG_LONG=8 \
	-DUSE_MEMORY_H

CFLAGS += \
	-I$(FFMPEG_DIR)/libavcodec/ \
	-I$(FFMPEG_DIR)/libav



DIRS += \
	$(OBJDIR)/$(DECORE_DIR)

DIRS += \
	$(OBJDIR)/$(ENCORE_DIR) \
	$(OBJDIR)/$(ENCORE_DIR)/att_mmx \
	$(OBJDIR)/$(ENCORE_DIR)/colorspace \
	$(OBJDIR)/$(ENCORE_DIR)/colorspace/intel_mmx \
	$(OBJDIR)/$(ENCORE_DIR)/intel_mmx \
	$(OBJDIR)/$(ENCORE_DIR)/mbcoding \
	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp \
	$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx \
	$(OBJDIR)/$(ENCORE_DIR)/mbprediction \
	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant \
	$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx

DIRS += \
	$(OBJDIR)/$(OGG_DIR)/src \
	$(OBJDIR)/$(VORBIS_DIR)/lib

LIBS := $(OUTPUT) $(MPEG3_DIR)/$(OBJDIR)/libmpeg3.a -lpng -lz -lpthread -lglib -ldl -lm

$(shell echo  $(CFLAGS) > $(OBJDIR)/c_flags)
$(shell echo  $(LIBRAW_CFLAGS) > $(OBJDIR)/libraw_cflags)
$(shell echo  $(OBJS) $(LIBRAW_OBJS) $(DV_OBJS) $(DVMMXOBJS) $(ENCOREMMX_OBJS) $(JPEG_NASM) $(JPEG_FAIL) $(FFMPEG_OBJS) $(FFMPEGIA32_OBJS) $(LAME_OBJS) > $(OBJDIR)/objs)

all: $(DIRS) $(OUTPUT)

$(DIRS):
	if ! test -d $@ ; then mkdir -p $@ ; fi

$(OUTPUT): $(LIBRAW_OBJS) $(DV_OBJS) $(DVMMXOBJ) $(OBJS) $(DVMMXOBJS) $(ENCOREMMX_OBJS) $(JPEG_NASM) $(FFMPEG_OBJS) $(FFMPEGIA32_OBJS) $(LAME_OBJS) $(JPEG_FAIL) 
	ar rcs $(OUTPUT) `cat $(OBJDIR)/objs`

$(OBJDIR)/qtdump: $(OBJDIR)/dump.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/qtdump $(OBJDIR)/dump.o $(LIBS)

$(OBJDIR)/make_streamable: $(OBJDIR)/make_streamable.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/make_streamable $(OBJDIR)/make_streamable.o $(LIBS)

$(OBJDIR)/dechunk: $(OBJDIR)/dechunk.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/dechunk $(OBJDIR)/dechunk.o $(LIBS)

$(OBJDIR)/rechunk: $(OBJDIR)/rechunk.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/rechunk $(OBJDIR)/rechunk.o $(LIBS)

$(OBJDIR)/recover: $(OBJDIR)/recover.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/recover $(OBJDIR)/recover.o $(LIBS)

$(OBJDIR)/yuv4toyuv: $(OBJDIR)/yuv4toyuv.o $(OUTPUT)
	$(CC) -o $(OBJDIR)/yuv4toyuv $(OBJDIR)/yuv4toyuv.o $(LIBS)

$(OBJDIR)/qtinfo: $(OBJDIR)/qtinfo.o
	$(CC) -o $(OBJDIR)/qtinfo $(OBJDIR)/qtinfo.o $(LIBS)

util:  $(OBJDIR)/qtdump $(OBJDIR)/make_streamable $(OBJDIR)/qtinfo $(OBJDIR)/dechunk $(OBJDIR)/yuv4toyuv $(OBJDIR)/recover $(OBJDIR)/rechunk
	

clean:
	rm -rf $(OBJDIR)
	rm -rf .deps

wc:
	cat *.c *.h | wc

backup: clean
	cd .. && \
	tar zcvf quicktime.tar.gz quicktime

install:
	

$(OBJDIR)/dump.o \
$(OBJDIR)/make_streamable.o \
$(OBJDIR)/dechunk.o \
$(OBJDIR)/recover.o \
$(OBJDIR)/yuv4toyuv.o \
$(OBJDIR)/qtinfo.o \
$(OBJDIR)/rechunk.o \
$(OBJS):
	$(CC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(JPEG_FAIL):
	$(CC) -c -O3 -march=i686 -fmessage-length=0 $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(DV_OBJS):
	$(CC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(LIBRAW_OBJS):
	$(CC) -c `cat $(OBJDIR)/libraw_cflags` $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(DVMMXOBJS):
	$(CC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.S) -o $*.o

$(DVMMXOBJ):
	$(CC) `cat $(OBJDIR)/c_flags` $(DV_DIR)/libdv/gasmoff.c -o $(OBJDIR)/$(DV_DIR)/libdv/gasmoff -lm
	$(OBJDIR)/$(DV_DIR)/libdv/gasmoff > $(DV_DIR)/libdv/asmoff.h

$(JPEG_NASM):
	nasm -f elf $(subst $(OBJDIR)/,, $*.s) -o $*.o

$(ENCOREMMX_OBJS):
	nasm -f elf $(subst $(OBJDIR)/,, $*.asm) -o $*.o


$(FFMPEG_OBJS):
	$(CC) `cat $(OBJDIR)/ffmpeg_flags` -c $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(LAME_OBJS):
	$(CC) `cat $(OBJDIR)/lame_flags` -c $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(FFMPEGIA32_OBJS):
	nasm -f elf $(subst $(OBJDIR)/,, $*.s) -o $*.o






$(OBJDIR)/atom.o:			  atom.c
$(OBJDIR)/avi_hdrl.o:                     avi_hdrl.c
$(OBJDIR)/avi_idx1.o:                     avi_idx1.c
$(OBJDIR)/avi_movi.o:                     avi_movi.c
$(OBJDIR)/avi_strh.o:                     avi_strh.c
$(OBJDIR)/cmodel_default.o:		  cmodel_default.c cmodel_permutation.h
$(OBJDIR)/cmodel_yuv420p.o:		  cmodel_yuv420p.c cmodel_permutation.h
$(OBJDIR)/cmodel_yuv422.o:		  cmodel_yuv422.c cmodel_permutation.h
$(OBJDIR)/codecs.o:			  codecs.c
$(OBJDIR)/colormodels.o:		  colormodels.c
$(OBJDIR)/ctab.o:			  ctab.c
$(OBJDIR)/dechunk.o:			  dechunk.c
$(OBJDIR)/dinf.o:			  dinf.c
$(OBJDIR)/div3.o:                         div3.c
$(OBJDIR)/divx.o:			  divx.c
$(OBJDIR)/dref.o:			  dref.c
$(OBJDIR)/dump.o:			  dump.c
$(OBJDIR)/dv.o:			  	  dv.c
$(OBJDIR)/edts.o:			  edts.c
$(OBJDIR)/elst.o:			  elst.c
$(OBJDIR)/graphics.o:			  graphics.c
$(OBJDIR)/hdlr.o:			  hdlr.c
$(OBJDIR)/ima4.o:			  ima4.c
$(OBJDIR)/jpeg.o:			  jpeg.c
$(OBJDIR)/jpeg_old.o:			  jpeg_old.c
$(OBJDIR)/libdv.o:			  libdv.c
$(OBJDIR)/libmjpeg.o:			  libmjpeg.c
$(OBJDIR)/make_streamable.o:		  make_streamable.c
$(OBJDIR)/matrix.o:			  matrix.c
$(OBJDIR)/mdat.o:			  mdat.c
$(OBJDIR)/mdhd.o:			  mdhd.c
$(OBJDIR)/mdia.o:			  mdia.c
$(OBJDIR)/minf.o:			  minf.c
$(OBJDIR)/moov.o:			  moov.c
$(OBJDIR)/mvhd.o:			  mvhd.c
$(OBJDIR)/plugin.o:			  plugin.c
$(OBJDIR)/qtinfo.o:			  qtinfo.c
$(OBJDIR)/qtmp3.o:                        qtmp3.c
$(OBJDIR)/qtpng.o:			  qtpng.c
$(OBJDIR)/quicktime.o: 		  	  quicktime.c
$(OBJDIR)/raw.o:			  raw.c
$(OBJDIR)/rawaudio.o:			  rawaudio.c
$(OBJDIR)/rechunk.o:			  rechunk.c
$(OBJDIR)/recover.o:			  recover.c
$(OBJDIR)/smhd.o:			  smhd.c
$(OBJDIR)/stbl.o:			  stbl.c
$(OBJDIR)/stco.o:			  stco.c
$(OBJDIR)/stsc.o:			  stsc.c
$(OBJDIR)/stsd.o:			  stsd.c
$(OBJDIR)/stsdtable.o: 		  	  stsdtable.c
$(OBJDIR)/stss.o:			  stss.c
$(OBJDIR)/stsz.o:			  stsz.c
$(OBJDIR)/stts.o:			  stts.c
$(OBJDIR)/tkhd.o:			  tkhd.c
$(OBJDIR)/trak.o:			  trak.c
$(OBJDIR)/twos.o:			  twos.c
$(OBJDIR)/udta.o:			  udta.c
$(OBJDIR)/ulaw.o:			  ulaw.c
$(OBJDIR)/util.o:			  util.c
$(OBJDIR)/v308.o:                         v308.c
$(OBJDIR)/v408.o:                         v408.c
$(OBJDIR)/v410.o:                         v410.c
$(OBJDIR)/vmhd.o:			  vmhd.c
$(OBJDIR)/vorbis.o:                       vorbis.c
$(OBJDIR)/workarounds.o:		  workarounds.c
$(OBJDIR)/yuv2.o:			  yuv2.c
$(OBJDIR)/yuv4.o:			  yuv4.c
$(OBJDIR)/yuv4toyuv.o:			  yuv4toyuv.c
$(OBJDIR)/yv12.o:			  yv12.c









# $(OBJDIR)/$(DECORE_DIR)/bitstream.o: 		    $(DECORE_DIR)/bitstream.c
# $(OBJDIR)/$(DECORE_DIR)/colorspace.o: 	    $(DECORE_DIR)/colorspace.c
# $(OBJDIR)/$(DECORE_DIR)/decoder.o: 		    $(DECORE_DIR)/decoder.c
# $(OBJDIR)/$(DECORE_DIR)/decore2.o: 		    $(DECORE_DIR)/decore2.c
# $(OBJDIR)/$(DECORE_DIR)/idct.o: 		    $(DECORE_DIR)/idct.c
# $(OBJDIR)/$(DECORE_DIR)/image.o: 		    $(DECORE_DIR)/image.c
# $(OBJDIR)/$(DECORE_DIR)/mbcoding.o: 		    $(DECORE_DIR)/mbcoding.c
# $(OBJDIR)/$(DECORE_DIR)/mbprediction.o: 	    $(DECORE_DIR)/mbprediction.c
# $(OBJDIR)/$(DECORE_DIR)/quantize.o: 		    $(DECORE_DIR)/quantize.c
# $(OBJDIR)/$(DECORE_DIR)/transfer.o: 		    $(DECORE_DIR)/transfer.c

$(OBJDIR)/$(DECORE_DIR)/debug.o: 		    $(DECORE_DIR)/debug.c
$(OBJDIR)/$(DECORE_DIR)/basic_prediction.o: 	    $(DECORE_DIR)/basic_prediction.c
$(OBJDIR)/$(DECORE_DIR)/clearblock.o: 		    $(DECORE_DIR)/clearblock.c
$(OBJDIR)/$(DECORE_DIR)/decore.o: 		    $(DECORE_DIR)/decore.c
$(OBJDIR)/$(DECORE_DIR)/gen_draw.o: 		    $(DECORE_DIR)/gen_draw.c
$(OBJDIR)/$(DECORE_DIR)/getbits.o: 		    $(DECORE_DIR)/getbits.c
$(OBJDIR)/$(DECORE_DIR)/gen_usetime.o:  	    $(DECORE_DIR)/gen_usetime.c
$(OBJDIR)/$(DECORE_DIR)/idct.o: 		    $(DECORE_DIR)/idct.c
$(OBJDIR)/$(DECORE_DIR)/mp4_vars.o: 		    $(DECORE_DIR)/mp4_vars.c
$(OBJDIR)/$(DECORE_DIR)/mp4_block.o: 		    $(DECORE_DIR)/mp4_block.c
$(OBJDIR)/$(DECORE_DIR)/mp4_decoder.o:  	    $(DECORE_DIR)/mp4_decoder.c
$(OBJDIR)/$(DECORE_DIR)/mp4_header.o: 		    $(DECORE_DIR)/mp4_header.c
$(OBJDIR)/$(DECORE_DIR)/mp4_iquant.o: 		    $(DECORE_DIR)/mp4_iquant.c
$(OBJDIR)/$(DECORE_DIR)/mp4_mblock.o: 		    $(DECORE_DIR)/mp4_mblock.c
$(OBJDIR)/$(DECORE_DIR)/mp4_picture.o:  	    $(DECORE_DIR)/mp4_picture.c
$(OBJDIR)/$(DECORE_DIR)/mp4_predict.o:  	    $(DECORE_DIR)/mp4_predict.c
$(OBJDIR)/$(DECORE_DIR)/mp4_recon.o: 		    $(DECORE_DIR)/mp4_recon.c
$(OBJDIR)/$(DECORE_DIR)/mp4_tables.o: 		    $(DECORE_DIR)/mp4_tables.c
$(OBJDIR)/$(DECORE_DIR)/mp4_vld.o: 		    $(DECORE_DIR)/mp4_vld.c
$(OBJDIR)/$(DECORE_DIR)/store.o: 		    $(DECORE_DIR)/store.c
$(OBJDIR)/$(DECORE_DIR)/postprocess.o:  	    $(DECORE_DIR)/postprocess.c
$(OBJDIR)/$(DECORE_DIR)/yuv2rgb.o: 		    $(DECORE_DIR)/yuv2rgb.c
$(OBJDIR)/$(DECORE_DIR)/transferidct.o: 	    $(DECORE_DIR)/transferidct.c


#$(OBJDIR)/$(ENCORE_DIR)/bitstream.o: 			     $(ENCORE_DIR)/bitstream.c
#$(OBJDIR)/$(ENCORE_DIR)/colorspace/colorspace.o: 	     $(ENCORE_DIR)/colorspace/colorspace.c
#$(OBJDIR)/$(ENCORE_DIR)/encoder.o: 			     $(ENCORE_DIR)/encoder.c
#$(OBJDIR)/$(ENCORE_DIR)/encore.o: 			     $(ENCORE_DIR)/encore.c
#$(OBJDIR)/$(ENCORE_DIR)/image.o: 			     $(ENCORE_DIR)/image.c
#$(OBJDIR)/$(ENCORE_DIR)/mbcoding/mbcoding.o: 		     $(ENCORE_DIR)/mbcoding/mbcoding.c
#$(OBJDIR)/$(ENCORE_DIR)/mbcoding/putvlc.o: 		     $(ENCORE_DIR)/mbcoding/putvlc.c
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/compensate.o: 	     $(ENCORE_DIR)/mbmotionestcomp/compensate.c
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/mbmotionestcomp.o:  $(ENCORE_DIR)/mbmotionestcomp/mbmotionestcomp.c
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/sad.o:   	     $(ENCORE_DIR)/mbmotionestcomp/sad.c
#$(OBJDIR)/$(ENCORE_DIR)/mbprediction/mbprediction.o: 	     $(ENCORE_DIR)/mbprediction/mbprediction.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/dct.o: 		     $(ENCORE_DIR)/mbtransquant/dct.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/fdct.o: 	     	     $(ENCORE_DIR)/mbtransquant/fdct.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/idct.o: 		     $(ENCORE_DIR)/mbtransquant/idct.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/mbtransquant.o: 	     $(ENCORE_DIR)/mbtransquant/mbtransquant.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/quantize.o: 	     $(ENCORE_DIR)/mbtransquant/quantize.c
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/transfer.o: 	     $(ENCORE_DIR)/mbtransquant/transfer.c
#$(OBJDIR)/$(ENCORE_DIR)/ratecontrol.o:  		     $(ENCORE_DIR)/ratecontrol.c
#$(OBJDIR)/$(ENCORE_DIR)/rgb2yuv.o: 			     $(ENCORE_DIR)/rgb2yuv.c
#$(OBJDIR)/$(ENCORE_DIR)/timer.o: 			     $(ENCORE_DIR)/timer.c

#$(OBJDIR)/$(ENCORE_DIR)/colorspace/intel_mmx/colorspace_mmx.o:       $(ENCORE_DIR)/colorspace/intel_mmx/colorspace_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/colorspace/intel_mmx/yuv_to_yuv.o:           $(ENCORE_DIR)/colorspace/intel_mmx/yuv_to_yuv.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/fdct_mmx.o: 	      $(ENCORE_DIR)/mbtransquant/intel_mmx/fdct_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/idct_sse.o: 	      $(ENCORE_DIR)/mbtransquant/intel_mmx/idct_sse.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/transfer_mmx.o:       $(ENCORE_DIR)/mbtransquant/intel_mmx/transfer_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbtransquant/intel_mmx/quant_mmx.o: 	      $(ENCORE_DIR)/mbtransquant/intel_mmx/quant_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/halfpel_mmx.o:     $(ENCORE_DIR)/mbmotionestcomp/intel_mmx/halfpel_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/compensate_mmx.o:  $(ENCORE_DIR)/mbmotionestcomp/intel_mmx/compensate_mmx.asm
#$(OBJDIR)/$(ENCORE_DIR)/mbmotionestcomp/intel_mmx/sad_mmx.o: 	      $(ENCORE_DIR)/mbmotionestcomp/intel_mmx/sad_mmx.asm



$(OBJDIR)/$(ENCORE_DIR)/bitstream.o: 	   $(ENCORE_DIR)/bitstream.c
$(OBJDIR)/$(ENCORE_DIR)/encore.o: 	   $(ENCORE_DIR)/encore.c
$(OBJDIR)/$(ENCORE_DIR)/mom_access.o: 	   $(ENCORE_DIR)/mom_access.c
$(OBJDIR)/$(ENCORE_DIR)/mom_util.o: 	   $(ENCORE_DIR)/mom_util.c
$(OBJDIR)/$(ENCORE_DIR)/mot_code.o: 	   $(ENCORE_DIR)/mot_code.c
$(OBJDIR)/$(ENCORE_DIR)/mot_est_comp.o:    $(ENCORE_DIR)/mot_est_comp.c
$(OBJDIR)/$(ENCORE_DIR)/mot_est_mb.o: 	   $(ENCORE_DIR)/mot_est_mb.c
$(OBJDIR)/$(ENCORE_DIR)/mot_util.o: 	   $(ENCORE_DIR)/mot_util.c
$(OBJDIR)/$(ENCORE_DIR)/putvlc.o: 	   $(ENCORE_DIR)/putvlc.c
$(OBJDIR)/$(ENCORE_DIR)/rate_ctl.o: 	   $(ENCORE_DIR)/rate_ctl.c
$(OBJDIR)/$(ENCORE_DIR)/text_bits.o: 	   $(ENCORE_DIR)/text_bits.c
$(OBJDIR)/$(ENCORE_DIR)/text_code.o: 	   $(ENCORE_DIR)/text_code.c
$(OBJDIR)/$(ENCORE_DIR)/text_code_mb.o:    $(ENCORE_DIR)/text_code_mb.c
$(OBJDIR)/$(ENCORE_DIR)/text_dct.o: 	   $(ENCORE_DIR)/text_dct.c
$(OBJDIR)/$(ENCORE_DIR)/vop_code.o:	   $(ENCORE_DIR)/vop_code.c














include depend.vorbis
include depend.ffmpeg
include depend.jpeg
include depend.lame
include depend.dv
include depend.libraw
