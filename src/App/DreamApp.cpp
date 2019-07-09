#include "DreamApp.h"

#include "DreamOS.h"
#include "Primitives/composite.h"

// DreamAppBase
DreamAppBase::DreamAppBase(DreamOS* pDreamOS, void* pContext) :
	m_pDreamOS(pDreamOS),
	m_pContext(pContext)
{
	// 
}

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

DreamAppHandle* DreamAppBase::GetAppHandle() {
	return nullptr;
}

RESULT DreamAppBase::BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	DreamOS *pDOS = GetDOS();

	CN(pDOS);
	CR(pDOS->BroadcastDreamAppMessage(pDreamAppMessage));

Error:
	return r;
}

unsigned int DreamAppBase::GetHandleLimit() {
	return 1;
}

composite* DreamAppBase::GetComposite() {
	return m_pComposite;
}

RESULT DreamAppBase::SetComposite(composite* pComposite) {
	RESULT r = R_PASS;

	CBM(m_pComposite == nullptr, "composite is already set");

	m_pComposite = pComposite;

Error:
	return r;
}

void* DreamAppBase::GetAppContext() {
	return m_pContext;
}

RESULT DreamAppBase::Initialize() {
	RESULT r = R_PASS;

	// Grab the context composite from DreamOS
	CN(m_pDreamOS);
	m_pComposite = m_pDreamOS->AddComposite();
	CN(m_pComposite);

	// Initialize the OBB (collisions)
	CR(m_pComposite->InitializeOBB());
	CR(m_pDreamOS->AddObjectToInteractionGraph(m_pComposite));

	// Initialize the App
	CR(InitializeApp(m_pContext));

Error:
	return r;
}

