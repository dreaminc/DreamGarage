#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

#include <thread>

class SoundClient {
public:
	enum class state {
		UNINITIALIZED,
		STOPPED,
		RUNNING,
		INVALID
	};

public:
	SoundClient();
	~SoundClient();

	virtual RESULT Initialize() = 0;
	
protected:
	virtual RESULT AudioProcess() = 0;

	SoundClient::state m_state = SoundClient::state::UNINITIALIZED;

public:
	bool IsRunning();
	RESULT Start();
	RESULT Stop();

private:
	std::thread	m_audioProcessingThread;
};

#endif SOUND_CLIENT_H_