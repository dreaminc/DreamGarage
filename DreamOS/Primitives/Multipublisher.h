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
		typename Multipublisher::T_SubscriberMap* pNewSubscriberMap = nullptr;

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
		typename Multipublisher::T_SubscriberMap* pSubscriberMap = nullptr;
		typename T_SubscriberMap::iterator indexedIt;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		it = m_indexedEvents.find(keyEvent);
		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		// Check if already registered
		pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);
		indexedIt = pSubscriberMap->find(indexEvent);

		// Look for the item's subscriber vector, if it doesn't exist add it
		if (indexedIt == pSubscriberMap->end()) {
			(*pSubscriberMap)[indexEvent] = T_SubscriberVector();
			T_SubscriberVector *pSubscriberVector = &((*pSubscriberMap)[indexEvent]);
			pSubscriberVector->push_back(pSubscriber);
		}
		else {
			// Vector already exists, check to see if the subscriber is already in the item's subscriber vector
			T_SubscriberVector *pSubscriberVector = &((*pSubscriberMap)[indexEvent]);
			auto subscriberVectorIt = find(pSubscriberVector->begin(), pSubscriberVector->end(), pSubscriber);

			CBM((subscriberVectorIt == pSubscriberVector->end()), "Object already subscribed to event %s", GetEventKeyString(keyEvent));
			pSubscriberVector->push_back(pSubscriber);
		}
		
		//(*pSubscriberMap)[indexEvent] = pSubscriber;

	Error:
		return r;
	}

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PIndexClass indexClass, PKeyClass keyClass, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		auto it = m_indexedEvents.find(keyClass);

		CNM(pSubscriber, "Subscriber cannot be NULL");
		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyClass));

		{
			typename Multipublisher::T_SubscriberMap* pSubscriberMap = nullptr;

			pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);
			CNM(pSubscriberMap, "Subscriber map not set for index");

			auto indexedIt = pSubscriberMap->find(indexClass);
			CBM((indexedIt != pSubscriberMap->end()), "Object not subscribed to event");

			T_SubscriberVector *pSubscriberVector = &((*pSubscriberMap)[indexClass]);
			auto subscriberVectorIt = find(pSubscriberVector->begin(), pSubscriberVector->end(), pSubscriber);
			CBM((subscriberVectorIt != pSubscriberVector->end()), "Subscriber not subscribed to event %s", GetEventKeyString(keyClass));

			pSubscriberVector->erase(subscriberVectorIt);

			// Check to see if vector is empty so we can remove it
			if (pSubscriberVector->size() == 0) {
				pSubscriberMap->erase(indexedIt);
				
				SetDirty();
				
				return r;
			}
		}

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
			PKeyClass keyEvent = reinterpret_cast<PKeyClass>(it->first);
			Multipublisher::T_SubscriberMap *pSubscriberMap = reinterpret_cast<T_SubscriberMap*>(it->second);

			if (pSubscriberMap != nullptr) {

				auto subscriberMapIt = pSubscriberMap->begin();

				while (subscriberMapIt != pSubscriberMap->end()) {

					T_SubscriberVector *pSubscriberVector = &(subscriberMapIt->second);
					auto subscriberVectorIt = find(pSubscriberVector->begin(), pSubscriberVector->end(), pSubscriber);

					// Above code should ensure that there is only one unique 
					// subscriber in each item vector
					if (subscriberVectorIt != pSubscriberVector->end()) {
						pSubscriberVector->erase(subscriberVectorIt);
						SetDirty();
					}

					subscriberMapIt++;

				}
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

		typename Multipublisher::T_SubscriberMap* pSubscriberMap = nullptr;
		auto it = m_indexedEvents.find(keyEvent);

		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber map is NULL");

		if (pSubscriberMap->size() > 0) {
			for (auto &indexSubItem : *pSubscriberMap) {
				for (auto &pSubscriber : indexSubItem.second) {
					//WCR(indexSubItem.second->Notify(pEvent));
					WCR(pSubscriber->Notify(pEvent));
				}
			}
		}

	Error:
		return r;
	}

	// This is multi publisher specific, the above will send to all (similar to publisher)
	RESULT NotifySubscribers(PIndexClass index, PKeyClass keyEvent, PKEventClass *pEvent) {
		RESULT r = R_PASS;

		typename Multipublisher::T_SubscriberMap* pSubscriberMap = nullptr;
		auto it = m_indexedEvents.find(keyEvent);

		CBM((it != m_indexedEvents.end()), "Event %s not registered", GetEventKeyString(keyEvent));

		pSubscriberMap = m_indexedEvents[keyEvent];
		CNM(pSubscriberMap, "Subscriber map is NULL");

		if (pSubscriberMap->size() > 0) {
			for (auto &indexSubItem : *pSubscriberMap) {
				
				if (indexSubItem.first == index) {

					// Make a copy so that we don't block other subscribers
					// if one is removed (we are guaranteed only one unique so this will not result in duplicates)
					auto tempSubscriberVector = indexSubItem.second;

					for (auto &pSubscriber : tempSubscriberVector) {
						//WCR(indexSubItem.second->Notify(pEvent));

						// TODO: This doesn't handle the removal of the subscriber in the vector
						WCR(pSubscriber->Notify(pEvent));

						// If dirty set ensure index still exists
						if (CheckAndCleanDirty()) {
							if (FindIndexClass(index) == false) {
								// TODO: Not sure if we want to exit the function here 
								// but this does the job
								return r;
							}/*
							else {
								// TODO: While not eloquent, this will ensure we don't continue to
								// iterate a vector where the subscriber has been removed - there's only going to be 
								// one unique, but this way we don't prevent block subscribers on other indexes 
								break;
							}
							*/
						}
					}
				}				
			}
		}

	Error:
		return r;
	}

	virtual bool EventHasSubscribers(PKeyClass keyEvent) override {
		RESULT r = R_PASS;

		typename Multipublisher::T_SubscriberMap* pSubscriberMap = nullptr;
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

	typedef std::map<PIndexClass, T_SubscriberVector> T_SubscriberMap;
	//typedef std::map<PIndexClass, T_EventSubscriber*> T_SubscriberMap;

	typedef std::map<PKeyClass, T_SubscriberMap*, I_Publisher<PKeyClass, PKEventClass>::MAP_COMPARE_FUNCTION_STRUCT> T_KeyMap;

	T_KeyMap m_indexedEvents;
};

#endif // ! MULTIPUBLISHER_H_
