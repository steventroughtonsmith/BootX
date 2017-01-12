include ../Global.mk

OBJECTS=ci.o ci_io.o Control2.o MAC-PARTS.o sl_words.o

all: $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $(BUILD_ROOT)/$@


