/*
 * EverythingNet - Common headers - Compiler compatibility - Struct packing (End)
 * Copyright (C) 2025 Techflash
 */

#ifdef _MSC_VER
/* pragma pack for MSVC */
#pragma pack(pop)

#elif defined(__GNUC__)
/* Nothing for GCC/Clang */

#else
#warning "Unknown compiler, assuming no required struct packing footer"
#endif
