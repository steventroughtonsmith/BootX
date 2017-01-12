/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 *  boot_args.h - Data stuctures for the information passed to the kernel.
 *
 *  Copyright (c) 1998-2000 Apple Computer, Inc.
 *
 *  DRI: Josh de Cesare
 */

#ifndef _BOOTX_BOOT_ARGS_H_
#define _BOOTX_BOOT_ARGS_H_

// Video information..

struct Boot_Video {
  unsigned long v_baseAddr; /* Base address of video memory */
  unsigned long v_display;  /* Display Code (if Applicable */
  unsigned long v_rowBytes; /* Number of bytes per pixel row */
  unsigned long v_width;    /* Width */
  unsigned long v_height;   /* Height */
  unsigned long v_depth;    /* Pixel Depth */
};
typedef struct Boot_Video Boot_Video, *Boot_Video_Ptr;


// DRAM Bank definitions - describes physical memory layout.
#define kMaxDRAMBanks (26) /* maximum number of DRAM banks */

struct DRAMBank {
  unsigned long base;       /* physical base of DRAM bank */
  unsigned long size;       /* size of bank */
};
typedef struct DRAMBank DRAMBank, *DRAMBankPtr;


// Boot argument structure - passed into kernel at boot time.

#define kBootArgsRevision (1)
#define kBootArgsVersion  (1)

#define BOOT_LINE_LENGTH  (256)

struct boot_args {
  unsigned short Revision;                /* Revision of boot_args structure */
  unsigned short Version;                 /* Version of boot_args structure */
  char CommandLine[BOOT_LINE_LENGTH];     /* Passed in command line */
  DRAMBank PhysicalDRAM[kMaxDRAMBanks];   /* base/range pairs for the 26 DRAM banks */
  Boot_Video     Video;                   /* Video Information */
  unsigned long  machineType;             /* Machine Type (gestalt) */
  void           *deviceTreeP;            /* Base of flattened device tree */
  unsigned long  deviceTreeLength;        /* Length of flattened tree */
  unsigned long  topOfKernelData;         /* Last address of kernel data area*/
};
typedef struct boot_args boot_args, *boot_args_ptr;

#endif /* ! _BOOTX_BOOT_ARGS_H_ */
