#ifndef DREAM_APP_HANDLE_H_
#define DREAM_APP_HANDLE_H_

#include "core/ehm/RESULT.h"

// Dream App
// dos/src/app/DreamAppHandle.h

// TODO: Either get rid of this or create general interface 
// for app-to-app communication both locally and over the mesh

class DreamAppHandle {
	friend class DreamAppManager;

public:
	DreamAppHandle() :
		m_fAppState(true)
	{
		// empty
	};

	~DreamAppHandle() {
		// empty
	};

protected:
	virtual bool GetAppState() { 
		return m_fAppState; 
	};

	RESULT SetAppState(bool fAppState) { 
		m_fAppState = fAppState; 
		return R_PASS;
	}

private:
	bool m_fAppState;
		
};

#endif DREAM_APP_HANDLE_H_