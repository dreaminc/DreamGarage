#include "OVRTouchController.h"
#include "Core/Utilities.h"

OVRTouchController::OVRTouchController(ovrSession ovrSession) :
	SenseController(),
	m_ovrSession(ovrSession)
{
	// empty
}

OVRTouchController::~OVRTouchController() {
	// empty
}

RESULT OVRTouchController::Initialize() {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

ovrControllerType OVRTouchController::GetOVRControllerType(ControllerType controllerType) {
	switch (controllerType) {
		case CONTROLLER_LEFT: return ovrControllerType::ovrControllerType_LTouch; break;
		case CONTROLLER_RIGHT: return ovrControllerType::ovrControllerType_RTouch; break;
	}

	return ovrControllerType::ovrControllerType_None;
}

// TODO: Create a longer queue buffer and provide update in OVR update

RESULT OVRTouchController::SubmitHapticImpulse(ControllerType controllerType, HapticCurveType shape, float amplitude, float msDuration, int cycles) {
	RESULT r = R_PASS;

	float freq = 1.0f / msDuration;
	util::Clamp(cycles, 1, (int)(800.0f / msDuration) + 1);

	CR(SubmitHapticBuffer(controllerType, shape, amplitude, freq, msDuration));

Error:
	return r;
}

RESULT OVRTouchController::SubmitHapticBuffer(ControllerType controllerType, HapticCurveType type, float amplitude, float freq, float msDuration) {
	RESULT r = R_PASS;

	ovrControllerType controllerTypeOVR = GetOVRControllerType(controllerType);

	float freqSampling = OVR_HAPTIC_FREQUENCY;

	// clamp freq to half sampling and not zero
	util::Clamp(freq, 0.1f, freqSampling / 2.0f);

	int bufferLength = (int)((msDuration / 1000.0f) * freqSampling);
	float vibrationPeriodSamples = (1.0f / freq) * freqSampling;
	std::vector<uint8_t> hapticBuffer;

	util::Clamp(amplitude, 0.0f, 1.0f);

	for (int i = 0; i < bufferLength; i++) {
		float sample;
		int periodSample = (i % (int)(vibrationPeriodSamples));
		
		switch (type) {
			case SenseController::HapticCurveType::SQUARE: {
				if (periodSample < (int)(vibrationPeriodSamples / 2.0f))
					sample = 1.0f;
				else
					sample = 0.0f;
			} break;

			case SenseController::HapticCurveType::SINE: {
				double sinVal = ((float)periodSample / vibrationPeriodSamples) * (2.0f * M_PI);
				sample = (std::sin(sinVal) / 2.0f) + 0.5f;
			} break;

			case SenseController::HapticCurveType::SAW: {
				sample = (float)(periodSample) / (vibrationPeriodSamples);
			} break;

			case SenseController::HapticCurveType::CONSTANT: {
				sample = 1.0f;
			} break;

			case SenseController::HapticCurveType::EXPONENTIAL: 
			default : {
				sample = 0.0f;
			} break;
		}

		util::Clamp(sample, 0.0f, 1.0f);
		uint8_t uSample = (uint8_t)((sample * amplitude) * 255);

		hapticBuffer.push_back(uSample);
	}

	CR(SubmitHapticBuffer(controllerTypeOVR, hapticBuffer));


	//ovr_SetControllerVibration

Error:
	return r;
}

RESULT OVRTouchController::SubmitHapticBuffer(ovrControllerType ovrControllerType, std::vector<uint8_t> hapticBuffer) {
	RESULT r = R_PASS;

	//uint8_t amplitude = (uint8_t)round(handTrigger[t] * 255);

	ovrHapticsPlaybackState ovrHapticsState;

	CRM((RESULT)ovr_GetControllerVibrationState(m_ovrSession, ovrControllerType, &ovrHapticsState), "Haptics skipped. Queue size %d", ovrHapticsState.SamplesQueued);

	/*
	for (int32_t i = 0; i < kLowLatencyBufferSizeInSamples; ++i) {
		samples.push_back(amplitude);
	}
	*/

	if (hapticBuffer.size() > 0) {
		ovrHapticsBuffer ovrHapticsBuffer;

		ovrHapticsBuffer.SubmitMode = ovrHapticsBufferSubmit_Enqueue;
		ovrHapticsBuffer.SamplesCount = (uint32_t)hapticBuffer.size();
		ovrHapticsBuffer.Samples = hapticBuffer.data();

		CRM((RESULT)ovr_SubmitControllerVibration(m_ovrSession, ovrControllerType, &ovrHapticsBuffer), "Haptics submit failed 0x%x", r);
	}

Error:
	return r;
}


