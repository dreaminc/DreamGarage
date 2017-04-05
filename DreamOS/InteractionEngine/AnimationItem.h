#ifndef ANIMATION_ITEM_H_
#define ANIMATION_ITEM_H_

#include "RESULT/EHM.h"
#include "Primitives/valid.h"
#include "Primitives/dirty.h"
#include "AnimationState.h"

#include <memory>

class VirtualObj;

class AnimationItem : public valid, public dirty {
public:
	struct AnimationFlags {

		unsigned fNoBlock : 1;
		unsigned fLooping  : 1;

		AnimationFlags() :
			fNoBlock(false),
			fLooping(false)
		{};

	} ANIMATION_FLAGS;

	//TODO currently unused
	enum class AnimationCurveType {
		LINEAR,
		INVALID
	} ANIMATION_CURVE_TYPE;

public:
	AnimationItem(AnimationState startState, AnimationState endState, double startTime, double duration);
	~AnimationItem();

	std::shared_ptr<AnimationItem> CreateCancelAnimation(VirtualObj *pObj, double msNow);

private:
	RESULT Initialize();

public:
	RESULT Update(VirtualObj *pObj, AnimationState& state, double msNow);

	bool IsComplete(double msNow);

	AnimationFlags GetFlags();
	RESULT SetFlags(AnimationFlags flags);

private:
	double m_startTime;
	double m_duration;

	AnimationFlags m_flags;
	AnimationCurveType m_curveType;

	AnimationState m_startState;
	AnimationState m_endState;
};

#endif // ! ANIMATION_ITEM_H_