#ifndef WASAPI_SPATIAL_SOUND_OBJECT_H_
#define WASAPI_SPATIAL_SOUND_OBJECT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SpataiSoundObject.h

#include "Sound/SpatialSoundObject.h"

#include <SpatialAudioClient.h>
#include <SpatialAudioHrtf.h>

#include <wrl/client.h>

class WASAPISpatialSoundObject : public SpatialSoundObject {
public:
	WASAPISpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection, 
		ISpatialAudioClient *pAudioSpatialClient, ISpatialAudioObjectRenderStreamForHrtf* spatialAudioStreamForHrtf);

	~WASAPISpatialSoundObject();


	virtual RESULT Initialize() override;
	virtual RESULT Kill() override;

	RESULT GetBuffer(BYTE **ppBuffer, UINT32 *pBufferLength);

	RESULT WriteTestSignalToAudioObjectBuffer(unsigned int numFrames, unsigned int samplingRate = 48000, unsigned int numChannels = 1, float frequency = 440.0f);
	RESULT LoadDataFromBuffer(unsigned int numFrames, unsigned int numChannels = 1);

	RESULT UpdateSpatialSoundObjectOrientation();

	virtual RESULT Update(unsigned int numFrames, unsigned int numChannels) override;

private:
	ISpatialAudioClient *m_pAudioSpatialClient = nullptr;
	ISpatialAudioObjectRenderStreamForHrtf* m_pSpatialAudioStreamForHrtf = nullptr;

private:
	Microsoft::WRL::ComPtr<ISpatialAudioObjectForHrtf> m_pSpatialAudioObjectHRTF = nullptr;	
};

#endif // ! WASAPI_SPATIAL_SOUND_OBJECT_H_