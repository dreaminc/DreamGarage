#ifndef SENSE_DEVICE_H_
#define SENSE_DEVICE_H_

#include "core/ehm/EHM.h"

// Dream Sense
// dos/src/sense/SenseDevice.h

// Base Sense Device Object

// Sense is a dedicated DreamOS sub-system design to asynchronously receive input or
// otherwise external stimuli from the user's environment.  This may include simple input peripherals 
// such as computer keyboards or mice, but should extend to all types of sensory input such as spatial 
// controls or otherwise.  The base SenseDevice will get filled out with shared functionality and interfaces
// to such devices and act as a shared handle for managers or controllers as they become needed.

#include <time.h>       

#include "core/types/DObject.h"

#include "core/types/Subscriber.h"

class SenseDevice : public DObject {
public:
	SenseDevice() {
		// empty stub
	}

	~SenseDevice() {
		// empty stub
	}

	typedef struct SenseDeviceEvent : public SubscriberEvent {
		clock_t SenseEventTickCount;

		SenseDeviceEvent() {
			SenseEventTickCount = clock();
		}

	} SENSE_DEVICE_EVENT;

};

#endif // ! SENSE_DEVICE_H_