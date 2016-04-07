#ifndef SPHERE_H_
#define SPHERE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/sphere.h
// Sphere Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#include "DimObj.h"

#include "Vertex.h"
#include "point.h"
#include "color.h"

#define MIN_SPHERE_DIVISIONS 3

class sphere : public DimObj {
public:
	int m_numAngularDivisions;
	int m_numVerticalDivisions;

public:
	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(NumberVertices()));
		CR(AllocateIndices(NumberIndices()));

	Error:
		return R_PASS;
	}

	inline int NumberVertices() {
		return (m_numVerticalDivisions - 2) * m_numAngularDivisions + 2;
	}

	inline int NumberIndices() {

		int numFanVerts = m_numAngularDivisions + 2;
		int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
		int numStrips = m_numVerticalDivisions - 3;

		int numIndices = (numFanVerts * 2) + (numTriangleStripVerts * numStrips);

		return numIndices;
	}

	sphere(double radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS) :
		m_radius(radius),
		m_numAngularDivisions(numAngularDivisions),
		m_numVerticalDivisions(numVerticalDivisions)
	{
		RESULT r = R_PASS;
		
		if (m_numAngularDivisions < MIN_SPHERE_DIVISIONS) m_numAngularDivisions = MIN_SPHERE_DIVISIONS;
		if (m_numVerticalDivisions < MIN_SPHERE_DIVISIONS) m_numVerticalDivisions = MIN_SPHERE_DIVISIONS;
		
		CR(Allocate());

		// Vertices 

		// Top vertex
		m_pVertices[0] = vertex(point(0.0f, radius, 0.0f), vector(0.0f, 1.0f, 0.0f), uvcoord(0.5f, 0.0f));

		int vertCount = 1;
		float thetaDiv = ((2.0f * M_PI) / (float)m_numAngularDivisions);
		float psiDiv = ((1.0f * M_PI) / (float)(m_numVerticalDivisions));

		for (int i = 1; i < (m_numVerticalDivisions - 1); i++) {
			float effPsi = psiDiv * (float)(i);
			point_precision sphereY = radius * cos(effPsi);
			point_precision effRadius = radius * sin(effPsi);

			for (int j = 0; j < m_numAngularDivisions; j++) {
				float effTheta = thetaDiv * (float)(j);
				point_precision sphereX = effRadius * sin(effTheta);
				point_precision sphereZ = effRadius * cos(effTheta);

				uv_precision u = 0.5f + ((atan2(cos(effTheta), sin(effTheta)))) / (2.0f * M_PI);
				uv_precision v = 0.5f - ((asin(cos(effPsi)))) / (M_PI);

				m_pVertices[vertCount++] = vertex(point(sphereX, sphereY, sphereZ), vector(sphereX, sphereY, sphereZ).Normal(), uvcoord(u, v));
			}
		}

		// Bottom vertex
		m_pVertices[NumberVertices() - 1] = vertex(point(0.0f, -radius, 0.0f), vector(0.0f, -1.0f, 0.0f), uvcoord(0.5f, 1.0f));

		// Indices
		// Top Triangle Fan
		int indexCount = 0;

		for (int i = 0; i < m_numAngularDivisions + 1; i++)
			m_pIndices[indexCount++] = i;
		m_pIndices[indexCount++] = 1;

		// Strips
		int numStrips = m_numVerticalDivisions - 3;
		int indexStripTop, indexStripBottom, bottomCount, topCount;

		for (int i = 0; i < numStrips; i++) {
			indexStripTop = 1 + (i * (m_numAngularDivisions));
			indexStripBottom = 1 + ((i + 1) * (m_numAngularDivisions));
			topCount = indexStripTop;
			bottomCount = indexStripBottom;

			for (int j = 0; j < m_numAngularDivisions * 2; j++) {
				if (j % 2 == 0)
					m_pIndices[indexCount++] = topCount++;
				else
					m_pIndices[indexCount++] = bottomCount++;
			}

			m_pIndices[indexCount++] = indexStripTop;
			m_pIndices[indexCount++] = indexStripBottom;
		}

		// Bottom Triangle Fan
		m_pIndices[indexCount++] = NumberVertices() - 1;
		for (int i = 0; i < m_numAngularDivisions; i++)
			m_pIndices[indexCount++] = NumberVertices() - 2 - i;
		m_pIndices[indexCount++] = NumberVertices() - 2;

		// Tangent / Bitangent
		// Top strip 

		Validate();
		return;
	Error:
		Invalidate();
		return;
	}

private:
	float m_radius;
};

#endif // !SPHERE_H_