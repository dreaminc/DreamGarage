#ifndef ANIMATION_QUEUE_H_
#define ANIMATION_QUEUE_H_

#include "InteractionEngine/AnimationItem.h"

#include <map>
#include <queue>
#include <deque>
#include <memory>

class VirtualObj;

class AnimationQueue {

public:
	AnimationQueue();
	~AnimationQueue();

public:
	RESULT Update(); // updates all objects
	RESULT PushAnimationItem(VirtualObj *pObj,
		AnimationState endState,
		double duration,
		AnimationItem::AnimationFlags flags = AnimationItem::AnimationFlags());
							//AnimationCurveType curve = AnimationCurveType::LINEAR);

	RESULT CancelAnimation(VirtualObj *pObj);

private:
	std::map<VirtualObj*, std::deque<std::shared_ptr<AnimationItem>>> m_objectQueue;

};

#endif // ! ANIMATION_QUEUE_H_