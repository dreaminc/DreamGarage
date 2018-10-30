#include "HysteresisObj.h"
#include "sphere.h"

HysteresisEvent::HysteresisEvent(HysteresisEventType eventType, VirtualObj *eventObject) :
	m_eventType(eventType),
	m_pEventObject(eventObject)
{
	// empty
}

HysteresisObj::HysteresisObj(float onThreshold, float offThreshold) :
	m_onThreshold(onThreshold),
	m_offThreshold(offThreshold) 
{
	Initialize();
}

HysteresisObj::HysteresisObj() {
	Initialize();
}

HysteresisObj::~HysteresisObj() {
	// empty
}

RESULT HysteresisObj::Initialize() {
	RESULT r = R_PASS;

	CR(RegisterEvent(HysteresisEventType::OFF));
	CR(RegisterEvent(HysteresisEventType::ON));

Error:
	return r;
}

RESULT HysteresisObj::Update(VirtualObj *pObj) {
	RESULT r = R_PASS;

	HysteresisEventType currentState = GetState(pObj);

	if (currentState != m_currentState) {
		m_currentState = currentState;

		CR(NotifySubscribers(m_currentState, new HysteresisEvent(m_currentState, pObj)));
	}

Error:
	return r;
}

HysteresisEventType HysteresisObj::GetState(VirtualObj *pObj) {

	// TODO: only an example, far from actual hysteresis
	bool fResolved = Resolve(pObj);
	HysteresisEventType eventType;
	if (fResolved) {
		eventType = HysteresisEventType::ON;
	}
	else {
		eventType = HysteresisEventType::OFF;
	}

	return eventType;	
}

