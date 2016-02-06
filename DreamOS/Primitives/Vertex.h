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
	float m_pPoint[VERTEX_DIMENSIONS];
	float m_pColor[COLOR_DIMENSIONS];
	// TODO: Normal
	// TODO: UV coordinate 

public:
	vertex() {
		for (int i = 0; i < VERTEX_DIMENSIONS; i++) m_pPoint[i] = 0.0f;
		for (int i = 0; i < COLOR_DIMENSIONS; i++) m_pColor[i] = 0.0f;
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
		return (void*)(sizeof(float) * VERTEX_DIMENSIONS);
	}

public:
	RESULT SetPoint(point p) {
		m_pPoint[V_X] = p.x();
		m_pPoint[V_Y] = p.y();
		m_pPoint[V_Z] = p.z();

		return R_PASS;
	}

	//RESULT SetPointW(float x, float y, float z, float w);
	RESULT SetPoint(float x, float y, float z) {
		m_pPoint[V_X] = x;
		m_pPoint[V_Y] = y;
		m_pPoint[V_Z] = z;
		
		return R_PASS;
	}
	
	//RESULT SetColorA(float x, float y, float z);
	RESULT SetColor(float r, float g, float b) {
		m_pColor[C_R] = r;
		m_pColor[C_G] = g;
		m_pColor[C_B] = b;

		return R_PASS;
	}

	RESULT SetColor(color c) {
		m_pColor[C_R] = c.r();
		m_pColor[C_G] = c.g();
		m_pColor[C_B] = c.b();

		return R_PASS;
	}

	RESULT SetVertex(vertex v) {
		for (int i = 0; i < VERTEX_DIMENSIONS; i++)
			m_pPoint[i] = v.m_pPoint[i];

		for (int i = 0; i < COLOR_DIMENSIONS; i++)
			m_pColor[i] = v.m_pColor[i];

		return R_PASS;
	}
};

#endif // ! VERTEX_H_
