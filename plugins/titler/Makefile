FREETYPE_DIR = $(shell expr ../../freetype* )
CFLAGS += -I$(FREETYPE_DIR)/include -I../../

include ../../global_config

OBJS = $(OBJDIR)/title.o  \
	$(OBJDIR)/titlewindow.o \
	../colors/$(OBJDIR)/colorpicker.o \
	../colors/$(OBJDIR)/plugincolors.o \
	$(FREETYPE_DIR)/objs/.libs/libfreetype.a

OUTPUT = ../$(OBJDIR)/titler.plugin

include ../../plugin_config

inst:
	cp $(OUTPUT) /usr/lib/cinelerra
	cp -a fonts /usr/lib/cinelerra





$(OBJDIR)/title.o: title.C
$(OBJDIR)/titlewindow.o: titlewindow.C



