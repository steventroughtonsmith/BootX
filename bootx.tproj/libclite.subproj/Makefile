include ../Global.mk

OBJECTS=bsearch.o bswap.o mem.o prf.o printf.o sprintf.o string.o strtol.o zalloc.o

all: $(OBJECTS)
	
%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $(BUILD_ROOT)/$@


