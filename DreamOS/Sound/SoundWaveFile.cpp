#include "SoundFileWave.h"

#include "Sandbox/PathManager.h"

SoundFileWave::SoundFileWave(const wchar_t *pszFilename) :
	SoundFile(pszFilename, SoundFile::type::WAVE)
{
	// empty
}

SoundFileWave::~SoundFileWave() {
	if (m_dataChunk.pChunkData != nullptr) {
		free(m_dataChunk.pChunkData);
		m_dataChunk.pChunkData = nullptr;
	}
}

RESULT SoundFileWave::Initialize() {
	RESULT r = R_PASS;
	size_t bytesRead = 0;

	// Open file from path
	FILE *pSoundFile = PathManager::instance()->OpenFile(PATH_SOUND, m_pszFilename, L"rb");
	CN(pSoundFile);

	// Read the RIFF Chunk
	bytesRead = sizeof(RIFFChunk);
	memset(&m_riffChunk, 0, sizeof(RIFFChunk));
	bytesRead = fread(&m_riffChunk, 1, sizeof(RIFFChunk), pSoundFile);
	CBM((bytesRead == sizeof(RIFFChunk)), "File read error");

	// Check RIFF chunk
	CBM((m_riffChunk.header.id == 0x46464952), "Riff Chunk Header ID Error");
	CBM((m_riffChunk.format == 0x45564157), "Riff chunk format not WAVE");

	// Format Chunk
	bytesRead = sizeof(FormatChunk);
	memset(&m_formatChunk, 0, sizeof(FormatChunk));
	bytesRead = fread(&m_formatChunk, 1, sizeof(FormatChunk), pSoundFile);
	CBM((bytesRead == sizeof(FormatChunk)), "File read error");

	// Check Format 
	CBM((m_formatChunk.header.id == 0x20746d66), "Format chunk ID incorrect");

	// Data Chunk
	memset(&m_dataChunk, 0, sizeof(DataChunk));

	// Data Chunk Header
	bytesRead = fread(&m_dataChunk.header, 1, sizeof(ChunkHeader), pSoundFile);
	CBM((bytesRead == sizeof(ChunkHeader)), "File read error");

	CBM((m_dataChunk.header.id == 0x61746164), "Data chunk header ID incorrect");

	// Copy in data
	size_t dataSize = m_dataChunk.header.size;
	m_dataChunk.pChunkData = (void*)malloc(dataSize);
	CN(m_dataChunk.pChunkData);

	bytesRead = fread(m_dataChunk.pChunkData, 1, dataSize, pSoundFile);
	CBM((bytesRead == dataSize), "Mismatch in data chunk size to bytes read");

	// Create a SoundBuffer from the data
	//m_pSoundBuffer = SoundBuffer::Make(m_formatChunk.numChannels, )

Error:
	if (pSoundFile != nullptr) {
		fclose(pSoundFile);
		pSoundFile = nullptr;
	}
	return r;
}