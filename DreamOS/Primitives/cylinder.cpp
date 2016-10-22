#include "cylinder.h"

RESULT cylinder::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}

unsigned int cylinder::NumberIndices() {
	int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
	int numStrips = m_numVerticalDivisions + 2;
	return (numTriangleStripVerts * numStrips);
}

unsigned int cylinder::NumberVertices() {
	int numVertsPerStripLine = m_numAngularDivisions + 1;
	int numStripLines = (m_numVerticalDivisions + 1) + 4;
	return (numStripLines) * (numVertsPerStripLine);
}

cylinder::cylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) :
	m_radius(radius),
	m_height(height),
	m_numAngularDivisions(numAngularDivisions),
	m_numVerticalDivisions(numVerticalDivisions)
{
	RESULT r = R_PASS;

	CR(SetCylinderVertices(radius, height, numAngularDivisions, numVerticalDivisions));

	// TODO: Allow for changing this - put it into a factory
	CR(InitializeAABB());

// Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT cylinder::SetCylinderVertices(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	RESULT r = R_PASS;

	if (m_numAngularDivisions < MIN_CYLINDER_DIVISIONS) m_numAngularDivisions = MIN_CYLINDER_DIVISIONS;
	//if (m_numVerticalDivisions < MIN_CYLINDER_DIVISIONS) m_numVerticalDivisions = MIN_CYLINDER_DIVISIONS;
	if (m_numVerticalDivisions < 1) m_numVerticalDivisions = 1;

	CR(Allocate());

	int numStrips = m_numVerticalDivisions + 3;
	int numStripDivs = (m_numAngularDivisions + 1);
	int vertCount = 0;

	float thetaDiv = static_cast<float>((2.0f * M_PI) / (m_numAngularDivisions));
	float heightDiv = static_cast<float>(height / m_numVerticalDivisions);

	// Bottom strip
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numStripDivs; j++) {
			point_precision sphereY = 0.0f;

			float effTheta = thetaDiv * static_cast<float>(j);
			point_precision sphereX = radius * sin(effTheta);
			point_precision sphereZ = radius * cos(effTheta);

			uv_precision u = static_cast<float>(0.5f + ((atan2(sin(effTheta - M_PI), cos(effTheta - M_PI)))) / (2.0f * M_PI));
			if (j == (numStripDivs - 1))
				u += 1.0f;
			uv_precision v = 0.0f;

			vector n = vector(0.0f, -1.0f, 0.0f).Normal();

			if (i == 0) {
				sphereX = 0.0f;
				sphereZ = 0.0f;
			}

			m_pVertices[vertCount] = vertex(point(sphereX, sphereY, sphereZ), n, uvcoord(u, v));

			// TODO: check cyl BTN
			vector b(cos(effTheta), 0.0f, -sin(effTheta));
			vector t = b.cross(n);

			m_pVertices[vertCount].SetTangentBitangent(t.Normal(), b.Normal());

			vertCount++;
		}
	}

	for (int i = 0; i <= m_numVerticalDivisions; i++) {
		// build from the ground up
		point_precision sphereY = height * (static_cast<float>(i) / (m_numVerticalDivisions));

		for (int j = 0; j < numStripDivs; j++) {
			//float effTheta = thetaDiv * static_cast<float>(numStripDivs - j - 1);
			float effTheta = thetaDiv * static_cast<float>(j);
			point_precision sphereX = radius * sin(effTheta);
			point_precision sphereZ = radius * cos(effTheta);

			uv_precision u = static_cast<float>(0.5f + ((atan2(sin(effTheta - M_PI), cos(effTheta - M_PI)))) / (2.0f * M_PI));
			if (j == (numStripDivs - 1))
				u += 1.0f;
			uv_precision v = static_cast<float>(static_cast<float>(i) / m_numVerticalDivisions);

			vector n = vector(sphereX, 0.0f, sphereZ).Normal();

			m_pVertices[vertCount] = vertex(point(sphereX, sphereY, sphereZ), n, uvcoord(u, v));

			// TODO: check cyl BTN
			vector b(cos(effTheta), 0.0f, -sin(effTheta));
			vector t = b.cross(n);

			m_pVertices[vertCount].SetTangentBitangent(t.Normal(), b.Normal());

			vertCount++;
		}
	}

	// Top strip
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numStripDivs; j++) {
			point_precision sphereY = height;

			float effTheta = thetaDiv * static_cast<float>(j);
			point_precision sphereX = radius * sin(effTheta);
			point_precision sphereZ = radius * cos(effTheta);

			uv_precision u = static_cast<float>(0.5f + ((atan2(sin(effTheta - M_PI), cos(effTheta - M_PI)))) / (2.0f * M_PI));
			if (j == (numStripDivs - 1))
				u += 1.0f;
			uv_precision v = 1.0f;

			vector n = vector(0.0f, 1.0f, 0.0f).Normal();

			if (i == 1) {
				sphereX = 0.0f;
				sphereZ = 0.0f;
			}

			m_pVertices[vertCount] = vertex(point(sphereX, sphereY, sphereZ), n, uvcoord(u, v));

			// TODO: check cyl BTN
			vector b(cos(effTheta), 0.0f, -sin(effTheta));
			vector t = b.cross(n);

			m_pVertices[vertCount].SetTangentBitangent(t.Normal(), b.Normal());

			vertCount++;
		}
	}

	// Indices
	int indexCount = 0;
	int indexStripTop, indexStripBottom;

	// Bottom Strip
	indexStripTop = numStripDivs;
	indexStripBottom = 0;

	for (int j = 0; j < (numStripDivs) * 2; j++) {
		if (j % 2 == 0)
			m_pIndices[indexCount] = indexStripTop++;
		else
			m_pIndices[indexCount] = indexStripBottom++;

		indexCount++;
	}

	for (int i = 2; i < (m_numVerticalDivisions + 2); i++) {

		indexStripBottom = (i * numStripDivs);
		indexStripTop = ((i + 1) * numStripDivs);

		for (int j = 0; j < (numStripDivs) * 2; j++) {
			if (j % 2 == 0)
				m_pIndices[indexCount] = indexStripTop++;
			else
				m_pIndices[indexCount] = indexStripBottom++;

			indexCount++;
		}
	}

	// Top Strip
	indexStripBottom = (m_numVerticalDivisions + 3) * numStripDivs;
	indexStripTop = (m_numVerticalDivisions + 4) * numStripDivs;

	for (int j = 0; j < (numStripDivs) * 2; j++) {
		if (j % 2 == 0)
			m_pIndices[indexCount] = indexStripTop++;
		else
			m_pIndices[indexCount] = indexStripBottom++;

		indexCount++;
	}

	//CR(SetColor(c));

Error:
	return r;
}