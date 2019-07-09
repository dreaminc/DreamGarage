#ifndef VERSION_H_
#define VERSION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/version.h
// Simple version container class for versions in the 
// format of major.minor.doubleminor

#include <string>

class version {
public:
	version(const version &ver);
	version(int major, int minor = 0, int doubleminor = 0);
	version(float fVer);
	version(long lVer);
	version(std::string strVer);
	~version();

	RESULT SetVersion(int major, int minor = 0, int doubleminor = 0);
	RESULT SetVersion(float fVer);
	RESULT SetVersion(long lVer);
	RESULT SetVersion(std::string strVer);

	std::string GetString(bool fShowMinorMinor = false);

private:
	int m_major;
	int m_minor;
	int m_doubleminor;

public:
	inline int &major() { return this->m_major; }
	inline int &minor() { return this->m_minor; }
	inline int &doubleminor() { return this->m_doubleminor; }

public:

	version& operator=(const version &arg);

	bool operator ==(const version &b) const;
	bool operator !=(const version &b) const;
	bool operator <(const version &b) const;
	bool operator >(const version &b) const;
	bool operator <=(const version &b) const;
	bool operator >=(const version &b) const;

	bool operator ==(const float &b) const;
	bool operator !=(const float &b) const;
	bool operator <(const float &b) const;
	bool operator >(const float &b) const;
	bool operator <=(const float &b) const;
	bool operator >=(const float &b) const;
};

#endif	// ! VERSION_H_
