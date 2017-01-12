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
 *  device_tree.h - Data structures for the flattened device tree.
 *
 *  Copyright (c) 1998-2000 Apple Computer, Inc.
 *
 *  DRI: Josh de Cesare
 */

#ifndef _BOOTX_DEVICE_TREE_H_
#define _BOOTX_DEVICE_TREE_H_

#define kPropNameLength (32)
#define kPropValueMaxLength (0x100000)

struct DTProperty {
  char name[kPropNameLength];  // NUL terminated property name
  unsigned long length;        // Length (bytes) of property value that follows
  //unsigned long value[1];    // Variable length value of property
                               // Padded to a multiple of a longword
};
typedef struct DTProperty DTProperty, *DTPropertyPtr;

struct DTNode {
  unsigned long nProperties;   // Number of props[] elements (0 => end of list)
  unsigned long nChildren;     // Number of children[] elements
  //DTProperty props[];        // nProperties DeviceTreeNodeProperty structs
  //DTNode children[];         // nChildren DeviceTreeNode structs
};
typedef struct DTNode DTNode, *DTNodePtr;

#endif /* ! _BOOTX_DEVICE_TREE_H_ */
