#include "UIEvent.h"
#include "Primitives/VirtualObj.h"

UIEvent::UIEvent(UIEventType eventType, VirtualObj *pObj) :
	m_eventType(eventType),
	m_pObj(pObj)
{

}
