#ifndef ACTIVE_OBJECT_H_
#define ACTIVE_OBJECT_H_

#include "RESULT/EHM.h"

// Active Object 
// An active object is an object that is currently being interacted with
// and as such retains a given state

#include "Primitives/point.h"
#include "Primitives/vector.h"

class VirtualObj;
class ContactPoint;

class ActiveObject {
public:
	enum class state : uint8_t {
		NOT_INTERSECTED = 0,		
		RAY_INTERSECTED = 1 << 0, 	 
		OBJ_INTERSECTED = 1 << 1,
		INVALID			= 0xFF
	};

	enum class type : uint8_t {
		INTERSECT,
		COLLIDE,
		INVALID
	};

public:
	ActiveObject(VirtualObj *pObject);
	ActiveObject(VirtualObj *pObject, VirtualObj *pEventObject);

	RESULT UpdateObject(const point &ptIntersection, const vector &vNormal, ActiveObject::state newState);
	
	ActiveObject::state GetState();

	RESULT SetState(ActiveObject::state newState);
	RESULT AddState(ActiveObject::state newState);
	RESULT RemoveState(ActiveObject::state newState);
	bool HasState(ActiveObject::state newState);

	RESULT SetContactPoint(ContactPoint contactPoint);
	RESULT SetInteractionPoint(point ptIntersection);
	RESULT SetIntersectionNormal(vector vNormal);

	point GetIntersectionPoint();
	vector GetIntersectionNormal();
	VirtualObj *GetObject();
	VirtualObj *GetEventObject();

	RESULT SetEventObject(VirtualObj *pEventObject);

private:
	ActiveObject::state m_state = state::NOT_INTERSECTED;
	
	VirtualObj *m_pObject = nullptr;
	VirtualObj *m_pEventObject = nullptr;

	point m_ptIntersection;
	vector m_vNormal;
};

ActiveObject::state operator&(ActiveObject::state lhs, ActiveObject::state rhs);
ActiveObject::state operator|(ActiveObject::state lhs, ActiveObject::state rhs);

#endif // ! ACTIVE_OBJECT_H_
