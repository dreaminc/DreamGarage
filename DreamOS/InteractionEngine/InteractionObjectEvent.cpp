#include "InteractionObjectEvent.h"

#include "PhysicsEngine/ContactPoint.h"

InteractionObjectEvent::InteractionObjectEvent(InteractionEventType eventType, std::shared_ptr<ray> pInteractionRay, VirtualObj *pObject) :
	m_eventType(eventType),
	m_pInteractionRay(pInteractionRay),
	m_pObject(pObject),
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