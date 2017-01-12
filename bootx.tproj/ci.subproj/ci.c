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
 *  ci.c - Functions for accessing Open Firmware's Client Interface
 *
 *  Copyright (c) 1998-2000 Apple Computer, Inc.
 *
 *  DRI: Josh de Cesare
 */

#include <sl.h>

static ClientInterfacePtr gCIPtr;

long InitCI(ClientInterfacePtr ciPtr)
{
  gCIPtr = ciPtr;
  
  return 0;
}

long CallCI(CIArgs *ciArgsPtr)
{
  long ret;
	
//	if (strcmp(ciArgsPtr->service,"interpret")==0)
//	{
//		if (strcmp(ciArgsPtr->args.interpret_0_0.forth,"emit")!=0)
//		{
//			printf("[CallCI] %s\n", ciArgsPtr->args.interpret_0_0.forth );
//		}
//	}
	
  ret = (*gCIPtr)(ciArgsPtr);
  return ret;
}


// Device Tree

// Peer take a phandle and returns the next peer.
// It returns zero of there are no more peers.
CICell Peer(CICell phandle)
{
  CIArgs ciArgs;
  CICell peer_ph;
  long ret;
  
  ciArgs.service = "peer";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.peer.phandle = phandle;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  peer_ph = ciArgs.args.peer.peerPhandle;
  
  return peer_ph;
}

// Child take a phandle and returns the first child.
// It returns zero of there in no child.
CICell Child(CICell phandle)
{
  CIArgs ciArgs;
  CICell child_ph;
  long ret;
  
  ciArgs.service = "child";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.child.phandle = phandle;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  child_ph = ciArgs.args.child.childPhandle;
  
  return child_ph;
}

// Parent take a phandle and returns the parent node.
// It returns zero of if the phandle is the root node.
CICell Parent(CICell phandle)
{
  CIArgs ciArgs;
  CICell parent_ph;
  long ret;
  
  ciArgs.service = "parent";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.parent.childPhandle = phandle;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  parent_ph = ciArgs.args.parent.parentPhandle;
  
  return parent_ph;
}

// FindDevice take a device spec and returns the phandle.
// It returns zero of if the device was not found.
CICell FindDevice(char *devSpec)
{
  CIArgs ciArgs;
  CICell phandle;
  long ret;
  
  ciArgs.service = "finddevice";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.finddevice.devSpec = devSpec;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  phandle = ciArgs.args.finddevice.phandle;
  
  return phandle;
}

// InstanceToPath take an ihandle, buf and buflen.  Set the device path
// to the package in buf upto buflen characters and returns the length
// Length will be -1 if the ihandle is invalid.
CICell InstanceToPath(CICell ihandle, char *buf, long buflen)
{
  CIArgs ciArgs;
  CICell length;
  long ret;
  
  ciArgs.service = "instance-to-path";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.instanceToPath.ihandle = ihandle;
  ciArgs.args.instanceToPath.buf = buf;
  ciArgs.args.instanceToPath.buflen = buflen;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  length = ciArgs.args.instanceToPath.length;
  
  return length;
}

// InstanceToPackage take an ihandle and returns the phandle for it.
// returns -1 if the phandle can't be found.
CICell InstanceToPackage(CICell ihandle)
{
  CIArgs ciArgs;
  CICell phandle;
  long ret;
  
  ciArgs.service = "instance-to-package";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.instanceToPackage.ihandle = ihandle;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  phandle = ciArgs.args.instanceToPackage.phandle;
  
  return phandle;
}

// InstanceToPackages

// PackageToPath take a phandle, buf and buflen.  Set the device path
// to the package in buf upto buflen characters and returns the length
// Length will be -1 if the phandle is invalid.
CICell PackageToPath(CICell phandle, char *buf, long buflen)
{
  CIArgs ciArgs;
  CICell length;
  long ret;
  
  if (gOFVersion >= kOFVersion2x) {
    ciArgs.service = "package-to-path";
    ciArgs.nArgs = 3;
    ciArgs.nReturns = 1;
    ciArgs.args.packageToPath.phandle = phandle;
    ciArgs.args.packageToPath.buf = buf;
    ciArgs.args.packageToPath.buflen = buflen;
    
    ret = CallCI(&ciArgs);
    if (ret != 0) return kCIError;
    
    length = ciArgs.args.packageToPath.length;
  } else {
    ret = CallMethod_3_1(SLWordsIH, "slw_pwd", phandle,
			 (CICell)buf, buflen, &length);
    if (ret != 0) return kCIError;
    
    buf[length] = '\0';
  }
  
  return length;
}

// Canon

// GetPropLen takes a phandle and prop name
// and returns the size of the property
// or -1 if the property is not valid.
CICell GetPropLen(CICell phandle, char *name)
{
  CIArgs ciArgs;
  CICell size;
  long ret;
  
  ciArgs.service = "getproplen";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 1;
  ciArgs.args.getproplen.phandle = phandle;
  ciArgs.args.getproplen.name = name;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  size = ciArgs.args.getproplen.size;
  
  return size;
}

// GetProp takes a phandle, prop name, buffer and length
// and copied the property value in to the buffer.
// returns -1 if the property is not valid.
CICell GetProp(CICell phandle, char *name, char *buf, long buflen)
{
  CIArgs ciArgs;
  CICell size;
  long ret;
  
  ciArgs.service = "getprop";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 1;
  ciArgs.args.getprop.phandle = phandle;
  ciArgs.args.getprop.name = name;
  ciArgs.args.getprop.buf = buf;
  ciArgs.args.getprop.buflen = buflen;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  size = ciArgs.args.getprop.size;
  
  return size;
}

// NextProp takes a phandle, prev name, and a buffer
// and copied the next property name in to the buffer.
// returns -1 if the property is not valid.
// returns 0 if the prev was the last property.
// returns 1 otherwise.
CICell NextProp(CICell phandle, char *previous, char *buf)
{
  CIArgs ciArgs;
  CICell flag;
  long ret;
  
  ciArgs.service = "nextprop";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.nextprop.phandle = phandle;
  ciArgs.args.nextprop.previous = previous;
  ciArgs.args.nextprop.buf = buf;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  flag = ciArgs.args.nextprop.flag;
  
  return flag;
}

// SetProp takes a phandle, prop name, buffer and length
// and copied the buffer in to the property value.
// returns -1 if the property could not be set or created.
CICell SetProp(CICell phandle, char *name, char *buf, long buflen)
{
  CIArgs ciArgs;
  CICell size;
  long ret;
  
  ciArgs.service = "setprop";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 1;
  ciArgs.args.setprop.phandle = phandle;
  ciArgs.args.setprop.name = name;
  ciArgs.args.setprop.buf = buf;
  ciArgs.args.setprop.buflen = buflen;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  size = ciArgs.args.setprop.size;
  
  return size;
}


// Device I/O

// Open takes a device specifier and returns an iHandle
// It returns zero if the device can not be found or opened.
CICell Open(char *devSpec)
{
  CIArgs ciArgs;
  CICell ihandle;
  long ret;
  
  ciArgs.service = "open";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.open.devSpec = devSpec;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return 0;
  
  ihandle = ciArgs.args.open.ihandle;
  
  return ihandle;
}

// Close takes an iHandle and closes the device.
void Close(CICell ihandle)
{
  CIArgs ciArgs;
  
  ciArgs.service = "close";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 0;
  ciArgs.args.close.ihandle = ihandle;
  
  CallCI(&ciArgs);
}

// Read takes an iHandle, an address and a length and return the actual
// Length read.  Returns -1 if the operaction failed.
CICell Read(CICell ihandle, long addr, long length)
{
  CIArgs ciArgs;
  long actual;
  long ret;
  
  ciArgs.service = "read";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.read.ihandle = ihandle;
  ciArgs.args.read.addr = addr;
  ciArgs.args.read.length = length;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  actual = ciArgs.args.read.actual;
	
	long* pt = addr;
	
	//printf("[CI READ %x]\n", *pt);
	
  // Spin the wait cursor.
  Spin();
  
  return actual;
}

// Write takes an iHandle, an address and a length and return the actual
// Length written.  Returns -1 if the operaction failed.
CICell Write(CICell ihandle, long addr, long length)
{
  CIArgs ciArgs;
  long actual;
  long ret;
  
  ciArgs.service = "write";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.write.ihandle = ihandle;
  ciArgs.args.write.addr = addr;
  ciArgs.args.write.length = length;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  actual = ciArgs.args.write.actual;
  
  return actual;
}

// Seek takes an iHandle, and a 64 bit position
// and moves to that address in file.
// returns seeks result, or -1 if seek is not supported.
CICell Seek(CICell ihandle, long long position)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "seek";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.seek.ihandle = ihandle;
  ciArgs.args.seek.pos_high = position >> 32;
  ciArgs.args.seek.pos_low = position & 0x00000000FFFFFFFFULL;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  
  ret = ciArgs.args.seek.result;
  
  return ret;
}


// Other Device Method Invocation

// Lots of CallMethod_n_m functions...
// n is number of args, m is number of returns.

long CallMethod_0_0(CICell iHandle, char *method)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_0_0.iHandle = iHandle;
  ciArgs.args.callMethod_0_0.method = method;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_0_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_0_1(CICell iHandle, char *method, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 2;
  ciArgs.args.callMethod_0_1.iHandle = iHandle;
  ciArgs.args.callMethod_0_1.method = method;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_0_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.callMethod_0_1.return1;
  
  return kCINoError;
}

long CallMethod_1_0(CICell iHandle, char *method, CICell arg1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_1_0.iHandle = iHandle;
  ciArgs.args.callMethod_1_0.method = method;
  ciArgs.args.callMethod_1_0.arg1 = arg1;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_1_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_1_1(CICell iHandle, char *method, CICell arg1, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 2;
  ciArgs.args.callMethod_1_1.iHandle = iHandle;
  ciArgs.args.callMethod_1_1.method = method;
  ciArgs.args.callMethod_1_1.arg1 = arg1;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_1_0.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.callMethod_1_1.return1;
  
  return kCINoError;
}

long CallMethod_2_0(CICell iHandle, char *method,
		    CICell arg1, CICell arg2)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_2_0.iHandle = iHandle;
  ciArgs.args.callMethod_2_0.method = method;
  ciArgs.args.callMethod_2_0.arg1 = arg1;
  ciArgs.args.callMethod_2_0.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_2_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_3_0(CICell iHandle, char *method,
		    CICell arg1, CICell arg2, CICell arg3)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 5;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_3_0.iHandle = iHandle;
  ciArgs.args.callMethod_3_0.method = method;
  ciArgs.args.callMethod_3_0.arg1 = arg1;
  ciArgs.args.callMethod_3_0.arg2 = arg2;
  ciArgs.args.callMethod_3_0.arg3 = arg3;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_3_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_3_1(CICell iHandle, char *method,
		    CICell arg1, CICell arg2, CICell arg3, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 5;
  ciArgs.nReturns = 2;
  ciArgs.args.callMethod_3_1.iHandle = iHandle;
  ciArgs.args.callMethod_3_1.method = method;
  ciArgs.args.callMethod_3_1.arg1 = arg1;
  ciArgs.args.callMethod_3_1.arg2 = arg2;
  ciArgs.args.callMethod_3_1.arg3 = arg3;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_3_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.callMethod_3_1.return1;
  
  return kCINoError;
}

long CallMethod_4_0(CICell iHandle, char *method,
		    CICell arg1, CICell arg2, CICell arg3, CICell arg4)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 6;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_4_0.iHandle = iHandle;
  ciArgs.args.callMethod_4_0.method = method;
  ciArgs.args.callMethod_4_0.arg1 = arg1;
  ciArgs.args.callMethod_4_0.arg2 = arg2;
  ciArgs.args.callMethod_4_0.arg3 = arg3;
  ciArgs.args.callMethod_4_0.arg4 = arg4;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_4_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_5_0(CICell iHandle, char *method, CICell arg1, CICell arg2,
		    CICell arg3, CICell arg4, CICell arg5)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 7;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_5_0.iHandle = iHandle;
  ciArgs.args.callMethod_5_0.method = method;
  ciArgs.args.callMethod_5_0.arg1 = arg1;
  ciArgs.args.callMethod_5_0.arg2 = arg2;
  ciArgs.args.callMethod_5_0.arg3 = arg3;
  ciArgs.args.callMethod_5_0.arg4 = arg4;
  ciArgs.args.callMethod_5_0.arg5 = arg5;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_5_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long CallMethod_6_0(CICell iHandle, char *method, CICell arg1, CICell arg2,
		    CICell arg3, CICell arg4, CICell arg5, CICell arg6)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "call-method";
  ciArgs.nArgs = 8;
  ciArgs.nReturns = 1;
  ciArgs.args.callMethod_6_0.iHandle = iHandle;
  ciArgs.args.callMethod_6_0.method = method;
  ciArgs.args.callMethod_6_0.arg1 = arg1;
  ciArgs.args.callMethod_6_0.arg2 = arg2;
  ciArgs.args.callMethod_6_0.arg3 = arg3;
  ciArgs.args.callMethod_6_0.arg4 = arg4;
  ciArgs.args.callMethod_6_0.arg5 = arg5;
  ciArgs.args.callMethod_6_0.arg6 = arg6;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.callMethod_6_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}


// Memory

// Claim takes a virt address, a size, and an alignment.
// It return baseaddr or -1 for claim failed.
CICell Claim(CICell virt, CICell size, CICell align)
{
  CIArgs ciArgs;
  CICell baseaddr;
  long ret;

  if (gOFVersion >= kOFVersion2x) {
    // Claim actually works, so use it.
    ciArgs.service = "claim";
    ciArgs.nArgs = 3;
    ciArgs.nReturns = 1;
    ciArgs.args.claim.virt = virt;
    ciArgs.args.claim.size = size;
    ciArgs.args.claim.align = align;
    
    ret = CallCI(&ciArgs);
    if (ret != 0) return kCIError;
    
    baseaddr = ciArgs.args.claim.baseaddr;
  } else {
    // Claim does not work.  Do it by hand.
    if ((gMMUIH == 0) || (gMMUIH == 0)) return kCIError;
    
    // Get the physical memory
    ret = CallMethod_3_1(gMemoryIH, "claim", virt, size, 0, &baseaddr);
    if ((ret != kCINoError) || (virt != baseaddr)) return kCIError;
    
    // Get the logical memory
    ret = CallMethod_3_1(gMMUIH, "claim", virt, size, 0, &baseaddr);
    if ((ret != kCINoError) || (virt != baseaddr)) return kCIError;
    
    // Map them together.
    ret = CallMethod_4_0(gMMUIH, "map", virt, virt, size, 0);
    if (ret != kCINoError) return kCIError;
  }
  
  return baseaddr;
}

// Release takes a virt address, a size
void Release(CICell virt, CICell size)
{
  CIArgs ciArgs;
  
  ciArgs.service = "release";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 0;
  ciArgs.args.claim.virt = virt;
  ciArgs.args.claim.size = size;
  
  CallCI(&ciArgs);
}


// Control Transfer

// Boot trys to boot the bootspec
void Boot(char *bootspec)
{
  CIArgs ciArgs;
  
  ciArgs.service = "boot";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 0;
  ciArgs.args.boot.bootspec = bootspec;
  
  CallCI(&ciArgs);
}

// Enter the user interface.
// Executing the 'go' command returns to the client.
void Enter(void)
{
  CIArgs ciArgs;
  
  ciArgs.service = "enter";
  ciArgs.nArgs = 0;
  ciArgs.nReturns = 0;
  
  CallCI(&ciArgs);
}

// Exit the client program.
void Exit(void)
{
  CIArgs ciArgs;
  
  ciArgs.service = "exit";
  ciArgs.nArgs = 0;
  ciArgs.nReturns = 0;
  
  CallCI(&ciArgs);
}

// Clain

// Quiesce stops any async tasks in Open Firmware.
void Quiesce(void)
{
  CIArgs ciArgs;
  
  ciArgs.service = "quiesce";
  ciArgs.nArgs = 0;
  ciArgs.nReturns = 0;
  
  CallCI(&ciArgs);
}


// User Interface

// Lots of Interpret_n_m fundtions...
// n is number of args, m is number of returns.

long Interpret_0_0(char *forthString)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 1;
  ciArgs.args.interpret_0_0.forth = forthString;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_0_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long Interpret_1_0(char *forthString, CICell arg1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 1;
  ciArgs.args.interpret_1_0.forth = forthString;
  ciArgs.args.interpret_1_0.arg1 = arg1;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_1_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long Interpret_1_1(char *forthString, CICell arg1, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 2;
  ciArgs.args.interpret_1_1.forth = forthString;
  ciArgs.args.interpret_1_1.arg1 = arg1;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_1_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_1_1.return1;
  
  return kCINoError;
}

long Interpret_2_1(char *forthString, CICell arg1, CICell arg2, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 2;
  ciArgs.args.interpret_2_1.forth = forthString;
  ciArgs.args.interpret_2_1.arg1 = arg1;
  ciArgs.args.interpret_2_1.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_2_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_2_1.return1;
  
  return kCINoError;
}

long Interpret_3_1(char *forthString, CICell arg1, CICell arg2, CICell arg3,
		  CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 2;
  ciArgs.args.interpret_3_1.forth = forthString;
  ciArgs.args.interpret_3_1.arg1 = arg1;
  ciArgs.args.interpret_3_1.arg2 = arg2;
  ciArgs.args.interpret_3_1.arg3 = arg3;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_3_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_3_1.return1;
  
  return kCINoError;
}

long Interpret_3_2(char *forthString, CICell arg1, CICell arg2, CICell arg3,
		   CICell *ret1, CICell *ret2)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 3;
  ciArgs.args.interpret_3_2.forth = forthString;
  ciArgs.args.interpret_3_2.arg1 = arg1;
  ciArgs.args.interpret_3_2.arg2 = arg2;
  ciArgs.args.interpret_3_2.arg3 = arg3;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_3_2.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_3_2.return1;
  *ret2 = ciArgs.args.interpret_3_2.return2;
  
  return kCINoError;
}

long Interpret_4_0(char *forthString, CICell arg1, CICell arg2, CICell arg3,
		  CICell arg4)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 5;
  ciArgs.nReturns = 1;
  ciArgs.args.interpret_4_0.forth = forthString;
  ciArgs.args.interpret_4_0.arg1 = arg1;
  ciArgs.args.interpret_4_0.arg2 = arg2;
  ciArgs.args.interpret_4_0.arg3 = arg3;
  ciArgs.args.interpret_4_0.arg4 = arg4;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_4_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long Interpret_0_1(char *forthString, CICell *ret1)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 2;
  ciArgs.args.interpret_0_1.forth = forthString;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_0_1.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_0_1.return1;
  
  return kCINoError;
}

long Interpret_0_2(char *forthString, CICell *ret1, CICell *ret2)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 3;
  ciArgs.args.interpret_0_2.forth = forthString;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_0_2.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_0_2.return1;
  *ret2 = ciArgs.args.interpret_0_2.return2;
  
  return kCINoError;
}

long Interpret_0_3(char *forthString, CICell *ret1, CICell *ret2, CICell *ret3)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 1;
  ciArgs.nReturns = 4;
  ciArgs.args.interpret_0_3.forth = forthString;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_0_3.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_0_3.return1;
  *ret2 = ciArgs.args.interpret_0_3.return2;
  *ret3 = ciArgs.args.interpret_0_3.return3;
  
  return kCINoError;
}

long Interpret_1_3(char *forthString, CICell arg1, CICell *ret1, CICell *ret2,
		  CICell *ret3)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 2;
  ciArgs.nReturns = 4;
  ciArgs.args.interpret_1_3.forth = forthString;
  ciArgs.args.interpret_1_3.arg1 = arg1;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_1_3.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_1_3.return1;
  *ret2 = ciArgs.args.interpret_1_3.return2;
  *ret3 = ciArgs.args.interpret_1_3.return3;
  
  return kCINoError;
}

long Interpret_2_3(char *forthString, CICell arg1, CICell arg2, CICell *ret1,
		  CICell *ret2, CICell *ret3)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 4;
  ciArgs.args.interpret_2_3.forth = forthString;
  ciArgs.args.interpret_2_3.arg1 = arg1;
  ciArgs.args.interpret_2_3.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_2_3.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_2_3.return1;
  *ret2 = ciArgs.args.interpret_2_3.return2;
  *ret3 = ciArgs.args.interpret_2_3.return3;
  
  return kCINoError;
}

long Interpret_2_4(char *forthString, CICell arg1, CICell arg2, CICell *ret1,
		  CICell *ret2, CICell *ret3, CICell *ret4)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 5;
  ciArgs.args.interpret_2_4.forth = forthString;
  ciArgs.args.interpret_2_4.arg1 = arg1;
  ciArgs.args.interpret_2_4.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_2_4.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_2_4.return1;
  *ret2 = ciArgs.args.interpret_2_4.return2;
  *ret3 = ciArgs.args.interpret_2_4.return3;
  *ret4 = ciArgs.args.interpret_2_4.return4;
  
  return kCINoError;
}

long Interpret_2_0(char *forthString, CICell arg1, CICell arg2)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 1;
  ciArgs.args.interpret_2_0.forth = forthString;
  ciArgs.args.interpret_2_0.arg1 = arg1;
  ciArgs.args.interpret_2_0.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_2_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}

long Interpret_2_2(char *forthString, CICell arg1, CICell arg2, CICell *ret1,
		  CICell *ret2)
{
  CIArgs ciArgs;
  long ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 3;
  ciArgs.nReturns = 3;
  ciArgs.args.interpret_2_2.forth = forthString;
  ciArgs.args.interpret_2_2.arg1 = arg1;
  ciArgs.args.interpret_2_2.arg2 = arg2;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_2_2.catchResult != 0) return kCICatch;
  
  *ret1 = ciArgs.args.interpret_2_2.return1;
  *ret2 = ciArgs.args.interpret_2_2.return2;
  
  return kCINoError;
}

long Interpret_3_0(char *forthString, CICell arg1, CICell arg2, CICell arg3)
{
  CIArgs ciArgs;
  int ret;
  
  ciArgs.service = "interpret";
  ciArgs.nArgs = 4;
  ciArgs.nReturns = 1;
  ciArgs.args.interpret_3_0.forth = forthString;
  ciArgs.args.interpret_3_0.arg1 = arg1;
  ciArgs.args.interpret_3_0.arg2 = arg2;
  ciArgs.args.interpret_3_0.arg3 = arg3;
  
  ret = CallCI(&ciArgs);
  if (ret != 0) return kCIError;
  if (ciArgs.args.interpret_3_0.catchResult != 0) return kCICatch;
  
  return kCINoError;
}
