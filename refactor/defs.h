/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Standard definitions read when starting up
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 *
 * $Id: defs.h,v 1.3 2001/09/21 14:14:19 dds Exp $
 */

//#define __cplusplus
#define __STDC__ 1
#define __ELF__ 
#define unix 
#define i386 
#define __i386 
#define __i386__ 
#define __tune_i386__
#define linux
#define __linux
#define __linux__ 
#define __unix 
#define __GNUC__ 2 
#define __GNUC_MINOR__ 96
#define __WCHAR_TYPE__ short unsigned int

#ifdef __cplusplus
#pragma includepath "/usr/include/g++-3"
#define __GNUG__ 3
#define __EXCEPTIONS
#endif

#pragma includepath "/usr/lib/gcc-lib/i386-redhat-linux/2.96/include"
#pragma includepath "/usr/include"

#define __extension__
