#include "InteractionObjectEvent.h"

#include "PhysicsEngine/ContactPoint.h"

InteractionObjectEvent::InteractionObjectEvent() :
	m_eventType(InteractionEventType::INTERACTION_EVENT_INVALID),
	m_interactionRay(),
	m_pObject(nullptr),
	m_pInteractionObject(nullptr),
	m_numContacts(0)
{
	memset(m_ptContact, 0, sizeof(point) * 4);
	memset(m_vNormal, 0, sizeof(vector) * 4);
}

//InteractionObjectEvent::InteractionObjectEvent(InteractionEventType eventType, std::shared_ptr<ray> pInteractionRay, VirtualObj *pObject) :
InteractionObjectEvent::InteractionObjectEvent(InteractionEventType eventType, 
											   const ray& interactionRay, 
											   VirtualObj *pObject, 
											   VirtualObj *pInteractionObject) :
	m_eventType(eventType),
	m_interactionRay(interactionRay),
	m_pObject(pObject),
	m_pInteractionObject(pInteractionObject),
	m_numContacts(0)
{
	memset(m_ptContact, 0, sizeof(point) * 4);
	memset(m_vNormal, 0, sizeof(vector) * 4);
}

InteractionObjectEvent::InteractionObjectEvent(InteractionEventType eventType, 
											   VirtualObj *pObject, 
											   VirtualObj *pInteractionObject) :
	m_eventType(eventType),
	m_interactionRay(),
	m_pObject(pObject),
	m_pInteractionObject(pInteractionObject),
	m_numContacts(0)
{
	memset(m_ptContact, 0, sizeof(point) * 4);
	memset(m_vNormal, 0, sizeof(vector) * 4);
}

RESULT InteractionObjectEvent::AddPoint(point ptContact, vector vNormal) {
	RESULT r = R_PASS;

	CB((m_numContacts < 4));

	m_ptContact[m_numContacts] = ptContact;
	m_vNormal[m_numContacts] = vNormal;

	m_numContacts++;

Error:
	return r;
}

RESULT InteractionObjectEvent::AddPoint(ContactPoint contactPoint) {
	RESULT r = R_PASS;

	CB((m_numContacts < 4));

	m_ptContact[m_numContacts] = contactPoint.GetPoint();
	m_vNormal[m_numContacts] = contactPoint.GetNormal();

	m_numContacts++;

Error:
	return r;
}

RESULT InteractionObjectEvent::SetValue(int val) {
	m_value = val;
	return R_PASS;
}

RESULT InteractionObjectEvent::SetControllerState(ControllerState state) {
	m_state = state;
	return R_PASS;
}