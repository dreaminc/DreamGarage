#ifndef OGL_UNIFORM_H_
#define OGL_UNIFORM_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLUniform.h

// This is an OpenGL Uniform variable, owned by a OGLProgram (these are shared across shaders)

#include <string>

#include "hal/ogl/OGLCommon.h"
#include "hal/ogl/GLSLObject.h"

#include "core/matrix/matrix.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"
#include "core/primitives/quaternion.h"


class OGLImp;
class OGLProgram;
class OGLTexture;

class OGLUniform : public GLSLObject {
public:
	OGLUniform(OGLProgram *pParentProgram, const char *pszUniformName, GLint uniformLocationIndex, GLint GLType);
	~OGLUniform();

	RESULT SetUniform4fv(GLfloat *pVal4fv);
	RESULT SetUniformFloat(GLfloat *pVal1fv);
	RESULT SetUniformInteger(GLint value);
	RESULT Set44MatrixUniform(matrix<float, 4, 4> mat);		// TODO: Generalize this in the lower specialized class more

	RESULT SetUniform(bool flag) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(point pt) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(vector v) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(quaternion q) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(matrix<float, 4, 4> mat) { return R_NOT_IMPLEMENTED; }

	std::string GetName() { return m_strUniformName; }

	GLint GetUniformIndex() { return m_uniformIndex; }

private:
	std::string m_strUniformName;
	GLint m_uniformIndex;
	GLint m_GLType;
};

// Explicit class based specialization (instead of a template based approach)
class OGLUniformBool : public OGLUniform {
public:
	RESULT SetUniform(bool flag);
};

class OGLUniformFloat : public OGLUniform {
public:
	RESULT SetUniform(float flag);
};

class OGLUniformInt : public OGLUniform {
public:
	RESULT SetUniform(int val);
};

class OGLUniformPoint : public OGLUniform {
public:
	RESULT SetUniform(point pt);
};

class OGLUniformVector : public OGLUniform {
public:
	RESULT SetUniform(vector v);
};

class OGLUniformQuaternion : public OGLUniform {
public:
	RESULT SetUniform(quaternion q);
};

// TODO: Generalize this further 
class OGLUniformMatrix4 : public OGLUniform {
public:
	RESULT SetUniform(matrix<float, 4, 4> mat);
};

class OGLUniformSampler2D : public OGLUniform {
public:
	RESULT SetUniform(OGLTexture *pTexture);
	RESULT SetUniform(GLint textureIndex);
};

class OGLUniformSamplerCube : public OGLUniform {
public:
	RESULT SetUniform(OGLTexture *pTexture);
	RESULT SetUniform(GLint textureIndex);
};

#endif // ! OGL_UNIFORM_H_