#ifndef ANIMATION_ITEM_H_
#define ANIMATION_ITEM_H_

#include "RESULT/EHM.h"
#include "Primitives/valid.h"
#include "Primitives/dirty.h"

#include <chrono>
#include <memory>

#include "Primitives/point.h"
#include "Primitives/quaternion.h"
#include "Primitives/vector.h"

class VirtualObj;

/*
enum class AnimationFlags {

	NO_BLOCK = 0x1,
	LOOPING = 0x2

} ANIMATION_FLAGS;
/*

enum class AnimationType {
	POSITION,
	ORIENTATION,
	SCALE,
	INVALID
} ANIMATION_TYPE;
//*/

class AnimationState {
public:
	point ptPosition;
	quaternion qRotation;
	vector vScale;

public:
	RESULT Compose(AnimationState state);
	VirtualObj* Apply(VirtualObj *pObj);
};

class AnimationItem : public valid, public dirty {
public:
	enum class AnimationFlags {

		NO_BLOCK = 0x1,
		LOOPING = 0x2

	} ANIMATION_FLAGS;

	enum class AnimationCurveType {
		LINEAR,
		INVALID
	} ANIMATION_CURVE_TYPE;

public:
	AnimationItem(AnimationState startState, AnimationState endState, double duration);
	~AnimationItem();

	std::shared_ptr<AnimationItem> CreateCancelAnimation();

private:
	RESULT Initialize();

public:
	RESULT StartAnimation(VirtualObj *pObj = nullptr);
	AnimationState Update();
	RESULT EndAnimation();

	RESULT UpdateStartTime();
	bool IsComplete();

	RESULT SetFlags(int flags);
	int GetFlags();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
	double m_duration;

	int m_flags;
	AnimationCurveType m_type;

	AnimationState m_startState;
	AnimationState m_endState;
};




#endif // ! ANIMATION_ITEM_H_