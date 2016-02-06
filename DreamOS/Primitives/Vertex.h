#ifndef VERTEX_H_
#define VERTEX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/vertex.h
// Vertex primitive 

// TODO: Switch to using point and color primitives as needed
// TODO: Manage types (using float for now - templates)

#define VERTEX_DIMENSIONS 3
#define V_X 0 
#define V_Y 1 
#define V_Z 2 
#define V_W 3 

#define COLOR_DIMENSIONS 3
#define C_R 0 
#define C_G 1 
#define C_B 2 
#define C_A 3 

#include "matrix.h"
#include "vector.h"
#include "point.h"
#include "color.h"

// TODO: Port to LinAlgLib (using point etc)
class vertex {
public:
	//float m_pPoint[VERTEX_DIMENSIONS];
	//float m_pColor[COLOR_DIMENSIONS];
	point m_point;
	color m_color;

	// TODO: Normal
	// TODO: UV coordinate 

public:
	vertex() {
		m_point.clear();
		m_color.clear();
	}

	vertex(point p) {
		SetPoint(p);
		SetColor(color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	vertex(point p, color c) {
		SetPoint(p);
		SetColor(c);
	}

	~vertex() {
		// Empty stub for now
	}

	static void *GetVertexOffset() {
		return NULL;
	}

	static void* GetColorOffset() {
		return (void*)(sizeof(point));
	}

	static void* GetNormalOffset() {
		return (void*)(sizeof(point) + sizeof(color));
	}

	static void* GetUVOffset() {
		return (void*)(sizeof(point) + sizeof(color) + sizeof(vector));
	}

	inline static int GetPointDimensions() {
		return point::rows();
	}

	inline static int GetColorDimensions() {
		return color::rows();
	}

	inline point GetPoint() {
		return m_point;
	}

	inline color GetColor() {
		return m_color;
	}

public:
	RESULT SetPoint(point p) {
		/*m_pPoint[V_X] = p.x();
		m_pPoint[V_Y] = p.y();
		m_pPoint[V_Z] = p.z();*/

		m_point = p;

		return R_PASS;
	}

	//RESULT SetPointW(float x, float y, float z, float w);
	RESULT SetPoint(point_precision x, point_precision y, point_precision z) {
		/*m_pPoint[V_X] = x;
		m_pPoint[V_Y] = y;
		m_pPoint[V_Z] = z;*/

		m_point = point(x, y, z);
		
		return R_PASS;
	}
	
	//RESULT SetColorA(float x, float y, float z);
	RESULT SetColor(color_precision r, color_precision g, color_precision b) {
		/*m_pColor[C_R] = r;
		m_pColor[C_G] = g;
		m_pColor[C_B] = b;*/

		m_color = color(r, g, b, 1.0f);

		return R_PASS;
	}

	RESULT SetColor(color_precision r, color_precision g, color_precision b, color_precision a) {
		/*m_pColor[C_R] = r;
		m_pColor[C_G] = g;
		m_pColor[C_B] = b;*/

		m_color = color(r, g, b, a);

		return R_PASS;
	}

	RESULT SetColor(color c) {
		/*m_pColor[C_R] = c.r();
		m_pColor[C_G] = c.g();
		m_pColor[C_B] = c.b();*/

		m_color = c;

		return R_PASS;
	}

	RESULT SetVertex(vertex v) {
		/*
		for (int i = 0; i < VERTEX_DIMENSIONS; i++)
			m_pPoint[i] = v.m_pPoint[i];

		for (int i = 0; i < COLOR_DIMENSIONS; i++)
			m_pColor[i] = v.m_pColor[i];
		*/

		m_point = v.GetPoint();
		m_color = v.GetColor();

		return R_PASS;
	}

	// TODO: Operator overloading
};

#endif // ! VERTEX_H_
