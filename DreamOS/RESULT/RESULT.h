#ifndef RESULT_H_
#define RESULT_H_

// DREAM OS
// DreamOS/RESULT/RESULT.h
// RESULT Header File

#ifndef NULL
    #define NULL 0
#endif

#define RESULT_START_FAIL 0x80000000

#define RESULT_START_OK 0x00000000

typedef enum {
    R_OK = RESULT_START_OK,
    R_SUCCESS,
    
    R_FAIL = RESULT_START_FAIL,
    R_ERROR,
    R_INVALID
} _RESULT;


typedef _RESULT RESULT;

#endif // ! RESULT_H_
