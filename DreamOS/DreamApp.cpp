#include "DreamApp.h"
#include "DreamOS.h"

// DreamAppBase
RESULT DreamAppBase::SetPriority(int priority) {
	m_priority = priority;
	return R_PASS;
}

RESULT DreamAppBase::FlagShutdown(std::string strShutdownFlagSignalName) {
	RESULT r = R_PASS;

	CB((m_fShutdownFlag == false));

	m_fShutdownFlag = true;
	m_strShutdownFlagSignalName = strShutdownFlagSignalName;

Error:
	return r;
}

bool DreamAppBase::IsAppShuttingDown() {
	return m_fShutdownFlag;
}

std::string DreamAppBase::GetShutdownFlagSignalName() {
	return m_strShutdownFlagSignalName;
}

RESULT DreamAppBase::SetAddToSceneFlag() {
	m_fAddToSceneFlag = true;
	return R_PASS;
}

bool DreamAppBase::CheckAndCleanAddToSceneFlag() {
	bool fRetVal = m_fAddToSceneFlag;
	m_fAddToSceneFlag = false;
	return fRetVal;
}

int DreamAppBase::GetPriority() {
	return m_priority;
}

RESULT DreamAppBase::ResetTimeRun() {
	m_usTimeRun = 0.0;
	return R_PASS;
}

RESULT DreamAppBase::IncrementTimeRun(double usTimeDelta) {
	m_usTimeRun += usTimeDelta;
	return R_PASS;
}

double DreamAppBase::GetTimeRun() {
	return m_usTimeRun;
}

// In short, this will return 
double DreamAppBase::GetEffectivePriorityValue() const {
	return (m_usTimeRun * m_priority);
}

// DreamApp<derived>

template<class derivedAppType>
RESULT DreamApp<derivedAppType>::Initialize() {
	RESULT r = R_PASS;

	// Grab the context composite from DreamOS
	CN(m_pDreamOS);
	m_pCompositeContext = m_pDreamOS->AddComposite();
	CN(m_pCompositeContext);

	// Initialize the OBB (collisions)
	CR(m_pCompositeContext->InitializeOBB());
	CR(m_pDreamOS->AddObjectToInteractionGraph(m_pCompositeContext));

	// Initialize the App
	CR(InitializeApp(m_pContext));

Error:
	return r;
}

