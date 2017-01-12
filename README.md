# BootX-34 for qemu/ppc

Has patch in boot script (bootinfo.hdr) to spoof enough devices to make Rhapsody 5.1, 5.3 (Mac OS X Server 1.0) happy.

Requirements (bring your own):
 * /SDKs/MacOSX10.1.5.sdk
 * /SDKs/MacOSX10.4u.sdk
 * powerpc-apple-darwin10-gcc-4.0.1 (try [XcodeLegacy](https://github.com/devernay/xcodelegacy))

Boot from BootX dmg, with 'boot-file' set to the HFS+/UFS partition you want to boot mach_kernel from.

```
-serial stdio
-drive file=MacOSXServer1.0.iso,index=0,format=raw,media=cdrom
-drive file=BootX_custom.dmg,index=2,format=raw,media=disk
-prom-env 'boot-device=ide2:2,\BootX
-prom-env 'boot-args=-v rd=sd0 debug=0xffe kdp=2'
-prom-env 'boot-file=ide0:11,\mach_kernel'
```

### Files

Rough pre-made image which can chain-load Mac OS X Server v1.0 [here] (https://github.com/steventroughtonsmith/BootX/releases/download/1/BootX_custom.dmg.zip).

### Screenshots

[![](http://hccdata.s3.amazonaws.com/gh_bootx_rhap53.jpg)](http://hccdata.s3.amazonaws.com/gh_bootx_rhap53.jpg)
