#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#include <stdint.h>

// DREAM OS
// DreamOS/RESULT/STDTYPES.h
// The standard types typedefs for the system
// Ultimately, this should replace the need for stdint.h on the native platform

// Define some common INT types
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;

typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

typedef float FLOAT32;
typedef double FLOAT64;

#ifndef NULL
	#define NULL 0
#endif

#endif // !STD_TYPES_H_
