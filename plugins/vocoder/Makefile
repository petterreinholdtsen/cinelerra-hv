include ../../global_config

OBJS = ../parametric/$(OBJDIR)/fourier.o \
	$(OBJDIR)/vocoder.o

OUTPUT = $(PLUGIN_DIR)/vocoder.plugin

include ../../plugin_config


../parametric/$(OBJDIR)/fourier.o: ../parametric/fourier.C
$(OBJDIR)/vocoder.o: vocoder.C
