#ifndef ACTIVE_OBJECT_QUEUE_H_
#define ACTIVE_OBJECT_QUEUE_H_

#include "RESULT/EHM.h"

// The Active Object Queue

#include <memory>
#include <vector>
#include <map>
#include <list>

#include "ActiveObject.h"

class VirtualObj;

class ActiveObjectQueue {
public:
	ActiveObjectQueue(ActiveObject::type activeObjectQueueType = ActiveObject::type::INVALID);

	RESULT ClearActiveObjects(VirtualObj *pInteractionObject = nullptr);
	RESULT SetAllActiveObjectStates(ActiveObject::state newState, VirtualObj *pInteractionObject = nullptr);

	std::shared_ptr<ActiveObject> AddActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr);
	std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr);
	std::shared_ptr<ActiveObject> FindActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject = nullptr);
	std::vector<std::shared_ptr<ActiveObject>> FindActiveObjectsWithState(ActiveObject::state state, VirtualObj *pInteractionObject = nullptr);
	
	bool HasActiveEventObject(VirtualObj *pInteractionObject, VirtualObj *pEventObject);

	RESULT RemoveActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr);
	RESULT RemoveActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject = nullptr);

	ActiveObject::state GetActiveObjectState(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject);

	RESULT AddInteractionObjectList(VirtualObj *pInteractionObject);
	RESULT RemoveInteractionObjectList(VirtualObj *pInteractionObject);

	const std::list<std::shared_ptr<ActiveObject>>& operator[](VirtualObj *pObject);

	ActiveObject::type GetType() {
		return m_type;
	}

private:
	std::map<VirtualObj*, std::list<std::shared_ptr<ActiveObject>>> m_activeObjects;
	ActiveObject::type m_type;
};


#endif