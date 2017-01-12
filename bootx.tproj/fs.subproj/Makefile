include ../Global.mk

OBJECTS=ufs.o net.o ufs_byteorder.o cache.o ext2fs_bswap.o ext2fs.o fs.o HFSCompare.o hfs.o

all: $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $(BUILD_ROOT)/$@


