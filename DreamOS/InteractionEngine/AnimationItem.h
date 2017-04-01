#ifndef ANIMATION_ITEM_H_
#define ANIMATION_ITEM_H_

#include "RESULT/EHM.h"
#include "Primitives/valid.h"

#include <chrono>

#include "Primitives/point.h"
#include "Primitives/quaternion.h"
#include "Primitives/vector.h"

class VirtualObj;

/*
enum class AnimationFlags {

	NO_BLOCK = 0x1,
	LOOPING = 0x2

} ANIMATION_FLAGS;
enum class AnimationCurveType {
	LINEAR,
	INVALID
} ANIMATION_CURVE_TYPE;

enum class AnimationType {
	POSITION,
	ORIENTATION,
	SCALE,
	INVALID
} ANIMATION_TYPE;
//*/

struct AnimationState {
	point ptPosition;
	quaternion qRotation;
	vector vScale;
};

class AnimationItem : public valid {

public:
	AnimationItem(AnimationState startState, AnimationState endState, double duration);
	~AnimationItem();

private:
	RESULT Initialize();

public:
	RESULT Update(VirtualObj* pObj, double msTimeStep);

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
	double m_duration;

	unsigned int m_flags;
	//AnimationCurveType m_type;

	AnimationState m_startState;
	AnimationState m_endState;
};




#endif // ! ANIMATION_ITEM_H_