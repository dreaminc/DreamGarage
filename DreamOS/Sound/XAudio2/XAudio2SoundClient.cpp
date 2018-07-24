#include "XAudio2SoundClient.h"

#include <string>

XAudio2SoundClient::XAudio2SoundClient() {
	// empty
}

XAudio2SoundClient::~XAudio2SoundClient() {
	// Empty
}

// TODO:
RESULT XAudio2SoundClient::AudioSpatialProcess() {
	return R_NOT_IMPLEMENTED;
}

// TODO:
RESULT XAudio2SoundClient::AudioCaptureProcess() {
	return R_NOT_IMPLEMENTED;
}

// TODO:
RESULT XAudio2SoundClient::AudioRenderProcess() {
	return R_NOT_IMPLEMENTED;
}

// TODO:
RESULT XAudio2SoundClient::Initialize() {
	RESULT r = R_PASS;

	HRESULT hr = S_OK;

	DEBUG_LINEOUT("Initializing XAudio2 Sound Client");	

//Error:
	return R_NOT_IMPLEMENTED;
}