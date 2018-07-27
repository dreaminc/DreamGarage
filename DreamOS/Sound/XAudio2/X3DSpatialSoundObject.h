#ifndef X3D_SPATIAL_SOUND_OBJECT_H_
#define X3D_SPATIAL_SOUND_OBJECT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SpataiSoundObject.h

#include "Sound/SpatialSoundObject.h"

#include <Windows.h>
#include <XAudio2.h>
#include <x3daudio.h>
#include <xapo.h>
#include <hrtfapoapi.h>

#include <wrl/client.h>

class X3DSpatialSoundObject : public SpatialSoundObject {
public:
	X3DSpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection, std::shared_ptr<IXAudio2> pXAudio2, std::shared_ptr<IXAudio2MasteringVoice> pXAudio2MasterVoice);
	~X3DSpatialSoundObject();


	virtual RESULT Initialize() override;
	virtual RESULT Kill() override;
	virtual RESULT Update(unsigned int numFrames, unsigned int numChannels) override;

	//RESULT GetBuffer(BYTE **ppBuffer, UINT32 *pBufferLength);
	//
	//RESULT WriteTestSignalToAudioObjectBuffer(unsigned int numFrames, unsigned int samplingRate = 48000, unsigned int numChannels = 1, float frequency = 440.0f);
	//RESULT LoadDataFromBuffer(unsigned int numFrames, unsigned int numChannels = 1);
	//
	//RESULT UpdateSpatialSoundObjectOrientation();

private:
	std::shared_ptr<IXAudio2> m_pXAudio2 = nullptr;
	std::shared_ptr<IXAudio2MasteringVoice> m_pXAudio2MasterVoice = nullptr;

	Microsoft::WRL::ComPtr<IXAPO> m_pXAPO;
	Microsoft::WRL::ComPtr<IXAPOHrtfParameters> m_pHRTFfParams;

	std::shared_ptr<IXAudio2SourceVoice> m_pXAudio2SourceVoice = nullptr;
	std::shared_ptr<IXAudio2SubmixVoice> m_pXAudio2SubmixVoice = nullptr;
};

#endif // ! X3D_SPATIAL_SOUND_OBJECT_H_