#ifndef HYSTERESIS_OBJ_H_
#define HYSTERESIS_OBJ_H_

// DREAM OS
// Hysteresis Base Object

#include "DimObj.h"
#include "Publisher.h"
#include "UI/UIMallet.h"

enum HysteresisEventType {
	ON,
	OFF
};

struct HysteresisEvent {
	HysteresisEventType m_eventType;
	VirtualObj *m_pEventObject;

	HysteresisEvent(HysteresisEventType eventType, VirtualObj *eventObject);
};

class HysteresisObj : public VirtualObj, public Publisher<HysteresisEventType, HysteresisEvent> {

public:
	HysteresisObj(float onThreshold, float offThreshold);
	HysteresisObj();
	~HysteresisObj();

public:

	RESULT Initialize();
	RESULT Update(VirtualObj *pObj);

	HysteresisEventType GetState(VirtualObj *pObj);

	virtual bool Resolve(VirtualObj *pObj) = 0;


protected:
	float m_offThreshold;
	float m_onThreshold;

	HysteresisEventType m_currentState = OFF;
};

#endif // ! HYSTERESIS_OBJ_H_
