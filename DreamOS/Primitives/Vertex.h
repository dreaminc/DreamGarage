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

#include "matrix/matrix.h"
#include "vector.h"
#include "point.h"
#include "uvcoord.h"
#include "color.h"

// TODO: Port to LinAlgLib (using point etc)
class vertex {
public:
	point m_point;
	color m_color;
	vector m_normal;
	uvcoord m_uvcoord;
	vector m_tangent;
	vector m_bitangent;
	

public:
	vertex() {
		m_point.clear();
		m_color.clear();
		m_normal.clear();
		m_uvcoord.clear();
		m_tangent.clear();
		m_bitangent.clear();
	}

	vertex(point p) {
		SetPoint(p);
		SetColor(color(1.0f, 1.0f, 1.0f, 1.0f));
		m_normal.clear();
		m_uvcoord.clear();
		m_tangent.clear();
		m_bitangent.clear();
	}

	vertex(point p, color c) {
		SetPoint(p);
		SetColor(c);
		m_normal.clear();
		m_uvcoord.clear();
		m_tangent.clear();
		m_bitangent.clear();
	}

	vertex(point p, vector n) {
		SetPoint(p);
		SetColor(color(1.0f, 1.0f, 1.0f, 1.0f));
		SetNormal(n);
		m_uvcoord.clear();
		m_tangent.clear();
		m_bitangent.clear();
	}

	vertex(point p, color c, vector n) {
		SetPoint(p);
		SetColor(c);
		SetNormal(n);
		m_uvcoord.clear();
		m_tangent.clear();
		m_bitangent.clear();
	}

	vertex(point p, vector n, uvcoord uv) {
		SetPoint(p);
		SetColor(color(1.0f, 1.0f, 1.0f, 1.0f));
		SetNormal(n);
		SetUV(uv);
		m_tangent.clear();
		m_bitangent.clear();
	}

	~vertex() {
		// Empty stub for now
	}

	static void *GetVertexOffset() {
		int64_t offset = NULL;
		return (void*)(offset);
	}

	static void* GetColorOffset() {
		int64_t offset = (int64_t)(GetVertexOffset()) + sizeof(point);
		return (void*)(offset);
	}

	static void* GetNormalOffset() {
		int64_t offset = (int64_t)(GetColorOffset()) + sizeof(color);
		return (void*)(offset);
	}

	static void* GetUVOffset() {
		int64_t offset = (int64_t)(GetNormalOffset()) + sizeof(vector);
		return (void*)(offset);
	}

	static void* GetTangentOffset() {
		int64_t offset = (int64_t)(GetUVOffset()) + sizeof(uvcoord);
		return (void*)(offset);
	}

	static void* GetBitangentOffset() {
		int64_t offset = (int64_t)(GetTangentOffset()) + sizeof(vector);
		return (void*)(offset);
	}

	inline static int GetPointDimensions() {
		return point::rows();
	}

	inline static int GetColorDimensions() {
		return color::rows();
	}

	inline static int GetNormalDimensions() {
		return vector::rows();
	}

	inline static int GetUVCoordDimensions() {
		return uvcoord::rows();
	}

	inline static int GetTangentDimensions() {
		return vector::rows();
	}

	inline static int GetBitangentDimensions() {
		return vector::rows();
	}

	inline point GetPoint() {
		return m_point;
	}

	inline color GetColor() {
		return m_color;
	}

	inline vector GetNormal() {
		return m_normal;
	}

	inline uvcoord GetUV() {
		return m_uvcoord;
	}

	inline vector GetTangent() {
		return m_tangent;
	}

	inline vector GetBitangent() {
		return m_bitangent;
	}

public:
	RESULT SetPoint(point p) {
		m_point = p;
		return R_PASS;
	}

	//RESULT SetPointW(float x, float y, float z, float w);
	RESULT SetPoint(point_precision x, point_precision y, point_precision z) {
		m_point = point(x, y, z);
		return R_PASS;
	}

	RESULT TranslatePoint(point pt) {
		m_point += pt;
		return R_PASS;
	}

	RESULT TranslateX(point_precision val) {
		m_point.x() += val;
		return R_PASS;
	}

	RESULT TranslateY(point_precision val) {
		m_point.y() += val;
		return R_PASS;
	}

	RESULT TranslateZ(point_precision val) {
		m_point.z() += val;
		return R_PASS;
	}
	
	//RESULT SetColorA(float x, float y, float z);
	RESULT SetColor(color_precision r, color_precision g, color_precision b) {
		m_color = color(r, g, b, 1.0f);
		return R_PASS;
	}

	RESULT SetColor(color_precision r, color_precision g, color_precision b, color_precision a) {
		m_color = color(r, g, b, a);
		return R_PASS;
	}

	RESULT SetColor(color c) {
		m_color = c;
		return R_PASS;
	}

	RESULT SetAlpha(color_precision a) {
		m_color.a() = a;
		return R_PASS;
	}

	RESULT SetRandomColor() {
		m_color = color(COLOR_RANDOM);
		return R_PASS;
	}

	RESULT SetNormal(vector n) {
		m_normal = n;
		return R_PASS;
	}

	RESULT SetNormal(vector_precision i, vector_precision j, vector_precision k) {
		m_normal = vector(i, j, k);
		return R_PASS;
	}

	RESULT SetUV(uvcoord uv) {
		m_uvcoord = uv;
		return R_PASS;
	}

	RESULT SetUV(uv_precision u, uv_precision v) {
		m_uvcoord = uvcoord(u, v);
		return R_PASS;
	}

	RESULT SetTangent(vector t) {
		m_tangent = t;
		return R_PASS;
	}

	RESULT SetTangent(vector_precision i, vector_precision j, vector_precision k) {
		m_tangent = vector(i, j, k);
		return R_PASS;
	}

	RESULT SetTangentBitangentFromNormal() {
		// Set random normal vectors for t,b in the perpendicuar plane of the normal
		vector n = GetNormal();

		vector tangent = vector(n.z(), n.z(), -n.x() - n.y());

		vector bitangent = n.cross(tangent);

		return SetTangentBitangent(tangent.Normal(), bitangent.Normal());
	}

	RESULT SetBitangent(vector b) {
		m_bitangent = b;
		return R_PASS;
	}

	RESULT SetBitangent(vector_precision i, vector_precision j, vector_precision k) {
		m_bitangent = vector(i, j, k);
		return R_PASS;
	}

	RESULT SetTangentBitangent(vector t, vector b) {
		SetTangent(t);
		SetBitangent(b);
		return R_PASS;
	}

	RESULT SetVertex(vertex v) {
		m_point = v.GetPoint();
		m_color = v.GetColor();
		m_normal = v.GetNormal();
		m_uvcoord = v.GetUV();
		m_tangent = v.GetTangent();
		m_bitangent = v.GetBitangent();

		return R_PASS;
	}

	// TODO: Operator overloading
};

#endif // ! VERTEX_H_
