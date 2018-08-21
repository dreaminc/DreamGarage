#include "DreamModule.h"
#include "DreamOS.h"

// DreamModuleBase
RESULT DreamModuleBase::SetPriority(int priority) {
	m_priority = priority;
	return R_PASS;
}

RESULT DreamModuleBase::FlagShutdown(std::string strShutdownFlagSignalName) {
	RESULT r = R_PASS;

	CB((m_fShutdownFlag == false));

	m_fShutdownFlag = true;
	m_strShutdownFlagSignalName = strShutdownFlagSignalName;

Error:
	return r;
}

bool DreamModuleBase::IsShuttingDown() {
	return m_fShutdownFlag;
}

std::string DreamModuleBase::GetShutdownFlagSignalName() {
	return m_strShutdownFlagSignalName;
}

int DreamModuleBase::GetPriority() {
	return m_priority;
}

RESULT DreamModuleBase::ResetTimeRun() {
	m_usTimeRun = 0.0;
	return R_PASS;
}

RESULT DreamModuleBase::IncrementTimeRun(double usTimeDelta) {
	m_usTimeRun += usTimeDelta;
	return R_PASS;
}

double DreamModuleBase::GetTimeRun() {
	return m_usTimeRun;
}

// In short, this will return 
double DreamModuleBase::GetEffectivePriorityValue() const {
	return (m_usTimeRun * m_priority);
}

RESULT DreamModuleBase::Print(std::string strOptString) {
	//DEBUG_LINEOUT_RETURN("%s: %s running %fus pri: %d", strOptString.c_str(), (m_strName.length() > 0) ? m_strName.c_str() : "DreamModule", GetTimeRun(), GetPriority());
	DEBUG_LINEOUT("%s: %s running %fus pri: %d", strOptString.c_str(), (m_strName.length() > 0) ? m_strName.c_str() : "DreamModule", GetTimeRun(), GetPriority());
	return R_PASS;
}