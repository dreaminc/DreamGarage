#include "quad.h"
#include "BoundingQuad.h"
#include "plane.h"

// copy ctor
quad::quad(quad& q) :
	m_params(q.m_params)
{
	m_pVertices = q.m_pVertices;
	m_pIndices = q.m_pIndices;

	q.m_pVertices = nullptr;
	q.m_pIndices = nullptr;
}

// move ctor
quad::quad(quad&& q) :
	m_params(q.m_params)
{
	m_pVertices = q.m_pVertices;
	m_pIndices = q.m_pIndices;

	q.m_pVertices = nullptr;
	q.m_pIndices = nullptr;
}

// Square
quad::quad(float side, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_params(side, side, numHorizontalDivisions, numVerticalDivisions, vNormal)
{
	RESULT r = R_PASS;

	m_params.quadType = type::SQUARE;	
	m_params.pTextureHeight = pTextureHeight;
	m_params.heightMapScale = DEFAULT_HEIGHT_MAP_SCALE;

	CR(SetVertices(side, side, vNormal));
	
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());

	CR(InitializeBoundingQuad(GetOrigin(), side, side, vNormal));
	//TODO: CR(InitializeBoundingPlane());

 Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// Rectangle
quad::quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_params(width, height, numHorizontalDivisions, numVerticalDivisions, vNormal)
{
	RESULT r = R_PASS;

	m_params.quadType = quad::type::RECTANGLE;
	m_params.pTextureHeight = pTextureHeight;
	m_params.heightMapScale = DEFAULT_HEIGHT_MAP_SCALE;

	CR(SetVertices(width, height, vNormal));

	//CR(InitializeAABB());
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());
	
	CR(InitializeBoundingQuad(GetOrigin(), width, height, vNormal));
	//TODO: CR(InitializeBoundingPlane());

Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// This needs to be re-designed, too specific for 2D blits.
//quad::quad(float height, float width, point& ptCenter, uvcoord& uvBottomLeft, uvcoord& uvUpperRight, vector vNormal) :
quad::quad(float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) :
	m_params(width, height, 1, 1, vNormal)
{
	RESULT r = R_PASS;
	
	m_params.quadType = quad::type::RECTANGLE;
	m_params.heightMapScale = DEFAULT_HEIGHT_MAP_SCALE;

	// TODO: UV thing
	CR(SetVertices(width, height, vNormal, uvTopLeft, uvBottomRight));

	CR(InitializeBoundingQuad(GetOrigin(), width, height, vNormal));

Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

quad::quad(BoundingQuad *pBoundingQuad, bool fTriangleBased) :
	m_params()
{
	RESULT r = R_PASS;

	m_params.quadType = quad::type::RECTANGLE;
	m_params.heightMapScale = DEFAULT_HEIGHT_MAP_SCALE;

	CR(SetVertices(pBoundingQuad, fTriangleBased));
	
Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

quad::quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, CurveType curveType, vector vNormal) :
	m_params(width, height, numHorizontalDivisions, numVerticalDivisions, vNormal)

{
	RESULT r = R_PASS;

	m_params.quadType = quad::type::RECTANGLE;
	m_params.heightMapScale = DEFAULT_HEIGHT_MAP_SCALE;
	m_params.quadCurveType = curveType;

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

	unsigned int numVerts = (m_params.numVerticalDivisions + 1) * (m_params.numHorizontalDivisions + 1);
	return numVerts;
}

inline unsigned int quad::NumberIndices() {
	//return NUM_QUAD_TRIS * 3; 

	unsigned int numDivisions = m_params.numVerticalDivisions * m_params.numHorizontalDivisions;
	unsigned int numTris = numDivisions * 2;
	unsigned int numIndices = numTris * 3;

	return numIndices;
}

bool quad::IsBillboard() {
	return m_params.fBillboard;
}

void quad::SetBillboard(bool fBillboard) {
	m_params.fBillboard = fBillboard;
}

bool quad::IsScaledBillboard() {
	return m_params.fScaledBillboard;
}

void quad::SetScaledBillboard(bool fScale) {
	m_params.fScaledBillboard = fScale;
}

float quad::GetWidth() {
	return m_params.width;
}

float quad::GetHeight() {
	return m_params.height;
}

// Note: Always in absolute space (vs composite)
plane quad::GetPlane() {
	plane retPlane(GetOrigin(true), GetNormal(true));

	return retPlane;
}

RESULT quad::UpdateParams(float width, float height, vector vNormal) {
	RESULT r = R_PASS;

	if (width != m_params.width || height != m_params.height || vNormal != m_params.vNormal) {
		m_params.width = width;
		m_params.height = height;
		m_params.vNormal = vNormal;

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

	if (pBoundingQuad->GetWidth(false) != m_params.width || 
		pBoundingQuad->GetHeight(false) != m_params.height || 
		pBoundingQuad->GetNormal() != m_params.vNormal) 
	{
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

	m_params.width = pBoundingQuad->GetWidth(false);
	m_params.height = pBoundingQuad->GetHeight(false);
	m_params.vNormal = pBoundingQuad->GetNormal();

	CR(SetVertices(m_params.width, m_params.height, m_params.vNormal));

Error:
	return r;
}

RESULT quad::FlipUVVertical() {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_params.numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_params.numVerticalDivisions + 1; j++) {
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

	for (int i = 0; i < m_params.numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_params.numVerticalDivisions + 1; j++) {
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

RESULT quad::SetUVValues(float top, float left, float bottom, float right) {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_params.numHorizontalDivisions + 1; i++) {
		uv_precision uValue = left + (i * (right - left) / m_params.numHorizontalDivisions);

		for (int j = 0; j < m_params.numVerticalDivisions + 1; j++) {
			uv_precision vValue = top + (j * (bottom - top) / m_params.numVerticalDivisions);

			m_pVertices[vertCount++].SetUV(uValue, vValue);
		}
	}

	CR(SetDirty());

Error:
	return r;
}

RESULT quad::SetDiffuseTexture(texture* pTexture) {
	RESULT r = R_PASS;

	CN(pTexture);
	
	// if the new texture is flipped, and quad is not flipped
	if (pTexture->IsUVVerticalFlipped() && !m_params.fTextureUVFlipVertical) {
		FlipUVVertical();
		m_params.fTextureUVFlipVertical = true;
	}
	// if the new texture is not flipped and quad is flipped
	else if (!pTexture->IsUVVerticalFlipped() && m_params.fTextureUVFlipVertical) {
		FlipUVVertical();
		m_params.fTextureUVFlipVertical = false;
	}

	m_pTextureDiffuse = pTexture;

Error:
	return r;
}

vector quad::GetNormal(bool fAbsolute) {
	vector vNormal = m_params.vNormal;

	if (fAbsolute) {
		//vNormal = GetModelMatrix() * vNormal;
		//vNormal.RotateByQuaternion(GetOrientation(true));
		vNormal = RotationMatrix(GetOrientation(true)) * vNormal;
	}

	return vNormal;
}

// TODO: not supporting triangle based yet
//RESULT quad::SetVertices(float width, float height, vector vNormal) {
RESULT quad::SetVertices(float width, float height, vector vNormal, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight) {
	RESULT r = R_PASS;

	CR(Allocate());

	m_params.width = width;
	m_params.height = height;
	m_params.vNormal = vNormal;

	//m_heightMapScale = sqrt(m_width * m_width + m_height * m_height);

	m_params.heightMapScale = 2.0f;

	float halfHeight = height / 2.0f;
	float halfWidth = width / 2.0f;

	float widthInc = width / m_params.numHorizontalDivisions;
	float heightInc = height / m_params.numVerticalDivisions;

	int vertCount = 0;
	int indexCount = 0;
	int A, B, C, D;

	// Set up indices 
	TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

	for (int i = 0; i < m_params.numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_params.numVerticalDivisions + 1; j++) {

			double yValue = 0.0f;

			uv_precision uRange = uvBottomRight.u() - uvTopLeft.u();
			uv_precision vRange = uvBottomRight.v() - uvTopLeft.v();

			uv_precision uValue = uvTopLeft.u() + (((float)(i) / (float)(m_params.numHorizontalDivisions)) * uRange);
			uv_precision vValue = uvTopLeft.v() + (((float)(j) / (float)(m_params.numVerticalDivisions)) * vRange);

			if (m_params.pTextureHeight != nullptr) {
				yValue = m_params.pTextureHeight->GetAverageValueAtUV(uValue, 1.0f - vValue);
				yValue *= m_params.heightMapScale;
			}

			m_pVertices[vertCount] = vertex(point((widthInc * i) - halfWidth, static_cast<float>(yValue), (heightInc * j) - halfHeight),
				vector::jVector(1.0f),
				uvcoord(uValue, 1.0f - vValue));

			// TODO: Calculate normal (based on geometry)

			m_pVertices[vertCount].SetTangent(vector(1.0f, 0.0f, 0.0f));
			m_pVertices[vertCount].SetBitangent(vector(0.0f, 0.0f, -1.0f));

			//m_pVertices[vertCount].SetTangent(vector(0.0f, 0.0f, 1.0f));
			//m_pVertices[vertCount].SetBitangent(vector(1.0f, 0.0f, 0.0f));

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

	for (int i = 0; i < m_params.numHorizontalDivisions; i++) {
		for (int j = 0; j < m_params.numVerticalDivisions; j++) {
			A = (i) + ((m_params.numHorizontalDivisions + 1) * j);
			B = (i + 1) + ((m_params.numHorizontalDivisions + 1) * j);

			C = (i) + ((m_params.numHorizontalDivisions + 1) * (j + 1));
			D = (i + 1) + ((m_params.numHorizontalDivisions + 1) * (j + 1));

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
	T range = (endVal - startVal);
	T increment = range / (divisions - 1);

	for (int i = 0; i < divisions; i++) {
		T xVal = startVal + (i * (increment));
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
				yVal = radius - std::sqrt(std::pow(radius, 2) - std::pow(xVal, 2));
			} break;
		}

		returnValues.push_back(std::make_pair(xVal, yVal));
	}

	return returnValues;
}

template <typename T>
std::pair<T, T> quad::GetCurveFocus(quad::CurveType curveType, T val) {
	std::pair<T, T> ptFocus; 

	switch (curveType) {
		case CurveType::FLAT: {
			ptFocus = std::make_pair(0.0f, 0.0f);
		} break;

		case CurveType::PARABOLIC: {
			ptFocus = std::make_pair(0.0f, (T)((1.0f)/(val * 4.0f)));
		} break;

			// TODO: make radius programmatic
		case CurveType::CIRCLE: {
			ptFocus = std::make_pair(0.0f, val);
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
T quad::GetCurveArcLength(T startVal, T endVal, int divisions, quad::CurveType curveType, T val) {
	return GetCurveBufferArcLength(GetCurveBuffer(startVal, endVal, divisions, curveType, val));
}

template <typename T>
std::pair<T, T> quad::GetStartEndForCurveLengthWithMidpoint(T length, T midpoint, int divisions, quad::CurveType curveType, T val) {
	T startVal = midpoint - length/2.0f;
	T endVal = midpoint + length / 2.0f;
	
	T leftVal = midpoint;
	T rightVal = midpoint;

	T lastLeftVal = midpoint;
	T lastRightVal = midpoint;

	T leftCurveVal = 0.0f;
	T lastLeftCurveVal = 0.0f;
	T rightCurveVal = 0.0f;
	T lastRightCurveVal = 0.0f;

	T leftDistance = 0.0f;
	T rightDistance = 0.0f;

	T increment = length / divisions;

	auto curveBuffer = GetCurveBuffer(startVal, endVal, divisions, curveType, val);
	T accumulator = 0.0f;

	// Start at midpoint, and calculate the distance going one step at a time from there given divisions
	while (accumulator <= length) {
		lastLeftVal = leftVal;
		lastRightVal = rightVal;

		leftVal -= increment;
		rightVal += increment;

		// Add left distance
		lastLeftCurveVal = GetCurveInterpolatedValue(lastLeftVal, curveBuffer);
		leftCurveVal = GetCurveInterpolatedValue(leftVal, curveBuffer);
		T leftCurveDiff = leftCurveVal - lastLeftCurveVal;
		leftDistance = std::sqrt(std::pow(leftCurveDiff, 2.0f) + std::pow(increment, 2.0f));
		accumulator += leftDistance;

		// Add right distance
		lastRightCurveVal = GetCurveInterpolatedValue(lastRightVal, curveBuffer);
		rightCurveVal = GetCurveInterpolatedValue(rightVal, curveBuffer);
		T rightCurveDiff = rightCurveVal - lastRightCurveVal;
		rightDistance = std::sqrt(std::pow(rightCurveDiff, 2.0f) + std::pow(increment, 2.0f));
		accumulator += leftDistance;
	}

	// Fix error 
	T valueError = (accumulator - length);
	T halfValueError = valueError / 2.0f;
	
	// This is not perfect, but attempts to interpolate over the last segment
	T leftErrAdj = (increment * (leftDistance - halfValueError)) / leftDistance;
	T rightErrAdj = (increment * (rightDistance - halfValueError)) / rightDistance;

	startVal = lastLeftVal - leftErrAdj;
	endVal = lastRightVal + rightErrAdj;

	return std::make_pair(startVal, endVal);
}

template <typename T>
T quad::GetCurveInterpolatedValue(T xVal, std::vector<std::pair<T, T>> curveValues) {
	T lastXVal = curveValues[0].first;
	T curXVal = 0.0f;

	T lastYVal = curveValues[0].second;
	T curYVal = 0.0f;

	// can't handle xVals outside of bounds
	if (xVal < lastXVal) {
		return 0.0f;
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
		return 0.0f;
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

	T interpolatedYVal = GetCurveInterpolatedValue(xVal, curveValues);

	std::pair<T, T> vNormal = std::make_pair<T, T>(ptFocus.first - xVal, ptFocus.second - interpolatedYVal);
	NormalizePair<T>(vNormal);

	return vNormal;
}

// TODO: Curve on arbitrary axis 
RESULT quad::ApplyCurveToVertices() {
	RESULT r = R_PASS;

	size_t divisions = m_params.numHorizontalDivisions + 1;
	float effRange = ((float)(m_params.numHorizontalDivisions));

	float val = m_params.width / 2.0f;

	float startVal = 0.0f;
	float endVal = 0.0f;

	auto pairStartEnd = GetStartEndForCurveLengthWithMidpoint(m_params.width, 0.0f, (int)(divisions), m_params.quadCurveType, val);
	startVal = pairStartEnd.first;
	endVal = pairStartEnd.second;

	float curveArcLength = GetCurveArcLength<float>(startVal, endVal, (int)(divisions), m_params.quadCurveType, val);

	std::vector<std::pair<float, float>> curveValues = GetCurveBuffer<float>(startVal, endVal, (int)(divisions), m_params.quadCurveType, val);
	curveArcLength = GetCurveBufferArcLength<float>(curveValues);

	auto ptFocus = GetCurveFocus(m_params.quadCurveType, val);

	CB((m_params.quadCurveType != CurveType::FLAT));

	for (int i = 0; i < m_params.numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_params.numVerticalDivisions + 1; j++) {
			int vertNum = (i * (m_params.numHorizontalDivisions + 1)) + j;

			// Displacement
			vertex *pVertex = &(m_pVertices[vertNum]);
			
			point ptVert = pVertex->GetPoint();
			ptVert = ptVert + (m_params.vNormal * curveValues[i].second);
			ptVert.x() = curveValues[i].first;

			pVertex->SetPoint(ptVert);

			// UV
			// UV is automatically scaled since the X values are moved around
			//uvcoord uv = pVertex->GetUV();
			//uv.u() = 
			//pVertex->SetUV(pUVals[i], uv.v());

			// Calculate normal (based on geometry)
			auto pairNormal = GetCurveNormal(curveValues[i].first, curveValues, ptFocus);
			vector vNormal = vector(pairNormal.first, pairNormal.second, 0.0f);

			pVertex->SetNormal(vNormal.Normal());
			pVertex->SetTangent(vNormal.cross(vector::kVector(1.0f)).Normal());
			pVertex->SetBitangent(vector::kVector(1.0f));
		}
	}

Error:
	return r;
}

// TODO: Parallelogram
// TODO: Trapezoid
// TODO: Rhombus
// TODO: Trapezium + Evaluate Points
quad::type quad::EvaluatePoints(point a, point b, point c) {
	return type::INVALID;
}