CFLAGS += -I../burn 

include ../../global_config

OBJS = $(OBJDIR)/holo.o  \
	$(OBJDIR)/holowindow.o \
	../burn/$(OBJDIR)/effecttv.o

OUTPUT = ../$(OBJDIR)/holo.plugin

include ../../plugin_config

$(OBJDIR)/holo.o: holo.C
$(OBJDIR)/holowindow.o: holowindow.C
