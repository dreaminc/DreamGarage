#ifndef OGL_PROGRAM_H_
#define OGL_PROGRAM_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgram.h

// OpenGL Program class

#include <functional>
#include <vector>
#include <map>

#include "OGLCommon.h"

#include "core/dimension/DimObj.h"

#include "core/primitives/light.h"

#include "core/camera/stereocamera.h"

#include "shaders/OGLShader.h"
#include "shaders/OGLVertexShader.h"
#include "shaders/OGLFragmentShader.h"
#include "shaders/OGLGeometryShader.h"

#include "shaders/OGLUniform.h"
#include "shaders/OGLUniformBlock.h"

// TODO: This will go into a specific OGLProgram
#include "OGLLightsBlock.h"
#include "OGLMaterialBlock.h"

#include "pipeline/ProgramNode.h"

class OGLImp;
class OGLVertexAttribute;
class OGLUniform;
class OGLFramebuffer;
class OGLTexture;
class ObjectStore;

class OGLProgram : public ProgramNode {
public:
	OGLProgram(OGLImp *pParentImp, std::string strName = "oglprogram", PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	~OGLProgram();

	// ProgramNode Interface
	virtual RESULT SetupConnections() = 0;
	virtual RESULT ProcessNode(long frameID = 0) override;

	virtual RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL);
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
	RESULT MakeGeometryShader(const wchar_t *pszFilename);

	// TODO: Likely more eloquent way to do this
	RESULT RenderObjectStoreBoundingVolumes(ObjectStore *pObjectStore);
	RESULT RenderObjectStore(ObjectStore *pObjectStore);
	RESULT RenderObject(DimObj *pDimObj);
	RESULT RenderObjectBoundingVolume(DimObj *pDimObj);
	RESULT RenderChildren(DimObj *pDimObj);	
	RESULT RenderChildrenBoundingVolumes(DimObj *pDimObj);
	//RESULT RenderObject(VirtualObj *pVirtualObj);

	// Update
	static RESULT UpdateObjectStore(ObjectStore *pObjectStore);
	static RESULT UpdateObject(DimObj *pDimObj);
	static RESULT UpdateChildren(DimObj *pDimObj);
	
	RESULT SetLights(ObjectStore *pSceneGraph);
	virtual RESULT SetLights(std::vector<light*> *pLights);
	virtual RESULT SetMaterial(material *pMaterial);
	
	RESULT SetCamera(camera *pCamera);
	RESULT SetStereoCamera(stereocamera* pStereoCamera, EYE_TYPE eye);

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) = 0;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) = 0;
	virtual RESULT SetCameraUniforms(camera *pCamera) = 0;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) = 0;
	
	// Shaders
	RESULT CreateShader(GLenum type, GLuint *pShaderID);

	RESULT AddSharedShaderFilename(GLenum shaderType, std::wstring strShaderFilename);
	RESULT AddSharedShaderFilename(std::wstring strShaderFilename);

	RESULT ClearSharedShaders();
	std::vector<std::wstring> GetSharedShaderFilenames(GLenum shaderType);
	
	std::map<GLenum, std::vector<std::wstring>> m_sharedShaderFilenamesTyped;
	std::vector<std::wstring> m_sharedShaderFilenamesGlobal;
	

	GLuint GetOGLProgramIndex() { 
		return m_OGLProgramIndex;
	}

	OGLImp *GetOGLImp() {
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

	RESULT AttachShader(OGLShader *pOpenGLShader);

	RESULT BindToDepthBuffer();
	
	RESULT BindToFramebuffer();
	RESULT BindToFramebuffer(OGLFramebuffer* pFramebuffer);

	RESULT UnbindFramebuffer();
	RESULT BindToScreen(int pxWidth, int pxHeight);

	RESULT InitializeRenderToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeDepthToTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight);
	
	//GLuint GetOGLDepthbufferIndex();
	//RESULT SetDepthTexture(int textureNumber);

public:
	OGLFramebuffer *GetOGLFramebuffer();
	texture *GetOGLFramebufferColorTexture();
	RESULT SetFrameBuffer(OGLFramebuffer* pFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);

protected:
	OGLImp *m_pParentImp;
	version m_versionOGL;
	GLuint m_OGLProgramIndex;

	// OGL Framebuffer
	// This can be used to render the program to a texture / framebuffer
	OGLFramebuffer *m_pOGLFramebuffer = nullptr;

	RESULT InitializeFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int channels = 4);
	RESULT InitializeFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int channels = 4);

	RESULT InitializeFrameBufferWithDepth(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int channels = 4);
	
	RESULT InitializeDepthFrameBuffer(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight);
	RESULT InitializeDepthFrameBuffer(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight);
	
	RESULT UpdateFramebufferToViewport(OGLFramebuffer*&pOGLFramebuffer, GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT, int channels = 4);
	RESULT UpdateFramebufferToViewport(GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT, int channels = 4);

	RESULT UpdateFramebufferToCamera(OGLFramebuffer*&pOGLFramebuffer, camera *pCamera, GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT, int channels = 4);
	RESULT UpdateFramebufferToCamera(camera *pCamera, GLenum internalDepthFormat = GL_DEPTH_COMPONENT16, GLenum typeDepth = GL_FLOAT, int channels = 4);

	// Render Texture
	OGLTexture *m_pOGLRenderTexture;
	RESULT InitializeRenderTexture(OGLTexture*&pOGLRenderTexture, GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);
	RESULT InitializeRenderTexture(GLenum internalDepthFormat, GLenum typeDepth, int pxWidth, int pxHeight, int channels);

	// Shaders
	OGLVertexShader *m_pVertexShader = nullptr;
	OGLGeometryShader *m_pGeometryShader = nullptr;
	OGLFragmentShader *m_pFragmentShader = nullptr;

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

	int m_frameBufferDivisionFactor = 1;
};

#endif // ! OGL_PROGRAM_H_