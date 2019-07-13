#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Subscriber.h
// Observer Object
// Couples with the Subject object to constitute the observer-notify pattern

//class Publisher;

typedef struct SubscriberEvent {
	uint32_t SenseEventSize;
} SUBSCRIBER_EVENT;

template<typename SEventClass>
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

	virtual RESULT Notify(SEventClass *pSubscriberEvent) = 0;

	RESULT SetRemove() {
		m_fRemove = true;
	}

	bool CheckAndCleanToBeRemoved() {
		bool fRemove = m_fRemove;
		m_fRemove = false;
		return fRemove;
	}

private:
	//Publisher *m_pSubject;
	bool m_fRemove = false;

};

#endif // ! SUBSCRIBER_H_