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
#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "texture.h"

class BoundingQuad;

class quad : public DimObj {
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

public:
	quad(quad& q);	// copy ctor
	quad(quad&& q);	// move ctor

	quad(float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float width, float height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal = vector::jVector());
	quad(BoundingQuad *pBoundingQuad, bool fTriangleBased);

	quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, CurveType curveType = CurveType::FLAT, vector vNormal = vector::jVector());

	//RESULT SetVertices(float width, float height, vector vNormal);
	RESULT SetVertices(float width, float height, vector vNormal, const uvcoord& uvTopLeft = uvcoord(0.0f, 0.0f), const uvcoord& uvBottomRight = uvcoord(1.0f, 1.0f));
	RESULT SetVertices(BoundingQuad* pBoundingQuad, bool fTriangleBased);

	RESULT ApplyCurveToVertices();

	RESULT FlipUVVertical();
	RESULT FlipUVHorizontal();

	vector GetNormal();
	
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

private:
	type m_quadType = type::INVALID;
	CurveType m_quadCurveType = CurveType::INVALID;

	int m_numVerticalDivisions = 1;
	int m_numHorizontalDivisions = 1;

	texture *m_pTextureHeight = nullptr;
	double m_heightMapScale = 1.0f;

	float m_width;
	float m_height;
	vector m_vNormal;

	bool m_fBillboard = false;
	bool m_fScaledBillboard = false;

};

#endif // ! QUAD_H_
