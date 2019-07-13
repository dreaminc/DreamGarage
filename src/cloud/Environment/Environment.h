#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/Environment.h
// Base Environment object

#include <string>

class Environment {
public:
	Environment() :
		m_environmentID(-1)
	{
		// empty
	}

	Environment(long environmentID) :
		m_environmentID(environmentID)
	{
		// empty
	}

	RESULT PrintEnvironment() {
		DEBUG_LINEOUT("Environment ID %d", m_environmentID);
		return R_PASS;
	}

	long GetEnvironmentID() { return m_environmentID; }
	RESULT SetEnvironmentID(long environmentID) { m_environmentID = environmentID; return R_PASS; }

private:
	long m_environmentID;
};

#endif	// ! ENVIRONMENT_H_