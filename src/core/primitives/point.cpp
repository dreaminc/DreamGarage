#include "point.h"
#include "vector.h"

point::point() {
	clear();
}

point::point(point_precision val) {
	this->clear();
	this->element(0, 0) = val;
	this->element(1, 0) = val;
	this->element(2, 0) = val;
	this->element(3, 0) = 1.0f;
}

point::point(point_precision x, point_precision y, point_precision z) {
	this->clear();
	this->element(0, 0) = x;
	this->element(1, 0) = y;
	this->element(2, 0) = z;
	this->element(3, 0) = 1.0f;
}

point::point(point_precision values[3]) {
	this->clear();
	this->element(0, 0) = values[0];
	this->element(1, 0) = values[1];
	this->element(2, 0) = values[2];
	this->element(3, 0) = 1.0f;
}

point::point(point_precision x, point_precision y, point_precision z, point_precision w) {
	this->clear();

	this->element(0, 0) = x;
	this->element(1, 0) = y;
	this->element(2, 0) = z;
	this->element(3, 0) = w;
}

point::point(const matrix<point_precision, 4, 1>& arg) :
	matrix<point_precision, 4, 1>(arg)
{
	// empty
}

point& point::operator=(const matrix<point_precision, 4, 1> &arg) {
    if (this == &arg)      // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.

    memcpy(this->m_data, arg.m_data, sizeof(point_precision) * 4 * 1);

    return *this;
}

RESULT point::SetZeroW() {
	this->element(3, 0) = 1.0f;
	return R_PASS;
}

RESULT point::Reverse() {

	x() = -x();
	y() = -y();
	z() = -z();

	return R_PASS;
}

bool point::IsZero() {
	if (m_data[0] == 0.0f && m_data[1] == 0.0f && m_data[2] == 0.0f)
		return true;

	return false;
}

RESULT point::translate(point_precision x, point_precision y, point_precision z) {
	this->x() += x;
	this->y() += y;
	this->z() += z;

	return R_PASS;
}

RESULT point::translateX(point_precision x) {
	this->x() += x;
	return R_PASS;
}

RESULT point::translateY(point_precision y) {
	this->y() += y;
	return R_PASS;
}

RESULT point::translateZ(point_precision z) {
	this->z() += z;
	return R_PASS;
}

// This should also work with vector
RESULT point::translate(vector v) {
	(*this) += v;
	return R_PASS;
}

RESULT point::Print(const char* const &pszOptName, bool fReturn) {
	if (fReturn) {
		DEBUG_LINEOUT_RETURN("%s(%f, %f, %f, %f)", (pszOptName != nullptr) ? pszOptName : "pt", x(), y(), z(), w());
	}
	else {
		DEBUG_LINEOUT("%s(%f, %f, %f, %f)", (pszOptName != nullptr) ? pszOptName : "pt", x(), y(), z(), w());
	}

	return R_PASS;
}

std::string point::toString(bool fW) {
	std::string strRet = "(";

	strRet += std::to_string(x());
	strRet += ",";
	strRet += std::to_string(y());
	strRet += ",";
	strRet += std::to_string(z());

	if (fW) {
		strRet += ",";
		strRet += std::to_string(w());
	}

	strRet += ")";

	return strRet;
}

point_precision& point::GetComponent(component c) {
	switch (c) {
		case component::X: {
			return x();
		} break;

		case component::Y: {
			return y();
		} break;

		case component::Z: {
			return z();
		} break;

		case component::W:
		case component::INVALID:
		default: {
			return w();
		} break;
	}

	return w();
}

// General purpose component swap
RESULT point::swap(component a, component b) {
	point_precision temp = GetComponent(a);
	
	GetComponent(a) = GetComponent(b);
	GetComponent(b) = temp;

	return R_PASS;
}

// Operators

vector point::operator-(const point& rhs) const {
	vector retVector(*this);

	retVector.m_data[0] -= rhs.m_data[0];
	retVector.m_data[1] -= rhs.m_data[1];
	retVector.m_data[2] -= rhs.m_data[2];

	return retVector;
}

point point::operator+(const point& rhs) const {
	return point(*this).operator+=(rhs);
}

point& point::operator+=(const point& rhs) {
	m_data[0] = (m_data[0] + rhs.m_data[0]);
	m_data[1] = (m_data[1] + rhs.m_data[1]);
	m_data[2] = (m_data[2] + rhs.m_data[2]);

	return (*this);
}

point point::operator+(const vector& rhs) const {
	return point(*this).operator+=(rhs);
}

point& point::operator+=(const vector& rhs) {
	m_data[0] = (m_data[0] + rhs.m_data[0]);
	m_data[1] = (m_data[1] + rhs.m_data[1]);
	m_data[2] = (m_data[2] + rhs.m_data[2]);

	return (*this);
}

point& point::operator-=(const vector& rhs) {
	m_data[0] = (m_data[0] - rhs.m_data[0]);
	m_data[1] = (m_data[1] - rhs.m_data[1]);
	m_data[2] = (m_data[2] - rhs.m_data[2]);

	return (*this);
}

point point::operator-(const vector& rhs) const {
	return point(*this).operator-=(rhs);
}

bool point::operator>(const point &rhs) const {
	// Same object?
	if (this == &rhs)
		return false;

	return (
		(x() > rhs.x()) &&
		(y() > rhs.y()) &&
		(z() > rhs.z())
		);
}

bool point::operator>=(point &rhs) const {
	// Same object?
	if (this == &rhs)
		return true;

	return (
		(x() >= rhs.x()) &&
		(y() >= rhs.y()) &&
		(z() >= rhs.z())
		);
}

bool point::operator<(point &rhs) const {
	// Same object?
	if (this == &rhs)
		return false;

	return (
		(x() < rhs.x()) &&
		(y() < rhs.y()) &&
		(z() < rhs.z())
		);
}

bool point::operator<=(point &rhs) const {
	// Same object?
	if (this == &rhs)
		return true;

	return (
		(x() <= rhs.x()) &&
		(y() <= rhs.y()) &&
		(z() <= rhs.z())
		);
}

// Utility
point point::max(point &lhs, point &rhs) {
	if (lhs > rhs)
		return lhs;
	else
		return rhs;
}

point point::min(point &lhs, point &rhs) {
	if (lhs < rhs)
		return lhs;
	else
		return rhs;
}

point point::midpoint(point &lhs, point &rhs) {
	point returnPoint;

	returnPoint.x() = (lhs.x() + rhs.x()) / 2.0f;
	returnPoint.y() = (lhs.y() + rhs.y()) / 2.0f;
	returnPoint.z() = (lhs.z() + rhs.z()) / 2.0f;

	return returnPoint;
}