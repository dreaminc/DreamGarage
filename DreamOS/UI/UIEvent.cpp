#include "UIEvent.h"
#include "Primitives/VirtualObj.h"

UIEvent::UIEvent(UIEventType eventType, VirtualObj *pObj, VirtualObj *pInteractionObject, point ptContact, point ptScroll) :
	m_eventType(eventType),
	m_pObj(pObj),
	m_pInteractionObject(pInteractionObject),
	m_ptContact(ptContact),
	m_ptScroll(ptScroll)
{

}
