include ../../global_config



OBJS = \
	$(OBJDIR)/common.o \
	$(OBJDIR)/encode.o \
	$(OBJDIR)/ieeefloat.o \
	$(OBJDIR)/musicin.o \
	$(OBJDIR)/portableio.o \
	$(OBJDIR)/psy.o \
	$(OBJDIR)/subs.o \
	$(OBJDIR)/tonal.o

CFLAGS += -DPURGE -DUNIX  -DNDEBUG  -DINLINE=inline 
LIBS = -lm

OUTPUT = ../$(OBJDIR)/toolame.plugin

$(shell echo $(CFLAGS) > $(OBJDIR)/c_flags)
$(shell echo $(OBJS) $(LIBS) > $(OBJDIR)/libs)

$(OUTPUT): $(OBJS)
	gcc -o $(OUTPUT) `cat $(OBJDIR)/libs`

clean:
	rm -rf $(OBJDIR)
	find \( -name core \
		-o -name '*.o' \
		-o -name '*.a' \
		-o -name '*.so' \) -exec rm -f {} \;

install:
	cp $(OUTPUT) /usr/lib/cinelerra

$(OBJS):
	$(GCC) -c `cat $(OBJDIR)/c_flags` $(subst $(OBJDIR)/,, $*.c) -o $*.o

$(OBJDIR)/common.o:	  common.c
$(OBJDIR)/encode.o: 	  encode.c
$(OBJDIR)/ieeefloat.o:    ieeefloat.c
$(OBJDIR)/musicin.o: 	  musicin.c
$(OBJDIR)/portableio.o:   portableio.c
$(OBJDIR)/psy.o: 	  psy.c
$(OBJDIR)/subs.o: 	  subs.c
$(OBJDIR)/tonal.o: 	  tonal.c
