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
		NOT_INTERSECTED,		
		INTERSECTED, 	 
		INVALID				
	};

public:
	ActiveObject(VirtualObj *pObject);

	RESULT UpdateObject(const point &ptIntersection, const vector &vNormal, ActiveObject::state newState);
	
	ActiveObject::state GetState();
	RESULT SetState(ActiveObject::state newstate);

	RESULT SetContactPoint(ContactPoint contactPoint);
	RESULT SetInteractionPoint(point ptIntersection);
	RESULT SetIntersectionNormal(vector vNormal);

	point GetIntersectionPoint();
	vector GetIntersectionNormal();
	VirtualObj *GetObject();

private:
	ActiveObject::state m_state = state::NOT_INTERSECTED;
	
	VirtualObj *m_pObject = nullptr;

	point m_ptIntersection;
	vector m_vNormal;
};

#endif // ! ACTIVE_OBJECT_H_
