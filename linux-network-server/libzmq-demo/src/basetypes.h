/*
 * ltypes.h
 *
 *  Created on: 2017?и║11??9ии?
 *      Author: Administrator
 */

#ifndef LTYPE_H_
#define LTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define IN
#define OUT

typedef char                    CHAR;
typedef short int               SHORT;
typedef int                     INT;
typedef long               		LONG;
typedef long long               li64;

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            UINT;
typedef unsigned long           DWORD;
typedef unsigned long long      UINT64;

typedef void                    VOID;
typedef int                     BOOL;
typedef int                   	HANDLE;
typedef void*					PVOID;

typedef void*					v8;
typedef char*                   i4p;
typedef short int*              i8p;
typedef int*                    i16p;
typedef long*               	li32p;
typedef long long*              li64p;

typedef unsigned char*          u4p;
typedef unsigned short*     	u8p;
typedef unsigned *           	u16p;
typedef unsigned long *      	uli32p;
typedef	unsigned long long*     uli64p;

typedef const char              C_CHAR;
typedef const unsigned char     C_UCHAR;
typedef const void              C_VOID;

typedef const char*             c_i4p;
typedef const unsigned char*    c_u8p;
typedef const void*             c_v8p;


#ifdef __cplusplus
}
#endif

#endif // LTYPE_H_

