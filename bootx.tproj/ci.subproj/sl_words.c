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
 *  sl_words.c - Forth and C code for the sl_words package.
 *
 *  Copyright (c) 1998-2000 Apple Computer, Inc.
 *
 *  DRI: Josh de Cesare
 */

#include <sl.h>

#if SL_DEBUG
void InitDebugWords(void);
#endif

extern const char gMacParts[];
extern const char *gControl2Source[];

CICell SLWordsIH = 0;

long InitSLWords(long ofVers)
{
  long result, cnt;
  
  result = Interpret_1_1
    (
     " drop"
     
     " hex"
     " unselect-dev"
     
     // Create the slWords pseudo-device
     " \" /packages\" find-device"
     " new-device"
     " \" sl_words\" device-name"
     
     " : open true ;"
     " : close ;"
     
     // Define all sl words here.
     
     // init the outputLevel
     " 0 value outputLevel"
     
     // slw_set_output_level ( level -- )
     " : slw_set_output_level"
     "   dup 0= if 0 stdout ! then"
     "   to outputLevel"
     " ;"
     
     // slw_emit ( ch -- )
     " : slw_emit (emit) 2 outputLevel <= if emit else drop then ;"
     
     // slw_cr ( -- )
     " : slw_cr   debug-cr 2 outputLevel <= if cr then ;"
     
     // Static init stuff for keyboard
     " 0 value keyboardIH"
     " 20 buffer: keyMap"
     
     // slw_init_keymap ( keyboardIH -- keyMap )
     " : slw_init_keymap"
     "   to keyboardIH"
     "   keyMap dup 20 0 fill"
     " ;"
     
     // slw_update_keymap
     " : slw_update_keymap { ; dpth }"
     "   depth -> dpth"
     "   keyboardIH if"
     "     \" get-key-map\" keyboardIH $call-method"
     "     depth dpth - 1 = if 20 then"
     "     4 / 0 do"
     "       dup i 4 * + l@ keyMap i 4 * + tuck l@ or swap l!"
     "     loop drop"
     "   then"
     " ;"
     
     // Set up the spin cursor stuff.
     " 0 value spinType"
     " 0 value screenIH"
     " 0 value cursorAddr"
     " 0 value cursorX"
     " 0 value cursorY"
     " 0 value cursorW"
     " 0 value cursorH"
     " 0 value cursorPixelSize"
     " 0 value cursorStage"
     " 0 value cursorTime"
     " 0 value cursorDelay"
     
     // slw_spin ( -- )
     " : slw_spin"
     "   screenIH 0<> cursorAddr 0<> and if"
     "     get-msecs dup cursorTime - cursorDelay >= if"
     "       to cursorTime"
     "       slw_update_keymap"
     "       spinType 0= if"
     "         cursorStage 1+ 3 mod dup to cursorStage"
     "         cursorW * cursorH * cursorAddr +"
     "         cursorX cursorY cursorW cursorH"
     "         \" draw-rectangle\" screenIH $call-method"
     "       else"
     "         cursorStage 1+ 6 mod dup to cursorStage"
     "         dup 3 > if 6 swap - then dup >r"
     "         1+ cursorW * cursorPixelSize * cursorAddr +"
     "         cursorX cursorY cursorW cursorH r> 1+ -"
     "         \" draw-rectangle\" screenIH $call-method"
     "       then"
     "     else"
     "       drop"
     "     then"
     "   then"
     " ;"
     
     // slw_spin_init ( screenIH cursorAddr cursorX cursorY cursorW cursorH --)
     " : slw_spin_init"
     "   to cursorH to cursorW"
     "   to cursorY to cursorX"
     "   to cursorAddr to screenIH"
     "   d# 111 to cursorDelay"
     "   ['] slw_spin to spin" 
     " ;"
     
     // slw_spin_init2 ( screenIH cursorAddr cursorX cursorY cursorW cursorH--)
     " : slw_spin_init2"
     "   1 to spinType"
     "   to cursorH dup FFFF and to cursorW 10 >> to cursorPixelSize"
     "   to cursorY to cursorX"
     "   to cursorAddr to screenIH"
     "   d# 50 to cursorDelay"
     "   ['] slw_spin to spin" 
     " ;"
     
     // slw_pwd ( phandle addr len -- act )
     " : slw_pwd"
     "   ['] pwd 138 - execute"
     " ;"
     
     // slw_sum ( adr len -- sum )
     " : slw_sum { adr len }"
     "   len 0 tuck do"
     "     dup 1 and if 10000 or then"
     "     1 >> adr i + c@ + ffff and"
     "   loop"
     " ;"
     
     " device-end"
     
     " 0 0 \" sl_words\" $open-package"
     
     , ofVers, &SLWordsIH);
  
  if (result != kCINoError) return result;
  if (SLWordsIH == 0) return kCIError;
  
  if (gOFVersion < kOFVersion3x) {
    result = Interpret_1_0
      (
       " dev /packages/obp-tftp"
       " ['] load C + l!"
       , kLoadSize);
    if (result != kCINoError) return result;
  }
  
  if (gOFVersion < kOFVersion3x) {
    result = Interpret_1_0
      (
       " dev /packages/mac-parts"
       " \" lame\" device-name"
       " dev /packages"
       " 1 byte-load"
       " device-end"
       , (long)gMacParts);
    if (result != kCINoError) return result;
  }
  
  if (gOFVersion < kOFVersion2x) {
    for(cnt = 0; gControl2Source[cnt] != '\0'; cnt++) {
      result = Interpret_0_0(gControl2Source[cnt]);
      if (result == kCIError) return kCIError;
      if (result == kCICatch) return kCINoError;
    }
  }
  
#if SL_DEBUG
  InitDebugWords();
#endif
  
  return kCINoError;
}

#if SL_DEBUG
void InitDebugWords(void)
{
  Interpret_0_0
    (
     // .sc ( -- )
     " : .sc ?state-valid ci-regs 4+ l@ l@ dup 0= \" Bad Stack\" (abort\")"
     " cr .\" Stack Trace\""
    " begin dup while dup 8 + l@ cr u. l@ repeat drop ;"
     );
}
#endif

void SetOutputLevel(long level)
{
  CallMethod_1_0(SLWordsIH, "slw_set_output_level", level);
}


char *InitKeyMap(CICell keyboardIH)
{
  long ret;
  char *keyMap;
  
  ret = CallMethod_1_1(SLWordsIH, "slw_init_keymap",
		       keyboardIH, (CICell *)&keyMap);
  if (ret != kCINoError) return NULL;
  
  return keyMap;
}

void UpdateKeyMap(void)
{
  CallMethod_0_0(SLWordsIH, "slw_update_keymap");
}


void SpinInit(long spinType, CICell screenIH, char *cursorAddr,
	      long cursorX, long cursorY, long cursorW, long cursorH,
	      long pixelSize)
{
  if (spinType == 0) {
    CallMethod_6_0(SLWordsIH, "slw_spin_init",
		   screenIH, (long)cursorAddr,
		   cursorX, cursorY, cursorW, cursorH);
  } else {
    CallMethod_6_0(SLWordsIH, "slw_spin_init2",
		   screenIH, (long)cursorAddr,
		   cursorX, cursorY, cursorW | pixelSize << 16, cursorH);
  }
}

void Spin(void)
{
  CallMethod_0_0(SLWordsIH, "slw_spin");
}

long GetPackageProperty(CICell phandle, char *propName,
			char **propAddr, long *propLen)
{
  long ret, nameLen = strlen(propName);
  
  ret = Interpret_3_2("get-package-property if 0 0 then",
		      phandle, nameLen, (CICell)propName,
		      (CICell *)propLen, (CICell *)propAddr);
  if ((ret != kCINoError) || (*propAddr == NULL)) return -1;
  
  return 0;
}
