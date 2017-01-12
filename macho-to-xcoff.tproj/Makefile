CFLAGS=-isysroot /SDKs/MacOSX10.4u.sdk/

all: macho-to-xcoff.o
	$(CC) $(CFLAGS) $< -o macho-to-xcoff

clean:
	rm *.o
	rm macho-to-xcoff

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@
