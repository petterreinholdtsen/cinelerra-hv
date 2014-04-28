
TARGET = robot

OBJS = $(TARGET).o
CFLAGS = -O2 -Iparapin

OUTPUT = $(TARGET)

LIBS = parapin/libparapin.a -lpthread

$(OUTPUT): $(OBJS)
	gcc -o $(OUTPUT) $(OBJS) $(LIBS)



$(OBJS):
	gcc $(CFLAGS) -c $*.c -o $*.o


$(TARGET).o: $(TARGET).c
delay.o: delay.c
