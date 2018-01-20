#include "SoundFile.h"

#include "SoundFileWave.h"

SoundFile::SoundFile(const wchar_t *pszFilename, SoundFile::type soundFileType) :
	m_soundFileType(soundFileType),
	m_pszFilename(const_cast<wchar_t*>(pszFilename))
{
	// empty
}

SoundFile::~SoundFile() {
	// empty
}

SoundFile *SoundFile::LoadSoundFile(const wchar_t *strFilename, SoundFile::type soundFileType) {
	SoundFile *pSoundFile = nullptr;
	RESULT r = R_PASS;

	switch (soundFileType) {
		case SoundFile::type::WAVE: {
			pSoundFile = new SoundFileWave(strFilename);
			CN(pSoundFile);
		} break;

		default: {
			DEBUG_LINEOUT("Sound file type %x not supported", soundFileType);
		} break;
	}

	CR(pSoundFile->Initialize());

	return pSoundFile;

Error:
	if (pSoundFile != nullptr) {
		delete pSoundFile;
		pSoundFile = nullptr;
	}

	return pSoundFile;
}