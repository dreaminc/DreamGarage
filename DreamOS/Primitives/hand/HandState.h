#ifndef HAND_STATE_H_
#define HAND_STATE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/hand/HandState.h


#include "Primitives/point.h"
#include "Primitives/quaternion.h"

#include "HandType.h"

struct HandState {

	point ptPalm;
	quaternion qOrientation;

	HAND_TYPE handType;
	bool fTracked;
	unsigned int reserved0;
	unsigned int reserved1;

	RESULT PrintState() {
		ptPalm.Print();
		return R_PASS;
	}
};

#endif // HAND_STATE_H_