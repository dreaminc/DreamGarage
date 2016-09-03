#include "ObjectStoreImpFlatList.h"
#include <functional>

#define MAX_FLAT_Z (0.99f) // rendering issues start to occur around 1.0f

ObjectStoreImpFlatList::ObjectStoreImpFlatList()
{
	ResetIterator();
}

ObjectStoreImpFlatList::~ObjectStoreImpFlatList()
{
	// empty
}

RESULT ObjectStoreImpFlatList::SetMaxZ(std::shared_ptr<DimObj> pObject)
{
	point origin = pObject->GetOrigin();
	float z = abs(origin.z());
	m_maxZ = z > m_maxZ ? z : m_maxZ;
	return R_PASS;
}
/*
//RESULT Traverse(RESULT (*f)(std::shared_ptr<DimObj>), std::vector<std::shared_ptr<VirtualObj>> objects) {
RESULT Traverse(std::function<RESULT(std::shared_ptr<DimObj> pObject)> f, std::vector<std::shared_ptr<VirtualObj>> objects) {
	RESULT r = R_PASS;
	for (auto& v : objects) {
		auto d = std::dynamic_pointer_cast<DimObj>(v);
		if (d && d->HasChildren()) {
			CR((f)(d));
			CR(Traverse(f, d->GetChildren()));
		}
	}
Error:
	return r;
}
*/
RESULT ObjectStoreImpFlatList::PushObject(VirtualObj *pObject)
{
/*
	float curZ = pObject->GetOrigin().z();
	m_maxZ = curZ > m_maxZ ? curZ : m_maxZ;
	m_minZ = curZ < m_minZ ? curZ : m_minZ;
*/
	return ObjectStoreImpList::PushObject(pObject);
}

RESULT ObjectStoreImpFlatList::UpdateScene()
{
	/*
	for (auto &v : m_objects) {
		float curZ = v->GetOrigin().z();
		m_maxZ = curZ > m_maxZ ? curZ : m_maxZ;
		m_minZ = curZ < m_minZ ? curZ : m_minZ;
	}*/
/*
	auto m_vObjects = std::vector<std::shared_ptr<VirtualObj>>();
	for (auto& v : m_objects) {
		m_vObjects.push_back(std::make_shared<VirtualObj>(v));
	}*/
	//Traverse(SetMaxZ, m_vObjects);
	//std::function<RESULT(std::shared_ptr<DimObj>)> f = SetMaxZ;
	//Traverse(SetMaxZ, m_vObjects);

	//std::list<VirtualObj*> expanded_objects;

	return R_PASS;
}
