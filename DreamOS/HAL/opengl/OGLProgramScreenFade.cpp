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

	m_pUniformFadeColor->SetUniform(m_vFadeColor);

	// TODO: update duration related to progress

	CR(OGLProgramScreenQuad::ProcessNode(frameID));

Error:
	return r;
}

RESULT OGLProgramScreenFade::FadeIn() {
	return R_PASS;
}

RESULT OGLProgramScreenFade::FadeOut() {
	return R_PASS;
}
