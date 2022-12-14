#ifndef W32HTTP_COMMON_H
#define W32HTTP_COMMON_H

#pragma once

// #define MODERN_SOCKET 0

#define _WIN32_WINNT 0x400
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <strsafe.h>
#include <stdio.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

#if defined(_MSC_VER) || defined(__WATCOMC__) 
typedef unsigned __int8		u8;
typedef unsigned __int16	u16;
typedef unsigned __int32  u32;
typedef unsigned __int64	u64;

typedef __int8  s8;
typedef __int16 s16;
typedef __int32 s32; 
typedef __int64 s64;
#elif defined(__GNUC__)
#include <stdint.h>
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t    s8; 
typedef int16_t   s16;
typedef int32_t   s32; 
typedef int64_t   s64;
#else
#error "Incompatible compiler, use MSVC!"
#endif

typedef u8 utf8;

#ifndef byte_t 
typedef u8 byte_t;
#endif 

#include "resource.h"
#endif
