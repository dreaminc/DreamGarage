#include "NamedPipeClient.h"

NamedPipeClient::NamedPipeClient(std::wstring strPipename) :
	m_strPipename(strPipename)
{
	// empty
}

NamedPipeClient::~NamedPipeClient() {
	// empty
}

// TODO: 