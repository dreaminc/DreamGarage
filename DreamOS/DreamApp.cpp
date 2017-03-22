#include "DreamApp.h"
#include "DreamOS.h"

// DreamAppBase
RESULT DreamAppBase::SetPriority(int priority) {
	m_priority = priority;
	return R_PASS;
}

int DreamAppBase::GetPriority() {
	return m_priority;
}

RESULT DreamAppBase::ResetTimeRun() {
	m_usTimeRun = 0.0f;
	return R_PASS;
}

RESULT DreamAppBase::IncrementTimeRun(float usTimeDelta) {
	m_usTimeRun += usTimeDelta;
	return R_PASS;
}

float DreamAppBase::GetTimeRun() {
	return m_usTimeRun;
}

// In short, this will return 
float DreamAppBase::GetEffectivePriorityValue() const {
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
	CR(m_pDreamOS->AddInteractionObject(m_pCompositeContext));

	// Initialize the App
	CR(InitializeApp(m_pContext));

Error:
	return r;
}