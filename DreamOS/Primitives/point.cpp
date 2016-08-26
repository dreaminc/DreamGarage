#include "point.h"
#include "vector.h"

vector operator-(point &lhs, point &rhs) {
	vector rtv;

	rtv.x(rhs.x() - lhs.x());
	rtv.y(rhs.y() - lhs.y());
	rtv.z(rhs.z() - lhs.z());

	return rtv;
}

vector point::operator-(const point& rhs) const {
	vector retVector = (*this) - rhs;
	return (retVector);
}

point operator+(point &lhs, point &rhs) {
	point rtp;

	rtp.x(rhs.x() + lhs.x());
	rtp.y(rhs.y() + lhs.y());
	rtp.z(rhs.z() + lhs.z());

	return rtp;
}

point point::operator+(const point& rhs) const {
	point retPoint = (*this) + rhs;
	return (retPoint);
}


point operator+(point &lhs, vector &rhs) {
	point rtp;

	rtp.x(lhs.x() + rhs.x());
	rtp.y(lhs.y() + rhs.y());
	rtp.z(lhs.z() + rhs.z());

	return rtp;
}

point point::operator+(const vector& rhs) const {
	point retPoint = (*this) + rhs;
	return (retPoint);
}

point operator-(point &lhs, vector &rhs) {
	point rtp;

	rtp.x(lhs.x() - rhs.x());
	rtp.y(lhs.y() - rhs.y());
	rtp.z(lhs.z() - rhs.z());

	return rtp;
}

point point::operator-(const vector& rhs) const {
	point retPoint = (*this) - rhs;
	return (retPoint);
}