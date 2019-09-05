#include "UIEvent.h"

#include "core/primitives/VirtualObj.h"

UIEvent::UIEvent(UIEventType eventType, VirtualObj *pObj, VirtualObj *pInteractionObject, point ptEvent, vector vDelta) :
	m_eventType(eventType),
	m_pObj(pObj),
	m_pInteractionObject(pInteractionObject),
	m_ptEvent(ptEvent),
	m_vDelta(vDelta)
{

}
