#ifndef ANIMATION_QUEUE_H_
#define ANIMATION_QUEUE_H_

#include <map>
#include <queue>
#include <deque>
#include <memory>
#include <functional>

class VirtualObj;
enum class AnimationCurveType;
#include "AnimationItem.h"

class AnimationQueue {

public:
	AnimationQueue();
	~AnimationQueue();

public:
	RESULT Update(double sNow); // updates all objects
	RESULT PushAnimationItem(VirtualObj *pObj,
		AnimationState endState,
		double startTime,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> fnStartCallback = nullptr,
		std::function<RESULT(void*)> fnEndCallback = nullptr,
		void* pCallbackContext = nullptr);

	RESULT CancelAnimation(VirtualObj *pObj, double startTime);

private:
	std::map<VirtualObj*, std::deque<std::shared_ptr<AnimationItem>>> m_objectQueue;

};

#endif // ! ANIMATION_QUEUE_H_