#ifndef HYSTERESIS_OBJECT_H_
#define HYSTERESIS_OBJECT_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/hysteresis/HysteresisObject.h

// Hysteresis Base Object

#include "core/dimension/DimObj.h"
#include "core/types/Publisher.h"

enum HysteresisEventType {
	ON,
	OFF
};

enum HysteresisObjectType {
	CYLINDER,
	SPHERE,
	PLANE
};

struct HysteresisEvent {
	HysteresisEventType m_eventType;
	VirtualObj *m_pEventObject;

	HysteresisEvent(HysteresisEventType eventType, VirtualObj *eventObject);
};

class HysteresisObject : public VirtualObj, public Publisher<HysteresisEventType, HysteresisEvent> {

public:
	HysteresisObject(float onThreshold, float offThreshold);
	HysteresisObject();

	virtual ~HysteresisObject();

public:

	RESULT Initialize();
	RESULT Update();

	RESULT RegisterObject(VirtualObj *pObj);

	HysteresisEventType UpdateState(VirtualObj *pObj);
	HysteresisEventType GetState(VirtualObj *pObj);

	virtual bool Resolve(VirtualObj *pObj) = 0;


protected:
	float m_offThreshold = 0.25f;
	float m_onThreshold = 0.5f;

	std::map<VirtualObj*, HysteresisEventType> m_currentStates;
};

#endif // ! HYSTERESIS_OBJECT_H_
