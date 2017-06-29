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
	typedef enum {
		SQUARE,
		RECTANGLE,
		PARALLELOGRAM,
		TRAPEZOID,
		RHOMBUS,
		TRAPEZIUM,
		INVALID
	} QUAD_TYPE;

public:
	quad(quad& q);	// copy ctor
	quad(quad&& q);	// move ctor

	quad(float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float height, float width, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad(float height, float width, point& ptCenter, const uvcoord& uvBottomLeft, const uvcoord& uvTopRight, vector vNormal = vector::jVector());
	quad(BoundingQuad *pBoundingQuad, bool fTriangleBased);

	RESULT SetVertices(float width, float height, vector vNormal);
	RESULT SetVertices(BoundingQuad* pBoundingQuad, bool fTriangleBased);

	RESULT FlipUVVertical();
	RESULT FlipUVHorizontal();

	vector GetNormal();
	
	QUAD_TYPE EvaluatePoints(point a, point b, point c);

	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices();
	inline unsigned int NumberIndices();

	bool IsBillboard();
	void SetBillboard(bool fBillboard);

	bool IsScaledBillboard();
	void SetScaledBillboard(bool fScale);

	RESULT UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad, bool fTriangleBased = true);
	RESULT UpdateParams(float width, float height, vector vNormal);

private:
	QUAD_TYPE m_quadType;

	int m_numVerticalDivisions;
	int m_numHorizontalDivisions;

	texture *m_pTextureHeight;
	double m_heightMapScale;

	float m_width;
	float m_height;
	vector m_vNormal;

	bool m_fBillboard = false;
	bool m_fScaledBillboard = false;

};

#endif // ! QUAD_H_
