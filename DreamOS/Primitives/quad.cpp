#include "quad.h"
#include "BoundingQuad.h"

// copy ctor
quad::quad(quad& q) :
	m_quadType(q.m_quadType),
	m_numHorizontalDivisions(q.m_numHorizontalDivisions),
	m_numVerticalDivisions(q.m_numVerticalDivisions),
	m_pTextureHeight(q.m_pTextureHeight),
	m_heightMapScale(q.m_heightMapScale)
{
	m_pVertices = q.m_pVertices;
	m_pIndices = q.m_pIndices;

	q.m_pVertices = nullptr;
	q.m_pIndices = nullptr;
}

// move ctor
quad::quad(quad&& q) :
	m_quadType(q.m_quadType),
	m_numHorizontalDivisions(q.m_numHorizontalDivisions),
	m_numVerticalDivisions(q.m_numVerticalDivisions),
	m_pTextureHeight(q.m_pTextureHeight),
	m_heightMapScale(q.m_heightMapScale)
{
	m_pVertices = q.m_pVertices;
	m_pIndices = q.m_pIndices;

	q.m_pVertices = nullptr;
	q.m_pIndices = nullptr;
}

// Square
quad::quad(float side, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_quadType(type::SQUARE),
	m_numHorizontalDivisions(numHorizontalDivisions),
	m_numVerticalDivisions(numVerticalDivisions),
	m_pTextureHeight(pTextureHeight),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(side, side, vNormal));
	
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());

	CR(InitializeBoundingQuad(GetOrigin(), side, side, vNormal));
	//TODO: CR(InitializeBoundingPlane());

	// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// Rectangle
quad::quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_quadType(type::RECTANGLE),
	m_numHorizontalDivisions(numHorizontalDivisions),
	m_numVerticalDivisions(numVerticalDivisions),
	m_pTextureHeight(pTextureHeight),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(width, height, vNormal));

	//CR(InitializeAABB());
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());
	
	CR(InitializeBoundingQuad(GetOrigin(), width, height, vNormal));
	//TODO: CR(InitializeBoundingPlane());

// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// This needs to be re-designed, too specific for 2D blits.
//quad::quad(float height, float width, point& ptCenter, uvcoord& uvBottomLeft, uvcoord& uvUpperRight, vector vNormal) :
quad::quad(float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) :
	m_quadType(type::RECTANGLE),
	m_numHorizontalDivisions(1),
	m_numVerticalDivisions(1),
	m_pTextureHeight(nullptr),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;
	
	// TODO: UV thing
	CR(SetVertices(width, height, vNormal, uvTopLeft, uvBottomRight));

	CR(InitializeBoundingQuad(GetOrigin(), width, height, vNormal));

//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

quad::quad(BoundingQuad *pBoundingQuad, bool fTriangleBased) :
	m_quadType(type::RECTANGLE),
	m_numHorizontalDivisions(1),
	m_numVerticalDivisions(1),
	m_pTextureHeight(nullptr),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(pBoundingQuad, fTriangleBased));
	
//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

quad::quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, CurveType curveType, vector vNormal) :
	m_quadType(type::RECTANGLE),
	m_numHorizontalDivisions(numHorizontalDivisions),
	m_numVerticalDivisions(numVerticalDivisions),
	m_pTextureHeight(nullptr),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE),
	m_quadCurveType(curveType)
{
	RESULT r = R_PASS;

	// TODO: UV thing
	CR(SetVertices(width, height, vNormal, uvTopLeft, uvBottomRight));
	
	CR(ApplyCurveToVertices());

	CR(InitializeOBB());

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT quad::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));
	//CR(AllocateTriangleIndexGroups(NUM_QUAD_TRIS));

	Error:
	return R_PASS;
}

inline unsigned int quad::NumberVertices() {
//return NUM_QUAD_POINTS; 

	unsigned int numVerts = (m_numVerticalDivisions + 1) * (m_numHorizontalDivisions + 1);
	return numVerts;
}

inline unsigned int quad::NumberIndices() {
	//return NUM_QUAD_TRIS * 3; 

	unsigned int numDivisions = m_numVerticalDivisions * m_numHorizontalDivisions;
	unsigned int numTris = numDivisions * 2;
	unsigned int numIndices = numTris * 3;

	return numIndices;
}

bool quad::IsBillboard() {
	return m_fBillboard; 
}

void quad::SetBillboard(bool fBillboard) {
	m_fBillboard = fBillboard; 
}

bool quad::IsScaledBillboard() {
	return m_fScaledBillboard; 
}

void quad::SetScaledBillboard(bool fScale) {
	m_fScaledBillboard = fScale; 
}

float quad::GetWidth() {
	return m_width;
}

float quad::GetHeight() {
	return m_height;
}

RESULT quad::UpdateParams(float width, float height, vector vNormal) {
	RESULT r = R_PASS;

	if (width != m_width || height != m_height || vNormal != m_vNormal) {
		m_width = width;
		m_height = height;
		m_vNormal = vNormal;

		CR(SetVertices(width, height, vNormal));
	}
	else {
		r = R_SKIPPED;
	}

Error:
	return r;
}

RESULT quad::UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad, bool fTriangleBased) {
	RESULT r = R_PASS;

	if (pBoundingQuad->GetWidth() != m_width || pBoundingQuad->GetHeight() != m_height || pBoundingQuad->GetNormal() != m_vNormal) {
		CR(SetVertices(pBoundingQuad, fTriangleBased));
	}

	SetOrigin(pBoundingQuad->GetOrigin());
	SetOrientation(pBoundingQuad->GetOrientation());
	//SetScale(pBoundingQuad->GetScale());
Error:
	return r;
}

RESULT quad::SetVertices(BoundingQuad* pBoundingQuad, bool fTriangleBased) {
	RESULT r = R_PASS;

	SetOrigin(pBoundingQuad->GetOrigin());

	m_width = pBoundingQuad->GetWidth();
	m_height = pBoundingQuad->GetHeight();
	m_vNormal = pBoundingQuad->GetNormal();

	CR(SetVertices(m_width, m_height, m_vNormal));

Error:
	return r;
}

RESULT quad::FlipUVVertical() {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {
			uv_precision uValue = m_pVertices[vertCount].GetUV().u();
			uv_precision vValue = m_pVertices[vertCount].GetUV().v();

			vValue = (1.0f - vValue);

			m_pVertices[vertCount++].SetUV(uValue, vValue);
		}
	}

	CR(SetDirty());

Error:
	return r;
}

RESULT quad::FlipUVHorizontal() {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {
			uv_precision uValue = m_pVertices[vertCount].GetUV().u();
			uv_precision vValue = m_pVertices[vertCount].GetUV().v();

			uValue = (1.0f - uValue);

			m_pVertices[vertCount++].SetUV(uValue, vValue);
		}
	}

	CR(SetDirty());

Error:
	return r;
}

vector quad::GetNormal() {
	return m_vNormal;
}

// TODO: not supporting triangle based yet
//RESULT quad::SetVertices(float width, float height, vector vNormal) {
RESULT quad::SetVertices(float width, float height, vector vNormal, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight) {
	RESULT r = R_PASS;

	CR(Allocate());

	m_width = width;
	m_height = height;
	m_vNormal = vNormal;

	float halfHeight = height / 2.0f;
	float halfWidth = width / 2.0f;

	float widthInc = width / m_numHorizontalDivisions;
	float heightInc = height / m_numVerticalDivisions;

	int vertCount = 0;
	int indexCount = 0;
	int A, B, C, D;

	// Set up indices 
	TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {

			double yValue = 0.0f;

			uv_precision uRange = uvBottomRight.u() - uvTopLeft.u();
			uv_precision vRange = uvBottomRight.v() - uvTopLeft.v();

			uv_precision uValue = uvTopLeft.u() + (((float)(i) / (float)(m_numHorizontalDivisions)) * uRange);
			uv_precision vValue = uvTopLeft.v() + (((float)(j) / (float)(m_numVerticalDivisions)) * vRange);

			if (m_pTextureHeight != nullptr) {
				yValue = m_pTextureHeight->GetValueAtUV(uValue, vValue);
				yValue *= m_heightMapScale;
			}

			m_pVertices[vertCount] = vertex(point((widthInc * i) - halfWidth, static_cast<float>(yValue), (heightInc * j) - halfHeight),
				vector::jVector(1.0f),
				uvcoord(uValue, 1.0f - vValue));

			// TODO: Calculate normal (based on geometry)

			m_pVertices[vertCount].SetTangent(vector(-1.0f, 0.0f, 0.0f));
			m_pVertices[vertCount].SetBitangent(vector(0.0f, 0.0f, -1.0f));

			vertCount++;
		}
	}

	{
		vNormal.Normalize();
		quaternion qOrientation = quaternion(vector::jVector(1.0f), vNormal);
		RotationMatrix rotMat(qOrientation);
		for (unsigned int i = 0; i < NumberVertices(); i++) {
			m_pVertices[i].m_point = rotMat * m_pVertices[i].m_point;
			m_pVertices[i].m_normal = rotMat * m_pVertices[i].m_normal;
		}
	}

	for (int i = 0; i < m_numHorizontalDivisions; i++) {
		for (int j = 0; j < m_numVerticalDivisions; j++) {
			A = (i)+((m_numHorizontalDivisions + 1) * j);
			B = (i + 1) + ((m_numHorizontalDivisions + 1) * j);

			C = (i)+((m_numHorizontalDivisions + 1) * (j + 1));
			D = (i + 1) + ((m_numHorizontalDivisions + 1) * (j + 1));

			pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
			SetTriangleNormal(A, B, C);

			pTriIndices[indexCount++] = TriangleIndexGroup(C, B, D);
			SetTriangleNormal(C, B, D);
		}
	}

Error:
	return r;
}

// TODO: Move these into a math curve lib
template <typename T>
std::vector<std::pair<T, T>> quad::GetCurveBuffer(T startVal, T endVal, int divisions, quad::CurveType curveType, T val) {
	std::vector<std::pair<T, T>> returnValues;

	float xVal = startVal;
	T range = startVal - endVal;
	T increment = range / (divisions - 1);

	for (int i = 0; i < divisions; i++) {
		T xVal = i * (increment);
		T yVal = 0.0f;

		switch (curveType) {
			case CurveType::FLAT: {
				yVal = 0.0f;
			} break;

			case CurveType::PARABOLIC: {
				yVal = val * std::pow(xVal, 2);
			} break;

			// TODO: make radius programmatic
			case CurveType::CIRCLE: {
				T radius = val;
				yVal = radius - std::sqrt(std::pow(radius, 2) - std::pow(effX, 2));
			} break;
		}

		returnValues.push_back(std::make_pair<T, T>(xVal, yVal));
	}

	return returnValues;
}

template <typename T>
std::pair<T, T> quad::GetCurveFocus(quad::CurveType curveType, T val) {
	std::pair<T, T> ptFocus; 

	switch (curveType) {
		case CurveType::FLAT: {
			ptFocus = std::make_pair<T, T>(0.0f, 0.0f);
		} break;

		case CurveType::PARABOLIC: {
			ptFocus = std::make_pair<T, T>(0.0f, (T)((1.0f)/(val * 4.0f)));
		} break;

			// TODO: make radius programmatic
		case CurveType::CIRCLE: {
			ptFocus = std::make_pair<T, T>(0.0f, val);
		} break;
	}

	return ptFocus;
}

template <typename T>
T quad::GetCurveBufferArcLength(std::vector<std::pair<T, T>> curveValues) {
	T retVal = 0;

	for (int i = 1; i < curveValues.size(); i++) {
		T xDiff = (curveValues[i].first - curveValues[i - 1].first);
		T yDiff = (curveValues[i].second - curveValues[i - 1].second);
		T incDistance = std::sqrt(std::pow(xDiff, 2.0f) + std::pow(yDiff, 2.0f));

		retVal += incDistance;
	}

	return retVal;
}

template <typename T>
T quad::GetCurveInterpolatedValue(T xVal, std::vector<std::pair<T, T>> curveValues) {
	T lastXVal = curveValues[0].first;
	T curXVal = 0.0f;

	T lastYVal = curveValues[0].second;
	T curYVal = 0.0f;

	// can't handle xVals outside of bounds
	if (xVal < lastXVal) {
		return std::make_pair<T, T>(0, 0);
	}

	// Find interpolated value
	for (int i = 0; i < curveValues.size(); i++) {
		lastXVal = curXVal;
		lastYVal = curYVal;

		curXVal = curveValues[i].first;
		curYVal = curveValues[i].second;

		if (curXVal > xVal)
			break;
	}

	// Can't handle X values outside of bounds
	if (xVal > curXVal) {
		return std::make_pair<T, T>(0, 0);
	}

	// Linear interpolation
	T xRatio = ((xVal - lastXVal) / (curXVal - lastXVal));
	T yDiff = curYVal - lastYVal;
	T interpolatedYVal = lastYVal + (yDiff * xRatio);

	return interpolatedYVal;
}

template <typename T>
RESULT NormalizePair(std::pair<T, T> &vPair) {
	
	T magnitude = std::sqrt(std::pow(vPair.first, 2) + std::pow(vPair.second, 2));

	vPair.first /= magnitude;
	vPair.second /= magnitude;

	return R_PASS;
}

template <typename T>
std::pair<T, T> quad::GetCurveNormal(T xVal, std::vector<std::pair<T, T>> curveValues, std::pair<T, T> ptFocus) {
	std::pair<T, T> vNormal;

	T interpolatedYVal = GetCurveInterpolatedValue(xVal, curveValues);

	std::pair<T, T> vNormal = std::make_pair<T, T>(ptFocus.first - xVal, ptFocus.second);

	NormalizePair<T>(vNormal);

	return vNormal;
}

// TODO: Curve on arbitrary axis 
RESULT quad::ApplyCurveToVertices() {
	RESULT r = R_PASS;

	size_t numVals = m_numHorizontalDivisions + 1;
	float effRange = ((float)(m_numHorizontalDivisions));

	float *pCurveY = nullptr;
	float *pCurveX = nullptr;
	uv_precision *pUVals = nullptr;
	vector *pVNormals = nullptr;

	CB((m_quadCurveType != CurveType::FLAT));

	pCurveY = new float[numVals];
	pCurveX = new float[numVals];
	pVNormals = new vector[numVals];
	pUVals = new uv_precision[numVals];

	memset(pCurveX, 0, sizeof(float) * numVals);
	memset(pCurveY, 0, sizeof(float) * numVals);
	memset(pVNormals, 0, sizeof(vector) * numVals);
	memset(pUVals, 0, sizeof(uv_precision) * numVals);

	//auto curveValues = GetCurveBuffer(-1.0f, 1.0f, m_numHorizontalDivisions + 1, m_quadCurveType, 1.0f);

	// Calculate curve
	for (int i = 0; i < numVals; i++) {
		// Puts effective val in [-1, 1] range
		float effVal = -1.0f + 2.0f * ((float)(i) / effRange);

		switch (m_quadCurveType) {
			case CurveType::FLAT: {
				pCurveY[i] = 0.0f;
			} break;

			case CurveType::PARABOLIC: {
				float maxHeight = 1.0f;
				pCurveX[i] = effVal * m_width;
				pCurveY[i] = effVal * effVal;
				pVNormals[i] = vector(-pCurveX[i], maxHeight - pCurveY[i], 0.0f).Normal();
			} break;

			// TODO: make radius programmatic
			case CurveType::CIRCLE: {
				float radius = m_width / 2.0f;
				
				float effX = effVal * radius;

				pCurveX[i] = effX;
				pCurveY[i] = radius - std::sqrt((radius * radius) - (effX * effX));

				//if (std::isnan(displacementAmount))
				//	displacementAmount = 0.0f;

				// Normal calculated from radius (pointing at center)
				pVNormals[i] = vector(-effX, radius - pCurveY[i], 0.0f).Normal();
			} break;
		}
	}

	// distance
	float totalDistance = 0.0f;
	for (int i = 1; i < numVals; i++) {
		float incDistance2 = std::pow((pCurveX[i] - pCurveX[i - 1]), 2.0f) + std::pow((pCurveY[i] - pCurveY[i - 1]), 2.0f);
		float incDistance = std::sqrt(incDistance2);
		totalDistance += incDistance;
	}

	// Calculate effective X values and respective U values
	float distanceAccumulator = 0.0f;
	for (int i = 1; i < numVals; i++) {
		float incDistance2 = std::pow((pCurveX[i] - pCurveX[i - 1]), 2.0f) + std::pow((pCurveY[i] - pCurveY[i - 1]), 2.0f);
		float incDistance = std::sqrt(incDistance2);

		distanceAccumulator += incDistance;

		pUVals[i] = (distanceAccumulator / totalDistance);
	}

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {
			int vertNum = (i * (m_numHorizontalDivisions + 1)) + j;

			// Displacement
			vertex *pVertex = &(m_pVertices[vertNum]);
			point ptVert = pVertex->GetPoint();

			ptVert = ptVert + (m_vNormal * pCurveY[i]);
			ptVert.x() = (pUVals[i] - 0.5f) * m_width;

			pVertex->TranslatePoint(ptVert);

			// UV
			uvcoord oldUV = pVertex->GetUV();
			pVertex->SetUV(pUVals[i], oldUV.v());

			// Calculate normal (based on geometry)
			pVertex->SetNormal(pVNormals[i]);
			pVertex->SetTangent(pVNormals[i].cross(vector::kVector(1.0f)).Normal());
			pVertex->SetBitangent(vector::kVector(1.0f));
		}
	}

Error:
	if (pCurveY != nullptr) {
		delete[] pCurveY;
		pCurveY = nullptr;
	}

	if (pCurveX != nullptr) {
		delete[] pCurveX;
		pCurveX = nullptr;
	}

	if (pUVals != nullptr) {
		delete[] pUVals;
		pUVals = nullptr;
	}

	if (pVNormals != nullptr) {
		delete[] pVNormals;
		pVNormals = nullptr;
	}

	return r;
}

// TODO: Parallelogram
// TODO: Trapezoid
// TODO: Rhombus
// TODO: Trapezium + Evaluate Points
quad::type quad::EvaluatePoints(point a, point b, point c) {
	return type::INVALID;
}