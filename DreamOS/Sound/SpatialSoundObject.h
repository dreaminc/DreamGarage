#ifndef SPATIAL_SOUND_OBJECT_H_
#define SPATIAL_SOUND_OBJECT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SpataiSoundObject.h

#include "Primitives/VirtualObj.h"

class camera;

class SpatialSoundObject : public VirtualObj {

public:
	SpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection);

	//~SpatialSoundObject();

	virtual RESULT Initialize() = 0;
	virtual RESULT Kill() = 0;

	// Will need to take over other functions
	virtual RESULT SetSpatialObjectPosition(point ptPosition) = 0;

	// TODO: This needs to be generalized in the arch since it is camera dependent (IMPORTANT)
	virtual RESULT SetSpatialSoundObjectOrientation(vector vEmitterDirection, vector vListenerDirection) = 0;

	virtual RESULT Update() = 0;

private:
	camera *m_pListenerCamera = nullptr;

protected:
	vector m_vEmitterDirection;
	vector m_vListenerDirection;
};

#endif // ! SPATIAL_SOUND_OBJECT_H_