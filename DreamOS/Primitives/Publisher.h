#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <list>
#include <map>

#include <math.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Publisher.h
// Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern

//class Subscriber;
#include "Subscriber.h"

// TODO: Bring back the PEventClass - no more subscriber predefined event struct?
// First lets see if this topology works at all

template <typename PKeyClass, typename PKEventClass>
class Publisher {
public:

	// Forward declaration for template
	struct MAP_COMPARE_FUNCTION_STRUCT {
		bool operator()(char const *a, char const *b) const {
			return std::strcmp(a, b) < 0;
		}

		bool operator()(int lhs, int rhs) const {
			return lhs < rhs;
		}
	};

	/*
	char* Publisher<PKeyClass, PEventClass>::GetEventKeyString(PKeyClass keyEvent) {
		return "FUNCNOTDEFINED";
	}
	*/
	
	char* GetEventKeyString(int keyEvent) {
		int numLength = (keyEvent == 0) ? 2 : (int)(log10(keyEvent)) + 2;

		char *pszNumString = new char[numLength];
		memset(pszNumString, 0, sizeof(char) * numLength);
		//itoa(keyEvent, pszNumString, 10);		// TODO: Note this is not POSIX compliant and will fail compilation
		//_itoa_s(keyEvent, pszNumString, sizeof(char) * numLength, 10);		// TODO: Note this is not POSIX compliant and will fail compilation
        
        // Going C99 standard
        snprintf(pszNumString, sizeof(char) * numLength, "%d", keyEvent);

		return pszNumString;
	}

	char* GetEventKeyString(char* keyEvent) {
		int strLength = strlen(keyEvent) + 1;
		char *pszString = new char[strLength];
		memset(pszString, 0, sizeof(char) * strLength);
		memcpy(pszString, keyEvent, sizeof(char) * (strLength - 1));

		return pszString;
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

			//typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.begin();
			auto it = m_events.begin();
			
			while (it != m_events.end()) {
				std::list<Subscriber<PKEventClass>*> *pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

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

protected:
	RESULT RegisterEvent(PKeyClass keyEvent) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;
		
		//typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);
		auto it = m_events.find(keyEvent);
        std::list<Subscriber<PKEventClass>*>* pNewSubscriberList = NULL;

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it == m_events.end()), "Event %s already registered", pszEvent);
		
		// Create a new subscriber list for the event entry


		// typename std::list<Subscriber<PKEventClass>*>*	pNewSubscriberList = (std::list<Subscriber<PKEventClass>*>*)(new std::list<Subscriber<PKEventClass>*>());
		pNewSubscriberList = (std::list<Subscriber<PKEventClass>*>*)(new std::list<Subscriber<PKEventClass>*>());
		m_events[keyEvent] = pNewSubscriberList;

		DEBUG_LINEOUT("%s Registered event %s", GetPublisherName(), pszEvent);

	Error:
		if (pszEvent != NULL) {
			delete[] pszEvent;
			pszEvent = NULL;
		}

		return r;
	}
	
public:
	// This requires the event to be registered 
	RESULT RegisterSubscriber(PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;
		typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it;
		std::list<Subscriber<PKEventClass>*> *pSubscriberList = NULL;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		it = m_events.find(keyEvent);

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it != m_events.end()), "Event %s not registered", pszEvent);

		// Check if already registered
		pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

		//for (std::list<Subscriber<PKEventClass>*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
		for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			Subscriber<PKEventClass>* pTempSubscriber = reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator);
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

	/* TODO:
	// Similar to the above, but will register to the ALL list so will get notified for all events
	// but will get secondary preference at the moment (TODO: Add priority)
	RESULT RegisterSubscriber(Subscriber<PKEventClass>* pSubscriber) {

	}
	*/

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		//typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);
		auto it = m_events.find(keyEvent);
		char *pszEvent = GetEventKeyString(keyEvent);
        
        typename std::list<Subscriber<PKEventClass>*> *pSubscriberList = NULL;
        
        CNM(pSubscriber, "Subscriber cannot be NULL");
		CBM((it == m_events.end()), "Event %s not registered", pszEvent);

		pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

		if (pSubscriberList != NULL) {
			//for (typename std::list<Subscriber<PKEventClass>*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
				Subscriber<PKEventClass>* pListSubscriber = reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator);

				if (pListSubscriber == pSubscriber) {
					pSubscriberList->remove(pSubscriber);
					return r;
				}
			}
		}


		CBM((0), "Subscriber not found for event %s", pszEvent);

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
	RESULT UnregisterSubscriber(Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;

		typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.begin();
        CNM(pSubscriber, "Subscriber cannot be NULL");

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

	RESULT NotifySubscribers(PKeyClass keyEvent, PKEventClass *pEvent) {
		RESULT r = R_PASS;
		char *pszEvent = NULL;
        typename std::list<Subscriber<PKEventClass>*> *pSubscriberList = NULL;
		//typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.find(keyEvent);
		auto it = m_events.find(keyEvent);

		pszEvent = GetEventKeyString(keyEvent);
		CBM((it != m_events.end()), "Event %s not registered", pszEvent);
		
		pSubscriberList = m_events[keyEvent];
		CNM(pSubscriberList, "Subscriber list is NULL");

		if (pSubscriberList->size() > 0) {
			//for (typename std::list<Subscriber<PKEventClass>*>::iterator eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
				WCR(reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator)->Notify(pEvent));
			}
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

	// Maintain a list of all subscribers and the events they're subscribed to
	//TODO: std::map<Subscriber<PKEventClass>*, std::list<Subscriber<PKEventClass>*> m_subscribers;

	// Events 
	std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT> m_events;

	// TODO: Add an subscribe all function
	// These subscribers get notified of ALL events
	//std::list<Subscriber<PKEventClass>*> m_pSubscribeAllList;
};

#endif // ! PUBLISHER_H_