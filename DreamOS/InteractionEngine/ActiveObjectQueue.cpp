#include "ActiveObjectQueue.h"

ActiveObjectQueue::ActiveObjectQueue(ActiveObject::type activeObjectQueueType) :
	m_type(activeObjectQueueType)
{
	// empty
}

RESULT ActiveObjectQueue::SetAllActiveObjectStates(ActiveObject::state newState, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			for (auto &pActiveObject : activeObjects.second) {
				CR(pActiveObject->SetState(newState));
			}
		}
	}
	else {
		for (auto &pActiveObject : m_activeObjects[pInteractionObject]) {
			CR(pActiveObject->SetState(newState));
		}
	}

Error:
	return r;
}

// Active Objects
RESULT ActiveObjectQueue::ClearActiveObjects(VirtualObj *pInteractionObject) {
	if (pInteractionObject == nullptr) {
		m_activeObjects.clear();
	}
	else {
		m_activeObjects[pInteractionObject].clear();
	}

	return R_PASS;
}

std::shared_ptr<ActiveObject> ActiveObjectQueue::AddActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = nullptr;

	CBM((FindActiveObject(pVirtualObject, pInteractionObject) == nullptr), "Active Object already active");

	pNewActiveObject = std::make_shared<ActiveObject>(pVirtualObject);
	CN(pNewActiveObject);
	pNewActiveObject->SetState(ActiveObject::state::NOT_INTERSECTED);

	//m_activeObjects.push_back(pNewActiveObject);

	// If interaction object is nullptr active object will be added to all 
	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			activeObjects.second.push_back(pNewActiveObject);
		}
	}
	else {
		m_activeObjects[pInteractionObject].push_back(pNewActiveObject);
	}

	// Success:
	return pNewActiveObject;

Error:
	return nullptr;
}

std::shared_ptr<ActiveObject> ActiveObjectQueue::FindActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject) {
	// If interaction object is null - return all (this is only really useful for checking all 
	// lanes to avoid duplication)
	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			for (auto &pActiveObject : activeObjects.second) {
				if (pActiveObject->GetObject() == pVirtualObject) {
					return pActiveObject;
				}
			}
		}
	}
	else {
		for (auto &pActiveObject : m_activeObjects[pInteractionObject]) {
			if (pActiveObject->GetObject() == pVirtualObject) {
				return pActiveObject;
			}
		}
	}

	return nullptr;
}

std::shared_ptr<ActiveObject> ActiveObjectQueue::FindActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject) {
	// If interaction object is null - return all (this is only really useful for checking all 
	// lanes to avoid duplication)
	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			for (auto &pActiveObject : activeObjects.second) {
				if (pActiveObject == pActiveObject) {
					return pActiveObject;
				}
			}
		}
	}
	else {
		for (auto &pActiveObject : m_activeObjects[pInteractionObject]) {
			if (pActiveObject == pActiveObject) {
				return pActiveObject;
			}
		}
	}

	return nullptr;
}

RESULT ActiveObjectQueue::RemoveActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pActiveObject, pInteractionObject);
	CBM((pNewActiveObject != nullptr), "Active object not found");

	//m_activeObjects.remove(pNewActiveObject);

	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			if (std::find(activeObjects.second.begin(), activeObjects.second.end(), pNewActiveObject) != activeObjects.second.end()) {
				activeObjects.second.remove(pNewActiveObject);
				break;
			}
		}
	}
	else {
		if (std::find(m_activeObjects[pInteractionObject].begin(), m_activeObjects[pInteractionObject].end(), pNewActiveObject) != m_activeObjects[pInteractionObject].end()) {
			m_activeObjects[pInteractionObject].remove(pNewActiveObject);
		}
	}

Error:
	return r;
}

RESULT ActiveObjectQueue::RemoveActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pVirtualObject, pInteractionObject);
	CNM(pNewActiveObject, "Active object not found");

	CR(RemoveActiveObject(pNewActiveObject, pInteractionObject));

Error:
	return r;
}

ActiveObject::state ActiveObjectQueue::GetActiveObjectState(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pVirtualObject, pInteractionObject);
	CBM((pNewActiveObject != nullptr), "Active object not found");

	return pNewActiveObject->GetState();

Error:
	return ActiveObject::state::INVALID;
}

std::vector<std::shared_ptr<ActiveObject>> ActiveObjectQueue::FindActiveObjectsWithState(ActiveObject::state state, VirtualObj *pInteractionObject) {
	std::vector<std::shared_ptr<ActiveObject>> retVector;

	if (pInteractionObject == nullptr) {
		for (auto &activeObjects : m_activeObjects) {
			for (auto &pActiveObject : activeObjects.second) {
				if (pActiveObject->HasState(state)) {
					retVector.push_back(pActiveObject);
				}
			}
		}
	}
	else {
		for (auto &pActiveObject : m_activeObjects[pInteractionObject]) {
			if (pActiveObject->HasState(state)) {
				retVector.push_back(pActiveObject);
			}
		}
	}

	return retVector;
}

RESULT ActiveObjectQueue::AddInteractionObjectList(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	CN(pInteractionObject);
	CB((m_activeObjects.count(pInteractionObject) == 0));

	m_activeObjects[pInteractionObject] = std::list<std::shared_ptr<ActiveObject>>();

Error:
	return r;
}

RESULT ActiveObjectQueue::RemoveInteractionObjectList(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	CN(pInteractionObject);
	CB((m_activeObjects.count(pInteractionObject) != 0));

	m_activeObjects.erase(pInteractionObject);

Error:
	return r;
}

const std::list<std::shared_ptr<ActiveObject>>& ActiveObjectQueue::operator[](VirtualObj *pObject) {
	return m_activeObjects[pObject];
}