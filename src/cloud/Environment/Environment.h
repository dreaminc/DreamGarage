#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/Environment/Environment.h

// Base Environment object

#include <string>

// TODO: Is this a model?

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

	~Environment() = default;

	RESULT PrintEnvironment() {
		DEBUG_LINEOUT("Environment ID %d", (int)m_environmentID);
		return R_PASS;
	}

	long GetEnvironmentID() { return m_environmentID; }
	RESULT SetEnvironmentID(long environmentID) { m_environmentID = environmentID; return R_PASS; }

private:
	long m_environmentID;
};

#endif	// ! ENVIRONMENT_H_