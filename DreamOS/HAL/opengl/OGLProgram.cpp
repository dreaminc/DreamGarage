#include "OGLProgram.h"
#include "OpenGLImp.h"
#include "OGLObj.h"

#include "OGLFramebuffer.h"
#include "OGLAttachment.h"
#include "OGLTexture.h"

#include "Scene/ObjectStore.h"

OGLProgram::OGLProgram(OpenGLImp *pParentImp, std::string strName) :
	ProgramNode(strName),
	m_pParentImp(pParentImp),
	m_OGLProgramIndex(NULL),
	m_pVertexShader(nullptr),
	m_pFragmentShader(nullptr),
	m_versionOGL(0),
	m_pOGLFramebuffer(nullptr),
	m_pOGLRenderTexture(nullptr)
{
	// empty
}

OGLProgram::~OGLProgram() {
	ReleaseProgram();
}

RESULT OGLProgram::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	// TODO: Do stuff

	CR(r);

Error:
	return r;
}

// Note that all vertex attrib, uniforms, uniform blocks are actually 
// allocated in the OGLInitialize function
RESULT OGLProgram::OGLInitialize() {
	RESULT r = R_PASS;

	CR(CreateProgram());
	CR(IsProgram());

Error:
	return r;
}

RESULT OGLProgram::SetLights(ObjectStore *pSceneGraph) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = pSceneGraph->GetSceneGraphStore();
	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);

	CR(SetLights(pLights));

Error:
	return r;
}

RESULT OGLProgram::SetLights(std::vector<light*> *pLights) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgram::SetMaterial(material *pMaterial) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgram::UpdateUniformBlockBuffers() {
	RESULT r = R_PASS;

	for (auto const& oglUniformBlock : m_uniformBlocks) {
		CRM(oglUniformBlock->UpdateOGLUniformBlockBuffers(), "Failed to bind %s uniform block", oglUniformBlock->GetUniformBlockName());
	}

Error:
	return r;
}

RESULT OGLProgram::BindToDepthBuffer() {
	RESULT r = R_PASS;

	CR(m_pOGLFramebuffer->Bind());
	CR(m_pOGLFramebuffer->SetAndClearViewport(true, true));

Error:
	return r;
}

RESULT OGLProgram::BindToFramebuffer(OGLFramebuffer* pFramebuffer) {
	RESULT r = R_PASS;

	//CR(m_pOGLFramebuffer->BindOGLFramebuffer());

	// Render to our framebuffer
	// By default, uses the member framebuffer
	OGLFramebuffer* pOGLFramebuffer = (pFramebuffer == nullptr) ? m_pOGLFramebuffer : pFramebuffer;
	CR(pOGLFramebuffer->Bind());
	CR(pOGLFramebuffer->SetAndClearViewport(true, true));

	// Check framebuffer
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLProgram::UnbindFramebuffer() {
	RESULT r = R_PASS;

	//CR(m_pOGLFramebuffer->UnbindOGLFramebuffer());

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, 0));

Error:
	return r;
}

RESULT OGLProgram::BindToScreen(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CR(m_pOGLFramebuffer->BindToScreen(pxWidth, pxHeight));

Error:
	return r;
}

RESULT OGLProgram::InitializeRenderToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	CR(InitializeFrameBuffer(internalDepthFormat, typeDepth, pxWidth, pxHeight, channels));

Error:
	return r;
}

/*
RESULT OGLProgram::SetDepthTexture(int textureNumber) {
	return m_pOGLFramebuffer->SetDepthTexture(textureNumber);
}

GLuint OGLProgram::GetOGLDepthbufferIndex() {
	return m_pOGLFramebuffer->GetOGLDepthbufferIndex();
}
*/

RESULT OGLProgram::InitializeDepthToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	CR(InitializeDepthFrameBuffer(internalDepthFormat, typeDepth, pxWidth, pxHeight));

Error:
	return r;
}

RESULT OGLProgram::InitializeDepthFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight) {
	return InitializeDepthFrameBuffer(m_pOGLFramebuffer, internalDepthFormat, typeDepth, pxWidth, pxHeight);
}

// TODO: This is not generic, hacking right now to get shadows to work first then will generalize

RESULT OGLProgram::InitializeFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, channels);
	CN(pOGLFramebuffer);

	CR(SetFrameBuffer(pOGLFramebuffer, internalDepthFormat, typeDepth, pxWidth, pxHeight, channels));

Error:
	return r;
}

RESULT OGLProgram::InitializeFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	RESULT r = R_PASS;

	CN(m_pParentImp);
	{
		int pxWidth = m_pParentImp->GetViewport().Width();
		int pxHeight = m_pParentImp->GetViewport().Height();

		CR(InitializeFrameBuffer(pOGLFramebuffer, internalDepthFormat, typeDepth, pxWidth, pxHeight, channels));
	}

Error:
	return r;
}

RESULT OGLProgram::InitializeFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	RESULT r = R_PASS;

	CN(m_pParentImp);
	{
		int pxWidth = m_pParentImp->GetViewport().Width();
		int pxHeight = m_pParentImp->GetViewport().Height();

		CR(InitializeFrameBuffer(m_pOGLFramebuffer, internalDepthFormat, typeDepth, pxWidth, pxHeight, channels));
	}

Error:
	return r;
}

RESULT OGLProgram::InitializeFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	return InitializeFrameBuffer(m_pOGLFramebuffer, internalDepthFormat, typeDepth, pxWidth, pxHeight, channels);
}

RESULT OGLProgram::UpdateFramebufferToCamera(OGLFramebuffer*&pOGLFramebuffer, camera *pCamera, GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	RESULT r = R_PASS;

	int pxWidth = pCamera->GetViewWidth();
	int pxHeight = pCamera->GetViewHeight();

	CN(pOGLFramebuffer);

	if (pOGLFramebuffer != nullptr) {
		if (pOGLFramebuffer->GetWidth() != pxWidth || pOGLFramebuffer->GetHeight() != pxHeight) {
			return pOGLFramebuffer->Resize(pxWidth, pxHeight, internalDepthFormat, typeDepth);
		}
	}

Error:
	return r;
}

RESULT OGLProgram::UpdateFramebufferToCamera(camera *pCamera, GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	return UpdateFramebufferToCamera(m_pOGLFramebuffer, pCamera, internalDepthFormat, typeDepth, channels);
}

RESULT OGLProgram::UpdateFramebufferToViewport(GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	return UpdateFramebufferToViewport(m_pOGLFramebuffer, internalDepthFormat, typeDepth, channels);
}

RESULT OGLProgram::UpdateFramebufferToViewport(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	RESULT r = R_PASS;

	int pxWidth = m_pParentImp->GetViewport().Width();
	int pxHeight = m_pParentImp->GetViewport().Height();

	CN(pOGLFramebuffer);

	if (pOGLFramebuffer != nullptr) {
		if (pOGLFramebuffer->GetWidth() != pxWidth || pOGLFramebuffer->GetHeight() != pxHeight) {
			return pOGLFramebuffer->Resize(pxWidth, pxHeight, internalDepthFormat, typeDepth);
		}
	}

Error:
	return r;
}

RESULT OGLProgram::InitializeFrameBufferWithDepth(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int channels) {
	RESULT r = R_PASS;
	CN(m_pParentImp);

	{
		int pxWidth = m_pParentImp->GetViewport().Width();
		int pxHeight = m_pParentImp->GetViewport().Height();

		pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, channels);
		
		CR(pOGLFramebuffer->OGLInitialize());
		CR(pOGLFramebuffer->Bind());
		CR(pOGLFramebuffer->MakeColorAttachment());

		CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
		CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));
		//CR(pOGLFramebuffer->SetOGLTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

		CR(pOGLFramebuffer->MakeDepthAttachment());		// Note: This will create a new depth buffer
		CR(pOGLFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(internalDepthFormat, typeDepth));
		//CR(pOGLFramebuffer->InitializeDepthAttachment(internalDepthFormat, typeDepth));

		CR(pOGLFramebuffer->InitializeOGLDrawBuffers(1));

		// Always check that our framebuffer is ok
		CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));
	}

Error:
	return r;
}

RESULT OGLProgram::InitializeDepthFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 1);
	CN(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeDepthAttachment());		// Note: This will create a new depth buffer
	CR(pOGLFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(internalDepthFormat, typeDepth));
	//CR(pOGLFramebuffer->InitializeDepthAttachment(internalDepthFormat, typeDepth));

	//CR(pOGLFramebuffer->SetOGLDepthbufferTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));
	CR(pOGLFramebuffer->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(pOGLFramebuffer->InitializeOGLDrawBuffers(0));

	// Check that our framebuffer is OK
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLProgram::SetFrameBuffer(OGLFramebuffer *pFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	/*
	CN(pFramebuffer);
	
	CR(pFramebuffer->OGLInitialize());	
	CR(pFramebuffer->Bind());

	CR(pFramebuffer->MakeColorAttachment());
	CR(pFramebuffer->GetColorAttachment()->MakeOGLTexture());

	CR(pFramebuffer->MakeDepthAttachment());		// Note: This will create a new depth buffer
	CR(pFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());

	//CR(pFramebuffer->SetOGLTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));
	CR(pFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(pFramebuffer->InitializeOGLDrawBuffers(1));

	// Always check that our framebuffer is ok
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));
	*/

	CN(pFramebuffer);

	CR(pFramebuffer->OGLInitialize());
	CR(pFramebuffer->Bind());

	// Color attachment
	CR(pFramebuffer->MakeColorAttachment());
	CR(pFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CR(pFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	// Depth attachment 
	CR(pFramebuffer->MakeDepthAttachment());
	CR(pFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CR(pFramebuffer->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(pFramebuffer->InitializeOGLDrawBuffers(1));

	// Check that our framebuffer is OK
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLProgram::InitializeRenderTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	return InitializeRenderTexture(m_pOGLRenderTexture, internalDepthFormat, typeDepth, pxWidth, pxHeight, channels);
}

RESULT OGLProgram::InitializeRenderTexture(OGLTexture*&pOGLRenderTexture, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	pOGLRenderTexture = new OGLTexture(m_pParentImp, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE); 
	CN(pOGLRenderTexture);

	CR(pOGLRenderTexture->SetWidth(pxWidth));
	CR(pOGLRenderTexture->SetHeight(pxWidth));
	CR(pOGLRenderTexture->SetChannels(channels));

	CR(pOGLRenderTexture->OGLInitializeTexture(GL_TEXTURE_2D, 0, internalDepthFormat, GL_DEPTH_COMPONENT, typeDepth));

	CR(pOGLRenderTexture->Bind());

	CR(pOGLRenderTexture->SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CR(pOGLRenderTexture->SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CR(pOGLRenderTexture->SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CR(pOGLRenderTexture->SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

Error:
	return r;
}

RESULT OGLProgram::BindUniformBlocks() {
	RESULT r = R_PASS;

	//CRM(m_pLightsBlock->BindUniformBlock(), "Failed to bind %s to lights uniform block", GetLightsUniformBlockName());
	for (auto const& oglUniformBlock : m_uniformBlocks) {
		CRM(oglUniformBlock->BindUniformBlock(), "Failed to bind %s uniform block", oglUniformBlock->GetUniformBlockName());
	}

Error:
	return r;
}

RESULT OGLProgram::InitializeUniformBlocks() {
	RESULT r = R_PASS;

	for (auto const& oglUniformBlock : m_uniformBlocks) {
		CRM(oglUniformBlock->OGLInitialize(), "Failed to bind %s uniform block", oglUniformBlock->GetUniformBlockName());
	}

Error:
	return r;
}

/*
RESULT OGLProgram::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	CR(m_pVertexShader->SetLights(pLights));
	CR(m_pVertexShader->UpdateUniformBlockBuffers());

Error:
	return r;
}
*/

RESULT OGLProgram::OGLInitialize(const wchar_t *pszVertexShaderFilename, const wchar_t *pszFragmentShaderFilename, version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders
	CRM(MakeVertexShader(pszVertexShaderFilename), "Failed to create vertex shader");
	CRM(MakeFragmentShader(pszFragmentShaderFilename), "Failed to create fragment shader");

	// Link the program
	CRM(LinkProgram(), "Failed to link program");

	// TODO: This could all be done in one call in the OGLShader honestly
	// Attributes
	// TODO: Tabulate attributes (get them from shader, not from class)
	WCR(GetVertexAttributesFromProgram());
	WCR(BindAttributes());
	//WCR(m_pVertexShader->GetAttributeLocationsFromShader());
	//WCR(m_pVertexShader->EnableAttributes());

	//CR(PrintActiveAttributes());

	// Uniform Variables
	CR(GetUniformVariablesFromProgram());

	// Uniform Blocks
	CR(GetUniformBlocksFromProgram());
	CR(BindUniformBlocks());
//	CR(InitializeUniformBlocks());

	//WCR(m_pVertexShader->GetUniformLocationsFromShader());
	//WCR(m_pVertexShader->BindUniformBlocks());
	//WCR(m_pVertexShader->InitializeUniformBlocks());

	//WCR(m_pFragmentShader->GetUniformLocationsFromShader());
	//WCR(m_pFragmentShader->BindUniformBlocks());
	//WCR(m_pFragmentShader->InitializeUniformBlocks());


	// TODO:  Currently using a global material 
	SetMaterial(&material(60.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));

Error:
	return r;
}

// TODO: Just a pass through, but might make sense to absorb the functionality here
// or stuff it into the OGLShader
RESULT OGLProgram::CreateShader(GLenum type, GLuint *pShaderID) {
	RESULT r = R_PASS;

	CR(m_pParentImp->CreateShader(type, pShaderID));

Error:
	return r;
}

RESULT OGLProgram::IsProgram() {
	return m_pParentImp->IsProgram(m_OGLProgramIndex);
}

RESULT OGLProgram::CreateProgram() {
	RESULT r = R_PASS;

	CBM((m_OGLProgramIndex == NULL), "Cannot CreateGLProgram if program id not null");

	CR(m_pParentImp->CreateProgram(&m_OGLProgramIndex));
	CBM((m_OGLProgramIndex != 0), "Failed to create OGLProgram");

	DEBUG_LINEOUT("CREATED program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::ReleaseProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->DeleteProgram(m_OGLProgramIndex));

	DEBUG_LINEOUT("DELETED program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::LinkProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->LinkProgram(m_OGLProgramIndex));

	GLint param = GL_FALSE;
	CR(m_pParentImp->glGetProgramiv(m_OGLProgramIndex, GL_LINK_STATUS, &param));
	CBM((param == GL_TRUE), "Failed to link GL Program: %s", GetProgramInfoLog());

	DEBUG_LINEOUT("Successfully linked program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::UseProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->UseProgram(m_OGLProgramIndex));

	//DEBUG_LINEOUT("USING program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

// Vertex Attributes
RESULT OGLProgram::RegisterVertexAttribute(OGLVertexAttribute **pOGLVertexAttribute, std::string strVertexAttributeName) {
	RESULT r = R_PASS;

	auto it = m_registeredProgramShaderVertexAttribute.find(strVertexAttributeName);
	CBM((it == m_registeredProgramShaderVertexAttribute.end()), "Uniform %s already registered", strVertexAttributeName.c_str());

	m_registeredProgramShaderVertexAttribute[strVertexAttributeName] = (pOGLVertexAttribute);

Error:
	return r;
}

RESULT OGLProgram::InitializeAttributes() {
	RESULT r = R_PASS;

	CR(GetVertexAttributesFromProgram());
	CR(BindAttributes());
	CR(EnableAttributes());

Error:
	return r;
}

RESULT OGLProgram::BindAttributes() {
	RESULT r = R_PASS;

	for (auto const& oglVertexAttribute : m_vertexAttributes) {
		CR(oglVertexAttribute->BindAttribute());
	}

Error:
	return r;
}

// TODO: Don't have this hard coded 
RESULT OGLProgram::EnableAttributes() {
	RESULT r = R_PASS;

	for (auto const& oglVertexAttribute : m_vertexAttributes) {
		CR(oglVertexAttribute->EnableAttribute());
	}

Error:
	return r;
}


RESULT OGLProgram::GetVertexAttributesFromProgram() {
	RESULT r = R_PASS;

	GLint attributes_n;
	CR(m_pParentImp->glGetProgramInterfaceiv(m_OGLProgramIndex, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &attributes_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	DEBUG_LINEOUT("%d active attributes", attributes_n);
	for (int i = 0; i < attributes_n; i++) {
		GLint results[3];
		CR(m_pParentImp->glGetProgramResourceiv(m_OGLProgramIndex, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results));

		GLint pszName_n = results[0] + 1;
		char *pszName = new char[pszName_n];
		CR(m_pParentImp->glGetProgramResourceName(m_OGLProgramIndex, GL_PROGRAM_INPUT, i, pszName_n, nullptr, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", results[2], pszName, OpenGLUtility::GetOGLTypeString(results[1]));

		OGLVertexAttribute *pOGLVertexAttribute = new OGLVertexAttribute(this, pszName, results[2], results[1]);
		m_vertexAttributes.push_back(pOGLVertexAttribute);

		auto it = m_registeredProgramShaderVertexAttribute.find(std::string(pszName));
		if (it != m_registeredProgramShaderVertexAttribute.end()) {
			*(it->second) = pOGLVertexAttribute;
			DEBUG_LINEOUT("%s Vertex Attribute found in OGLProgram registry", pszName);
		}

		if (pszName != nullptr) {
			delete[] pszName;
			pszName = nullptr;
		}
	}

Error:
	return r;
}

// Uniform Blocks

RESULT OGLProgram::BindUniformBlock(GLint uniformBlockIndex, GLint uniformBlockBindingPoint) {
	RESULT r = R_PASS;
//	GLenum glerr;
	DWORD werr;

	CR(m_pParentImp->glUniformBlockBinding(m_OGLProgramIndex, uniformBlockIndex, uniformBlockBindingPoint));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound uniform block index %d to binding point %d err:0x%x", uniformBlockIndex, uniformBlockBindingPoint, werr);

Error:
	return r;
}

RESULT OGLProgram::RegisterUniform(OGLUniform **pOGLUniform, std::string strUniformName) {
	RESULT r = R_PASS;

	auto it = m_registeredProgramShaderUniforms.find(strUniformName);
	CBM((it == m_registeredProgramShaderUniforms.end()), "Uniform %s already registered", strUniformName.c_str());

	m_registeredProgramShaderUniforms[strUniformName] = (pOGLUniform);

Error:
	return r;
}

RESULT OGLProgram::RegisterUniformBlock(OGLUniformBlock **pOGLUniformBlock, std::string strUniformBlockName) {
	RESULT r = R_PASS;

	auto it = m_registeredProgramShaderUniformBlocks.find(strUniformBlockName);
	CBM((it == m_registeredProgramShaderUniformBlocks.end()), "Uniform Block %s already registered", strUniformBlockName.c_str());

	m_registeredProgramShaderUniformBlocks[strUniformBlockName] = (pOGLUniformBlock);

Error:
	return r;
}

RESULT OGLProgram::GetUniformVariablesFromProgram() {
	RESULT r = R_PASS;

	GLint variables_n = 0;
	CR(m_pParentImp->glGetProgramInterfaceiv(m_OGLProgramIndex, GL_UNIFORM, GL_ACTIVE_RESOURCES, &variables_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
	int properties_n = sizeof(properties) / sizeof(properties[0]);

	DEBUG_LINEOUT("%d active uniform variables", variables_n);
	for (int i = 0; i < variables_n; i++) {
		GLint *pResults = new GLint[properties_n];
		CR(m_pParentImp->glGetProgramResourceiv(m_OGLProgramIndex, GL_UNIFORM, i, 4, properties, 4, nullptr, pResults));

		// Skip uniforms in blocks
		if (pResults[3] != -1) continue;

		GLint pszName_n = pResults[0] + 1;
		char *pszName = new char[pszName_n];
		CR(m_pParentImp->glGetProgramResourceName(m_OGLProgramIndex, GL_UNIFORM, i, pszName_n, nullptr, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", pResults[2], pszName, OpenGLUtility::GetOGLTypeString(pResults[1]));

		OGLUniform *pOGLUniform = new OGLUniform(this, pszName, pResults[2], pResults[1]);
		m_uniformVariables.push_back(pOGLUniform);

		auto it = m_registeredProgramShaderUniforms.find(std::string(pszName));
		if (it != m_registeredProgramShaderUniforms.end()) {
			*(it->second) = pOGLUniform;
		}
		else {
			DEBUG_LINEOUT("Warning: %s Uniform NOT found in OGLProgram registry", pszName);
		}

		if (pszName != nullptr) {
			delete[] pszName;
			pszName = nullptr;
		}
		
		if (pResults != nullptr) {
			delete[] pResults;
			pResults = nullptr;
		}
	}

Error:
	return r;
}

RESULT OGLProgram::GetUniformBlocksFromProgram() {
	RESULT r = R_PASS;

	GLint variables_n = 0;
	CR(m_pParentImp->glGetProgramInterfaceiv(m_OGLProgramIndex, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &variables_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_BUFFER_DATA_SIZE};
	int properties_n = sizeof(properties) / sizeof(properties[0]);

	DEBUG_LINEOUT("%d active uniform blocks", variables_n);
	for (int i = 0; i < variables_n; i++) {
		GLint *pResults = new GLint[properties_n];
		CR(m_pParentImp->glGetProgramResourceiv(m_OGLProgramIndex, GL_UNIFORM_BLOCK, i, properties_n, properties, 4, nullptr, pResults));

		GLint pszName_n = pResults[0] + 1;
		char *pszName = new char[pszName_n];
		CR(m_pParentImp->glGetProgramResourceName(m_OGLProgramIndex, GL_UNIFORM_BLOCK, i, pszName_n, nullptr, pszName));

		// For debug - can remove if needed or wrap in debug
		GLint uniformBlockIndex = 0;
		CR(m_pParentImp->glGetUniformBlockIndex(m_OGLProgramIndex, pszName, &uniformBlockIndex));
		CBM((uniformBlockIndex == i), "Uniform location index %d and counter %d mistmatch", uniformBlockIndex, i);

		DEBUG_LINEOUT("%-5d %s block index size %d", i, pszName, pResults[1]);

		OGLUniformBlock  *pOGLUniformBlock = new OGLUniformBlock(this, pResults[1], i, pszName);
		CRM(pOGLUniformBlock->OGLInitialize(), "Failed to bind %s uniform block", pszName);
		m_uniformBlocks.push_back(pOGLUniformBlock);

		auto it = m_registeredProgramShaderUniformBlocks.find(std::string(pszName));
		if (it != m_registeredProgramShaderUniformBlocks.end()) {
			*(it->second) = pOGLUniformBlock;
		}
		else {
			DEBUG_LINEOUT("Warning: %s Uniform Block NOT found in OGLProgram registry", pszName);
		}

		if (pszName != nullptr) {
			delete[] pszName;
			pszName = nullptr;
		}

		if (pResults != nullptr) {
			delete[] pResults;
			pResults = nullptr;
		}
	}

Error:
	return r;
}

RESULT OGLProgram::BindAttribLocation(GLint index, const char* pszName) {
	RESULT r = R_PASS;
	DWORD werr;

	CR(m_pParentImp->glBindAttribLocation(m_OGLProgramIndex, index, pszName));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound attribute %s to index location %d err:0x%x", pszName, index, werr);

Error:
	return r;
}

char* OGLProgram::GetProgramInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = nullptr;
	int pszInfoLog_n = 4096;
	int charsWritten_n = -1;

	CR(m_pParentImp->glGetProgramiv(m_OGLProgramIndex, GL_INFO_LOG_LENGTH, &pszInfoLog_n));
	CBM((pszInfoLog_n > 0), "Program Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	memset(pszInfoLog, 0, sizeof(char) * pszInfoLog_n);
	CR(m_pParentImp->glGetProgramInfoLog(m_OGLProgramIndex, pszInfoLog_n, &charsWritten_n, pszInfoLog));

Error:
	return pszInfoLog;
}

/*
RESULT OGLProgram::MakeShader(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;



Error:
	return r;
}
*/

// TODO: This is repeated functionality - should create MakeShader function 
// that either takes a shader type or uses the extension

RESULT OGLProgram::MakeVertexShader(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	OGLVertexShader *pVertexShader = new OGLVertexShader(this);
	CN(pVertexShader);
	CRM(m_pParentImp->CheckGLError(), "Create OpenGL Vertex Shader failed");

	CRM(pVertexShader->InitializeFromFile(pszFilename, m_versionOGL), "Failed to initialize vertex shader from file");

	CRM(AttachShader(pVertexShader), "Failed to attach vertex shader");

Error:
	return r;
}

RESULT OGLProgram::MakeFragmentShader(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	OGLFragmentShader *pFragmentShader = new OGLFragmentShader(this);
	CN(pFragmentShader);
	CRM(m_pParentImp->CheckGLError(), "Create OpenGL Fragment Shader failed");

	CRM(pFragmentShader->InitializeFromFile(pszFilename, m_versionOGL), "Failed to initialize fragment shader from file");

	CRM(AttachShader(pFragmentShader), "Failed to attach fragment shader");

Error:
	return r;
}

RESULT OGLProgram::RenderObjectStoreBoundingVolumes(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStoreImp = pObjectStore->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	pObjectStore->Reset();
	while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != NULL) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);
		
		if (pDimObj == NULL) {
			continue;
		}
		else {
			CR(RenderObjectBoundingVolume(pDimObj));
		}
	}

Error:
	return r;
}

// Critical Path, EHM removed
// Debug manually
RESULT OGLProgram::RenderObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;
	
	ObjectStoreImp *pObjectStoreImp = pObjectStore->GetSceneGraphStore();
	VirtualObj *pVirtualObj = nullptr;

	pObjectStore->Reset();

	while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != nullptr) {
		if (pVirtualObj != nullptr && pVirtualObj->IsVisible() == true) {
			RenderObject((DimObj*)pVirtualObj);
		}
	}

//Error:
	return r;
}

RESULT OGLProgram::RenderObjectBoundingVolume(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// TODO: Temp - this might want to be a separate flag
	if (pDimObj->IsVisible() == false)
		return R_PASS;

	OGLObj *pOGLObj = dynamic_cast<OGLObj*>(pDimObj);
	//OGLObj *pOGLObj = reinterpret_cast<OGLObj*>(pDimObj);

	if (pOGLObj != nullptr) {
		// TODO: This is a bit wonky, RenderBoundingVolume creates the OGL Bounding volume 
		// which might not be the right flow
		if (pOGLObj->GetOGLBoundingVolume() != nullptr) {
			// Update bounding volume:
			pOGLObj->UpdateOGLBoundingVolume();
			SetMaterial(&material()); // refgeo material
			SetObjectUniforms(pOGLObj->GetOGLBoundingVolume());
		}

		// This is called even when bounding volume is null since it'll create the reference geometry
		CR(pOGLObj->RenderOGLBoundingVolume());
	}

	if (pDimObj->HasChildren()) {
		CR(RenderChildrenBoundingVolumes(pDimObj));
	}

Error:
	return r;
}

RESULT OGLProgram::RenderObject(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// TODO: Remove this dynamic cast
	OGLObj *pOGLObj = dynamic_cast<OGLObj*>(pDimObj);

	// IsVisible will return false for Virtual Objects
	if (pOGLObj != nullptr) {

		// Update buffers if marked as dirty
		if (pDimObj->CheckAndCleanDirty()) {
			pOGLObj->UpdateOGLBuffers();
		}

		// TODO: This should be replaced with a materials store or OGLMaterial that 
		// preallocates and swaps binding points (Wait for textures)
		SetObjectUniforms(pDimObj);
		SetMaterial(pDimObj->GetMaterial());
		SetObjectTextures(pOGLObj);	// TODO: Should this be absorbed by SetObjectUniforms?
		
		std::function<RESULT(OGLProgram*, void*)> fnObjectCallback = nullptr;
		if ((fnObjectCallback = pOGLObj->GetOGLProgramPreCallback()) != nullptr) {
			CR(fnObjectCallback(this, nullptr));
		}

		pOGLObj->Render();

		if ((fnObjectCallback = pOGLObj->GetOGLProgramPostCallback()) != nullptr) {
			CR(fnObjectCallback(this, nullptr));
		}
	}

	if (pDimObj->HasChildren()) {
		CR(RenderChildren(pDimObj));
	}

Error:
	return r;
}

RESULT OGLProgram::RenderChildrenBoundingVolumes(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// TODO: Rethink this since it's in the critical path
	auto objects = pDimObj->GetChildren();

	for (auto &pVirtualObj : objects) {
		DimObj *pChildDimObj = dynamic_cast<DimObj*>(pVirtualObj.get());

		if (pChildDimObj != nullptr) {
			CR(RenderObjectBoundingVolume(pChildDimObj));
		}
	}

Error:
	return r;
}

RESULT OGLProgram::RenderChildren(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// TODO: Rethink this since it's in the critical path
	for (auto &pVirtualObj : pDimObj->GetChildren()) {
		//auto pDimObjChild = std::dynamic_pointer_cast<DimObj>(pVirtualObj);
		//CR(RenderObject(pDimObjChild.get()));

		if (pVirtualObj->IsVisible() == true)
			RenderObject((DimObj*)(pVirtualObj.get()));
	}

//Error:
	return r;
}

/*
RESULT OGLProgram::RenderObject(VirtualObj *pVirtualObj) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

	if (pDimObj != nullptr) {
		return RenderObject(pDimObj);
	}

	return R_FAIL;
}
*/

// TODO: Consolidate?
RESULT OGLProgram::SetStereoCamera(stereocamera* pStereoCamera, EYE_TYPE eye) {
	RESULT r = R_PASS;

	/*
	auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix();
	auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
	auto matViewOrientation = pStereoCamera->GetOrientationMatrix();

	WCR(m_pVertexShader->SetViewMatrixUniform(matV));
	WCR(m_pVertexShader->SetProjectionMatrixUniform(matP));
	WCR(m_pVertexShader->SetViewOrientationMatrixUniform(matViewOrientation));
	WCR(m_pVertexShader->SetViewProjectionMatrixUniform(matVP));
	WCR(m_pVertexShader->SetEyePositionUniform(ptEye));
	*/

	CR(SetCameraUniforms(pStereoCamera, eye));

Error:
	return r;
}

RESULT OGLProgram::SetCamera(camera *pCamera) {
	RESULT r = R_PASS;

	/*
	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	auto matViewOrientation = pCamera->GetOrientationMatrix();

	WCR(m_pVertexShader->SetViewMatrixUniform(matV));
	WCR(m_pVertexShader->SetProjectionMatrixUniform(matP));
	WCR(m_pVertexShader->SetViewOrientationMatrixUniform(matViewOrientation));
	WCR(m_pVertexShader->SetViewProjectionMatrixUniform(matVP));
	WCR(m_pVertexShader->SetEyePositionUniform(ptEye));
	*/

	CR(SetCameraUniforms(pCamera));

Error:
	return r;
}

RESULT OGLProgram::AttachShader(OpenGLShader *pOpenGLShader) {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	switch (pOpenGLShader->GetShaderType()) {
		case GL_VERTEX_SHADER: {
			this->m_pVertexShader = dynamic_cast<OGLVertexShader*>(pOpenGLShader);
			CN(this->m_pVertexShader);
		} break;

		case GL_FRAGMENT_SHADER: {
			this->m_pFragmentShader = dynamic_cast<OGLFragmentShader*>(pOpenGLShader);
			CN(this->m_pFragmentShader);
		} break;

		default: {
			CBM((0), "Shader type 0x%x cannot be attached", pOpenGLShader->GetShaderType());
		}
	}

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//numShaders = param;

	CRM(m_pParentImp->glAttachShader(m_OGLProgramIndex, pOpenGLShader->GetShaderID()), "AttachShader failed with GL log:%s", pOpenGLShader->GetInfoLog());

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//CBM((param = numShaders + 1), "Failed to attach shader, num shaders attached %d", param);

	// Assign the shader to the implementation stage
	DEBUG_LINEOUT("Attached shader %d type 0x%x", pOpenGLShader->GetShaderID(), pOpenGLShader->GetShaderType());

Error:
	return r;
}

OGLFramebuffer *OGLProgram::GetOGLFramebuffer() {
	return m_pOGLFramebuffer;
}

texture *OGLProgram::GetOGLFramebufferColorTexture() {
	if (m_pOGLFramebuffer != nullptr)
		return m_pOGLFramebuffer->GetColorTexture();
	else
		return nullptr;
}

// Set Matrix Functions
/*
RESULT OGLProgram::SetEyePosition(point ptEye) {
	return SetPointUniform(ptEye, GetEyePositionUniformName());
}

RESULT OGLProgram::SetModelMatrix(matrix<float, 4, 4> matModel) {
	return Set44MatrixUniform(matModel, GetModelMatrixUniformName());
}

RESULT OGLProgram::SetViewMatrix(matrix<float, 4, 4> matView) {
	return Set44MatrixUniform(matView, GetViewMatrixUniformName());
}

RESULT OGLProgram::SetProjectionMatrix(matrix<float, 4, 4> matProjection) {
	return Set44MatrixUniform(matProjection, GetProjectionMatrixUniformName());
}

RESULT OGLProgram::SetModelViewMatrix(matrix<float, 4, 4> matModelView) {
	return Set44MatrixUniform(matModelView, GetModelViewMatrixUniformName());
}

RESULT OGLProgram::SetViewProjectionMatrix(matrix<float, 4, 4> matViewProjection) {
	return Set44MatrixUniform(matViewProjection, GetViewProjectionMatrixUniformName());
}

RESULT OGLProgram::SetNormalMatrix(matrix<float, 4, 4> matNormal) {
	return Set44MatrixUniform(matNormal, GetNormalMatrixUniformName());
}

RESULT OGLProgram::SetViewOrientationMatrix(matrix<float, 4, 4> matViewOrientaton) {
	return Set44MatrixUniform(matViewOrientaton, GetViewOrientationMatrixUniformName());
}
*/

/*
// TODO: Re-design using attribute registration approach
RESULT OGLProgram::EnableVertexPositionAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexPositionAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OGLProgram::EnableVertexColorAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexColorAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OGLProgram::EnableVertexNormalAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexNormalAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexUVCoordAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableUVCoordAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexTangentAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableTangentAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexBitangentAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableBitangentAttribute();
	else
		return R_FAIL;
}
*/