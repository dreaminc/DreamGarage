#ifndef MULTIPUBLISHER_H_
#define MULTIPUBLISHER_H_

#include <list>
#include <map>
#include <string>

#include <math.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Multipublisher.h
// Multi Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern
// for a multiple objects - objects indexed against the PIndexClass

#include "Publisher.h"
#include "Subscriber.h"
#include "Primitives/VirtualObj.h"

template <typename PIndexClass, typename PKeyClass, typename PKEventClass>
class Multipublisher : public I_Publisher<PKeyClass, PKEventClass> {
public:
	Multipublisher() {
		// empty
	}

	~Multipublisher() {
		auto it = m_indexedEvents.begin();

		while (it != m_indexedEvents.end()) {
			// TODO

			it++;
		}
	}

protected:
	virtual RESULT RegisterEvent(PKeyClass keyEvent) override {
		RESULT r = R_PASS;
		
		auto it = m_events.find(keyEvent);
		std::map<PIndexClass, Subscriber<PKEventClass>*>* pNewSubscriberMap = nullptr;

		CBM((it == m_events.end()), "Event %s already registered", GetEventKeyString(keyEvent));

		// Create a new subscriber list for the event entry
		pNewSubscriberMap = (std::map<PIndexClass, Subscriber<PKEventClass>*>*)(new std::map<PIndexClass, Subscriber<PKEventClass>*>());
		m_indexedEvents[keyEvent] = pNewSubscriberMap;

		//DEBUG_LINEOUT("%s Registered event %s", GetPublisherName(), pszEvent);

	Error:
		return r;
	}

public:
	virtual bool IsEventRegistered(PKeyClass keyEvent) override {
		auto it = m_indexedEvents.find(keyEvent);
		return (!(it == m_indexedEvents.end()));
	}

	std::map<PKeyClass, std::map<PIndexClass, Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT> GetEvents() {
		return m_indexedEvents;
	}

public:
	// This requires the event to be registered 
	RESULT RegisterSubscriber(PIndexClass indexEvent, PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = nullptr;
		typename std::map<PKeyClass, std::map<PIndexClass, Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it;
		std::map<PIndexClass, Subscriber<PKEventClass>*> *pSubscriberMap = nullptr;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		it = m_indexedEvents.find(keyEvent);
		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		// Check if already registered
		pSubscriberMap = reinterpret_cast<std::map<PIndexClass, Subscriber<PKEventClass>*>*>(it->second);
		auto indexedIt = pSubscriberMap->find(indexEvent);

		CBM((indexedIt == pSubscriberMap->end(), "Object already subscribed to event %s", GetEventKeyString(keyEvent));

		// If not found - add it here
		pSubscriberMap[indexEvent] = pSubscriber;

		// TODO: Don't currently support multiple subscribers to the same object 
		// If we want this - there needs to be a list of subscribers 

	Error:
		return r;
	}

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PIndexClass indexEvent, PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		auto it = m_events.find(keyEvent);

		typename std::map<PIndexClass, Subscriber<PKEventClass>*> *pSubscriberMap = nullptr;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		CBM((it == m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = reinterpret_cast<std::map<PIndexClass, Subscriber<PKEventClass>*>*>(it->second);

		if (pSubscriberMap != nullptr) {
			auto indexedIt = pSubscriberMap->find(indexEvent);

			if (indexedIt != pSubscriberMap->end()) {
				pSubscriberMap->erase(indexedIt);
				return r;
			}

			// TODO: Don't currently support multiple subscribers to the same object 
			// If we want this - there needs to be a list of subscribers 

			CBM((0), "Index not found for event %s", GetEventKeyString(keyEvent));
		}

		CBM((0), "Subscriber not found for event %s", GetEventKeyString(keyEvent));

	Error:
		return r;
	}

	// This will unsubscribe a subscriber from all events
	// Error handling warranted by the fact that something is really wrong if
	// things get out of line
	RESULT UnregisterSubscriber(Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		auto it = m_indexedEvents.begin();
		CNM(pSubscriber, "Subscriber cannot be NULL");

		while (it != m_indexedEvents.end()) {
			pSubscriberMap = reinterpret_cast<std::map<PIndexClass, Subscriber<PKEventClass>*>*>(it->second);
			auto indexIt = pSubscriberMap->begin();

			PKeyClass keyEvent = reinterpret_cast<PKeyClass>(it->first);

			while (indexIt != pSubscriberMap->end()) {
				PIndexClass indexEvent = reinterpret_cast<PKeyClass>(indexIt->first);

				CRM(UnregisterSubscriber(indexEvent, keyEvent, pSubscriber), "Failed to unsubscribe for event %s", GetEventKeyString(keyEvent));

				indexIt++;
			}

			it++;
		}

	Error:
		return r;
	}

	virtual RESULT NotifySubscribers(PKeyClass keyEvent, PKEventClass *pEvent) override {
		RESULT r = R_PASS;

		typename std::map<PIndexClass, Subscriber<PKEventClass>*> *pSubscriberMap = nullptr;
		auto it = m_events.find(keyEvent);

		CBM((it != m_events.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber map is NULL");

		if (pSubscriberMap->size() > 0) {
			for (auto &indexSubItem : pSubscriberMap) {
				WCR(reinterpret_cast<Subscriber<PKEventClass>*>(indexSubItem->second)->Notify(pEvent));
			}
		}

	Error:
		return r;
	}

	virtual bool EventHasSubscribers(PKeyClass keyEvent) override {
		RESULT r = R_PASS;

		typename std::map<PIndexClass, Subscriber<PKEventClass>*> *pSubscriberMap = nullptr;
		auto it = m_events.find(keyEvent);

		CBM((it != m_events.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber list is NULL");

		if (pSubscriberMap->size() > 0) {
			return true;
		}

	Error:
		return false;
	}

public:
	std::map<PKeyClass, std::map<PIndexClass, Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT> m_indexedEvents;
};

#endif // ! MULTIPUBLISHER_H_