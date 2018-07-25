#include "SpatialSoundObject.h"

SpatialSoundObject::SpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection) :
	VirtualObj(ptOrigin),
	m_vEmitterDirection(vEmitterDirection),
	m_vListenerDirection(vListenerDirection)
{
	// empty
}