#ifndef INTERACTION_OBJECT_EVENT_H_
#define INTERACTION_OBJECT_EVENT_H_

#include "RESULT/EHM.h"

// Dream Interaction Object Event

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/ray.h"
#include <memory>

#include "ActiveObject.h"

class VirtualObj;
class ContactPoint;

typedef enum InteractionEventType {
	ELEMENT_INTERSECT_BEGAN,
	ELEMENT_INTERSECT_MOVED,
	ELEMENT_INTERSECT_ENDED,
	ELEMENT_INTERSECT_CANCELLED,
	INTERACTION_EVENT_SELECT,
	INTERACTION_EVENT_MENU,
	INTERACTION_EVENT_SELECT_DOWN,
	INTERACTION_EVENT_SELECT_UP,
	INTERACTION_EVENT_WHEEL,		// TODO: Should we change this to pad and pass a point?
	INTERACTION_EVENT_KEY_DOWN,
	INTERACTION_EVENT_KEY_UP,
	INTERACTION_EVENT_INVALID
} INTERACTION_EVENT_TYPE;

// Object Interaction Event
typedef struct InteractionObjectEvent {

	InteractionEventType m_eventType;
	ray m_interactionRay;
	VirtualObj *m_pInteractionObject = nullptr;
	VirtualObj *m_pObject = nullptr;
	point m_ptContact[4];
	vector m_vNormal[4];
	int m_numContacts;
	ActiveObject::state m_activeState;
	
	int m_value;

	// TODO: Add time of collision
	InteractionObjectEvent();
	InteractionObjectEvent(InteractionEventType eventType, VirtualObj *pObject = nullptr, VirtualObj *pInteractionObject = nullptr);
	InteractionObjectEvent(InteractionEventType eventType, const ray &interactionRay, VirtualObj *pObject = nullptr, VirtualObj *pInteractionObject = nullptr);

public:
	RESULT AddPoint(point ptContact, vector vNormal);
	RESULT AddPoint(ContactPoint contactPoint);

	RESULT SetValue(int val);

} INTERACTION_OBJECT_EVENT;

#endif // ! INTERACTION_OBJECT_EVENT_H_