/*
 * EverythingNet - Common headers - Compiler compatibility - Struct packing (Start)
 * Copyright (C) 2025 Techflash
 */

#ifdef _MSC_VER
/* pragma pack for MSVC */

/* disable the stupid warning that yells at us for doing exactly this thing */
#pragma warning(disable: 4103)
#pragma pack(push, 1)

#elif defined(__GNUC__)
/* Nothing for GCC/Clang */

#else
#warning "Unknown compiler, assuming no required struct packing header"
#endif
