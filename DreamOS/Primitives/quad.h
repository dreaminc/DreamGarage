#ifndef QUAD_H_
#define QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quad.h
// Quadrilateral Primitive (square, rectangle, parallelogram, trapezoid, rhombus, trapezium)
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_QUAD_POINTS 4
#define NUM_QUAD_TRIS 2
#define DEFAULT_HEIGHT_MAP_SCALE 1.0f

#include "DimObj.h"

#include "PrimParams.h"

#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "texture.h"

class BoundingQuad;
class plane;

class quad : public virtual DimObj {
public:
	enum class type {
		SQUARE,
		RECTANGLE,
		PARALLELOGRAM,
		TRAPEZOID,
		RHOMBUS,
		TRAPEZIUM,
		CURVED,
		INVALID
	};

	enum class CurveType {
		FLAT,
		PARABOLIC,
		CIRCLE,
		INVALID
	};

	struct params :
		public PrimParams
	{
		virtual PRIMITIVE_TYPE GetPrimitiveType() override { return PRIMITIVE_TYPE::VOLUME; }

		params(double width = 1.0f, double height = 1.0f, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, vector vNormal = vector::jVector()) :
			width(width),
			height(height),
			numVerticalDivisions(numVerticalDivisions),
			numHorizontalDivisions(numHorizontalDivisions),
			vNormal(vNormal)
		{ }

		type quadType = type::INVALID;
		CurveType quadCurveType = CurveType::INVALID;
		int numVerticalDivisions = 1;
		int numHorizontalDivisions = 1;
		texture *pTextureHeight = nullptr;
		double heightMapScale = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		vector vNormal;
		bool fBillboard = false;
		bool fScaledBillboard = false;
		bool fTextureUVFlipVertical = false;
	};

public:
	quad(quad& q);	// copy ctor
	quad(quad&& q);	// move ctor

	quad(float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float width, float height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal = vector::jVector());
	quad(BoundingQuad *pBoundingQuad, bool fTriangleBased);

	quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, CurveType curveType = CurveType::FLAT, vector vNormal = vector::jVector());

	//RESULT SetVertices(float width, float height, vector vNormal);
	RESULT SetVertices(float width, float height, vector vNormal, const uvcoord& uvTopLeft = uvcoord(0.0f, 0.0f), const uvcoord& uvBottomRight = uvcoord(1.0f, 1.0f));
	RESULT SetVertices(BoundingQuad* pBoundingQuad, bool fTriangleBased);

	// TODO: We should move this into a util / general math lib
	template <typename T=float>
	std::vector<std::pair<T, T>> GetCurveBuffer(T startVal, T endVal, int divisions, quad::CurveType curveType, T val = 1.0f);

	template <typename T = float>
	std::pair<T, T> GetCurveFocus(quad::CurveType curveType, T val = 1.0f);

	template <typename T = float>
	T GetCurveInterpolatedValue(T xVal, std::vector<std::pair<T, T>> curveValues);

	template <typename T = float>
	std::pair<T, T> GetCurveNormal(T xVal, std::vector<std::pair<T, T>> curveValues, std::pair<T, T> ptFocus);

	template <typename T = float>
	T GetCurveBufferArcLength(std::vector<std::pair<T, T>> curveValues);

	template <typename T = float>
	T GetCurveArcLength(T startVal, T endVal, int divisions, quad::CurveType curveType, T val = 1.0f);

	template <typename T = float>
	std::pair<T, T> GetStartEndForCurveLengthWithMidpoint(T length, T midpoint, int divisions, quad::CurveType curveType, T val = 1.0f);

	RESULT ApplyCurveToVertices();

	RESULT FlipUVVertical();
	RESULT FlipUVHorizontal();

	RESULT SetUVValues(float top, float left, float bottom, float right);

	RESULT SetDiffuseTexture(texture* pTexture);

	vector GetNormal(bool fAbsolute = false);
	
	type EvaluatePoints(point a, point b, point c);

	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices();
	inline unsigned int NumberIndices();

	bool IsBillboard();
	void SetBillboard(bool fBillboard);

	bool IsScaledBillboard();
	void SetScaledBillboard(bool fScale);

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad, bool fTriangleBased = true);
	RESULT UpdateParams(float width, float height, vector vNormal);

	float GetWidth();
	float GetHeight();

	plane GetPlane();

private:
	quad::params m_params;

};

#endif // ! QUAD_H_
