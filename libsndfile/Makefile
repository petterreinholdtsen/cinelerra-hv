include ../global_config

$(shell sh -c 'mkdir -p $(OBJDIR)/G72x $(OBJDIR)/GSM610' )

OBJS = \
	$(OBJDIR)/G72x/g721.o \
	$(OBJDIR)/G72x/g723_16.o \
	$(OBJDIR)/G72x/g723_24.o \
	$(OBJDIR)/G72x/g723_40.o \
	$(OBJDIR)/G72x/g72x.o \
	$(OBJDIR)/GSM610/add.o \
	$(OBJDIR)/GSM610/code.o \
	$(OBJDIR)/GSM610/decode.o \
	$(OBJDIR)/GSM610/gsm_create.o \
	$(OBJDIR)/GSM610/gsm_decode.o \
	$(OBJDIR)/GSM610/gsm_destroy.o \
	$(OBJDIR)/GSM610/gsm_encode.o \
	$(OBJDIR)/GSM610/gsm_option.o \
	$(OBJDIR)/GSM610/long_term.o \
	$(OBJDIR)/GSM610/lpc.o \
	$(OBJDIR)/GSM610/preprocess.o \
	$(OBJDIR)/GSM610/rpe.o \
	$(OBJDIR)/GSM610/short_term.o \
	$(OBJDIR)/GSM610/table.o \
	$(OBJDIR)/aiff.o \
	$(OBJDIR)/alaw.o \
	$(OBJDIR)/au.o \
	$(OBJDIR)/au_g72x.o \
	$(OBJDIR)/common.o \
	$(OBJDIR)/newpcm.o \
	$(OBJDIR)/paf.o \
	$(OBJDIR)/pcm.o \
	$(OBJDIR)/raw.o \
	$(OBJDIR)/sndfile.o \
	$(OBJDIR)/svx.o \
	$(OBJDIR)/ulaw.o \
	$(OBJDIR)/wav.o \
	$(OBJDIR)/wav_float.o \
	$(OBJDIR)/wav_gsm610.o \
	$(OBJDIR)/wav_ima_adpcm.o \
	$(OBJDIR)/wav_ms_adpcm.o

OUTPUT = $(OBJDIR)/libsndfile.a

$(shell echo  $(CFLAGS) > $(OBJDIR)/c_flags)

all: $(OUTPUT) $(SHARED_OUTPUT)

$(OUTPUT): $(OBJS)
	ar rcs $(OUTPUT) $(OBJS)

$(SHARED_OUTPUT): $(OBJS)
	gcc -shared -o $(SHARED_OUTPUT) $(OBJS)

clean:
	rm -rf $(OBJDIR)

$(OBJS):
	$(GCC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(OBJDIR)/sndfile.o: sndfile.c
$(OBJDIR)/raw.o: raw.c
$(OBJDIR)/wav.o: wav.c
