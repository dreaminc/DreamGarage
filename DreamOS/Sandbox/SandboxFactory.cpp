#include "SandboxFactory.h"

#include "./Sandbox/Windows/Windows64App.h"

SandboxApp* SandboxFactory::MakeSandbox(SANDBOX_APP_TYPE type) {
	SandboxApp *pSandbox = NULL;
	
	switch (type) {
		case SANDBOX_APP_WIN32: {
			pSandbox = new Windows64App(_T("DreamOSSandbox"));
		} break;

		case SANDBOX_APP_OSX:
		default: {

		} break;
	}

	return pSandbox;
}