#ifndef DREAM_MODULE_H_
#define DREAM_MODULE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamModule.h
// The base Dream Module object

// Modules are different than apps in a number of ways.  Like apps, Modules have 
// an update function that will be called by Sandbox in the protected area.  However,
// unlike apps - Modules also have a ModuleProcess which is a dedicated thread (managed by the module itself) 
// that will run and execute tasks.

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

class DreamModuleTask {
public:
	enum state {
		PENDING,
		RUNNING,
		COMPLETE,
		FAILED,
		INVALID
	};

	DreamModuleTask(std::function<RESULT(void*)> fnTask, void *pContext = nullptr) :
		m_pContext(pContext),
		m_fnTask(fnTask)
	{
		// empty
	}

	~DreamModuleTask() {
		if (m_fnTask != nullptr) {
			m_fnTask = nullptr;
		}

		if (m_pContext != nullptr) {
			m_pContext = nullptr;
		}
	}

	RESULT Execute() {
		RESULT r = R_PASS;

		CN(m_fnTask);
		CR(m_fnTask(m_pContext));

	Success:
		m_state = state::COMPLETE;
		return r;

	Error:
		m_state = state::FAILED;
		return r;
	}

private:
	std::function<RESULT(void*)> m_fnTask;
	DreamModuleTask::state m_state = DreamModuleTask::state::INVALID;
	void *m_pContext = nullptr;
};

class DreamModuleBase {
	friend class DreamModuleManager;
	friend class DreamOS;
	friend struct DreamModuleBaseCompare;

public:
	virtual RESULT InitializeModule(void *pContext = nullptr) = 0;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;
	virtual RESULT Shutdown(void *pContext = nullptr) = 0;

	virtual DreamOS *GetDOS() = 0;

protected:
	virtual void *GetContext() = 0;
	virtual RESULT Print(std::string strOptString = "");

	RESULT FlagShutdown(std::string strShutdownFlagSignalName = "normal");
	bool IsShuttingDown();
	std::string GetShutdownFlagSignalName();

	// TODO: Add multi-tasking process to module
	// TODO: this will be added in the async modle loader etc 
	//RESULT ModuleProcess(void *pContext = nullptr);

protected:
	RESULT SetPriority(int priority);
	int GetPriority();
	RESULT ResetTimeRun();
	RESULT IncrementTimeRun(double usTimeDelta);
	double GetTimeRun();
	double GetEffectivePriorityValue() const;

protected:

	virtual std::string GetName() {
		return m_strName;
	}

	RESULT SetName(std::string strAppName) {
		m_strName = strAppName;
		return R_PASS;
	}

	RESULT SetModuleDescription(std::string strAppDescription) {
		m_strDescription = strAppDescription;
		return R_PASS;
	}

	UINT64 GetUIDValue() {
		return m_uid.GetID();
	}

	UID GetUID() {
		return m_uid;
	}

	// Module Thread Process
	RESULT StartModuleProcess(void *pContext = nullptr);
	RESULT KillModuleProcess();
	virtual RESULT ModuleProcess(void *pContext) { return R_NOT_IMPLEMENTED; }

	bool IsModuleProcessRunning() {
		return m_moduleThread.joinable();
	}

private:
	double m_usTimeRun = 0.0;
	int m_priority = 0;

	bool m_fShutdownFlag = false;
	std::string m_strShutdownFlagSignalName;

	std::thread	m_moduleThread;

protected:
	std::string m_strName;
	std::string m_strDescription;

private:
	UID m_uid;
};

// Using Fixed-priority preemptive scheduling: https://en.wikipedia.org/wiki/Fixed-priority_pre-emptive_scheduling
struct DreamModuleBaseCompare {
	bool operator()(const std::shared_ptr<DreamModuleBase> &lhsModule, const std::shared_ptr<DreamModuleBase> &rhsModule) const {
		// Note: This is actually returning the lowest value (not highest) since priority is inverted
		return lhsModule->GetEffectivePriorityValue() > rhsModule->GetEffectivePriorityValue();
	}
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

	virtual RESULT InitializeModule(void *pContext = nullptr) = 0;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	//TODO: these can be moved into DreamApp.tpp
	virtual void *GetContext() override {
		return m_pContext;
	}

	static derivedModuleType* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
		return derivedModuleType::SelfConstruct(pDreamOS, pContext);
	};

	virtual DreamOS *GetDOS() override {
		return m_pDreamOS;
	}

private:
	DreamOS *m_pDreamOS = nullptr;
	void *m_pContext = nullptr;
};

#endif // ! DREAM_MODULE_H_