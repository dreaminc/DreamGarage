#include "DimObj.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"

#include "PhysicsEngine/CollisionManifold.h"

DimObj::DimObj() :
	VirtualObj(),	// velocity, origin
	m_pVertices(nullptr),
	m_pIndices(nullptr),
	m_material(),
	m_pColorTexture(nullptr),
	m_pBumpTexture(nullptr),
	m_pObjects(nullptr),
	m_pParent(nullptr),
	m_fVisible(true),
	m_fWireframe(false),
	m_pBoundingVolume(nullptr)
	//m_aabv()
{
	/* stub */
}

DimObj::~DimObj() {
	Destroy();
}

OBJECT_TYPE DimObj::GetType() {
	return OBJECT_DIMENSION;
}

RESULT DimObj::Destroy() {
	if (m_pIndices != nullptr) {
		delete[] m_pIndices;
		m_pIndices = nullptr;
	}

	if (m_pVertices != nullptr) {
		delete[] m_pVertices;
		m_pVertices = nullptr;
	}

	return R_PASS;
}

RESULT DimObj::AllocateVertices(uint32_t numVerts) {
	RESULT r = R_PASS;

	m_pVertices = new vertex[numVerts];
	CN(m_pVertices);

Error:
	return r;
}

RESULT DimObj::AllocateIndices(uint32_t numIndices) {
	RESULT r = R_PASS;

	m_pIndices = new dimindex[numIndices];
	CN(m_pIndices);

Error:
	return r;
}

RESULT DimObj::AllocateQuadIndexGroups(uint32_t numQuads) {
	RESULT r = R_PASS;

	m_pIndices = (dimindex*)(new QuadIndexGroup[numQuads]);
	CN(m_pIndices);

Error:
	return r;
}

RESULT DimObj::AllocateTriangleIndexGroups(uint32_t numTriangles) {
	RESULT r = R_PASS;

	m_pIndices = (dimindex*)(new TriangleIndexGroup[numTriangles]);
	CN(m_pIndices);

Error:
	return r;
}

RESULT DimObj::UpdateBuffers() {
	return R_NOT_IMPLEMENTED;
}

bool DimObj::IsVisible() {
	return m_fVisible;
}

RESULT DimObj::SetVisible(bool fVisible) { 
	m_fVisible = fVisible;

	if (HasChildren()) {
		for (auto& child : GetChildren()) {
			std::shared_ptr<DimObj> pDimObj = std::dynamic_pointer_cast<DimObj>(child);
			if (pDimObj) {
				pDimObj->SetVisible(fVisible);
			}
		}
	}

	return R_PASS;
}

bool DimObj::IsWireframe() {
	return m_fWireframe; 
}

RESULT DimObj::SetWireframe(bool fWireframe) {
	m_fWireframe = fWireframe; 
	return R_PASS; 
}

RESULT DimObj::SetColor(color c) {
	for (unsigned int i = 0; i < NumberVertices(); i++)
		m_pVertices[i].SetColor(c);

	SetDirty();

	return R_PASS;
}

RESULT DimObj::TransformUV(matrix<uv_precision, 2, 1> a, matrix<uv_precision, 2, 2> b) {
	for (unsigned int i = 0; i < NumberVertices(); i++) {
		uvcoord uv = m_pVertices[i].GetUV();
		uv = a + b * uv;
		m_pVertices[i].SetUV(uv);
	}

	SetDirty();

	return R_PASS;
}

RESULT DimObj::SetMaterialTexture(MaterialTexture type, texture *pTexture) {
	RESULT r = R_PASS;

	#define SET_TEXTURE(type, texture) case DimObj::MaterialTexture::type: texture = pTexture; break

	switch (type) {
		SET_TEXTURE(Ambient, m_pTextureAmbient);
		SET_TEXTURE(Diffuse, m_pTextureDiffuse);
		SET_TEXTURE(Specular, m_pTextureSpecular);
	}

	pTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_COLOR);

//Error:
	return r;
}

RESULT DimObj::SetMaterialAmbient(float ambient) {
	m_material.SetAmbientIntensity(ambient);
	return R_PASS;
}

RESULT DimObj::SetColorTexture(texture *pTexture) {
	RESULT r = R_PASS;

	CBM((m_pColorTexture == nullptr), "Cannot overwrite color texture");
	m_pColorTexture = pTexture;
	m_pColorTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_COLOR);

Error:
	return r;
}

RESULT DimObj::ClearColorTexture() {
	RESULT r = R_PASS;

	CB((m_pColorTexture != nullptr));
	m_pColorTexture = nullptr;

Error:
	return r;
}

texture* DimObj::GetColorTexture() {
	return m_pColorTexture;
}

RESULT DimObj::SetBumpTexture(texture *pBumpTexture) {
	RESULT r = R_PASS;

	CBM((m_pBumpTexture == nullptr), "Cannot overwrite bump texture");
	m_pBumpTexture = pBumpTexture;
	m_pBumpTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_BUMP);

Error:
	return r;
}

RESULT DimObj::ClearBumpTexture() {
	RESULT r = R_PASS;

	CB((m_pBumpTexture != nullptr));
	m_pBumpTexture = nullptr;

Error:
	return r;
}

texture* DimObj::GetBumpTexture() {
	return m_pBumpTexture;
}

texture* DimObj::GetTextureAmbient() {
	return m_pTextureAmbient;
}

texture* DimObj::GetTextureDiffuse() {
	return m_pTextureDiffuse;
}

texture* DimObj::GetTextureSpecular() {
	return m_pTextureSpecular;
}

RESULT DimObj::SetRandomColor() {
	for (unsigned int i = 0; i < NumberVertices(); i++)
		m_pVertices[i].SetRandomColor();

	return R_PASS;
}

// Children (composite objects)
RESULT DimObj::AddChild(std::shared_ptr<DimObj> pDimObj) {
	if (m_pObjects == nullptr) {
		m_pObjects = std::unique_ptr<std::vector<std::shared_ptr<VirtualObj>>>(new std::vector<std::shared_ptr<VirtualObj>>);
	}

	m_pObjects->push_back(pDimObj);
	pDimObj->SetParent(this);

	return R_PASS;
}

RESULT DimObj::ClearChildren() {
	m_pObjects->clear();
	return R_PASS;
}

bool DimObj::HasChildren() {
	return (m_pObjects != nullptr) && (m_pObjects->size() != 0);
}

std::vector<std::shared_ptr<VirtualObj>> DimObj::GetChildren() {
	return *(m_pObjects.get());
}

// Intersections and Collision
bool DimObj::Intersect(VirtualObj* pObj) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);

	if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr || GetBoundingVolume() == nullptr) {
		return false;
	}
	else {
		return GetBoundingVolume()->Intersect(pDimObj->GetBoundingVolume().get());
	}
}

CollisionManifold DimObj::Collide(VirtualObj* pObj) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);

	if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr || GetBoundingVolume() == nullptr) {
		return CollisionManifold(this, pObj);
	}
	else {
		return GetBoundingVolume()->Collide(pDimObj->GetBoundingVolume().get());
	}
}

bool DimObj::Intersect(const ray &rCast) {
	if (GetBoundingVolume() == nullptr) {
		return false;
	}
	else {
		if (GetBoundingVolume()->Intersect(rCast)) {
			if (HasChildren()) {
				for (auto &pChild : GetChildren()) {
					DimObj *pDimChild = (std::dynamic_pointer_cast<DimObj>(pChild)).get();

					// Bounding Volume is oriented correctly using the DimObj overloads
					if (pDimChild->GetBoundingVolume()->Intersect(rCast)) {
						return true;
					}
				}

				return false;
			}
			else {
				return true;
			}
		}
	}

	return false;
}

CollisionManifold DimObj::Collide(const ray &rCast) {
	if (GetBoundingVolume() == nullptr) {
		return CollisionManifold(this, nullptr);
	}
	else {
		if (GetBoundingVolume()->Intersect(rCast)) {
			if (HasChildren()) {
				for (auto &pChild : GetChildren()) {
					DimObj *pDimChild = (std::dynamic_pointer_cast<DimObj>(pChild)).get();

					// Bounding Volume is oriented correctly using the DimObj overloads
					if (pDimChild->GetBoundingVolume()->Intersect(rCast)) {
						// TODO: This does not support multiple simultaneous objects
						return pDimChild->GetBoundingVolume()->Collide(rCast);
					}
				}

				return CollisionManifold(this, nullptr);
			}
			else {
				return GetBoundingVolume()->Collide(rCast);
			}
		}
	}

	return CollisionManifold(this, nullptr);
}

point DimObj::GetOrigin(bool fAbsolute) {
	point ptOrigin = m_objectState.m_ptOrigin;

	if (fAbsolute && m_pParent != nullptr) {
		ptOrigin = m_pParent->GetModelMatrix() * ptOrigin;
	}

	return ptOrigin;
}

point DimObj::GetPosition(bool fAbsolute) {
	return GetOrigin(fAbsolute);
}

quaternion DimObj::GetOrientation(bool fAbsolute) {
	quaternion qOrientation = m_objectState.m_qRotation;

	if (fAbsolute && m_pParent != nullptr)
		qOrientation *= m_pParent->GetOrientation();

	return qOrientation;
}

double DimObj::GetMass() {
	double mass = m_objectState.GetMass();
	
	if (m_pObjects != nullptr) {
		for (auto it = m_pObjects->begin(); it < m_pObjects->end(); it++) {
			mass += (*it)->GetMass();
		}
	}

	return mass;
}

double DimObj::GetInverseMass() {
	double invmass = m_objectState.GetInverseMass();
	
	if (m_pObjects != nullptr) {
		for (auto it = m_pObjects->begin(); it < m_pObjects->end(); it++) {
			invmass += (*it)->GetInverseMass();
		}
	}

	return invmass;
}

RESULT DimObj::SetParent(DimObj* pParent) {
	m_pParent = pParent;
	return R_PASS;
}

DimObj* DimObj::GetParent() {
	return m_pParent;
}

bool DimObj::CompareParent(DimObj* pParent) {
	if (m_pParent == nullptr)
		return false;
	return pParent == m_pParent;
}

// This assumes the other vertices have a valid position and uv mapping
// This will set the tangents/bi-tangents for all three vertices
// Source: http://learnopengl.com/#!Advanced-Lighting/Normal-Mapping
// TODO: Use matrix to simplify logic
//RESULT SetTangentBitangent(vertex v1, vertex v2, vertex v3) {
RESULT DimObj::SetTriangleTangentBitangent(dimindex i1, dimindex i2, dimindex i3) {
	RESULT r = R_PASS;
	vector tangent, bitangent;
	vertex *pV1 = nullptr, *pV2 = nullptr, *pV3 = nullptr;
	vector deltaPos1, deltaPos2;
	uvcoord deltaUV1, deltaUV2;
	point_precision factor = 0.0f;

	// TODO: More eloquent way than this
	CB((i1 < static_cast<unsigned int>(NumberIndices())));
	pV1 = &(m_pVertices[i1]);
	CN(pV1);

	CB((i2 < static_cast<unsigned int>(NumberIndices())));
	pV2 = &(m_pVertices[i2]);
	CN(pV2);

	CB((i3 < static_cast<unsigned int>(NumberIndices())));
	pV3 = &(m_pVertices[i3]);
	CN(pV3);

	deltaPos1 = pV2->GetPoint() - pV1->GetPoint();
	deltaPos2 = pV3->GetPoint() - pV1->GetPoint();

	deltaUV1 = pV2->GetUV() - pV1->GetUV();
	deltaUV2 = pV3->GetUV() - pV1->GetUV();

	factor = -1.0f / ((deltaUV1.u() * deltaUV2.v()) - (deltaUV1.v() * deltaUV2.u()));

	tangent = factor * ((deltaPos1 * deltaUV2.v()) - (deltaPos2 * deltaUV1.v()));
	tangent.Normalize();

	bitangent = factor * ((deltaPos2 * deltaUV1.u()) - (deltaPos1 * deltaUV2.u()));
	bitangent.Normalize();

	pV1->SetTangentBitangent(tangent, bitangent);
	pV2->SetTangentBitangent(tangent, bitangent);
	pV3->SetTangentBitangent(tangent, bitangent);

Error:
	return r;
}

// This will not take into consideration surfaces that are continuous 
// TODO: Create surface based normal calculation function (this works at the vertex level rather the triangle one)
RESULT DimObj::SetTriangleNormal(dimindex i1, dimindex i2, dimindex i3) {
	RESULT r = R_PASS;

	vertex *pV1 = nullptr, *pV2 = nullptr, *pV3 = nullptr;
	vector deltaPos1, deltaPos2;
	vector normalVector;

	// TODO: More eloquent way than this
	CB((i1 < NumberIndices()));
	pV1 = &(m_pVertices[i1]);
	CN(pV1);

	CB((i2 < NumberIndices()));
	pV2 = &(m_pVertices[i2]);
	CN(pV2);

	CB((i3 < NumberIndices()));
	pV3 = &(m_pVertices[i3]);
	CN(pV3);

	deltaPos1 = pV2->GetPoint() - pV1->GetPoint();
	deltaPos2 = pV3->GetPoint() - pV1->GetPoint();

	normalVector = deltaPos1.NormalizedCross(deltaPos2);

	pV1->SetNormal(normalVector);
	pV2->SetNormal(normalVector);
	pV3->SetNormal(normalVector);

Error:
	return r;
}

RESULT DimObj::SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR) {
	RESULT r = R_PASS;
	vertex *pVTR = nullptr, *pVBL = nullptr;

	// TODO: More eloquent way than this
	CB((TR < static_cast<unsigned int>(NumberIndices())));
	pVTR = &(m_pVertices[TR]);
	CN(pVTR);

	CB((BL < static_cast<unsigned int>(NumberIndices())));
	pVBL = &(m_pVertices[BL]);
	CN(pVBL);

	CR(SetTriangleTangentBitangent(TL, BR, BL));

	CR(pVTR->SetTangentBitangent(pVBL->GetTangent(), pVBL->GetBitangent()));

Error:
	return r;
}

RESULT DimObj::SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR, vector tangent, vector bitangent) {
	RESULT r = R_PASS;
	vertex *pVTL = nullptr, *pVTR = nullptr, *pVBL = nullptr, *pVBR = nullptr;

	// TODO: More eloquent way than this
	CB((TL < NumberIndices()));
	pVTL = &(m_pVertices[TL]);
	CN(pVTL);
	CR(pVTL->SetTangentBitangent(tangent, bitangent));

	CB((TR < NumberIndices()));
	pVTR = &(m_pVertices[TR]);
	CN(pVTR);
	CR(pVTR->SetTangentBitangent(tangent, bitangent));

	CB((BL < NumberIndices()));
	pVBL = &(m_pVertices[BL]);
	CN(pVBL);
	CR(pVBL->SetTangentBitangent(tangent, bitangent));

	CB((BR < NumberIndices()));
	pVBR = &(m_pVertices[BR]);
	CN(pVBR);
	CR(pVBR->SetTangentBitangent(tangent, bitangent));

Error:
	return r;
}

RESULT DimObj::CopyVertices(vertex pVerts[], int pVerts_n) {
	RESULT r = R_PASS;

	CBM((pVerts_n == NumberVertices()), "Cannot copy %d verts into DimObj with %d verts", pVerts_n, NumberVertices());
	for (int i = 0; i < pVerts_n; i++)
		m_pVertices[i].SetVertex(pVerts[i]);

Error:
	return r;
}

// TODO: Should this moved up into vertex?
RESULT DimObj::RotateVerticesByEulerVector(vector vEuler) {
	RESULT r = R_PASS;

	RotationMatrix rotMat(vEuler);

	// point and normal
	for (unsigned int i = 0; i < NumberVertices(); i++) {
		m_pVertices[i].m_point = rotMat * m_pVertices[i].m_point;
		m_pVertices[i].m_normal = rotMat * m_pVertices[i].m_normal;
	}

	// tangent bitangent
	/*
	// TODO:
	for (unsigned int i = 0; i < NumberIndices(); i++) {
	if (i % 3 == 0) {
	SetTriangleTangentBitangent(m_pIndices[i - 3], m_pIndices[i - 2], m_pIndices[i - 1]);
	}
	}
	*/

	//Error:
	return r;
}

RESULT DimObj::Notify(TimeEvent *event) {
	quaternion_precision factor = 0.05f;
	quaternion_precision filter = 0.1f;

	static quaternion_precision x = 1.0f;
	static quaternion_precision y = 1.0f;
	static quaternion_precision z = 1.0f;

	//x = ((1.0f - filter) * x) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
	//y = ((1.0f - filter) * y) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
	//z = ((1.0f - filter) * z) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));

	RotateBy(x * factor, y * factor, z * factor);

	return R_PASS;
}

// TODO: This shoudln't be baked in here ultimately
material* DimObj::GetMaterial() {
	return (&m_material);
}

matrix<virtual_precision, 4, 4> DimObj::GetModelMatrix(matrix<virtual_precision, 4, 4> childMat) {
	if (m_pParent != nullptr) {
		auto modelMatrix = VirtualObj::GetModelMatrix(childMat);
		return m_pParent->GetModelMatrix(modelMatrix);
	}
	else {
		return VirtualObj::GetModelMatrix(childMat);
	}
}

// Bounding Box
RESULT DimObj::UpdateBoundingVolume() {
	RESULT r = R_PASS;

	// This will go through the verts, find the center point and maximum size
	point ptMax, ptMin, ptMid;

	CB((NumberVertices() > 0));

	ptMax = m_pVertices[0].GetPoint();
	ptMin = m_pVertices[0].GetPoint();

	CN(m_pVertices);
	CN(m_pBoundingVolume);

	for (unsigned int i = 0; i < NumberVertices(); i++) {
		point ptVert = m_pVertices[i].GetPoint();

		// X
		if (ptVert.x() > ptMax.x())
			ptMax.x() = ptVert.x();
		else if (ptVert.x() < ptMin.x())
			ptMin.x() = ptVert.x();

		// Y
		if (ptVert.y() > ptMax.y())
			ptMax.y() = ptVert.y();
		else if (ptVert.y() < ptMin.y())
			ptMin.y() = ptVert.y();

		// Z
		if (ptVert.z() > ptMax.z())
			ptMax.z() = ptVert.z();
		else if (ptVert.z() < ptMin.z())
			ptMin.z() = ptVert.z();
	}

	ptMid = point::midpoint(ptMax, ptMin);
	
	CR(m_pBoundingVolume->UpdateBoundingVolume(ptMid, ptMax));
	CR(m_pBoundingVolume->SetDirty());

	if (m_pParent != nullptr) {
		m_pParent->UpdateBoundingVolume();
	}

Error:
	return r;
}

RESULT DimObj::InitializeAABB() {
	RESULT r = R_PASS;

	m_pBoundingVolume = std::shared_ptr<BoundingVolume>(new BoundingBox(this, BoundingBox::Type::AABB));
	CN(m_pBoundingVolume);

	m_objectState.SetMassDistributionType(ObjectState::MassDistributionType::VOLUME);

	CR(UpdateBoundingVolume());

Error:
	return r;
}

RESULT DimObj::InitializeOBB() {
	RESULT r = R_PASS;

	m_pBoundingVolume = std::shared_ptr<BoundingVolume>(new BoundingBox(this, BoundingBox::Type::OBB));
	CN(m_pBoundingVolume);

	m_objectState.SetMassDistributionType(ObjectState::MassDistributionType::VOLUME);

	CR(UpdateBoundingVolume());

Error:
	return r;
}

RESULT DimObj::InitializeBoundingSphere() {
	RESULT r = R_PASS;

	m_pBoundingVolume = std::shared_ptr<BoundingVolume>(new BoundingSphere(this));
	CN(m_pBoundingVolume);

	m_objectState.SetMassDistributionType(ObjectState::MassDistributionType::SPHERE);

	CR(UpdateBoundingVolume());

Error:
	return r;
}

RESULT DimObj::InitializeBoundingQuad(point ptOrigin, float width, float height, vector vNormal) {
	RESULT r = R_PASS;

	m_pBoundingVolume = std::shared_ptr<BoundingQuad>(new BoundingQuad(this, ptOrigin, vNormal, width, height));
	CN(m_pBoundingVolume);

	m_objectState.SetMassDistributionType(ObjectState::MassDistributionType::INVALID);

	//CR(UpdateBoundingVolume());

Error:
	return r;
}

std::shared_ptr<BoundingVolume> DimObj::GetBoundingVolume() {
	return std::shared_ptr<BoundingVolume>(m_pBoundingVolume);
}

RESULT DimObj::OnManipulation() {
	RESULT r = R_PASS;

	if (m_pBoundingVolume != nullptr) {
		CR(m_pBoundingVolume->SetDirty());

		// Update parent
		if (m_pParent != nullptr) {
			//CR(m_pParent->OnManipulation());
			CR(m_pParent->UpdateBoundingVolume());
		}
	}

Error:
	return r;
}
