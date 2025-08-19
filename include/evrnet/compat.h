/*
 * EverythingNet - Common headers - Compiler compatibility
 * Copyright (C) 2025 Techflash
 */

#ifndef _COMPAT_H
#define _COMPAT_H

/* MSVC */
#ifdef _MSC_VER
#define EVRNET_PACK(_struct, name) \
    __pragma(pack(push, 1)) \
    _struct name; \
    __pragma(pack(pop))

#elif defined(__GNUC__)
/* GCC/Clang */
#define EVRNET_PACK(_struct, name) \
    _struct __attribute__((packed)) name;

#else
#error "Don't know how to make a packed struct on this compiler"
#endif

#endif
