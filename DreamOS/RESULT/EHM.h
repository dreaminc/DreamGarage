#ifndef EHM_H_
#define EHM_H_

// DREAM OS
// DreamOS/RESULT/EHM.h
// Error Handling Macros

#include "RESULT.h"

#include <assert.h>

#include <stdio.h>
#include <stddef.h>

#define DEBUG_OUT_TO_CONSOLE
//#define DEBUG_OUT_TO_WIN_DEBUGGER

/*
#ifndef NOARRAYSIZE
template <typename T, size_t N> char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))
#endif
*/

// Logging (needs DreamLogger included)
//#define _ENABLE_LOGGING
#ifdef _ENABLE_LOGGING
	#define DOSLOG(level, strMsg, ...) do { \
		DreamLogger::instance()->Log(DreamLogger::Level::level, strMsg, ##__VA_ARGS__); \
	} while (0);
#else
	#define DOSLOG(level, strMsg, ...)
#endif
// ------------------------------------
 
#if defined(DEBUG_OUT_TO_CONSOLE)
	// TODO: Tie into the official console/interface system
	#define CONSOLE_OUT(str, ...) do { printf(str, ##__VA_ARGS__); } while(0);
#elif defined(DEBUG_OUT_TO_WIN_DEBUGGER)
	// empty
#endif


// Console Output
#ifdef _DEBUG
    #define DEBUG_OUT(str, ...) do { CONSOLE_OUT(str, ##__VA_ARGS__); } while(0);
    #define DEBUG_LINEOUT(str, ...) do { CONSOLE_OUT(str, ##__VA_ARGS__); CONSOLE_OUT("\n"); } while(0); 
	#define DEBUG_LINEOUT_RETURN(str, ...) do { CONSOLE_OUT(str, ##__VA_ARGS__); CONSOLE_OUT("\r"); } while(0); 
	#define DEBUG_SYSTEM_PAUSE() do { system("pause"); } while(0); 
#else
	#define DEBUG_OUT(str, ...)
	#define DEBUG_LINEOUT(str, ...)
	#define DEBUG_LINEOUT_RETURN(str, ...) 
	#define DEBUG_SYSTEM_PAUSE()
#endif


// DevEnv output (available in release) but should throw a production error
#ifdef _WIN32
	#define DOS_DEBUGGER_SIGNATURE "DOS::"
	#define DOS_DEBUGGER_SIGNATURE_SIZE	5		// size in bytes
	#define	DOS_DEBUGGER_OUTPUT_MAX_SIZE	1024

	extern void OutputDebugString(wchar_t*);
	static char szDebugOutputString[DOS_DEBUGGER_OUTPUT_MAX_SIZE] = { DOS_DEBUGGER_SIGNATURE };

	//#define DEVENV_LINEOUT(str) do { OutputDebugString(str); } while(0); 
		
	#define DEVENV_LINEOUT(str, ...) do {																												\
		sprintf_s(szDebugOutputString + DOS_DEBUGGER_SIGNATURE_SIZE, DOS_DEBUGGER_OUTPUT_MAX_SIZE - DOS_DEBUGGER_SIGNATURE_SIZE, str, ##__VA_ARGS__);	\
		if (szDebugOutputString[DOS_DEBUGGER_SIGNATURE_SIZE] != '\n' && szDebugOutputString[DOS_DEBUGGER_SIGNATURE_SIZE] != '\r')						\
			OutputDebugStringA(szDebugOutputString);																									\
		} while(0);
#endif

#define DEBUG_FILE_LINE
#ifdef DEBUG_FILE_LINE
    #define CurrentFileLine "%s line#%d\n", __FILE__, __LINE__
#else
    #define CurrentFileLine ""
#endif

#define RFAILED() (r&0x80000000)
#define RSUCCESS() (!RFAILED())

// Check RESULT value
// Ensures that RESULT is successful
#define CR(res) do{r=(res);if(r&0x80000000){goto Error;}}while(0);
#define CRM(res, msg, ...) do{r= (res);if(r&0x80000000){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Error: 0x%x\n",r);goto Error;}}while(0);

// Check result no assign (this allows for mapping with non RESULT error types without having to cast
#define CRNA(res) do{if(res&0x80000000){goto Error;r=R_FAIL;}}while(0);
#define CRNAM(res, msg, ...) do{if(res&0x80000000){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Error: 0x%x\n", res);r=R_FAIL;goto Error;}}while(0);

#define ACRM(res, msg, ...) do{if((res)&0x80000000){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0); }}while(0);
#define ACR(res) do{if((res)&0x80000000){assert(0); }}while(0);

// Warning
#define WCR(res) do{r=(res);if(r&0x80000000){/*goto Error;*/}}while(0);
#define WCRM(res, msg, ...) do{r = (res);if(r&0x80000000){DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Warning: 0x%x\n",r);/*goto Error;*/}}while(0)

// Check Boolean Result
// Ensures that condition evaluates to true
#define CB(condition) do{if(!(condition)) {r = R_FAIL; goto Error;}}while(0);
#define WCB(condition) do{if(!(condition)) {r = R_WARNING; goto Error;}}while(0);
#define CBR(condition, failCode) do{if(!(condition)) {r = failCode; goto Error;}}while(0);
#define CBM(condition, msg, ...) do{if(!(condition)) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_FAIL; goto Error; }}while(0);
#define CBRM(condition, failCode, msg, ...) do{if(!(condition)) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);

#define ACBM(condition, msg, ...) do{if(!condition){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACB(condition) do{if(!condition){assert(0);}}while(0);

// Check Range
#define CRANGE(val, valMin, valMax) do{if(!(val > valMax) || !(val < valMin)) {r = R_FAIL; goto Error;}}while(0);

// Check NULL Result
// Ensures that the pointer is not a NULL
#define CN(pointer) do{if((pointer) == NULL) {r = R_ERROR; goto Error;}}while(0);
#define WCN(pointer) do{if((pointer) == NULL) {r = R_WARNING; goto Error;}}while(0);
#define CNR(pointer, failCode) do{if((pointer) == NULL) {r = failCode; goto Error;}}while(0);
#define CNM(pointer, msg, ...) do{if((pointer) == NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_ERROR; goto Error; }}while(0);
#define CNMW(pointer, msg, ...) do{if((pointer) == NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_WARNING; }}while(0);
#define CNRM(pointer, failCode, msg, ...) do{if((pointer) == NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);

#define ACNM(pointer, msg, ...) do{if((pointer) == NULL){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACN(pointer) do{if((pointer) == NULL){assert(0);}}while(0);

// Check is null (ensure null) result
// Ensures that the pointer IS NULL
#define CBN(pointer) do{if((pointer) != NULL) {r = R_ERROR; goto Error;}}while(0);
#define WCBN(pointer) do{if((pointer) != NULL) {r = R_WARNING; goto Error;}}while(0);
#define CBNR(pointer, failCode) do{if((pointer) != NULL) {r = failCode; goto Error;}}while(0);
#define CBNM(pointer, msg, ...) do{if((pointer) != NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_ERROR; goto Error; }}while(0);
#define CBNMW(pointer, msg, ...) do{if((pointer) != NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_WARNING; }}while(0);
#define CBNRM(pointer, failCode, msg, ...) do{if((pointer) != NULL) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);

#define ABCNM(pointer, msg, ...) do{if((pointer) != NULL){DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ABCN(pointer) do{if((pointer) != NULL){assert(0);}}while(0);

// Check Pointer Result
// Ensures that the pointer is not a NULL
#define CP(pointer) CN(pointer)
#define WCP(pointer) WCN(pointer)
#define CPR(pointer, failCode) CNR(pointer, failCode)
#define CPM(pointer, msg, ...) CNM(pointer, msg, ##__VA_ARGS__)
#define CPRM(pointer, msg, ...) CNRM(pointer, failCode, msg, ##__VA_ARGS__)

// Check Handle Message
// Ensures that the pointer is not a NULL
#define CH(pointer) CN(pointer)
#define CHR(pointer) CNR(pointer, failCode)
#define CHM(pointer, msg, ...) CNM(pointer, msg, ##__VA_ARGS__)
#define CHRM(pointer, msg, ...) CNRM(pointer, failCode, msg, ##__VA_ARGS__)

// Check Valid 
// Ensures that the object created is valid
#define CV(pObject) do{if(!((pObject)->IsValid())) {r = R_FAIL; goto Error;}}while(0);
#define CVR(pObject, failCode) do{if(!((pObject)->IsValid())) {r = failCode; goto Error;}}while(0);
#define CVM(pObject, msg, ...) do{if(!((pObject)->IsValid())) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_FAIL; goto Error; }}while(0);
#define CVRM(pObject, failCode, msg, ...) do{if(!((pObject)->IsValid())) { DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode; goto Error; }}while(0);


#endif // ! EHM_H_
