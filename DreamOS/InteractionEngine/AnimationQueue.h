#ifndef ANIMATION_QUEUE_H_
#define ANIMATION_QUEUE_H_

#include <map>
#include <queue>
#include <deque>
#include <memory>
#include <functional>

class DimObj;
enum class AnimationCurveType;
#include "AnimationItem.h"

class AnimationQueue {

public:
	AnimationQueue();
	~AnimationQueue();

public:
	RESULT Update(double sNow); // updates all objects
	RESULT PushAnimationItem(DimObj *pObj,
		AnimationState endState,
		double startTime,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> fnStartCallback = nullptr,
		std::function<RESULT(void*)> fnEndCallback = nullptr,
		void* pCallbackContext = nullptr);

	RESULT CancelAnimation(DimObj *pObj, double startTime);
	RESULT RemoveAnimationObject(DimObj *pObj);
	RESULT RemoveAllObjects();

private:
	std::map<DimObj*, std::deque<std::shared_ptr<AnimationItem>>> m_objectQueue;

};

#endif // ! ANIMATION_QUEUE_H_