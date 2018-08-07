#ifndef OGLPROGRAM_SCREEN_FADE_H_
#define OGLPROGRAM_SCREEN_FADE_H_

#include "./RESULT/EHM.h"
#include "OGLProgramScreenQuad.h"

#define FADE_DURATION_SECONDS 0.5f

class OGLQuad;

class OGLProgramScreenFade : public OGLProgramScreenQuad {
public:
	OGLProgramScreenFade(OpenGLImp *pParentImp);
	
	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;
	virtual RESULT ProcessNode(long frameID) override;
	
	// fade control functions
public:
	RESULT FadeIn();
	RESULT FadeOut();

	// fade control uniforms
private:
	OGLUniformVector *m_pUniformFadeColor = nullptr;
	OGLUniformFloat *m_pUniformFadeProgress = nullptr;

	// local fade logic variables
private:
	color m_vFadeColor = color(COLOR_BLACK);
	float m_fadeDurationSeconds = FADE_DURATION_SECONDS;
	float m_fadeProgress = 0.0f;
};

#endif // ! OGLPROGRAM_SCREEN_FADE_H_