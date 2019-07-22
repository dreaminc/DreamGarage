#ifndef DREAM_APP_H_
#define DREAM_APP_H_

#include "core/ehm/EHM.h"

// Dream App
// dos/src/app/DreamApp.h

// The base Dream App object

#include "core/types/DObject.h"

#include <string>
#include <memory>

class DreamOS;
class composite;
class DreamAppHandle;
class PeerConnection;
class DreamAppMessage;

class DreamAppBase : public DObject {
	friend class DreamAppManager;
	friend struct DreamAppBaseCompare;
	friend class DreamOS;

public:
	DreamAppBase(DreamOS* pDreamOS, void* pContext = nullptr);
	~DreamAppBase() = default;

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;
	virtual RESULT Shutdown(void *pContext = nullptr) = 0;

	virtual RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) { return R_NOT_HANDLED; }

	virtual composite* GetComposite();
	RESULT SetComposite(composite* pComposite);
	void* GetAppContext();

	virtual DreamAppHandle* GetAppHandle();
	virtual DreamOS *GetDOS() = 0;
	virtual unsigned int GetHandleLimit();

	UID GetAppUID() {
		return getID();
	}

protected:
	virtual RESULT Print() { return R_NOT_IMPLEMENTED; }

	RESULT FlagShutdown(std::string strShutdownFlagSignalName = "normal");
	bool IsAppShuttingDown();
	std::string GetShutdownFlagSignalName();

	RESULT SetAddToSceneFlag();
	bool CheckAndCleanAddToSceneFlag();

protected:
	RESULT SetPriority(int priority);
	int GetPriority();
	RESULT ResetTimeRun();
	RESULT IncrementTimeRun(double usTimeDelta);
	double GetTimeRun();
	double GetEffectivePriorityValue() const;

protected:

	virtual std::string GetAppName() {
		return m_strAppName;
	}

	RESULT SetAppName(std::string strAppName) {
		m_strAppName = strAppName;
		return R_PASS;
	}

	RESULT SetAppDescription(std::string strAppDescription) {
		m_strAppDescription = strAppDescription;
		return R_PASS;
	}

	RESULT BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage);

	RESULT Initialize();

private:
	double m_usTimeRun = 0.0;
	int m_priority = 0;

	bool m_fShutdownFlag = false;
	std::string m_strShutdownFlagSignalName;

	bool m_fAddToSceneFlag = false;

private:
	std::string m_strAppName;
	std::string m_strAppDescription;

protected:
	DreamOS* m_pDreamOS = nullptr;
	composite* m_pComposite = nullptr;
	void* m_pContext = nullptr;
};


// Using Fixed-priority preemptive scheduling: https://en.wikipedia.org/wiki/Fixed-priority_pre-emptive_scheduling
struct DreamAppBaseCompare {
	bool operator()(const std::shared_ptr<DreamAppBase> &lhsApp , const std::shared_ptr<DreamAppBase> &rhsApp) const {
		// Note: This is actually returning the lowest value (not highest) since priority is inverted
		return lhsApp->GetEffectivePriorityValue() > rhsApp->GetEffectivePriorityValue();
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
	enum class Axes : uint16_t {
		NONE = 0,
		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,
		ALL = 0x7
	};

	DreamApp(DreamOS *pDreamOS, void *pContext = nullptr) :
		DreamAppBase(pDreamOS, pContext)
	{
		// 
	}

	~DreamApp() = default;

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	//vector GetCameraLookXZ();
	//RESULT UpdateCompositeWithCameraLook(float depth, float yPos);
	//RESULT UpdateCompositeWithHands(float yPos, Axes handAxes = Axes::ALL);

	static derivedAppType* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
		return derivedAppType::SelfConstruct(pDreamOS, pContext);
	};

	virtual DreamOS *GetDOS() override {
		return m_pDreamOS;
	}

	virtual RESULT Print() override {
		//DEBUG_LINEOUT_RETURN("%s running %fus pri: %d", (m_strAppName.length() > 0) ? m_strAppName.c_str() : "DreamApp", GetTimeRun(), GetPriority());
		return R_PASS;
	}
	
};

#include "DreamApp.tpp"

#endif // ! DREAM_APP_H_
