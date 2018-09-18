#include "point2D.h"
#include "vector.h"

point2D::point2D() {
	clear();
}

point2D::point2D(point_precision val) {
	this->element(0, 0) = val;
	this->element(1, 0) = val;
}

point2D::point2D(point_precision x, point_precision y) {
	this->element(0, 0) = x;
	this->element(1, 0) = y;
}

point2D::point2D(point_precision values[2]) {
	this->element(0, 0) = values[0];
	this->element(1, 0) = values[1];
}

point2D::point2D(const matrix<point_precision, 2, 1>& arg) :
	matrix<point_precision, 2, 1>(arg)
{
	// empty
}

RESULT point2D::Reverse() {

	x() = -x();
	y() = -y();

	return R_PASS;
}

bool point2D::IsZero() {
	if (m_data[0] == 0.0f && m_data[1] == 0.0f)
		return true;

	return false;
}

RESULT point2D::translate(point_precision x, point_precision y) {
	this->x() += x;
	this->y() += y;

	return R_PASS;
}

RESULT point2D::translateX(point_precision x) {
	this->x() += x;
	return R_PASS;
}

RESULT point2D::translateY(point_precision y) {
	this->y() += y;
	return R_PASS;
}

// This should also work with vector
RESULT point2D::translate(vector v) {
	(*this) += v;
	return R_PASS;
}

RESULT point2D::Print(const char* const &pszOptName, bool fReturn) {
	if (fReturn) {
		DEBUG_LINEOUT_RETURN("%s(%f, %f)", (pszOptName != nullptr) ? pszOptName : "pt2d", x(), y());
	}
	else {
		DEBUG_LINEOUT("%s(%f, %f)", (pszOptName != nullptr) ? pszOptName : "pt2d", x(), y());
	}

	return R_PASS;
}

std::string point2D::toString() {
	std::string strRet = "(";

	strRet += std::to_string(x());
	strRet += ",";
	strRet += std::to_string(y());

	strRet += ")";

	return strRet;
}

point_precision& point2D::GetComponent(component c) {
	switch (c) {
		case component::X: {
			return x();
		} break;

		case component::Y: {
			return y();
		} break;
	}

	return y();
}

// General purpose component swap
RESULT point2D::swap(component a, component b) {
	point_precision temp = GetComponent(a);
	
	GetComponent(a) = GetComponent(b);
	GetComponent(b) = temp;

	return R_PASS;
}

// Operators

point2D point2D::operator-(const point2D& rhs) const {
	point2D retPoint2D(*this);

	retPoint2D.m_data[0] -= rhs.m_data[0];
	retPoint2D.m_data[1] -= rhs.m_data[1];

	return retPoint2D;
}

point2D point2D::operator+(const point2D& rhs) const {
	return point2D(*this).operator+=(rhs);
}

point2D& point2D::operator+=(const point2D& rhs) {
	m_data[0] = (m_data[0] + rhs.m_data[0]);
	m_data[1] = (m_data[1] + rhs.m_data[1]);

	return (*this);
}

point2D point2D::operator+(const vector& rhs) const {
	return point2D(*this).operator+=(rhs);
}

point2D& point2D::operator+=(const vector& rhs) {
	m_data[0] = (m_data[0] + rhs.m_data[0]);
	m_data[1] = (m_data[1] + rhs.m_data[1]);

	return (*this);
}

point2D& point2D::operator-=(const vector& rhs) {
	m_data[0] = (m_data[0] - rhs.m_data[0]);
	m_data[1] = (m_data[1] - rhs.m_data[1]);

	return (*this);
}

point2D point2D::operator-(const vector& rhs) const {
	return point2D(*this).operator-=(rhs);
}

// Explicitly specializing the assignment operator
point2D& point2D::operator=(const matrix<point_precision, 2, 1> &arg) {
	if (this == &arg)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.

	memcpy(this->m_data, arg.m_data, sizeof(point_precision) * 2 * 1);

	return *this;
}

bool point2D::operator>(point2D &rhs) {
	// Same object?
	if (this == &rhs)
		return false;

	return (
		(x() > rhs.x()) &&
		(y() > rhs.y()) 
		);
}

bool point2D::operator>=(point2D &rhs) {
	// Same object?
	if (this == &rhs)
		return true;

	return (
		(x() >= rhs.x()) &&
		(y() >= rhs.y()) 
		);
}

bool point2D::operator<(point2D &rhs) {
	// Same object?
	if (this == &rhs)
		return false;

	return (
		(x() < rhs.x()) &&
		(y() < rhs.y()) 
		);
}

bool point2D::operator<=(point2D &rhs) {
	// Same object?
	if (this == &rhs)
		return true;

	return (
		(x() <= rhs.x()) &&
		(y() <= rhs.y()) 
		);
}

// Utility
point2D point2D::max(point2D &lhs, point2D &rhs) {
	if (lhs > rhs)
		return lhs;
	else
		return rhs;
}

point2D point2D::min(point2D &lhs, point2D &rhs) {
	if (lhs < rhs)
		return lhs;
	else
		return rhs;
}

point2D point2D::midpoint(point2D &lhs, point2D &rhs) {
	point2D returnPoint;

	returnPoint.x() = (lhs.x() + rhs.x()) / 2.0f;
	returnPoint.y() = (lhs.y() + rhs.y()) / 2.0f;

	return returnPoint;
}