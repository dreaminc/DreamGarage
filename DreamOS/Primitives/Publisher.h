#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <list>
#include <map>
#include <string>

#include <math.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Publisher.h
// Publisher Object
// Couples with the Subscriber object to constitute the observer-notify pattern

//class Subscriber;
#include "Subscriber.h"
#include "Primitives/VirtualObj.h"

// TODO: Bring back the PEventClass - no more subscriber predefined event struct?
// First lets see if this topology works at all

template <typename PKeyClass, typename PKEventClass>
class I_Publisher {
public:
	virtual RESULT RegisterEvent(PKeyClass keyEvent) = 0;
	virtual bool IsEventRegistered(PKeyClass keyEvent) = 0;

	virtual RESULT NotifySubscribers(PKeyClass keyEvent, PKEventClass *pEvent) = 0;
	virtual bool EventHasSubscribers(PKeyClass keyEvent) = 0;

public:
	struct MAP_COMPARE_FUNCTION_STRUCT {
		bool operator()(std::string a, std::string b) const {
			return a.compare(b) < 0;
		}

		bool operator()(char const *a, char const *b) const {
			return std::strcmp(a, b) < 0;
		}

		bool operator()(int lhs, int rhs) const {
			return lhs < rhs;
		}

		bool operator()(VirtualObj* lhs, VirtualObj* rhs) const {
			return lhs->getID().GetID() < rhs->getID().GetID();
		}
	};

	char* GetEventKeyString(int keyEvent) {
		int numLength = (keyEvent == 0) ? 2 : (int)(log10(keyEvent)) + 2;

		char *pszNumString = new char[numLength];
		memset(pszNumString, 0, sizeof(char) * numLength);

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

	char* GetEventKeyString(const std::string& keyEvent) {
		int strLength = static_cast<int>(keyEvent.length());// strlen(keyEvent) + 1;
		char *pszString = new char[strLength];

		memset(pszString, 0, sizeof(char) * strLength);
		memcpy(pszString, keyEvent.c_str(), sizeof(char) * (strLength - 1));

		return pszString;
	}

	char* GetEventKeyString(VirtualObj* pKeyObject) {
		return GetEventKeyString((int)(pKeyObject->getID().GetID()));
	}
};

template <typename PKeyClass, typename PKEventClass>
class Publisher : public I_Publisher<PKeyClass, PKEventClass> {
public:
	Publisher() {
		// empty
	}

	// This will not deallocate the subscribers, the subscribing object is responsible to do that
	// and to unregister themselves
	~Publisher() {
		auto it = m_events.begin();

		while (it != m_events.end()) {
			std::list<Subscriber<PKEventClass>*> *pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

			if (pSubscriberList != nullptr) {
				delete pSubscriberList;
				pSubscriberList = nullptr;
			}
			it++;
		}
	}

	char *GetPublisherName() {
		return "Publisher Base Class";
	}

protected:
	virtual RESULT RegisterEvent(PKeyClass keyEvent) override {
		RESULT r = R_PASS;		
		
		auto it = m_events.find(keyEvent);
        std::list<Subscriber<PKEventClass>*>* pNewSubscriberList = nullptr;

		CBM((it == m_events.end()), "Event %s already registered", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));
		
		// Create a new subscriber list for the event entry
		pNewSubscriberList = (std::list<Subscriber<PKEventClass>*>*)(new std::list<Subscriber<PKEventClass>*>());
		m_events[keyEvent] = pNewSubscriberList;

		//DEBUG_LINEOUT("%s Registered event %s", GetPublisherName(), pszEvent);

	Error:
		return r;
	}

public:
	virtual bool IsEventRegistered(PKeyClass keyEvent) override {
		auto it = m_events.find(keyEvent);
		return (!(it == m_events.end()));
	}

	std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, I_Publisher<PKeyClass, PKEventClass>::MAP_COMPARE_FUNCTION_STRUCT> GetEvents() {
		return m_events;
	}
	
public:
	// This requires the event to be registered 
	RESULT RegisterSubscriber(PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;
		
		typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it;
		std::list<Subscriber<PKEventClass>*> *pSubscriberList = nullptr;

		CNM(pSubscriber, "Subscriber cannot be NULL");
		it = m_events.find(keyEvent);
		CBM((it != m_events.end()), "Event %s not registered", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));

		// Check if already registered
		pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

		for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
			Subscriber<PKEventClass>* pTempSubscriber = reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator);
			CBM((pTempSubscriber != pSubscriber), "Already subscribed to %s", GetEventKeyString(keyEvent));
		}

		// If we made it this far then push the subscriber into the list for the given event
		m_events[keyEvent]->push_back(pSubscriber);

	Error:
		return r;
	}

	// Error handling warranted by the fact that this should only be called with confidence 
	// that the subscriber is subscriber to a given event per the subscriber or the Publisher 
	// releasing all subscriber events for whatever purpose
	RESULT UnregisterSubscriber(PKeyClass keyEvent, Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		auto it = m_events.find(keyEvent);
		
        typename std::list<Subscriber<PKEventClass>*> *pSubscriberList = nullptr;
        
        CNM(pSubscriber, "Subscriber cannot be NULL");
		CBM((it == m_events.end()), "Event %s not registered", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));

		pSubscriberList = reinterpret_cast<std::list<Subscriber<PKEventClass>*>*>(it->second);

		if (pSubscriberList != nullptr) {
			for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
				Subscriber<PKEventClass>* pListSubscriber = reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator);

				if (pListSubscriber == pSubscriber) {
					pSubscriberList->remove(pSubscriber);
					return r;
				}
			}
		}

		CBM((0), "Subscriber not found for event %s", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));

	Error:
		return r;
	}

	// This will unsubscribe a subscriber from all events
	// Error handling warranted by the fact that something is really wrong if
	// things get out of line
	RESULT UnregisterSubscriber(Subscriber<PKEventClass>* pSubscriber) {
		RESULT r = R_PASS;

		typename std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, MAP_COMPARE_FUNCTION_STRUCT>::iterator it = m_events.begin();
        CNM(pSubscriber, "Subscriber cannot be NULL");

		while (it != m_events.end()) {
			PKeyClass keyEvent = reinterpret_cast<PKeyClass>(it->first);

			CRM(UnregisterSubscriber(keyEvent, pSubscriber), "Failed to unsubscribe for event %s", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));
			
			it++;
		}

	Error:
		return r;
	}

	virtual RESULT NotifySubscribers(PKeyClass keyEvent, PKEventClass *pEvent) override {
		RESULT r = R_PASS;
		
        typename std::list<Subscriber<PKEventClass>*> *pSubscriberList = nullptr;
		auto it = m_events.find(keyEvent);

		CBM((it != m_events.end()), "Event %s not registered", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));
		
		pSubscriberList = m_events[keyEvent];
		CNM(pSubscriberList, "Subscriber list is NULL");

		if (pSubscriberList->size() > 0) {
			for (auto eventIterator = pSubscriberList->begin(); eventIterator != pSubscriberList->end(); eventIterator++) {
				WCR(reinterpret_cast<Subscriber<PKEventClass>*>(*eventIterator)->Notify(pEvent));
			}
		}

	Error:
		return r;
	}

	virtual bool EventHasSubscribers(PKeyClass keyEvent) override {
		RESULT r = R_PASS;

		typename std::list<Subscriber<PKEventClass>*> *pSubscriberList = nullptr;
		auto it = m_events.find(keyEvent);

		CBM((it != m_events.end()), "Event %s not registered", I_Publisher<PKeyClass, PKEventClass>::GetEventKeyString(keyEvent));

		pSubscriberList = m_events[keyEvent];
		CNM(pSubscriberList, "Subscriber list is NULL");

		if (pSubscriberList->size() > 0) {
			return true;
		}

	Error:
		return false;
	}

private:
	std::map<PKeyClass, std::list<Subscriber<PKEventClass>*>*, I_Publisher<PKeyClass, PKEventClass>::MAP_COMPARE_FUNCTION_STRUCT> m_events;
};

#endif // ! PUBLISHER_H_