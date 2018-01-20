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

int SoundFileWave::NumChannels() {
	return m_formatChunk.numChannels;
}

int SoundFileWave::SamplingRate() {
	return m_formatChunk.sampleRate;
}

int SoundFileWave::BitsPerSample() {
	return m_formatChunk.bitsPerSample;
}

void* SoundFileWave::GetDataBuffer() {
	return m_dataChunk.pChunkData;
}

int SoundFileWave::GetNumFrames() {
	int frameSize = ((m_formatChunk.bitsPerSample / 8) * m_formatChunk.numChannels);
	int numFrames = m_dataChunk.header.size / frameSize;
	return numFrames;
}

int SoundFileWave::GetNumSamples() {
	int sampleSize = (m_formatChunk.bitsPerSample / 8);
	int numSamples = m_dataChunk.header.size / sampleSize;
	return numSamples;
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

Error:
	if (pSoundFile != nullptr) {
		fclose(pSoundFile);
		pSoundFile = nullptr;
	}
	return r;
}

RESULT SoundFileWave::GetAudioBuffer(float* &pAudioData_n) {
	RESULT r = R_PASS;
	int numSamples = 0;

	CBM((pAudioData_n == nullptr), "Non-null pointer provided");
	CBM((BitsPerSample() == 16), "Don't currently support conversion from %d bits", BitsPerSample());

	numSamples = GetNumSamples();

	pAudioData_n = new float[numSamples];
	CN(pAudioData_n);

	switch (BitsPerSample()) {
	case 16: {
		int16_t *pAudioData = (int16_t*)m_dataChunk.pChunkData;
		CN(pAudioData);

		for (int i = 0; i < numSamples; i++) {
			pAudioData_n[i] = (float)pAudioData[i] / ((float)std::numeric_limits<uint16_t>::max() - 1);
		}
	} break;

	default: {
		CBM((0), "Don't currently support conversion from %d bits", BitsPerSample());
	} break;
	}

Error:
	return r;
}