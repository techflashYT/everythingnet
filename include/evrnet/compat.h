/*
 * EverythingNet - Common headers - Compiler compatibility
 * Copyright (C) 2025 Techflash
 */

#ifndef _COMPAT_H
#define _COMPAT_H

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
/* available as a GNU extension 'inline' */
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
#  warning "No known way to do inline functions in this compiler"
#  define inline /* nothing */
#endif


#endif
