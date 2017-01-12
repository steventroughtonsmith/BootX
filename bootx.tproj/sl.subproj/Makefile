include ../Global.mk

OBJECTS=display.o main.o drivers.o device_tree.o elf.o macho.o PEFSupport.o

all: $(OBJECTS)
	
%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $(BUILD_ROOT)/$@


