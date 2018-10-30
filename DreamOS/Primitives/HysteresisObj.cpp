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
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;

Error:
	Invalidate();
	return;
}

HysteresisObj::HysteresisObj() {
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;

Error:
	Invalidate();
	return;
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

	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] != currentState) {
		m_currentStates[pObj] = currentState;

		CR(NotifySubscribers(m_currentStates[pObj], new HysteresisEvent(m_currentStates[pObj], pObj)));
	}

Error:
	return r;
}

HysteresisEventType HysteresisObj::GetState(VirtualObj *pObj) {

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

