#include "HysteresisObject.h"
#include "sphere.h"

HysteresisEvent::HysteresisEvent(HysteresisEventType eventType, VirtualObj *eventObject) :
	m_eventType(eventType),
	m_pEventObject(eventObject)
{
	// empty
}

HysteresisObject::HysteresisObject(float onThreshold, float offThreshold) :
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

HysteresisObject::HysteresisObject() {
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;

Error:
	Invalidate();
	return;
}

HysteresisObject::~HysteresisObject() {
	// empty
}

RESULT HysteresisObject::Initialize() {
	RESULT r = R_PASS;

	CR(RegisterEvent(HysteresisEventType::OFF));
	CR(RegisterEvent(HysteresisEventType::ON));

Error:
	return r;
}

RESULT HysteresisObject::Update() {
	RESULT r = R_PASS;

	for (auto pObjIt = m_currentStates.begin(); pObjIt != m_currentStates.end(); ++pObjIt) {

		auto pObj = pObjIt->first;
		HysteresisEventType currentState = GetState(pObj);

		if (m_currentStates[pObj] != currentState) {
			m_currentStates[pObj] = currentState;

			CR(NotifySubscribers(m_currentStates[pObj], new HysteresisEvent(m_currentStates[pObj], pObj)));
		}
	}

Error:
	return r;
}

RESULT HysteresisObject::RegisterObject(VirtualObj *pObj) {
	RESULT r = R_PASS;

	m_currentStates[pObj] = OFF;

Error:
	return r;
}

HysteresisEventType HysteresisObject::GetState(VirtualObj *pObj) {

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

