#ifndef RENDERING_CONTEXT_H_ 
#define RENDERING_CONTEXT_H_

#include "core/ehm/EHM.h"

// Dream HAL
// dos/src/hal/RenderingContext.h

// This class is the base class that should then be extended for the various platform
// targets

#include "core/types/DObject.h"
#include "core/types/version.h"

class Sandbox;

class RenderingContext : public DObject {
public:
	RenderingContext() {
		// empty
	}

	~RenderingContext() {
			// empty
	}

	virtual RESULT InitializeRenderingContext() = 0;
	virtual RESULT InitializeRenderingContext(version versionOGL) = 0;
	virtual RESULT MakeCurrentContext() = 0;
	virtual RESULT ReleaseCurrentContext() = 0;

public:
	RESULT SetParentApp(Sandbox* pParentSandbox) {
		m_pParentSandbox = pParentSandbox;
		return R_PASS;
	}

	Sandbox *GetParentSandbox() { return m_pParentSandbox; }

protected:
	Sandbox *m_pParentSandbox = nullptr;		// Parent app
};

#endif // ! RENDERING_CONTEXT_H_

