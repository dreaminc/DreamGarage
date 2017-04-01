#ifndef ANIMATION_QUEUE_H_
#define ANIMATION_QUEUE_H_

#include "InteractionEngine/AnimationItem.h"

#include <map>
#include <queue>
#include <memory>

class VirtualObj;

class AnimationQueue {

public:
	AnimationQueue();
	~AnimationQueue();

public:
	RESULT Update(double msTimeStep); // updates all objects
	RESULT PushAnimationItem(VirtualObj *pObj,
		AnimationState endState,
		double duration);
							//AnimationCurveType curve = AnimationCurveType::LINEAR);

private:
	//std::map<VirtualObj*, std::queue<std::shared_ptr<AnimationItem>>> m_objectQueue;
	//std::map<VirtualObj*, int> m_objectQueue;
	std::map<VirtualObj*, std::queue<std::shared_ptr<AnimationItem>>> m_objectQueue;

};

#endif // ! ANIMATION_QUEUE_H_