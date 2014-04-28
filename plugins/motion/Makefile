include ../../global_config

OBJS := \
	$(OBJDIR)/affine.o \
	../downsample/$(OBJDIR)/downsampleengine.o \
	$(OBJDIR)/motion.o \
	$(OBJDIR)/motionscan.o \
	$(OBJDIR)/motionwindow.o

OUTPUT := $(PLUGIN_DIR)/motion.plugin

include ../../plugin_config

$(OBJDIR)/affine.o: affine.C
$(OBJDIR)/motion.o: motion.C
$(OBJDIR)/motionscan.o: motionscan.C
$(OBJDIR)/motionwindow.o: motionwindow.C
