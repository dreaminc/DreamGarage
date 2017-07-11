#include "UIEvent.h"
#include "Primitives/VirtualObj.h"

UIEvent::UIEvent(UIEventType eventType, VirtualObj *pObj, VirtualObj *pInteractionObject) :
	m_eventType(eventType),
	m_pObj(pObj),
	m_pInteractionObject(pInteractionObject)
{

}
