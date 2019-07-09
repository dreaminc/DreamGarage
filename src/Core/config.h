#ifndef CONFIG_H_
#define CONFIG_H_

// DREAM OS
// DreamOS/Core/config.h
// Core configuration header - this file should be force included in the build (/FI) 
// These configurations should apply to all files

// Type precision
// Float vs Double
#define FLOAT_PRECISION
//#define DOUBLE_PRECISION

// TODO: This is currently a configuration setting
// If in the future DreamOS needs to support different APIs 
// this should no longer be a user defined thing
#define RIGHT_HANDED
//#define LEFT_HANDED

//#define USE_LOCALHOST

// Comment out to use production
//#define _USE_TEST_APP	
//#define _UNIT_TESTING

#if defined(_WIN32)
	#define CORE_CONFIG_SANDBOX_PLATFORM SANDBOX_APP_WIN32

	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <Windows.h>

	#ifdef WIN32_LEAN_AND_MEAN
		#undef WIN32_LEAN_AND_MEAN
	#endif

	#ifndef WIN32
		#define WIN32
	#endif

	#pragma warning( disable : 4312)
	#pragma warning( disable : 4244)	// TODO: WebRTC port.h needs this, it's not a good one though
	#pragma warning( disable:  4102)	// This disables the warning for unused labels (useful for doc and consistency)

#elif defined(__APPLE__)
	#define CORE_CONFIG_SANDBOX_PLATFORM SANDBOX_APP_OSX
#elif defined (__linux__)
	#define CORE_CONFIG_SANDBOX_PLATFORM SANDBOX_APP_LINUX
#else
	#define CORE_CONFIG_SANDBOX_PLATFORM SANDBOX_APP_DREAMOS
#endif

// MSFT Visual Studio
#ifdef _MSC_VER
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS		// Remove the errors for using STANDARD C/++ calls (msft jerks...)
	#endif
#endif

#endif // ! CONFIG_H_
