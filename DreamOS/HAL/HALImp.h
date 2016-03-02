#ifndef HAL_IMP_H_
#define HAL_IMP_H_

// Dream OS
// DreamOS/HAL/HALImp.h
// The HAL Implementation class  is the parent class for implementations
// such as the OpenGL implementation and ultimately native ones as well

#include "./RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/Subscriber.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class HALImp : public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseMouseEvent> {
public:
	HALImp() {
		// empty stub
	}

	~HALImp() {
		// empty stub
	}

public:

private:
	UID m_uid;
};

#endif // ! HAL_IMP_H_