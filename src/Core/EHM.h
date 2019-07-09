#ifndef EHM_H_
#define EHM_H_

// DREAM OS
// DreamOS/RESULT/EHM.h
// Error Handling Macros

#include "RESULT.h"

#include <assert.h>

#include <stdio.h>
#include <stddef.h>

// Config
#define DEBUG_OUT_TO_CONSOLE
//#define DEBUG_OUT_TO_WIN_DEBUGGER
#define DEBUG_FILE_LINE

/*
#ifndef NOARRAYSIZE
template <typename T, size_t N> char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))
#endif
*/


#define RFAILED() (r&0x80000000)
#define RSUCCESS() (!RFAILED())
#define RCHECK(res) (!(res & 0x80000000))

// Logging

#include "DreamLogger/DreamLogger.h"

// Logging (needs DreamLogger included)
// This has been moved to project config, otherwise it breaks across multiple projects
// This requirement will be fixed when we move away from the monolithic build
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

/*
#ifndef __FUNCTION_NAME__
	#ifdef WIN32   //WINDOWS
		#define CurrentFunctionName  __FUNCTION__  
	#else          //*NIX
		#define CurrentFunctionName   __func__ 
	#endif
#else
	#define CurrentFunctionName __FUNCTION_NAME__
#endif
*/

#ifdef DEBUG_FILE_LINE
	#define CurrentFileLine "%s line#%d func:%s ", __FILE__, __LINE__, __FUNCTION__
#else
    #define CurrentFileLine ""
#endif

#define LOG_DOS_MSGS
#ifdef LOG_DOS_MSGS
	#define	DOS_LOG_OUTPUT_MAX_SIZE	1024
	static char szDosLogOutputString[DOS_DEBUGGER_OUTPUT_MAX_SIZE] = { DOS_DEBUGGER_SIGNATURE };

	#define DOSLogLine(level, crt, r) DOSLOG(ERR, "%s:%s:%s(%d):Error: 0x%x\n", crt, __BASE_FILE__, __FUNCTION__, __LINE__, r)
#define DOSLogLineMessage(level, crt, r, msg, ...) do {																	\
		sprintf_s(szDosLogOutputString, msg, ##__VA_ARGS__);															\
		DOSLOG(level, "%s:%s:%s(%d:0x%x):%s\n", crt, __BASE_FILE__, __FUNCTION__, __LINE__, r, szDosLogOutputString)			\
	} while (0);

	#define DOSLogError(crt, r) DOSLogLine(ERR, crt, r)
	#define DOSLogErrorMessage(crt, r, msg, ...) DOSLogLineMessage(ERR, crt, r, msg, ##__VA_ARGS__)

	#define DOSLogWarning(crt, r) DOSLogLine(WARN, crt, r)
	#define DOSLogWarningMessage(crt, r, msg, ...) DOSLogLineMessage(WARN, crt, r, msg, ##__VA_ARGS__)
#else
	DOSLogLine(level, crt, r)
	DOSLogLineMessage(level, crt, r, msg, ....)

	#define DOSLogError(crt, r)
	#define DOSLogErrorMessage(crt, r, msg, ...)

	#define DOSLogWarning(crt, r)
	#define DOSLogWarningMessage(crt, r, msg, ...)
#endif

// Check RESULT value
// Ensures that RESULT is successful
#define CR(res) do{r=(res);if(r&0x80000000){DOSLogError("CR", r); goto Error;}}while(0);
#define CRM(res, msg, ...) do{r= (res);if(r&0x80000000){DOSLogErrorMessage("CRM", res, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("Error: 0x%x\n",r);goto Error;}}while(0);

// CP - Critical Path
// This will resolve to the internal value in release
#ifdef _DEBUG
	#define CRCP(res) CR(res)
	#define CRMCP(res, msg, ...) CRM(res, msg, ##__VA_ARGS__)
	#define CBCP(condition) CB(condition)
	#define CBMCP(condition, msg, ...) CBM(condition, msg, ##__VA_ARGS__)
	#define CBCPS(condition) CB(condition)
	#define CBMCPS(condition, msg, ...) CBM(condition, msg, ##__VA_ARGS__)
#else
	#define CRCP(res) res
	#define CRMCP(res, msg, ...) res
	#define CBCP(condition) condition
	#define CBMCP(condition, msg, ...) condition
	#define CBCPS(condition) 
	#define CBMCPS(condition, msg, ...) 
#endif

// Check result no assign (this allows for mapping with non RESULT error types without having to cast
#define CRNA(res) do{if(res&0x80000000){DOSLogError("CRNA", r); r=R_FAIL;goto Error;}}while(0);
#define CRNAM(res, msg, ...) do{if(res&0x80000000){DOSLogErrorMessage("CRNAM", res, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("Error: 0x%x\n", res);r=R_FAIL;goto Error;}}while(0);

#define ACRM(res, msg, ...) do{if((res)&0x80000000){DOSLogErrorMessage("ACRM", res, msg, ##__VA_ARGS__); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0); }}while(0);
#define ACR(res) do{if((res)&0x80000000){DOSLogError("ACR", res);;assert(0); }}while(0);

// Warning
#define WCR(res) do{r=(res);if(r&0x80000000){DOSLogWarning("WCR", r);/*goto Error;*/}}while(0);
#define WCRM(res, msg, ...) do{r = (res);if(r&0x80000000){DOSLogWarningMessage("WCRM", res, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine);DEBUG_OUT(msg, ##__VA_ARGS__);DEBUG_OUT("Warning: 0x%x\n",r);/*goto Error;*/}}while(0)

// Check Boolean Result
// Ensures that condition evaluates to true
#define CB(condition) do{if(!(condition)) {r = R_FAIL; DOSLogError("CB", r);goto Error;}}while(0);
#define WCB(condition) do{if(!(condition)) {r = R_WARNING; DOSLogError("WCB", r);goto Error;}}while(0);
#define CBR(condition, failCode) do{if(!(condition)) {r = failCode; if(r&0x80000000){DOSLogError("CBR", r);} goto Error;}}while(0);
#define CBM(condition, msg, ...) do{if(!(condition)) { DOSLogErrorMessage("CBM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_FAIL; goto Error; }}while(0);
#define WCBM(condition, msg, ...) do{if(!(condition)) { DOSLogWarningMessage("WCBM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = R_WARNING;/*goto Error;*/}}while(0);
#define CBRM(condition, failCode, msg, ...) do{if(!(condition)) {r = failCode; if(r&0x80000000){ DOSLogErrorMessage("CBRM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); r = failCode;} goto Error; }}while(0);

#define ACBM(condition, msg, ...) do{if(!condition){DOSLogErrorMessage("ACBM", R_FAIL, msg, ##__VA_ARGS__); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACB(condition) do{if(!condition){DOSLogError("ACB", R_FAIL); assert(0);}}while(0);

// Check Range
#define CRANGE(val, valMin, valMax) do{if(!(val > valMax) || !(val < valMin)) {r = R_FAIL; DOSLogError("CRANGE", r); goto Error;}}while(0);

// Check NULL Result
// Ensures that the pointer is not a NULL
#define CN(pointer) do{if((pointer) == NULL) {r = R_ERROR; DOSLogError("CN", r); goto Error;}}while(0);
#define WCN(pointer) do{if((pointer) == NULL) {r = R_WARNING; DOSLogWarning("WCN", r); goto Error;}}while(0);
#define CNR(pointer, failCode) do{if((pointer) == NULL) {r = failCode; if(r&0x80000000){ DOSLogError("CNR", r);}goto Error;}}while(0);
#define CNM(pointer, msg, ...) do{if((pointer) == NULL) {r = R_ERROR; DOSLogErrorMessage("CNM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); goto Error; }}while(0);
#define WCNM(pointer, msg, ...) do{if((pointer) == NULL) { r = R_WARNING; DOSLogWarningMessage("WCNM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n");  }}while(0);
#define CNRM(pointer, failCode, msg, ...) do{if((pointer) == NULL) {r = failCode; if(r&0x80000000){ DOSLogErrorMessage("CNRM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n");} goto Error; }}while(0);

#define ACNM(pointer, msg, ...) do{if((pointer) == NULL){DOSLogErrorMessage("ACNM", R_FAIL, msg, ##__VA_ARGS__); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ACN(pointer) do{if((pointer) == NULL){DOSLogError("ACN", R_FAIL); assert(0);}}while(0);

// Check is null (ensure null) result
// Ensures that the pointer IS NULL
#define CBN(pointer) do{if((pointer) != NULL) {r = R_ERROR; DOSLogError("CBM", r); goto Error;}}while(0);
#define WCBN(pointer) do{if((pointer) != NULL) {r = R_WARNING; DOSLogWarning("WCBN", r); goto Error;}}while(0);
#define CBNR(pointer, failCode) do{if((pointer) != NULL) { r = failCode; if(r&0x80000000){ DOSLogError("CBNR", r);}goto Error;}}while(0);
#define CBNM(pointer, msg, ...) do{if((pointer) != NULL) { r = R_ERROR; DOSLogErrorMessage("CBNM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n");  goto Error; }}while(0);
#define WCBNM(pointer, msg, ...) do{if((pointer) != NULL) { r = R_WARNING; DOSLogWarningMessage("CBNMW", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); }}while(0);
#define CBNRM(pointer, failCode, msg, ...) do{if((pointer) != NULL) { r = failCode; if(r&0x80000000){ DOSLogErrorMessage("CBNRM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n");} goto Error; }}while(0);

#define ABCNM(pointer, msg, ...) do{if((pointer) != NULL){DOSLogErrorMessage("ABCNM", r, msg, ##__VA_ARGS__); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); assert(0);}}while(0);
#define ABCN(pointer) do{if((pointer) != NULL){DOSLogError("ABCN", r); assert(0);}}while(0);

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
#define CV(pObject) do{if(!((pObject)->IsValid())) {r = R_FAIL; DOSLogError("CV", r); goto Error;}}while(0);
#define CVR(pObject, failCode) do{if(!((pObject)->IsValid())) {r = failCode; DOSLogError("CVR", r); goto Error;}}while(0);
#define CVM(pObject, msg, ...) do{if(!((pObject)->IsValid())) {r = R_FAIL; DOSLogErrorMessage("CVM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); goto Error; }}while(0);
#define CVRM(pObject, failCode, msg, ...) do{if(!((pObject)->IsValid())) {r = failCode; DOSLogErrorMessage("CVRM", r, msg, ##__VA_ARGS__); DEBUG_OUT(CurrentFileLine); DEBUG_OUT(msg, ##__VA_ARGS__); DEBUG_OUT("\n"); goto Error; }}while(0);

#endif // ! EHM_H_
