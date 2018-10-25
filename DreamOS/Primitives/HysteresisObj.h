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
	HysteresisObj();
	~HysteresisObj();

public:
	virtual RESULT Update(UIMallet *pMallet) = 0;
};

#endif // ! HYSTERESIS_OBJ_H_
