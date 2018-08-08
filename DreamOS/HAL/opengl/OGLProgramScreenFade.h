#ifndef OGLPROGRAM_SCREEN_FADE_H_
#define OGLPROGRAM_SCREEN_FADE_H_

#include "./RESULT/EHM.h"
#include "OGLProgramScreenQuad.h"

#include <chrono>

#define FADE_DURATION_SECONDS 0.5f

typedef enum FadeState {
	FADE_IN,
	FADE_OUT,
	NONE
} FADE_STATE;

class OGLQuad;

class OGLProgramScreenFade : public OGLProgramScreenQuad {
public:
	OGLProgramScreenFade(OpenGLImp *pParentImp);
	
	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;
	virtual RESULT ProcessNode(long frameID) override;
	
	// fade control functions
public:
	RESULT FadeIn(std::function<RESULT(void*)> fnIn = nullptr);
	RESULT FadeOut(std::function<RESULT(void*)> fnOut = nullptr);
	RESULT FadeOutIn(std::function<RESULT(void*)> fnOut = nullptr, std::function<RESULT(void*)> fnIn = nullptr);

	// fade control uniforms
private:
	OGLUniformVector *m_pUniformFadeColor = nullptr;
	OGLUniformFloat *m_pUniformFadeProgress = nullptr;

	// local fade logic variables
private:
	color m_cFadeColor = color(COLOR_BLACK);
	float m_fadeDurationSeconds = FADE_DURATION_SECONDS;
	float m_fadeProgress = 0.0f;

	FadeState m_fadeState = FadeState::NONE;
	std::chrono::high_resolution_clock::time_point m_startTime;
};

#endif // ! OGLPROGRAM_SCREEN_FADE_H_