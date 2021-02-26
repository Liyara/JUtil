#ifndef JUTIL_INTEGERS_H
#define JUTIL_INTEGERS_H

#include <stdint.h>

/*#include <JUtil/Core/Macro.h>

#define INTEGERS_ERR_INDEX	0x03

#ifndef JUTIL_DISABLE_INTEGERS
	//prevent errors when including c library headers
	//#define	_SYS_TYPES_H			1
	//#define _BITS_STDINT_INTN_H		1
	//#define _BITS_STDINT_UINTN_H	1
	//#define _STDINT_H				1

	#ifndef int8_t
		#define int8_t signed char
	#endif

	#ifndef uint8_t
		#define uint8_t unsigned char
	#endif

	#ifndef int16_t
		#define int16_t short
	#endif

	#ifndef uint16_t
		#define uint16_t unsigned short
	#endif

	#ifndef int32_t
		#define int32_t int
	#endif

	#ifndef uint32_t
		#define uint32_t unsigned
	#endif

	#ifndef int64_t
		#define int64_t long long
	#endif

	#ifndef uint64_t
		#define uint64_t unsigned long long
	#endif

	#ifndef int_least8_t
		#define int_least8_t signed char
	#endif

	#ifndef uint_least8_t
		#define uint_least8_t unsigned char
	#endif

	#ifndef int_least16_t
		#define int_least16_t short
	#endif

	#ifndef uint_least16_t
		#define uint_least16_t unsigned short
	#endif

	#ifndef int_least32_t
		#define int_least32_t int
	#endif

	#ifndef uint_least32_t
		#define uint_least32_t unsigned
	#endif

	#ifndef int_least64_t
		#define int_least64_t long long
	#endif

	#ifndef uint_least64_t
		#define uint_least64_t unsigned long long
	#endif

	#ifndef int_fast8_t
		#define int_fast8_t signed char
	#endif

	#ifndef uint_fast8_t
		#define uint_fast8_t unsigned char
	#endif

	#ifndef int_fast16_t
		#define int_fast16_t short
	#endif

	#ifndef uint_fast16_t
		#define uint_fast16_t unsigned short
	#endif

	#ifndef int_fast32_t
		#define int_fast32_t int
	#endif

	#ifndef uint_fast32_t
		#define uint_fast32_t unsigned
	#endif

	#ifndef int_fast64_t
		#define int_fast64_t long long
	#endif

	#ifndef uint_fast64_t
		#define uint_fast64_t unsigned long long
	#endif

	#ifndef intmax_t
		#define intmax_t long long
	#endif

	#ifndef uintmax_t
		#define uintmax_t unsigned long long
	#endif

	#ifndef u_short
		#define u_short unsigned short
	#endif

	#ifndef u_int
		#define u_int unsigned int
	#endif

	#ifndef u_long
		#define u_long unsigned long
	#endif

	#ifndef ushort
		#define ushort unsigned short
	#endif

	#ifndef uint
		#define uint unsigned int
	#endif
#endif

#ifndef JUTIL_DISABLE_CHARACTERS
	#ifndef u_char
		#define u_char unsigned char
	#endif
#endif


#if !defined(_QUAD_T_DEFINED) && !defined(__quad_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(quad_t)
	#define __quad_t_defined
	#define _QUAD_T_DEFINED
	#define quad_t long long
#endif

#if !defined(_U_QUAD_T_DEFINED) && !defined(__u_quad_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(u_quad_t)
	#define __u_quad_t_defined
	#define _U_QUAD_T_DEFINED
	#define u_quad_t unsigned long long
#endif

#if !defined(_QADDR_T_DEFINED) && !defined(__qaddr_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(quadaddr_t)
	#define __qaddr_t_defined
	#define _QADDR_T_DEFINED
	#define qaddr_t quad_t*
#endif

#if !defined(_CADDR_T_DEFINED) && !defined(__caddr_t_defined) && !defined(JUTIL_DISABLE_CHARACTERS) && !defined(caddr_t)
	#define __caddr_t_defined
	#define _CADDR_T_DEFINED
	#define caddr_t char*
#endif

#if !defined(_DADDR_T_DEFINED) && !defined(__daddr_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(daddr_t)
	#define __daddr_t_defined
	#define _DADDR_T_DEFINED
	#define daddr_t long
#endif

#if !defined(_SEGSZ_T_DEFINED) && !defined(__segsz_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(segsz_t)
	#define __segez_t_defined
	#define _SEGSZ_T_DEFINED
	#define segsz_t long
#endif

#if !defined(_SWBLK_T_DEFINED) && !defined(__swblk_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(swblk_t)
	#define __swblk_t_defined
	#define _SWBLK_T_DEFINED
	#define swblk_t long
#endif

#if !defined(_DEV_T_DEFINED) && !defined(__dev_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(dev_t)
	#define __dev_t_defined
	#define _DEV_T_DEFINED
	#define dev_t u_long
#endif

#if !defined(_UID_T_DEFINED) && !defined(__uid_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(uid_t)
	#define __uid_t_defined
	#define _UID_T_DEFINED
	#define uid_t u_long
#endif

#if !defined(_FIXPT_T_DEFINED) && !defined(__fixpt_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(fixpt_t)
	#define __fixpt_t_defined
	#define _FIXPT_T_DEFINED
	#define fixpt_t u_long
#endif

#if !defined(_GID_T_DEFINED) && !defined(__gid_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(gid_t)
	#define __gid_t_defined
	#define _GID_T_DEFINED
	#define gid_t u_long;
#endif

#if !defined(_INO_T_DEFINED) && !defined(__ino_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(ino_t)
	#define __ino_t_defined
	#define _INO_T_DEFINED
	#define ino_t u_long
#endif

#if !defined(_MODE_T_DEFINED) && !defined(__mode_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(mode_t)
	#define __mode_t_defined
	#define _MODE_T_DEFINED
	#define mode_t u_short
#endif

#if !defined(_NLINK_T_DEFINED) && !defined(__nlink_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(nlink_t)
	#define __inlink_t_defined
	#define _NLINK_T_DEFINED
	#define nlink_t u_short
#endif


#if !defined(_OFF_T_DEFINED) && !defined(__off_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(off_t)
	#define __off_t_defined
	#define off_t quad_t
#endif

#if !defined(_PID_T_DEFINED) && !defined(__pid_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(pid_t)
	#define __pid_t_defined
	#define pid_t long
#endif

#if !defined(_INTPTR_T_DEFINED) && !defined(__intptr_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(intptr_t)
    #define _INTPTR_T_DEFINED
	#define __intptr_t_defined
	#define intptr_t int
#endif

#if !defined(_UINTPTR_T_DEFINED) && !defined(__uintptr_t_defined) && !defined(JUTIL_DISABLE_INTEGERS) && !defined(uintptr_t)
    #define _UINTPTR_T_DEFINED
    #define __uintptr_t_defined
	#define uintptr_t long long unsigned int
#endif*/

#endif // JUTIL_INTEGERS_H
