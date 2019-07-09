#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/rectangle.h
// 2D Rectangle primitive

// TODO: 2D (blit) flesh out

template<class T>
class rectangle {

public:
	T m_x, m_y;
	T m_width, m_height;

	rectangle() { }
	rectangle(T x, T y, T width, T height) : 
		m_x(x),
		m_y(y),
		m_width(width),
		m_height(height)
	{ 
		// empty
	}

	bool operator == (const rectangle& rhs) const {
		return (m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_width == rhs.m_width) && (m_height == rhs.m_height);
	}
	bool operator != (const rectangle& rhs) const {
		return !operator == (rhs);
	}
};

#endif // ! RECTANGLE_H_