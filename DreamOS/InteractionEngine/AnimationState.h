#ifndef ANIMATION_STATE_H_
#define ANIMATION_STATE_H_

#include "RESULT/EHM.h"

#include "Primitives/color.h"
#include "Primitives/point.h"
#include "Primitives/quaternion.h"
#include "Primitives/vector.h"

class DimObj;

class AnimationState {
public:
	point ptPosition;
	quaternion qRotation;
	vector vScale;
	color cColor;

public:
	RESULT Compose(AnimationState state);
	DimObj* Apply(DimObj *pObj);
};

#endif // ! ANIMATION_STATE_H_
