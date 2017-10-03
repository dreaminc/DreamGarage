#include "sphere.h"

RESULT sphere::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return R_PASS;
}

unsigned int sphere::NumberVertices() {
	int numVertsPerStrip = m_numAngularDivisions + 1;
	int numStrips = m_numVerticalDivisions;
	return (numStrips) * (numVertsPerStrip);
}

unsigned int sphere::NumberIndices() {
	int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
	int numStrips = m_numVerticalDivisions;

	return (numTriangleStripVerts * numStrips);
}

sphere::sphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) :
	m_radius(radius),
	m_numAngularDivisions(numAngularDivisions),
	m_numVerticalDivisions(numVerticalDivisions)
{
	RESULT r = R_PASS;

	CR(SetSphereVertices(radius, numAngularDivisions, numVerticalDivisions, point(), c));

	// TODO: Allow for changing this - put it into a factory
	//CR(InitializeAABB());
	CR(InitializeBoundingSphere());

// Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

sphere::sphere(BoundingSphere *pBoundingSphere, bool fTriangleBased) :
	m_radius(1.0f),
	m_numAngularDivisions(MIN_SPHERE_DIVISIONS * 3),
	m_numVerticalDivisions(MIN_SPHERE_DIVISIONS * 3)
{
	RESULT r = R_PASS;

	CR(SetSphereVertices(pBoundingSphere, fTriangleBased));

	// TODO: Allow for changing this - put it into a factory
	//CR(InitializeAABB());
	CR(InitializeBoundingSphere());

	// Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT sphere::UpdateFromBoundingSphere(BoundingSphere* pBoundingSphere, bool fTriangleBased) {
	RESULT r = R_PASS;

	if (pBoundingSphere->GetRadius() != m_radius) {
		CR(SetSphereVertices(pBoundingSphere, fTriangleBased));
	}

	SetOrigin(pBoundingSphere->GetOrigin());

	// TODO: This is optional
	SetOrientation(pBoundingSphere->GetOrientation());

	//SetScale(pBoundingSphere->GetScale());

Error:
	return r;
}

// TODO: Add non triangle based for sphere
RESULT sphere::SetSphereVertices(BoundingSphere* pBoundingSphere, bool fTriangleBased) {
	RESULT r = R_PASS;

	m_radius = pBoundingSphere->GetRadius(false) * 1.05f;	// Add a buffer so we can see the reference geometry 
	SetOrigin(pBoundingSphere->GetOrigin());

	//CR(SetSphereVertices(m_radius, m_numAngularDivisions, m_numVerticalDivisions, pBoundingSphere->GetOrigin()));
	CR(SetSphereVertices(m_radius, m_numAngularDivisions, m_numVerticalDivisions));

Error:
	return r;
}

RESULT sphere::SetSphereVertices(float radius, int numAngularDivisions, int numVerticalDivisions, point ptOrigin, color c) {
	RESULT r = R_PASS;

	if (m_numAngularDivisions < MIN_SPHERE_DIVISIONS) m_numAngularDivisions = MIN_SPHERE_DIVISIONS;
	if (m_numVerticalDivisions < MIN_SPHERE_DIVISIONS) m_numVerticalDivisions = MIN_SPHERE_DIVISIONS;

	CR(Allocate());

	// Vertices 
	int numStrips = m_numVerticalDivisions;
	int numStripDivs = (m_numAngularDivisions + 1);
	int vertCount = 0;
	float thetaDiv = static_cast<float>((2.0f * M_PI) / (m_numAngularDivisions));
	float psiDiv = static_cast<float>((1.0f * M_PI) / (m_numVerticalDivisions - 1));

	for (int i = 0; i < numStrips; i++) {
		float effPsi = psiDiv * static_cast<float>(i);
		point_precision sphereY = static_cast<float>(radius * cos(effPsi));
		point_precision effRadius = static_cast<float>(radius * sin(effPsi));

		for (int j = 0; j < numStripDivs; j++) {
			float effTheta = thetaDiv * static_cast<float>(j);
			point_precision sphereX = effRadius * sin(effTheta);
			point_precision sphereZ = effRadius * cos(effTheta);

			uv_precision u = static_cast<float>(0.5f + ((atan2(sin(effTheta - M_PI), cos(effTheta - M_PI)))) / (2.0f * M_PI));
			//uv_precision u = 0.5f + ((atan2(sin(effTheta), cos(effTheta)))) / (2.0f * M_PI);
			if (j == (numStripDivs - 1))
				u += 1.0f;
			uv_precision v = static_cast<float>(0.5f - ((asin(cos(effPsi)))) / (M_PI));

			vector n = vector(sphereX, sphereY, sphereZ).Normal();
			m_pVertices[vertCount] = vertex(point(sphereX, sphereY, sphereZ), n, uvcoord(u, v));

			// TODO: Fix Sphere BTN
			vector b(cos(effTheta), 0.0f, -sin(effTheta));
			vector t = b.cross(n);

			/*if (effPsi < M_PI / 2) {
			t = b.cross(n);
			}
			else if (effPsi > M_PI / 2) {
			t = n.cross(b);
			}
			else {
			b = vector(0.0f, -1.0f, 0.0f);
			t = n.cross(b);
			}
			*/

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

	// Adjust for center point
	if (!ptOrigin.IsZero()) {
		for (unsigned int i = 0; i < NumberVertices(); i++) {
			m_pVertices[i].TranslatePoint(ptOrigin);
		}
	}

	CR(SetVertexColor(c));

Error:
	return r;
}