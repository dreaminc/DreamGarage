#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Subscriber.h
// Observer Object
// Couples with the Subject object to constitute the observer-notify pattern

//class Publisher;

class Subscriber {
public:
	Subscriber() {
		// empty
	}

	/*
	Subscriber(Publisher *pSubject) {
		m_pSubject = pSubject;
	}
	*/

	~Subscriber() {
		// empty
	}

	struct SubscriberEvent {
		uint32_t SenseEventSize;
	};

	virtual RESULT Notify(void *SubscriberEvent) = 0;

private:
	//Publisher *m_pSubject;

};

#endif // ! SUBSCRIBER_H_