#ifndef DREAM_APP_HANDLE_H_
#define DREAM_APP_HANDLE_H_

#include "RESULT/RESULT.h"

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