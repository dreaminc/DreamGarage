#ifndef SPATIAL_SOUND_OBJECT_H_
#define SPATIAL_SOUND_OBJECT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SpataiSoundObject.h

#include "Primitives/VirtualObj.h"

#include "SoundBuffer.h"

class camera;
class SoundFile;

class SpatialSoundObject : public VirtualObj {

public:
	SpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection);

	//~SpatialSoundObject();

	virtual RESULT Initialize() = 0;
	virtual RESULT Kill() = 0;

	RESULT InitializeSoundBuffer(int numChannels, SoundBuffer::type bufferType);

	RESULT SetEmitterListenerDirection(vector vEmitterDirection, vector vListenerDirection);

	virtual RESULT Update(unsigned int numFrames, unsigned int numChannels) = 0;

	// TODO: Move this to internal buffer
	// Function needs to copy over stuff 
	virtual RESULT WriteTestSignalToAudioObjectBuffer(unsigned int numFrames, unsigned int samplingRate = 48000, unsigned int numChannels = 1, float frequency = 440.0f) { return R_NOT_IMPLEMENTED; }

	virtual RESULT LoadSoundFile(SoundFile *pSoundFile);
	virtual RESULT PlaySoundFile(SoundFile *pSoundFile);
	virtual RESULT LoopSoundFile(SoundFile *pSoundFile);

private:
	camera *m_pListenerCamera = nullptr;

protected:
	vector m_vEmitterDirection;
	vector m_vListenerDirection;

	SoundBuffer *m_pSoundBuffer = nullptr;
	bool m_fLoop = false;
	uint64_t m_startLoop = 0;
	uint64_t m_endLoop = 0;
};

#endif // ! SPATIAL_SOUND_OBJECT_H_