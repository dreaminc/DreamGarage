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