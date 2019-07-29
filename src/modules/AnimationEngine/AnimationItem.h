#ifndef ANIMATION_ITEM_H_
#define ANIMATION_ITEM_H_

#include "core/ehm/EHM.h"

// Dream Animation Engine
// dos/src/module/AnimationEngine/AnimationItem.h

#include <memory>
#include <functional>

#include "core/types/DObject.h"
#include "core/types/dirty.h"

#include "AnimationState.h"
#include "AnimationCurve.h"

class DimObj;

struct AnimationFlags {

	unsigned fNoBlock : 1;
	unsigned fLooping  : 1;

	AnimationFlags() :
		fNoBlock(false),
		fLooping(false)
	{};

};

class AnimationItem : public DObject, public dirty {
public:

public:
	AnimationItem(AnimationState startState, AnimationState endState, double startTime, double duration);
	~AnimationItem();

	std::shared_ptr<AnimationItem> CreateCancelAnimation(DimObj *pObj, double msNow);

private:
	RESULT Initialize();

public:
	RESULT Update(DimObj *pObj, AnimationState& state, double msNow);

	bool IsComplete(double msNow);

	AnimationFlags GetFlags();
	RESULT SetFlags(AnimationFlags flags);
	RESULT SetCurveType(AnimationCurveType type);

	std::function<RESULT(void*)> GetAnimationEndedCallback();
	RESULT SetAnimationEndedCallback(std::function<RESULT(void*)> callback);

	std::function<RESULT(void*)> GetAnimationStartCallback();
	RESULT SetAnimationStartCallback(std::function<RESULT(void*)> callback);

	void* GetCallbackContext();
	RESULT SetCallbackContext(void* context);

	bool ShouldAnimateColor();
	bool ShouldAnimateObject();

private:
	double m_startTime;
	double m_duration;

	AnimationFlags m_flags;
	AnimationCurve m_curveType;

	AnimationState m_startState;
	AnimationState m_endState;

	std::function<RESULT(void*)> m_fnOnAnimationEnded;
	std::function<RESULT(void*)> m_fnOnAnimationStart;
	void* m_fnOnAnimationContext;
};

#endif // ! ANIMATION_ITEM_H_