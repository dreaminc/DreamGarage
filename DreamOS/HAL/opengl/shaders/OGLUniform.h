#ifndef OGL_UNIFORM_H_
#define OGL_UNIFORM_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLUniform.h
// This is an OpenGL Uniform variable, owned by a OGLProgram (these are shared across shaders)

#include "../OpenGLCommon.h"
#include "../GLSLObject.h"

#include "Primitives/matrix.h"
#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"

//#include "../OGLTexture.h"

#include <string>

class OpenGLImp;
class OGLProgram;
class OGLTexture;

class OGLUniform : public GLSLObject {
public:
	OGLUniform(OGLProgram *pParentProgram, const char *pszUniformName, GLint uniformLocationIndex, GLint GLType);
	~OGLUniform();

	RESULT SetUniform4fv(GLfloat *pVal4fv);
	RESULT SetUniformInteger(GLint value);
	RESULT Set44MatrixUniform(matrix<float, 4, 4> mat);		// TODO: Generalize this in the lower specialized class more

	RESULT SetUniform(point pt) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(vector v) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(quaternion q) { return R_NOT_IMPLEMENTED; }
	RESULT SetUniform(matrix<float, 4, 4> mat) { return R_NOT_IMPLEMENTED; }

	std::string GetName() { return m_strUniformName; }

private:
	std::string m_strUniformName;
	GLint m_uniformIndex;
	GLint m_GLType;
};

// Explicit class based specialization (instead of a template based approach)
class OGLUniformPoint : public OGLUniform {
public:
	RESULT SetUniform(point pt) {
		return SetUniform4fv(reinterpret_cast<GLfloat*>(&pt));
	}
};

class OGLUniformVector : public OGLUniform {
public:
	RESULT SetUniform(vector v) {
		return SetUniform4fv(reinterpret_cast<GLfloat*>(&v));
	}
};

class OGLUniformQuaternion : public OGLUniform {
public:
	RESULT SetUniform(quaternion q) {
		return SetUniform4fv(reinterpret_cast<GLfloat*>(&q));
	}
};

// TODO: Generalize this further 
class OGLUniformMatrix4 : public OGLUniform {
public:
	RESULT SetUniform(matrix<float, 4, 4> mat) {
		return Set44MatrixUniform(mat);
	}
};

class OGLUniformSampler2D : public OGLUniform {
	RESULT SetUniform(OGLTexture *pTexture) {
		// TODO: DO IT
		return R_NOT_IMPLEMENTED;
	}
};

class OGLUniformSamplerCube : public OGLUniform {
	RESULT SetUniform(OGLTexture *pTexture) {
		// TODO: DO IT
		return R_NOT_IMPLEMENTED;
	}
};

#endif // ! OGL_UNIFORM_H_