#ifndef ANIMATION_ITEM_H_
#define ANIMATION_ITEM_H_

#include "RESULT/EHM.h"
#include "Primitives/valid.h"
#include "Primitives/dirty.h"

#include <memory>
#include <functional>

class VirtualObj;
#include "AnimationState.h"
#include "AnimationCurve.h"

struct AnimationFlags {

	unsigned fNoBlock : 1;
	unsigned fLooping  : 1;

	AnimationFlags() :
		fNoBlock(false),
		fLooping(false)
	{};

};

class AnimationItem : public valid, public dirty {
public:

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
	RESULT SetCurveType(AnimationCurveType type);

	std::function<RESULT(void*)> GetEndCallback();
	RESULT SetEndCallback(std::function<RESULT(void*)> callback);

	void* GetCallbackContext();
	RESULT SetCallbackContext(void* context);

private:
	double m_startTime;
	double m_duration;

	AnimationFlags m_flags;
	AnimationCurve m_curveType;

	AnimationState m_startState;
	AnimationState m_endState;

	std::function<RESULT(void*)> m_endCallback;
	void* m_callbackContext;
};

#endif // ! ANIMATION_ITEM_H_