#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/framebuffer.h
// Framebuffer Primitve
// The parent object for the Framebuffer 

#include "Primitives/Types/UID.h"

class framebuffer {
	framebuffer() {
		// empty
	}

	~framebuffer() {
		// empty
	}

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // ! FRAMEBUFFER_H_