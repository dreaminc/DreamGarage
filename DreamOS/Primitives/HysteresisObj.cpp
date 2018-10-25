#include "HysteresisObj.h"
#include "sphere.h"

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

RESULT HysteresisObj::Update(UIMallet *pMallet) {
	RESULT r = R_PASS;

	HysteresisEventType currentState = GetState(pMallet);

	if (currentState != m_currentState) {
		m_currentState = currentState;

		CR(NotifySubscribers(m_currentState, new HysteresisEvent(m_currentState, pMallet->GetMalletHead())));
	}

Error:
	return r;
}

HysteresisEventType HysteresisObj::GetState(UIMallet *pMallet) {

	// TODO: only an example, far from actual hysteresis
	bool fResolved = Resolve(pMallet);
	HysteresisEventType eventType;
	if (fResolved) {
		eventType = HysteresisEventType::ON;
	}
	else {
		eventType = HysteresisEventType::OFF;
	}

	return eventType;	
}

