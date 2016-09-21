#include "OGLUniform.h"

#include "../OpenGLImp.h"
#include "../OGLProgram.h"

#include "../OGLTexture.h"

OGLUniform::OGLUniform(OGLProgram *pParentProgram, const char *pszUniformName, GLint uniformLocationIndex, GLint GLType) :
	GLSLObject(pParentProgram),
	m_uniformIndex(uniformLocationIndex),
	m_GLType(GLType)
{
	m_strUniformName.assign(pszUniformName);
}

OGLUniform::~OGLUniform() {
	// empty
}

RESULT OGLUniform::SetUniform4fv(GLfloat *pVal4fv) {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	GLint location = -1;
	pParentImp->glGetUniformLocation(oglProgramID, m_strUniformName.c_str(), &location);

	CB((location >= 0));
	pParentImp->glUniform4fv(location, 1, pVal4fv);

Error:
	return r;
}

RESULT OGLUniform::SetUniformFloat(GLfloat *pVal1fv) {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	GLint location = -1;
	pParentImp->glGetUniformLocation(oglProgramID, m_strUniformName.c_str(), &location);

	CB((location >= 0));
	pParentImp->glUniform1fv(location, 1, pVal1fv);

Error:
	return r;

}


RESULT OGLUniform::SetUniformInteger(GLint value) {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	GLint location = -1;
	pParentImp->glGetUniformLocation(oglProgramID, m_strUniformName.c_str(), &location);

	CB((location >= 0));
	pParentImp->glUniform1i(location, value);

Error:
	return r;
}

// TODO: Generalize this in the lower specialized class more
RESULT OGLUniform::Set44MatrixUniform(matrix<float, 4, 4> mat) {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	GLint location = -1;
	pParentImp->glGetUniformLocation(oglProgramID, m_strUniformName.c_str(), &location);

	CB((location >= 0));
	pParentImp->glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&mat));

Error:
	return r;
}

// Explicit class based specialization (instead of a template based approach)
RESULT OGLUniformBool::SetUniform(bool flag) {
	return SetUniformInteger((flag)? 1 : 0);
}

RESULT OGLUniformPoint::SetUniform(point pt) {
	return SetUniform4fv(reinterpret_cast<GLfloat*>(&pt));
}

RESULT OGLUniformVector::SetUniform(vector v) {
	return SetUniform4fv(reinterpret_cast<GLfloat*>(&v));
}

RESULT OGLUniformQuaternion::SetUniform(quaternion q) {
	return SetUniform4fv(reinterpret_cast<GLfloat*>(&q));
}

// TODO: Generalize this further 
RESULT OGLUniformMatrix4::SetUniform(matrix<float, 4, 4> mat) {
	return Set44MatrixUniform(mat);
}

RESULT OGLUniformSampler2D::SetUniform(OGLTexture *pTexture) {
	//return SetUniformInteger(pTexture->GetGLTextureNumberDefine());
	//return SetUniformInteger(0);
	return SetUniformInteger(pTexture->GetTextureNumber());
}

RESULT OGLUniformSampler2D::SetUniform(GLint textureIndex) {
	return SetUniformInteger(textureIndex);
}

RESULT OGLUniformSamplerCube::SetUniform(OGLTexture *pTexture) {
	return SetUniformInteger(pTexture->GetOGLTextureIndex());
}
