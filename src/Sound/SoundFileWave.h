#ifndef SOUND_FILE_WAVE_H_
#define SOUND_FILE_WAVE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundFileWave.h
// This is a WAVE sound file implementation

#include "SoundFile.h"

class SoundFileWave : public SoundFile {
public:
	SoundFileWave(const wchar_t *pszFilename);
	~SoundFileWave();

public:

	// No padding in these bitwise structs
#pragma pack(push, 1) 
	struct ChunkHeader {
		uint32_t id;
		uint32_t size;
	};

	struct RIFFChunk {
		ChunkHeader header;
		uint32_t format;
	};

	struct FormatChunk {
		ChunkHeader header;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
	};

	struct DataChunk {
		ChunkHeader header;
		void *pChunkData;
	};
#pragma pack(pop)

public:
	virtual RESULT Initialize() override;

	virtual int NumChannels() override;
	virtual int SamplingRate() override;
	virtual int BitsPerSample() override;
	virtual int BytesPerSample() override;
	virtual int GetNumFrames() override;
	virtual int GetNumSamples() override;

	virtual void* GetDataBuffer() override;

	virtual RESULT GetAudioBuffer(float* &pAudioData_n) override;
	virtual RESULT GetAudioBuffer(float* &pAudioData_n, int numChannels) override;

private:
	RIFFChunk m_riffChunk;
	FormatChunk m_formatChunk;
	DataChunk m_dataChunk;

};

#endif // SOUND_FILE_WAVE_H_