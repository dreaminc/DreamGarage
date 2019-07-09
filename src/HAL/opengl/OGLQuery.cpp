#include "OGLQuery.h"

OGLQuery::OGLQuery(OpenGLImp *pParentImp) :
	m_pParentImp(pParentImp)
{
	// empty
}

OGLQuery::~OGLQuery() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glDeleteQueries(1, &m_queryID));

Error:
	return;
}

RESULT OGLQuery::OGLInitialize() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glGenQueries(1, &m_queryID));

Error:
	return r;
}

RESULT OGLQuery::BeginQuery(GLenum target) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBeginQuery(target, m_queryID));
	m_fPendingQuery = true;

Error:
	return r;
}


RESULT OGLQuery::EndQuery(GLenum target) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glEndQuery(target));
	m_fPendingQuery = false;

Error:
	return r;
}

RESULT OGLQuery::GetQueryObject(GLuint *pValue) {
	RESULT r = R_PASS;

	CR(m_pParentImp->glGetQueryObjectuiv(m_queryID, GL_QUERY_RESULT, pValue));

Error:
	return r;
}

