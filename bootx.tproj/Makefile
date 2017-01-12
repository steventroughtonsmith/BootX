SRC_ROOT=$(shell pwd)
BUILD_ROOT=$(SRC_ROOT)/BUILD

export SRC_ROOT
export BUILD_ROOT

include Global.mk

SUBDIRS = ci.subproj fs.subproj sl.subproj libclite.subproj

.PHONY: subdirs $(SUBDIRS)

all: prepass $(SUBDIRS)
	$(CC) $(LFLAGS) BUILD/*.o -o BUILD/BootX.macho
	macho-to-xcoff BUILD/BootX.macho BUILD/BootX.xcoff
	cat bootinfo.hdr BUILD/BootX.xcoff > DISK/BootX
	SetFile -t tbxi -c chrp -a I DISK/BootX
	rm -f /Applications/QEMU-PPC/BootX_custom.dmg
	hdiutil create -srcfolder DISK -format UFBI -fs HFS+ -sectors 16448 -layout SPUD /Applications/QEMU-PPC/BootX_custom.dmg
	#printf '\x3B' | dd of=/Applications/QEMU-PPC/BootX_custom.dmg bs=1 seek=1115 count=1 conv=notrunc 

prepass:
	mkdir -p DISK/
	mkdir -p BUILD/

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -rf BUILD
	rm -rf DISK	

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@


