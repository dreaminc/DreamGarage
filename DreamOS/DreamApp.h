#ifndef DREAM_APP_H_
#define DREAM_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"

#include <string>

class DreamOS;
class composite;

class DreamAppBase {
	friend class DreamAppManager;
	friend struct DreamAppBaseCompare;

public:
	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	virtual void *GetAppContext() = 0;

protected:
	RESULT SetPriority(int priority);
	int GetPriority();
	RESULT ResetTimeRun();
	RESULT IncrementTimeRun(float usTimeDelta);
	float GetTimeRun();
	float GetEffectivePriorityValue() const;

private:
	float m_usTimeRun = 0.0f;
	int m_priority = 0;
};


// Using Fixed-priority preemptive scheduling: https://en.wikipedia.org/wiki/Fixed-priority_pre-emptive_scheduling
struct DreamAppBaseCompare {
	bool operator()(const DreamAppBase &lhsApp , const DreamAppBase &rhsApp) const {
		return lhsApp.GetEffectivePriorityValue() < rhsApp.GetEffectivePriorityValue();
	}
};

template<class derivedAppType>
class DreamApp : public DreamAppBase, public valid {
	enum class UPDATE_MODEL {
		UPDATE_ON_SIGNAL,
		UPDATE_ON_FRAME,
		UPDATE_ON_TIME,
		UPDATE_INVALID
	};

public:
	DreamApp(DreamOS *pDreamOS, void *pContext = nullptr) :
		m_pDreamOS(pDreamOS),
		m_pCompositeContext(nullptr),
		m_pContext(pContext)
	{
		// Empty
	}

	~DreamApp() {
		// empty
	}

	RESULT Initialize();

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	void *GetAppContext() {
		return m_pContext;
	}

	static derivedAppType* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
		return derivedAppType::SelfConstruct(pDreamOS, pContext);
	};

	RESULT SetAppName(std::string strAppName) {
		m_strAppName = strAppName;
		return R_PASS;
	}

	RESULT SetAppDescription(std::string strAppDescription) {
		m_strAppDescription = strAppDescription;
		return R_PASS;
	}

	DreamOS *GetDOS() {
		return m_pDreamOS;
	}

private:
	composite *m_pCompositeContext;
	DreamOS *m_pDreamOS;
	void *m_pContext = nullptr;

private:
	std::string m_strAppName;
	std::string m_strAppDescription;
	UID m_uid;
};

#endif // ! DREAM_APP_H_