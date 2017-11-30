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
		INVALID
	};

	SoundFile(const wchar_t *pszFilename, SoundFile::type soundFileType);
	~SoundFile();

	virtual RESULT Initialize() = 0;
	
	SoundBuffer *GetSoundBuffer();

protected:
	SoundFile::type m_soundFileType;

	wchar_t *m_pszFilename = nullptr;
	SoundBuffer *m_pSoundBuffer = nullptr;

public:
	static SoundFile *LoadSoundFile(const wchar_t *strFilename, SoundFile::type soundFileType);
};

#endif // SOUND_FILE_LOADER_H_