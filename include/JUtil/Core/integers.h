#ifndef JUTIL_INTEGERS_H
#define JUTIL_INTEGERS_H

#include <JUtil/Core/Macro.h>

#define INTEGERS_ERR_INDEX	0x03

//prevent errors when including c library headers
#define	_SYS_TYPES_H			1
#define _BITS_STDINT_INTN_H		1
#define _BITS_STDINT_UINTN_H	1
#define _STDINT_H				1

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef signed char int_least8_t;
typedef unsigned char   uint_least8_t;
typedef short  int_least16_t;
typedef unsigned short  uint_least16_t;
typedef int  int_least32_t;
typedef unsigned   uint_least32_t;
typedef long long  int_least64_t;
typedef unsigned long long   uint_least64_t;

typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef short  int_fast16_t;
typedef unsigned short  uint_fast16_t;
typedef int  int_fast32_t;
typedef unsigned  int  uint_fast32_t;
typedef long long  int_fast64_t;
typedef unsigned long long   uint_fast64_t;

#if !defined(_INTPTR_T_DEFINED) && !defined(__intptr_t_defined)
    #define _INTPTR_T_DEFINED
	#define __intptr_t_defined
    typedef int intptr_t;
#endif

#if !defined(_UINTPTR_T_DEFINED) && !defined(__uintptr_t_defined)
    #define _UINTPTR_T_DEFINED
    #define __uintptr_t_defined
    typedef long long unsigned int uintptr_t;
#endif

typedef long long  intmax_t;
typedef unsigned long long uintmax_t;

#endif // JUTIL_INTEGERS_H
