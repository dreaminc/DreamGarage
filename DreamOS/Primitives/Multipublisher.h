#ifndef MULTIPUBLISHER_H_
#define MULTIPUBLISHER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Multipublisher.h
// Multi Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern
// for a multiple objects - objects indexed against the PIndexClass

#include "Publisher.h"
#include "Subscriber.h"
#include "Primitives/dirty.h"

#include <string>
#include <map>

#include <math.h>

template <typename PIndexClass, typename PKeyClass, typename PKEventClass>
class Multipublisher : public I_Publisher<PKeyClass, PKEventClass>, public dirty {
public:
	Multipublisher() :
		dirty(false)
	{
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
		
		auto it = m_indexedEvents.find(keyEvent);
		//typename std::map<PIndexClass, Subscriber<PKEventClass>*>* pNewSubscriberMap = nullptr;
		typename T_SubscriberMap* pNewSubscriberMap = nullptr;

		CBM((it == m_indexedEvents.end()), "Event %s already registered", GetEventKeyString(keyEvent));

		// Create a new subscriber list for the event entry
		pNewSubscriberMap = (T_SubscriberMap*)(new T_SubscriberMap());
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

	//std::map<PKeyClass, std::map<PIndexClass, Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT> GetEvents() {
	auto GetEvents() {
		return m_indexedEvents;
	}

public:
	// This requires the event to be registered 
	// This is an event subscription against a particular object
	RESULT RegisterSubscriber(PIndexClass indexEvent, PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;
		char *pszEvent = nullptr;

		typename T_KeyMap::iterator it;
		typename T_SubscriberMap* pSubscriberMap = nullptr;
		typename T_SubscriberMap::iterator indexedIt;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		it = m_indexedEvents.find(keyEvent);
		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		// Check if already registered
		pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);
		indexedIt = pSubscriberMap->find(indexEvent);

		CBM((indexedIt == pSubscriberMap->end()), "Object already subscribed to event %s", GetEventKeyString(keyEvent));

		// If not found - add it here
		(*pSubscriberMap)[indexEvent] = pSubscriber;

		// TODO: Don't currently support multiple subscribers to the same object 
		// If we want this - there needs to be a list of subscribers 

	Error:
		return r;
	}

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PIndexClass indexClass, PKeyClass keyClass, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		auto it = m_indexedEvents.find(keyClass);

		typename T_SubscriberMap* pSubscriberMap = nullptr;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		CBM((it == m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyClass));

		pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);

		if (pSubscriberMap != nullptr) {
			auto indexedIt = pSubscriberMap->find(indexClass);

			if (indexedIt != pSubscriberMap->end()) {
				pSubscriberMap->erase(indexedIt);
				return r;
			}

			// TODO: Don't currently support multiple subscribers to the same object 
			// If we want this - there needs to be a list of subscribers 

			CBM((0), "Index not found for event %s", GetEventKeyString(keyClass));
		}

		CBM((0), "Subscriber not found for event %s", GetEventKeyString(keyClass));

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
			T_SubscriberMap *pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);
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

	// This will simply remove all event subscribers 
	RESULT UnregisterSubscriber(PIndexClass indexClass) {
		RESULT r = R_PASS;
		bool fFound = false;

		for (auto &indexMap : m_indexedEvents) {
			auto it = indexMap.second->find(indexClass);

			if (it != indexMap.second->end()) {
				indexMap.second->erase(it);
				fFound = true;

				// Dirty used as an internal signal to ensure
				// old data not iterated on
				SetDirty();
			}
		}

		CBM((fFound), "No subscribers found for index");

	Error:
		return r;
	}

	bool FindIndexClass(PIndexClass indexClass) {
		bool fRefVal = false;

		for (auto &indexMap : m_indexedEvents) {
			for (auto &indexedMap : *(indexMap.second)) {
				if (indexedMap.first == indexClass) {
					return true;
				}
			}
		}

		return fRefVal;
	}

	bool FindKeyClass(PKeyClass keyClass) {
		auto it = m_indexedEvents.find(keyClass);

		if (it == m_indexedEvents.end()) {
			return false;
		}
		else {
			return true;
		}
	}

	virtual RESULT NotifySubscribers(PKeyClass keyEvent, PKEventClass *pEvent) override {
		RESULT r = R_PASS;

		typename T_SubscriberMap* pSubscriberMap = nullptr;
		auto it = m_indexedEvents.find(keyEvent);

		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber map is NULL");

		if (pSubscriberMap->size() > 0) {
			for (auto &indexSubItem : *pSubscriberMap) {
				WCR(indexSubItem.second->Notify(pEvent));
			}
		}

	Error:
		return r;
	}

	// This is multi publisher specific, the above will send to all (similar to publisher)
	RESULT NotifySubscribers(PIndexClass index, PKeyClass keyEvent, PKEventClass *pEvent) {
		RESULT r = R_PASS;

		typename T_SubscriberMap* pSubscriberMap = nullptr;
		auto it = m_indexedEvents.find(keyEvent);

		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber map is NULL");

		if (pSubscriberMap->size() > 0) {
			for (auto &indexSubItem : *pSubscriberMap) {
				
				if (indexSubItem.first == index) {
					WCR(indexSubItem.second->Notify(pEvent));
				}

				// If dirty set ensure index still exists
				if (CheckAndCleanDirty() && FindIndexClass(index) == false) {
					break;
				}
			}
		}

	Error:
		return r;
	}

	virtual bool EventHasSubscribers(PKeyClass keyEvent) override {
		RESULT r = R_PASS;

		typename T_SubscriberMap* pSubscriberMap = nullptr;
		auto it = m_indexedEvents.find(keyEvent);

		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber list is NULL");

		if (pSubscriberMap->size() > 0) {
			return true;
		}

	Error:
		return false;
	}

public:
	typedef Subscriber<PKEventClass> T_EventSubscriber;
	typedef std::vector<T_EventSubscriber*> T_SubscriberVector;
	//typedef std::map<PIndexClass, T_EventSubscriber*> T_KeyMap;

	//typedef std::map<PIndexClass, T_SubscriberVector> T_SubscriberMap;
	typedef std::map<PIndexClass, T_EventSubscriber*> T_SubscriberMap;

	typedef std::map<PKeyClass, T_SubscriberMap*, MAP_COMPARE_FUNCTION_STRUCT> T_KeyMap;

	T_KeyMap m_indexedEvents;
};

#endif // ! MULTIPUBLISHER_H_
