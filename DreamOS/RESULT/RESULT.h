#ifndef RESULT_H_
#define RESULT_H_

// DREAM OS
// DreamOS/RESULT/RESULT.h
// RESULT Header File

#include "STDTYPES.h"

#define RESULT_START_FAIL 0x80000000
#define RESULT_START_OK 0x00000000
#define RESULT_START_WARN 0x70000000

#define M_PI       3.14159265358979323846   // pi

typedef enum {
    //R_OK = RESULT_START_OK,
    R_K = RESULT_START_OK,
    R_PASS,
    R_SUCCESS,

	R_WARNING = RESULT_START_WARN,
	R_NOT_IMPLEMENTED,
	R_VIRTUAL,
	R_ABSTRACT,

	R_FILE_FOUND,
	R_DIRECTORY_FOUND,
	R_PATH_FOUND,
    
    R_FAIL = RESULT_START_FAIL,
    R_ERROR,
    
	R_NOT_FOUND,			// general purpose not found error
	R_FILE_NOT_FOUND,		// slightly more specific file not found error
	R_INVALID_PARAM,		// invalid parameter 
	R_PATH_NOT_FOUND,		// path not found
	R_INVALID_OBJECT,		// Wrong Object passed to function

    // Matrix Error Codes
	R_MATRIX_ROW_OUT_OF_RANGE,
	R_MATRIX_COL_OUT_OF_RANGE,
    R_MATRIX_MULT_DIMENSION_MISMATCH,

	R_INVALID
} _RESULT;

typedef _RESULT RESULT;

#endif // ! RESULT_H_
