#include "point.h"

vector operator-(point &lhs, point &rhs) {
	vector rtv;

	rtv.x(rhs.x() - lhs.x());
	rtv.y(rhs.y() - lhs.y());
	rtv.z(rhs.z() - lhs.z());

	return rtv;
}
