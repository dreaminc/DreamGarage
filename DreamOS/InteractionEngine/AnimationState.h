#ifndef ANIMATION_STATE_H_
#define ANIMATION_STATE_H_

#include "RESULT/EHM.h"
#include "Primitives/point.h"
#include "Primitives/quaternion.h"
#include "Primitives/vector.h"

class VirtualObj;

class AnimationState {
public:
	point ptPosition;
	quaternion qRotation;
	vector vScale;

public:
	RESULT Compose(AnimationState state);
	VirtualObj* Apply(VirtualObj *pObj);
};

#endif // ! ANIMATION_STATE_H_
