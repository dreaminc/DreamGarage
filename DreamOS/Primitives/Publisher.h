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

template <class PClass>
class Publisher {
public:
	Publisher() {
		//m_pSubsribers = new std::list<Subscriber*>();
		m_pEvents = new std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT>();
	}

	// This will not deallocate the subscribers, the subscribing object is responsible to do that
	~Publisher() {
		if (m_pSubsribers != NULL) {

		}
	}

	RESULT RegisterEvent(char *pszEvent) {
		RESULT r = R_PASS;

		std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_pEvents->find(pszEvent);
		CBM((it != m_pEvents->end()), "Event %s already registered", pszEvent);
		
		// Create a new subscriber list for the event entry
		m_pEvents[pszEvent] = new std::list<Subscriber*>();

	Error:
		return r;
	}
	
	// This requires the event to be registered 
	RESULT RegisterSubscriber(char *pszEvent, Subscriber* pSubscriber) {
		RESULT r = R_PASS;

		CNM(pSubscriber, "Subscriber cannot be NULL");

		std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_pEvents->find(pszEvent);
		CBM((it == m_pEvents->end()), "Event %s not registered", pszEvent);

		m_pEvents[pszEvent]->push_back(pSubscriber);

	Error:
		return r;
	}

	//RESULT NotifySubscribers(char *pszEvent, void *pContext) {
	RESULT NotifySubscribers(char *pszEvent, PClass pContext) {
		RESULT r = R_PASS;

		std::map<char *, int, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_pEvents->find(pszEvent);
		CBM((it == m_pEvents->end()), "Event %s not registered", pszEvent);

		std::list<Subscriber*> *pSubscriberList = m_pEvents[pszEvent];
		CNM(pSubscriberList, "Subscriber list is NULL");

		for (std::list<Subscriber*>::iterator it = pSubscriberList->begin(); i != pSubscriberList->end(); i++) {
			WCR(reinterpret_cast<Subscriber*>(*it)->Notify(pContext));
		}

	Error:
		return r;
	}

	typedef struct {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	} MAP_COMPARE_FUNCTION_STRUCT;

private:
	//std::list<Subscriber*> *m_pSubsribers; 
	std::map<char *, (std::list<Subscriber*>*), MAP_COMPARE_FUNCTION_STRUCT> *m_pEvents;
};

#endif // ! SUBSCRIBER_H_