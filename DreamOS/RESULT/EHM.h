#ifndef EHM_H_
#define EHM_H_

// DREAM OS
// DreamOS/RESULT/EHM.h
// Error Handling Macros

#include "RESULT.h"
#include <assert.h>

// TODO: Tie into the official console/interface system
#define CONSOLE_OUT(str) do { printf(str); } while(0);
#define CONSOLE_LINEOUT(str, ...) do { printf(str, __VA_ARGS__); } while(0);

#ifdef _DEBUG
    #define DEBUG_OUT(str) CONSOLE_OUT(str)
    #define DEBUG_LINEOUT(str, ...) CONSOLE_LINEOUT(str, ##__VA_ARGS__)
#else
    #define DEBUG_OUT(str)
    #define DEBUG_LINEOUT(str, ...)
#endif

#define DEBUG_FILE_LINE

#ifdef DEBUG_FILE_LINE
    #define CurrentFileLine "%s line#%d\n", __FILE__, __LINE__
#else
    #define CurrentFileLine ""
#endif

// Check RESULT value
// Ensures that RESULT is successful
#define CR(res) do{r=res;if(r&0x80000000){goto Error;}}while(0);
#define CRM(res, msg, ...) do{r= res;if(r&0x80000000){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Error: 0x%x\n",r);goto Error;}}while(0)

#define ACRM(res, msg, ...) do{if(res&0x80000000){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0); }}while(0);
#define ACR(res) do{if(res&0x80000000){assert(0); }}while(0);

// Check Boolean Result
// Ensures that condition evaluates to true
#define CB(condition) do{if(!condition) {r = R_FAIL; goto Error;}}while(0);
#define CBR(condition, failCode) do{if(!condition) {r = failCode; goto Error;}}while(0);
#define CBM(condition, msg, ...) do{if(!condition) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_FAIL; goto Error; }}while(0);
#define CBRM(condition, failCode, msg, ...) do{if(!condition) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);

#define ACBM(condition, msg, ...) do{if(!condition){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACB(condition) do{if(!condition){assert(0);}}while(0);

// Check NULL Result
// Ensures that the pointer is not a NULL
#define CN(pointer) do{if(pointer == NULL) {r = R_ERROR; goto Error;}}while(0);
#define CNR(pointer, failCode) do{if(pointer == NULL) {r = failCode; goto Error;}}while(0);
#define CNM(pointer, msg, ...) do{if(pointer == NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_ERROR; goto Error; }}while(0);
#define CNRM(pointer, failCode, msg, ...) do{if(pointer == NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);

#define ACNM(pointer, msg, ...) do{if(pointer == NULL){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACN(pointer) do{if(pointer == NULL){assert(0);}}while(0);

// Check Pointer Result
// Ensures that the pointer is not a NULL
#define CP(pointer) CN(pointer)
#define CPR(pointer, failCode) CNR(pointer, failCode)
#define CPM(pointer, msg, ...) CNM(pointer, msg, ##__VA_ARGS__)
#define CPRM(pointer, msg, ...) CNRM(pointer, failCode, msg, ##__VA_ARGS__)

// Check Handle Message
// Ensures that the pointer is not a NULL
#define CH(pointer) CN(pointer)
#define CHR(pointer) CNR(pointer, failCode)
#define CHM(pointer, msg, ...) CNM(pointer, msg, ##__VA_ARGS__)
#define CHRM(pointer, msg, ...) CNRM(pointer, failCode, msg, ##__VA_ARGS__)

// Calculate the memory offset of a field in a struct
#define STRUCT_FIELD_OFFSET(struct_type, field_name)    ((long)(long*)&(((struct_type *)0)->field_name))

#endif // ! EHM_H_
