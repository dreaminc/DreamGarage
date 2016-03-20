#include "version.h"
#include <math.h>

version::version(const version &ver) {
	// TODO: Memcpy could be faster
	m_major = ver.m_major;
	m_minor = ver.m_minor;
	m_doubleminor = ver.m_doubleminor;
}

version::version(int major, int minor, int doubleminor) {
	SetVersion(major, minor, doubleminor);
}

version::version(float fVer) {
	SetVersion(fVer);
}

version::~version() {
	// empty
}

RESULT version::SetVersion(int major, int minor, int doubleminor) {
	m_major = major;
	m_minor = minor;
	m_doubleminor = doubleminor;

	return R_PASS;
}


// Keep note this will convert the decimal to an equivalent integer
// so a number like 2.0002 will be converted to a version equivalent of 2.2
// Also note this will always set double minor to zero
RESULT version::SetVersion(float fVer) {
	RESULT r = R_PASS;

	double fractPart, intPart;
	int vMaj, vMin, vMinCount;

	fractPart = modf(fVer, &intPart);

	vMaj = (int)(intPart);

	// Convert fractional to integer only
	vMinCount = (int)ceil(-1.0f * log10(fractPart));
	vMin = (int)(fractPart * pow(10, vMinCount));

	m_major = vMaj;
	m_minor = vMin;
	m_doubleminor = 0;

Error:
	return r;
}

version& version::operator=(const version &arg) {
	if (this == &arg)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.

	m_major = arg.m_major;
	m_minor = arg.m_minor;
	m_doubleminor = arg.m_doubleminor;

	return *this;
}

bool version::operator==(const version &b) const {
	return (m_major == b.m_major && m_minor == b.m_minor && m_doubleminor == b.m_doubleminor);
}

bool version::operator!=(const version &b) const {
	return !((*this) == b);
}

bool version::operator<(const version &b) const {
	if (m_major < b.m_major) 
		return true;
	else if (m_major == b.m_major) 
		if (m_minor < b.m_minor) 
			return true;
		else if (m_minor == b.m_minor) 
			if (m_doubleminor < b.m_doubleminor) 
				return true;

	return false;
}			

bool version::operator>(const version &b) const {
	return (b < (*this));
}

bool version::operator<=(const version &b) const {
	return(((*this) == b) || ((*this) < b));
}

bool version::operator>=(const version &b) const {
	return (b <= (*this));
}

bool version::operator==(const float &b) const {
	return ((*this) == version(b));
}

bool version::operator!=(const float &b) const {
	return ((*this) != version(b));
}

bool version::operator<(const float &b) const {
	return ((*this) < version(b));
}

bool version::operator>(const float &b) const {
	return ((*this) > version(b));
}

bool version::operator<=(const float &b) const {
	return ((*this) <= version(b));
}

bool version::operator>=(const float &b) const {
	return ((*this) >= version(b));
}
