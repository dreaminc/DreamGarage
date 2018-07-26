#ifndef SOUND_FILE_LOADER_H_
#define SOUND_FILE_LOADER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundFileLoader.h
// A loader for sound files

#include <string>

#include "SoundBuffer.h"

class SoundFile {
public:
	enum class type {
		WAVE,
		MP3,
		INVALID
	};

	SoundFile(const wchar_t *pszFilename, SoundFile::type soundFileType);
	~SoundFile();

	virtual RESULT Initialize() = 0;

	virtual int NumChannels() = 0;
	virtual int SamplingRate() = 0;
	virtual int BitsPerSample() = 0;
	virtual int GetNumFrames() = 0;
	virtual int GetNumSamples() = 0;

	// There are ways to get fancy here, but we already did that in SoundBuffer
	// so going old school
	virtual void* GetDataBuffer() = 0;

	// TODO: Merge the two?
	virtual RESULT GetAudioBuffer(float* &pAudioData_n) = 0;
	virtual RESULT GetAudioBuffer(float* &pAudioData_n, int numChannels) = 0;

protected:
	SoundFile::type m_soundFileType;

	wchar_t *m_pszFilename = nullptr;

public:
	static SoundFile *LoadSoundFile(const wchar_t *strFilename, SoundFile::type soundFileType);
};

#endif // SOUND_FILE_LOADER_H_