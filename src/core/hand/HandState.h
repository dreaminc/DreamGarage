#ifndef HAND_STATE_H_
#define HAND_STATE_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/hand/HandState.h

// TODO: Not entirely sure this is the right way to do this 
// as it's a little too implementation specific
#include "core/primitives/point.h"
#include "core/primitives/quaternion.h"

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