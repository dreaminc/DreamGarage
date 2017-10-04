#include "DreamApp.h"
#include "DreamOS.h"
#include "Primitives/composite.h"
#include "Primitives/vector.h"

// DreamAppBase
RESULT DreamAppBase::SetPriority(int priority) {
	m_priority = priority;
	return R_PASS;
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

float DreamAppBase::GetTimeRun() {
	return m_usTimeRun;
}

// In short, this will return 
float DreamAppBase::GetEffectivePriorityValue() const {
	return (m_usTimeRun * m_priority);
}

