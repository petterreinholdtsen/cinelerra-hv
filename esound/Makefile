include ../global_config

$(shell sh -c 'if ! test -d $(OBJDIR)\; then mkdir $(OBJDIR)\; fi' )

OBJS = \
	$(OBJDIR)/audio.o \
	$(OBJDIR)/clients.o \
	$(OBJDIR)/esd_config.o \
	$(OBJDIR)/esdfile.o \
	$(OBJDIR)/esdlib.o \
	$(OBJDIR)/esdmgr.o \
	$(OBJDIR)/genrand.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/mix.o \
	$(OBJDIR)/players.o \
	$(OBJDIR)/proto.o \
	$(OBJDIR)/samples.o



OUTPUT = $(OBJDIR)/libesound.a
SHARED_OUTPUT = $(OBJDIR)/libesound.so
CC = gcc
CFLAGS += -I. -I../audiofile -DSYSCONFDIR=\"/usr/local/etc\"

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

$(OBJDIR)/audio.o:	 audio.c
	$(COMPILE)
$(OBJDIR)/clients.o:	 clients.c
	$(COMPILE)
$(OBJDIR)/esd_config.o:  esd_config.c
	$(COMPILE)
$(OBJDIR)/esdfile.o:	 esdfile.c
	$(COMPILE)
$(OBJDIR)/esdlib.o:	 esdlib.c
	$(COMPILE)
$(OBJDIR)/esdmgr.o:	 esdmgr.c
	$(COMPILE)
$(OBJDIR)/genrand.o:	 genrand.c
	$(COMPILE)
$(OBJDIR)/filter.o:	 filter.c
	$(COMPILE)
$(OBJDIR)/mix.o:	 mix.c
	$(COMPILE)
$(OBJDIR)/players.o:	 players.c
	$(COMPILE)
$(OBJDIR)/proto.o:	 proto.c
	$(COMPILE)
$(OBJDIR)/samples.o:	 samples.c
	$(COMPILE)













