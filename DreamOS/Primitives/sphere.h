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

	inline int NumberVertices() override {
		int numVertsPerStrip = m_numAngularDivisions + 1;
		int numStrips = m_numVerticalDivisions;
		return (numStrips) * (numVertsPerStrip);
	}

	inline int NumberIndices() override {
		int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
		int numStrips = m_numVerticalDivisions;

		return (numTriangleStripVerts * numStrips);
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
		int numStrips = m_numVerticalDivisions;
		int numStripDivs = (m_numAngularDivisions + 1);
		int vertCount = 0;
		float thetaDiv = ((2.0f * M_PI) / static_cast<float>(m_numAngularDivisions - 1));
		float psiDiv = ((1.0f * M_PI) / static_cast<float>(m_numVerticalDivisions - 1));

		for (int i = 0; i < numStrips; i++) {
			float effPsi = psiDiv * static_cast<float>(i);
			point_precision sphereY = radius * cos(effPsi);
			point_precision effRadius = radius * sin(effPsi);

			for (int j = 0; j < numStripDivs; j++) {
				float effTheta = thetaDiv * static_cast<float>(j);
				point_precision sphereX = effRadius * sin(effTheta);
				point_precision sphereZ = effRadius * cos(effTheta);

				uv_precision u = 0.5f + ((atan2(sin(effTheta - M_PI), cos(effTheta - M_PI)))) / (2.0f * M_PI);
				//uv_precision u = 0.5f + ((atan2(sin(effTheta), cos(effTheta)))) / (2.0f * M_PI);
				if (j == (numStripDivs - 1))
					u += 1.0f;
				uv_precision v = 0.5f - ((asin(cos(effPsi)))) / (M_PI);

				vector n = vector(sphereX, sphereY, sphereZ).Normal();
				m_pVertices[vertCount] = vertex(point(sphereX, sphereY, sphereZ), n, uvcoord(u, v));

				// TODO: Fix Sphere BTN
				vector b(n.x(), -n.y(), n.z());
				vector t;
				if (effPsi < M_PI / 2) {
					t = b.cross(n);
				}
				else if (effPsi > M_PI / 2) {
					t = n.cross(b);
				}
				else {
					b = vector(0.0f, -1.0f, 0.0f);
					t = n.cross(b);
				}

				//t = RotationMatrix(RotationMatrix::X_AXIS, effPsi) * RotationMatrix(RotationMatrix::Y_AXIS, effTheta) * vector::iVector();
				//b = n.cross(t);
				
				m_pVertices[vertCount].SetTangentBitangent(t.Normal(), b.Normal());
				
				vertCount++;
			}
		}

		// Indices
		int indexCount = 0;
		int indexStripTop, indexStripBottom;

		for (int i = 0; i < numStrips; i++) {
			
			indexStripTop = (i * numStripDivs);
			indexStripBottom = ((i + 1) * numStripDivs);

			for (int j = 0; j < (numStripDivs) * 2; j++) {
				if (j % 2 == 0)
					m_pIndices[indexCount] = indexStripTop++;
				else
					m_pIndices[indexCount] = indexStripBottom++;

				indexCount++;
			}
		}

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