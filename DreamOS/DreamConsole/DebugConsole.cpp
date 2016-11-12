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

std::shared_ptr<DebugData> DebugConsole::Register(const std::string& uniqueName)
{
	std::shared_ptr<DebugData> s = std::make_shared<DebugData>(uniqueName);
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

std::shared_ptr<DebugData> DebugConsole::Get(const std::string& uniqueName)
{
	for (auto& p : m_data)
	{
		if (p->GetName().compare(uniqueName) == 0)
		{
			return p;
		}
	}

	return Register(uniqueName);
}

DebugData::DebugData(const std::string& uniqueName) :
	m_uniqueName(uniqueName),
	m_value("Set this value")
{

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

void DebugData::SetValue(point pt)
{
	m_value = (GetName() + ": " + std::to_string(pt.x()) + ", " + std::to_string(pt.y()) + ", " + std::to_string(pt.z())).c_str();
}

void DebugData::SetValue(quaternion q)
{
	m_value = (GetName() + ": " + std::to_string(q.x()) + ", " + std::to_string(q.y()) + ", " + std::to_string(q.z()) + ", " + std::to_string(q.w())).c_str();
}

const std::string& DebugData::GetName()
{
	return m_uniqueName;
}