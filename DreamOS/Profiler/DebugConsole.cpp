#include "DebugConsole.h"
#include <memory>

DebugConsole::DebugConsole()
{
	Validate();
	return;
}

DebugConsole::~DebugConsole()
{
	m_data.clear();
}

const std::vector<std::shared_ptr<DebugData>>& DebugConsole::GetConsoleData()
{
	return m_data;
}

std::shared_ptr<DebugData> DebugConsole::Register()
{
	std::shared_ptr<DebugData> s = std::make_shared<DebugData>();
	m_data.emplace_back(s);
	return s;
}

void DebugConsole::Unregister(std::shared_ptr<DebugData> data)
{
	auto p = std::find(m_data.begin(), m_data.end(), data);
	if (p != m_data.end())
	{
		m_data.erase(p);
	}
}

DebugData::DebugData() 
{
	m_value = "Set this value";
}

DebugData::~DebugData()
{
};

const std::string& DebugData::GetValue()
{
	return m_value;
}

void DebugData::SetValue(std::string value)
{
	m_value = value;
}