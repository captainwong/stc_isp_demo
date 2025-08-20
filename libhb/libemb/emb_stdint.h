#ifndef __EMB_STDINT_H__
#define __EMB_STDINT_H__

#if defined(__C51__) && !defined(VSCODE)
// standard integer types
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

// size_t
#if !defined(__SIZE_T) && !defined(_SIZE_T)
#define __SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#else // __C51__
#include <stdint.h>
#include <stddef.h>
#endif // __C51__

#endif /* __EMB_STDINT_H__ */
