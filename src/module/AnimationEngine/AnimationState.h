#ifndef ANIMATION_STATE_H_
#define ANIMATION_STATE_H_

#include "core/ehm/EHM.h"

// Dream Animation Engine
// dos/src/module/AnimationEngine/AnimationState.h

#include "core/primitives/color.h"
#include "core/primitives/point.h"
#include "core/primitives/quaternion.h"
#include "core/primitives/vector.h"

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
	DimObj* ApplyColor(DimObj *pObj);
	DimObj* ApplyTransform(DimObj *pObj);
};

#endif // ! ANIMATION_STATE_H_
