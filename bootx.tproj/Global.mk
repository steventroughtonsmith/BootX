CC=powerpc-apple-darwin10-gcc-4.0.1

CFLAGS=-static -I$(SRC_ROOT)/ci.subproj/ -I$(SRC_ROOT)/fs.subproj/ -I$(SRC_ROOT)/include.subproj/ -I$(SRC_ROOT)/libclite.subproj -I$(SRC_ROOT)/sl.subproj -isysroot /SDKs/MacOSX10.1.5.sdk -mmacosx-version-min=10.0 

LFLAGS=-isysroot /SDKs/MacOSX10.1.5.sdk -nostdlib -e _StartTVector -seg1addr 01C00000 -lcc_kext

HFSDM=/Applications/HFS\ Disk\ Maker.app/Contents/MacOS/hfs_disk_maker_cli
