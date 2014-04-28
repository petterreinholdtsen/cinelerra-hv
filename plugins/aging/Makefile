CFLAGS += -I../burn

include ../../global_config

OBJS = $(OBJDIR)/aging.o  \
	$(OBJDIR)/agingwindow.o \
	../burn/$(OBJDIR)/effecttv.o

OUTPUT = ../$(OBJDIR)/aging.plugin

include ../../plugin_config

$(OBJDIR)/aging.o: aging.C
$(OBJDIR)/agingwindow.o: agingwindow.C
