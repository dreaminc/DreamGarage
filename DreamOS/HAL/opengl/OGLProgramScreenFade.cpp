#include "OGLProgramScreenFade.h"

OGLProgramScreenFade::OGLProgramScreenFade(OpenGLImp *pParentImp) :
	OGLProgramScreenQuad(pParentImp, "oglscreenfade")
{
	// empty
}

RESULT OGLProgramScreenFade::OGLInitialize() {
	RESULT r = R_PASS;

	// register screen quad uniforms
	CR(OGLProgramScreenQuad::OGLInitialize());

	// additional uniforms for controlling fade
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformFadeColor), std::string("u_vec4FadeColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformFadeProgress), std::string("u_fadeProgress")));

Error:
	return r;
}

RESULT OGLProgramScreenFade::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;
	
	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"mathCommon.shader"), "Failed to add math global shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"screenfade.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"screenfade.frag"), "Failed to create fragment shader");

	// Link the program
	CRM(LinkProgram(), "Failed to link program");

	WCR(GetVertexAttributesFromProgram());
	WCR(BindAttributes());

	// Uniform Variables
	CR(GetUniformVariablesFromProgram());

	// Uniform Blocks
	CR(GetUniformBlocksFromProgram());
	CR(BindUniformBlocks());

Error:
	return r;
}

RESULT OGLProgramScreenFade::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	// TODO: update duration related to progress
	switch (m_fadeState) {

	case (FADE_OUT): {

		auto tNow = std::chrono::high_resolution_clock::now();
		double msDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tNow - m_startTime).count();
		double sDiff = msDiff / 1000.0f;
		m_fadeProgress = sDiff / m_fadeDurationSeconds;

		if (m_fadeProgress > 1) {
			m_fadeProgress = 1.0f;
			m_fadeState = NONE;
		}

	} break;

	case (FADE_IN): {

		auto tNow = std::chrono::high_resolution_clock::now();
		double msDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tNow - m_startTime).count();
		double sDiff = msDiff / 1000.0f;
		m_fadeProgress = 1.0f - (sDiff / m_fadeDurationSeconds);

		if (m_fadeProgress < 0) {
			m_fadeProgress = 0.0f;
			m_fadeState = NONE;
			// execute function after setting to NONE
		}

	} break;

	}

	CR(OGLProgramScreenQuad::ProcessNode(frameID));
	m_pUniformFadeColor->SetUniform(m_cFadeColor);
	m_pUniformFadeProgress->SetUniform(m_fadeProgress);

Error:
	return r;
}

RESULT OGLProgramScreenFade::FadeIn(std::function<RESULT(void*)> fnIn) {
	RESULT r = R_PASS;
	CBR(m_fadeState == NONE, R_SKIPPED);

	m_startTime = std::chrono::high_resolution_clock::now();
	m_fadeState = FADE_IN;
	m_fadeProgress = 1.0f;

Error:
	return r;
}

RESULT OGLProgramScreenFade::FadeOut(std::function<RESULT(void*)> fnOut) {
	RESULT r = R_PASS;
	CBR(m_fadeState == NONE, R_SKIPPED);

	m_startTime = std::chrono::high_resolution_clock::now();
	m_fadeState = FADE_OUT;
	m_fadeProgress = 0.0f;

Error:
	return r;
}

RESULT OGLProgramScreenFade::FadeOutIn(std::function<RESULT(void*)> fnOut, std::function<RESULT(void*)> fnIn) {
	RESULT r = R_PASS;
	CBR(m_fadeState == NONE, R_SKIPPED);

	m_startTime = std::chrono::high_resolution_clock::now();
	m_fadeState = FADE_OUT;

Error:
	return r;
}
