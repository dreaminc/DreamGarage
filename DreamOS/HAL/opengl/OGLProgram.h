#ifndef OGL_PROGRAM_H_
#define OGL_PROGRAM_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/opengl/OGLProgram.h
// OpenGL Program class

#include <vector>
#include <map>

#include "OpenGLCommon.h"

#include "Primitives/DimObj.h"
#include "Primitives/light.h"
#include "Primitives/stereocamera.h"

#include "shaders/OpenGLShader.h"
#include "shaders/OGLVertexShader.h"
#include "shaders/OGLFragmentShader.h"

#include "shaders/OGLUniform.h"
#include "shaders/OGLUniformBlock.h"

// TODO: This will go into a specific OGLProgram
#include "OGLLightsBlock.h"
#include "OGLMaterialBlock.h"

class OpenGLImp;
class OGLVertexAttribute;
class OGLUniform;
class OGLFramebuffer;
class OGLTexture;
class ObjectStore;

class OGLProgram {
public:
	OGLProgram(OpenGLImp *pParentImp);
	~OGLProgram();

	virtual RESULT OGLInitialize();
	RESULT OGLInitialize(const wchar_t *pszVertexShaderFilename, const wchar_t *pszFragmentShaderFilename, version versionFile);
	
	RESULT CreateProgram();
	RESULT ReleaseProgram();
	RESULT LinkProgram();
	RESULT UseProgram();
	RESULT IsProgram();

	// Attributes
	RESULT BindAttribLocation(GLint index, const char* pszName);
	RESULT GetVertexAttributesFromProgram();
	RESULT InitializeAttributes();
	RESULT EnableAttributes();
	RESULT BindAttributes();

	// Uniform Variables
	RESULT GetUniformVariablesFromProgram();

	RESULT GetUniformBlocksFromProgram();
	RESULT BindUniformBlock(GLint uniformBlockIndex, GLint uniformBlockBindingPoint);
	char* GetProgramInfoLog();

	//RESULT MakeShader(const wchar_t *pszFilename, version versionFile);
	RESULT MakeVertexShader(const wchar_t *pszFilename);
	RESULT MakeFragmentShader(const wchar_t *pszFilename);

	// TODO: Likely more eloquent way to do this
	RESULT RenderSceneGraph(ObjectStore *pSceneGraph);
	RESULT RenderObject(DimObj *pDimObj);
	RESULT RenderChildren(DimObj *pDimObj);	
	RESULT RenderObject(VirtualObj *pVirtualObj);
	
	RESULT SetLights(ObjectStore *pSceneGraph);
	virtual RESULT SetLights(std::vector<light*> *pLights);
	virtual RESULT SetMaterial(material *pMaterial);
	
	RESULT SetCamera(camera *pCamera);
	RESULT SetStereoCamera(stereocamera *pStereoCamera, EYE_TYPE eye);

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) = 0;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) = 0;
	virtual RESULT SetCameraUniforms(camera *pCamera) = 0;
	virtual RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) = 0;
	
	// Shaders
	RESULT CreateShader(GLenum type, GLuint *pShaderID);

	GLuint GetOGLProgramIndex() { 
		return m_OGLProgramIndex;
	}

	OpenGLImp *GetOGLImp() {
		return m_pParentImp;
	}

	// Render Objects


public:
	// TODO: [SHADER] This should be baked into Shader
	// TODO: Build Shader Attribute Registration
	/*
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
	RESULT EnableVertexUVCoordAttribute();
	RESULT EnableVertexTangentAttribute();
	RESULT EnableVertexBitangentAttribute();
	*/

	RESULT AttachShader(OpenGLShader *pOpenGLShader);

	RESULT BindToDepthBuffer();
	RESULT BindToFramebuffer();
	RESULT UnbindFramebuffer();
	RESULT BindToScreen(int pxWidth, int pxHeight);

	RESULT InitializeRenderToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeDepthToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight);
	GLuint GetOGLDepthbufferIndex();
	RESULT SetDepthTexture(int textureNumber);

	// TODO: Is this the right way to do it?  It's better than feeding the texture directly in
	// This may be better with an OGLProgram hierarchy - children/dependents etc
	RESULT SetOGLProgramDepth(OGLProgram *pOGLProgramDepth);
protected:
	OGLProgram *m_pOGLProgramDepth;

protected:
	OpenGLImp *m_pParentImp;
	version m_versionOGL;
	GLuint m_OGLProgramIndex;

	// OGL Framebuffer
	// This can be used to render the program to a texture / framebuffer
	OGLFramebuffer *m_pOGLFramebuffer;
	OGLTexture *m_pOGLRenderTexture;
	//RESULT BindToFrameBuffer();
	RESULT InitializeFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeRenderTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeDepthFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight);

	// Shaders
	OGLVertexShader *m_pVertexShader;
	OGLFragmentShader *m_pFragmentShader;
	// TODO: Other shaders ?

	// Vertex Attributes
	RESULT RegisterVertexAttribute(OGLVertexAttribute **pOGLVertexAttribute, std::string strVertexAttributeName);
	std::map<std::string, OGLVertexAttribute**> m_registeredProgramShaderVertexAttribute;
	std::vector<OGLVertexAttribute*> m_vertexAttributes;

	// Uniforms
	RESULT RegisterUniform(OGLUniform **pOGLUniform, std::string strUniformName);
	std::map<std::string, OGLUniform**> m_registeredProgramShaderUniforms;
	std::vector<OGLUniform*> m_uniformVariables;

	// Uniform Blocks
	RESULT RegisterUniformBlock(OGLUniformBlock **pOGLUniformBlock, std::string strUniformBlockName);
	std::map<std::string, OGLUniformBlock**> m_registeredProgramShaderUniformBlocks;
	std::vector<OGLUniformBlock*> m_uniformBlocks;
	
	// TODO: This should go into the actual OGLProgram implementation
	///*
	//const char *GetLightsUniformBlockName() { return "ub_Lights"; }
	//const char *GetMaterialsUniformBlockName() { return "ub_material"; }
	//OGLLightsBlock *m_pLightsBlock;
	//OGLMaterialBlock *m_pMaterialsBlock;
	//*/

	RESULT UpdateUniformBlockBuffers();
	RESULT BindUniformBlocks();
	RESULT InitializeUniformBlocks();

	// TODO: Pipelines
};

#endif // ! OGL_PROGRAM_H_