#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <list>
#include <map>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Publisher.h
// Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern

//class Subscriber;
#include "Subscriber.h"

// TODO: Bring back the PEventClass - no more subscriber predefined event struct?
// First lets see if this topology works at all

template <class PKeyClass>
class Publisher {
public:

	// Forward declaration for template
	struct MAP_COMPARE_FUNCTION_STRUCT {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}

		bool operator()(int lhs, int rhs) {
			return lhs < rhs;
		}
	};

	/*
	struct MAP_COMPARE_FUNCTION_STRUCT {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	};
	//*/

	// TODO: Specialize
	char *GetEventKeyString(PKeyClass keyEvent) {
		return "FUNCNOTDEFINED";
	}

	Publisher() {
		//m_pSubsribers = new std::list<Subscriber*>();
		//m_pEvents = new std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>();
		// empty
	}

	// This will not deallocate the subscribers, the subscribing object is responsible to do that
	// and to unregister themselves
	~Publisher() {
		//if (m_pEvents != NULL) {
			std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.begin();
			
			while (it != m_events.end()) {
				std::list<Subscriber*> *pSubscriberList = reinterpret_cast<std::list<Subscriber*>*>(it->second);

				if (pSubscriberList != NULL) {
					delete pSubscriberList;
					pSubscriberList = NULL;
				}
				it++;
			}

			//delete m_pEvents;
			//m_pEvents = NULL;
		//}
	}

	char *GetPublisherName() {
		return "Publisher Base Class";
	}

	RESULT RegisterEvent(PKeyClass keyEvent) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it == m_events.end()), "Event %s already registered", pszEvent);
		
		// Create a new subscriber list for the event entry

		std::list<Subscriber*>*	pNewSubscriberList = (std::list<Subscriber*>*)(new std::list<Subscriber*>());
		m_events[keyEvent] = pNewSubscriberList;

		DEBUG_LINEOUT("%s Registered event %s", GetPublisherName(), pszEvent);

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}
	
	// This requires the event to be registered 
	RESULT RegisterSubscriber(PKeyClass keyEvent, Subscriber* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		CNM(pSubscriber, "Subscriber cannot be NULL");

		std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it == m_events.end()), "Event %s not registered", pszEvent);

		// Check if already registered
		std::list<Subscriber*> *pSubscriberList = reinterpret_cast<Subscriber*>(it->second);
		for (std::list<Subscriber*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			Subscriber *pTempSubscriber = reinterpret_cast<Subscriber*>(*eventIterator);
			CBM((pTempSubscriber != pSubscriber), "Already subscribed to %s", pszEvent);
		}

		// If we made it this far then push the subscriber into the list for the given event
		m_events[keyEvent]->push_back(pSubscriber);

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PKeyClass keyEvent, Subscriber* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		CNM(pSubscriber, "Subscriber cannot be NULL");

		std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);
		
		pszEvent = GetEventKeyString(keyEvent);
		CBM((it == m_events.end()), "Event %s not registered", pszEvent);

		std::list<Subscriber*> *pSubscriberList = reinterpret_cast<Subscriber*>(it->second);

		if (pSubscriberList != NULL) {
			for (std::list<Subscriber*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
				Subscriber* pListSubscriber = reinterpret_cast<Subscriber*>(*eventIterator);

				if (pListSubscriber == pSubscriber) {
					pSubscriberList->remove(pSubscriber);
					return r;
				}
			}
		}

		CBRM((0), "Subscriber not found for event %s", pszEvent);

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}

	// This will unsubscribe a subscriber from all events
	// Error handling warranted by the fact that something is really wrong if
	// things get out of line
	RESULT UnregisterSubscriber(Subscriber* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		CNM(pSubscriber, "Subscriber cannot be NULL");

		std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.begin();

		while (it != m_events.end()) {
			PKeyClass keyEvent = reinterpret_cast<PKeyClass>(it->first);

			pszEvent = GetEventKeyString(keyEvent);
			CRM(UnregisterSubscriber(keyEvent, pSubscriber), "Failed to unsubscribe for event %s", pszEvent);
			
			it++;
		}

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}

	RESULT NotifySubscribers(PKeyClass keyEvent, Subscriber::SubscriberEvent *pEvent) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it == m_events.end()), "Event %s not registered", pszEvent);

		std::list<Subscriber*> *pSubscriberList = m_events[keyEvent];
		CNM(pSubscriberList, "Subscriber list is NULL");

		for (std::list<Subscriber*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			WCR(reinterpret_cast<Subscriber*>(*eventIterator)->Notify(pContext));
		}

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}

private:
	//std::list<Subscriber*> *m_pSubsribers; 
	std::map<PKeyClass, std::list<Subscriber*>*, MAP_COMPARE_FUNCTION_STRUCT> m_events;
};


// Explicit specialization for char *
template<>
char* Publisher<char *>::GetEventKeyString(char *keyEvent) {
	int strLength = strlen(keyEvent) + 1;
	char *pszString = new char[strLength];
	memset(pszString, 0, sizeof(char) * strLength);
	memcpy(pszString, keyEvent, sizeof(char) * (strLength - 1));
	
	return pszString;
}

/*
template<>
struct Publisher<char *>::MAP_COMPARE_FUNCTION_STRUCT {
	bool operator()(char const *a, char const *b) {
		return std::strcmp(a, b) < 0;
	}
};
*/

// Explicit specialization for int
template <>
char *Publisher<int>::GetEventKeyString(int keyEvent) {
	int numLength = (keyEvent == 0) ? 2 : (int)(log10(keyEvent)) + 2;

	char *pszNumString = new char[numLength];
	memset(pszNumString, 0, sizeof(char) * numLength);
	//itoa(keyEvent, pszNumString, 10);		// TODO: Note this is not POSIX compliant and will fail compilation
	_itoa_s(keyEvent, pszNumString, sizeof(char) * numLength, 10);		// TODO: Note this is not POSIX compliant and will fail compilation

	return pszNumString;
}

/*
template<>
struct Publisher<int>::MAP_COMPARE_FUNCTION_STRUCT {
	bool operator()(int lhs, int rhs) {
		return lhs < rhs;
	}
};
*/

#endif // ! SUBSCRIBER_H_