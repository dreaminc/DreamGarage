#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Subscriber.h
// Observer Object
// Couples with the Subject object to constitute the observer-notify pattern

class Publisher;

template <class SClass>
class Subscriber {
public:
	Subscriber() {
		// empty
	}

	Subscriber(Publisher *pSubject) {
		m_pSubject = pSubject;
	}

	~Subscriber() {
		// empty
	}

	virtual RESULT Notify(SClass pContext) = 0;

private:
	Publisher *m_pSubject;

};

#endif // ! SUBSCRIBER_H_