#ifndef RESULT_H_
#define RESULT_H_

// DREAM OS
// DreamOS/RESULT/RESULT.h
// RESULT Header File

#include "STDTYPES.h"

#define RESULT_START_FAIL 0x80000000
#define RESULT_START_OK 0x00000000
#define RESULT_START_WARN 0x20000000

#define DREAM_EPSILON 1.192092896e-07f

#ifndef _USE_MATH_DEFINES
	#define M_E        2.71828182845904523536   // e
	#define M_LOG2E    1.44269504088896340736   // log2(e)
	#define M_LOG10E   0.434294481903251827651  // log10(e)
	#define M_LN2      0.693147180559945309417  // ln(2)
	#define M_LN10     2.30258509299404568402   // ln(10)
	#define M_PI       3.14159265358979323846   // pi
	#define M_PI_2     1.57079632679489661923   // pi/2
	#define M_PI_4     0.785398163397448309616  // pi/4
	#define M_1_PI     0.318309886183790671538  // 1/pi
	#define M_2_PI     0.636619772367581343076  // 2/pi
	#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
	#define M_SQRT2    1.41421356237309504880   // sqrt(2)
	#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
#endif

typedef enum {
    //R_OK = RESULT_START_OK,
    R_K = RESULT_START_OK,
    R_PASS,
    R_SUCCESS,
	R_PASS_TRUE,
	R_PASS_FALSE,
	R_QUEUE_EMPTY,

	R_WARNING = RESULT_START_WARN,
	R_NOT_IMPLEMENTED,
	R_VIRTUAL,
	R_ABSTRACT,
	R_SKIPPED,
	R_SYNC_SKIPPED,
	R_NO_EFFECT,
	R_RESET,
	R_NOT_HANDLED,
	R_DEPRECATED,				// This is not a failure persay, but function may not do anything or work correctly 

	R_FILE_FOUND,
	R_DIRECTORY_FOUND,
	R_PATH_FOUND,

	R_OBJECT_NOT_FOUND,
	R_COMPLETE,
	R_BUFFER_NOT_READY,
	R_HANDLER_FAILED,
    
    R_FAIL = RESULT_START_FAIL,
    R_ERROR,
    
	R_NOT_FOUND,				// general purpose not found error
	R_FILE_NOT_FOUND,			// slightly more specific file not found error
	R_FILE_ALREADY_EXISTS,		// file already exists
	R_INVALID_PARAM,			// invalid parameter 
	R_PATH_NOT_FOUND,			// path not found
	R_INVALID_OBJECT,			// Wrong Object passed to function
	R_OVERFLOW,					// Overflow error of some kind
	R_POOL_FULL,				// Pool has no more objects left
	R_BUFFER_FULL,				// Buffer is full
	R_BUFFER_EMPTY,				// Buffer is empty

    // Matrix Error Codes
	R_MATRIX_ROW_OUT_OF_RANGE,
	R_MATRIX_COL_OUT_OF_RANGE,
    R_MATRIX_MULT_DIMENSION_MISMATCH,
	R_EXIT,
	R_NOT_INITIALIZED,

	R_INVALID
} _RESULT;

typedef _RESULT RESULT;

#endif // ! RESULT_H_
