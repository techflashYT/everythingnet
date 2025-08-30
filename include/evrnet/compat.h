/*
 * EverythingNet - Common headers - Compiler compatibility
 * Copyright (C) 2025 Techflash
 */

#ifndef _COMPAT_H
#define _COMPAT_H
#include <evrnet/plat_detect.h>

/* MSVC */
#ifdef _MSC_VER
/* requires manual packing with compat-struct-pack.h - this is a no-op */
#define EVRNET_PACK(_struct, name) \
    _struct name;

#elif defined(__GNUC__)
/* GCC/Clang */
#define EVRNET_PACK(_struct, name) \
    _struct __attribute__((packed)) name;

#else
#error "Don't know how to make a packed struct on this compiler"
#endif

/* how do we / can we do inline */
#if defined(_MSC_VER) && _MSC_VER < 1200
/* available as a MS extension '__inline' */
#  define inline __inline
#elif defined(__GNUC__)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ > 199901L
/* available as 'inline' */
#  elif !defined(__STRICT_ANSI__)
/* available as GNU extension 'inline' */
#  else
#    warning "No known way to do inline functions in this compiler"
#    define inline /* nothing */
#  endif
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
#  warning "No known way to do inline functions in this compiler"
#  define inline /* nothing */
#endif

/* important integer types that C89 doesn't necessarily provide
 * old MSVC doesn't have these by default
 */
#if defined(_MSC_VER) && _MSC_VER < 1300
#  ifdef EVRNET_CPU_IS_32BIT
	typedef unsigned long  uintptr_t;
	typedef long           intptr_t;
#  elif defined(EVRNET_CPU_IS_64BIT)
	typedef unsigned long long uintptr_t;
	typedef long long          intptr_t;
#  endif
/* if you have any other compiler (e.g. a competent GCC/Clang), your stdint.h probably has these already. */
#endif

#endif
