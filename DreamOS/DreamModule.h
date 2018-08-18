#ifndef DREAM_MODULE_H_
#define DREAM_MODULE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamModule.h
// The base Dream Module object

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"
#include "Primitives/vector.h"
#include "Primitives/point.h"

#include <string>
#include <memory>

class DreamOS;
class composite;
class DreamAppHandle;
class PeerConnection;
class DreamAppMessage;

class DreamModuleBase {
	friend class DreamModuleManager;
	friend class DreamOS;

public:
	virtual RESULT InitializeModule(void *pContext = nullptr) = 0;
	virtual RESULT OnModuleDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;
	virtual RESULT Shutdown(void *pContext = nullptr) = 0;

	virtual DreamOS *GetDOS() = 0;

protected:
	virtual void *GetModuleContext() = 0;
	virtual RESULT Print() { return R_NOT_IMPLEMENTED; }

	RESULT FlagShutdown(std::string strShutdownFlagSignalName = "normal");
	bool IsModuleShuttingDown();
	std::string GetShutdownFlagSignalName();

protected:
	RESULT SetPriority(int priority);
	int GetPriority();
	RESULT ResetTimeRun();
	RESULT IncrementTimeRun(double usTimeDelta);
	double GetTimeRun();
	double GetEffectivePriorityValue() const;

protected:

	virtual std::string GetModuleName() {
		return m_strModuleName;
	}

	RESULT SetModuleName(std::string strAppName) {
		m_strModuleName = strAppName;
		return R_PASS;
	}

	RESULT SetModuleDescription(std::string strAppDescription) {
		m_strModuleDescription = strAppDescription;
		return R_PASS;
	}

	UINT64 GetUIDValue() {
		return m_uid.GetID();
	}

	UID GetAppUID() {
		return m_uid;
	}

private:
	double m_usTimeRun = 0.0;
	int m_priority = 0;

	bool m_fShutdownFlag = false;
	std::string m_strShutdownFlagSignalName;

private:
	std::string m_strModuleName;
	std::string m_strModuleDescription;
	UID m_uid;
};

template<class derivedModuleType>
class DreamModule : public DreamModuleBase, public valid {
	enum class UPDATE_MODEL {
		UPDATE_ON_SIGNAL,
		UPDATE_ON_FRAME,
		UPDATE_ON_TIME,
		UPDATE_INVALID
	};

public:

	DreamModule(DreamOS *pDreamOS, void *pContext = nullptr) :
		m_pDreamOS(pDreamOS),
		m_pContext(pContext)
	{
		// Empty
	}

	~DreamModule() {
		// empty
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		// Grab the context composite from DreamOS
		CN(m_pDreamOS);

		// Initialize the App
		CR(InitializeModule(m_pContext));

	Error:
		return r;
	}

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	//TODO: these can be moved into DreamApp.tpp
	void *GetAppContext() {
		return m_pContext;
	}

	static derivedModuleType* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
		return derivedModuleType::SelfConstruct(pDreamOS, pContext);
	};

	virtual DreamOS *GetDOS() override {
		return m_pDreamOS;
	}

	virtual RESULT Print() override {
		//DEBUG_LINEOUT_RETURN("%s running %fus pri: %d", (m_strAppName.length() > 0) ? m_strAppName.c_str() : "DreamApp", GetTimeRun(), GetPriority());
		return R_PASS;
	}

private:
	DreamOS *m_pDreamOS = nullptr;
	void *m_pContext = nullptr;
};

#endif // ! DREAM_MODULE_H_