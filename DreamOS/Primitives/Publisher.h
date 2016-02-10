#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <list>
#include <map>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Publisher.h
// Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern

class Subscriber;

class Publisher {
public:
	Publisher() {
		m_pSubsribers = new std::list<Subscriber*>();
		m_pEvents = new std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT>();
	}

	// This will not deallocate the subscribers, the subscribing object is responsible to do that
	~Publisher() {
		if (m_pSubsribers != NULL) {

		}
	}
	
	RESULT NotifySubscribers() {
		RESULT r = R_PASS;

	Error:
		return r;
	}

	typedef struct {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	} MAP_COMPARE_FUNCTION_STRUCT;

private:
	std::list<Subscriber*> *m_pSubsribers; 
	std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT> *m_pEvents;
};

#endif // ! SUBSCRIBER_H_