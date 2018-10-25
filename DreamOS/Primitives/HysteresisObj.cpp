#include "HysteresisObj.h"

HysteresisEvent::HysteresisEvent(HysteresisEventType eventType, VirtualObj *eventObject) :
	m_eventType(eventType),
	m_pEventObject(eventObject)
{
	// empty
}

HysteresisObj::HysteresisObj() {
	// empty
}

HysteresisObj::~HysteresisObj() {
	// empty
}


